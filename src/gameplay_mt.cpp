#include "gameplay_mt.h"

gameplay_mt::gameplay_mt(float time_mins, color my_turn, const game& g, u64 cap_tt) :
    gameplay_conventional(time_mins, my_turn,g), tt(cap_tt)
{
    locals.resize(std::thread::hardware_concurrency());
}

void gameplay_mt::renew_gamestate()
{
    for (thread_loc& local : locals)
    {
        local.ev = ev;
        local.g = front;
        local.time_up = false;
    }
}

std::vector<move> gameplay_mt::get_pv_(int depth)
{
    return get_pv(front, tt, depth);
}

void gameplay_mt::start_threads(int depth)
{
    for (int i = 0; i < locals.size(); i++)
    {

        if (front.get_game_context().turn == color::white)
            workers.push_back(std::thread([&, i]() {
            try
            {
                int searched_nodes = 0;
                int score = negamax<color::white>(depth, depth, neg_inf, pos_inf, 1, searched_nodes,i);
                locals[i].score = score;
                locals[i].searched_nodes = searched_nodes;
            }
            catch (time_up&)
            {
                locals[i].time_up = true;
            }
                }));

        else
            workers.push_back(std::thread([&, i]() {
            try
            {
                int searched_nodes = 0;
                int score = -negamax<color::black>(depth, depth, neg_inf, pos_inf, -1, searched_nodes,i);
                locals[i].score = score;
                locals[i].searched_nodes = searched_nodes;
            }
            catch (time_up&)
            {
                locals[i].time_up = true;
            }
                }));
    }
}

void gameplay_mt::join_threads()
{
    for (auto& t : workers)
    {
        if(t.joinable())
            t.join();
    }
}

std::tuple<int, int, int> gameplay_mt::iterative_deepening(float ms)
{
    int depth = 3;
    int score = 0;
    timer t(ms);
    renew_gamestate();
    move_timer = timer(ms);
    move_timer.restart();
    t.restart();
    while (true)
    {
        t.action_now();
        start_threads(depth);
        join_threads();
        if (locals[0].time_up) break;
        depth++;
        printf("Iteration took: %f of timespan,d=%i\n", t.action_took_time_of_span(), depth - 1);
    }
    printf("Iteration couldn't finish and took: %f of timespan,d=%i\n", t.action_took_time_of_span(), depth - 1);
    int searched_nodes_all = 0;
    int max_score = neg_inf-1000;
    for (auto& local : locals)
    {
        if (max_score < local.score) max_score = local.score;
        searched_nodes_all += local.searched_nodes;
    }

    return { max_score, depth,searched_nodes_all };
}

//rnb1k1nr/pppp1ppp/4p3/8/1bBqPB2/2NP2P1/PPP2P1P/R2QK1NR w KQkq - 3 7
template<color VColor>
int gameplay_mt::quiesence_search(int depth_left, int max_depth, int alpha, int beta, int c, int& searched_nodes, int idx)
{
    if (move_timer.time_is_up()) throw time_up();
    auto& local = locals[idx];
    auto& g = local.g;
    auto& ev_ = local.ev;
    auto& ordering = local.ordering;
    searched_nodes++;
    trans_entry_mt& e = tt[g.get_hash()];
    {
        std::lock_guard<std::mutex> lg(e.lock);
        // e.depth stores the values for a search to depth e.depth.
        // If e.depth < depth_left, then we actually want the evaluation of a depth much deeper than whats saved, so we can't use the entry
        if (e.f != move_flag::none && e.depth >= depth_left && e.age == ply)
        {
            switch (e.f)
            {
            case move_flag::exact:
                return e.val;
            case move_flag::alpha:
                alpha = std::max((int)e.val, alpha);
                break;
            case move_flag::beta:
                beta = std::min((int)e.val, beta);
                break;
            }
            if (alpha >= beta)
            {
                return e.val;
            }
        }
    }
    if (g.is_draw_by_rep() || g.is_draw_by_halfmoves()) return 0;
    int stand_pat = ev_(g) * c;
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    // If it crashes somehow I need to enable this
    /*if (depth_left == 0)
    {
        return stand_pat;
    }*/

    auto moves = g.legal_moves<VColor, true>();

    if (moves.empty()) return alpha;

    ordering.sort_nokillers(moves, e.f, e.best_move);

    for (move m : moves)
    {
        g.do_move<VColor>(m);
        ev_.do_move(m, c);
        int score = -quiesence_search<invert_color(VColor)>(depth_left - 1, max_depth, -beta, -alpha, -c, searched_nodes,idx);
        ev_.undo_move(m, c);
        g.undo_move<VColor>();

        if (score >= beta)
        {
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
        }
    }
    return alpha;
}

// See https://en.wikipedia.org/wiki/Negamax
template<color VColor>
int gameplay_mt::negamax(int depth_left, int max_depth, int alpha, int beta, int c, int& searched_nodes, int idx)
{
    if (move_timer.time_is_up()) throw time_up();
    auto& local = locals[idx];
    game& g = local.g;
    eval& ev_ = local.ev;
    move_ordering& ordering = local.ordering;
    searched_nodes++;
    //if (t.time_is_up()) throw time_up();
    int orig_alpha = alpha;
    u64 father_hash = g.get_hash();
    trans_entry_mt& e = tt[father_hash];
    {
        std::lock_guard<std::mutex> l(e.lock);
        e.nproc++;
        // e.depth stores the values for a search to depth e.depth.
        // If e.depth < depth_left, then we actually want the evaluation of a depth much deeper than whats saved, so we can't use the entry
        if (e.f != move_flag::none && e.depth >= depth_left && e.age == ply)
        {
            switch (e.f)
            {
            case move_flag::exact:
                return e.val;
            case move_flag::alpha:
                alpha = std::max((int)e.val, alpha);
                break;
            case move_flag::beta:
                beta = std::min((int)e.val, beta);
                break;
            }
            if (alpha >= beta)
            {
                e.nproc--;
                return e.val;
            }
        }
    }

    if (g.is_draw_by_rep() || g.is_draw_by_halfmoves())
    {
        std::lock_guard<std::mutex> l(e.lock);
        e.nproc--;
        return 0;
    }
    if (depth_left == 0)
    {
        int score = quiesence_search<VColor>(DEPTH_QSEARCH, max_depth, alpha, beta, c, searched_nodes,idx);
        searched_nodes--;
        {
            std::lock_guard<std::mutex> l(e.lock);
            e.nproc--;
        }
        return score;
    }

    auto moves = g.legal_moves<VColor>();

    if (moves.size() == 0)
    {
        {
            std::lock_guard<std::mutex> l(e.lock);
            e.nproc--;
            if (g.is_in_check<VColor>()) return pos_inf * c;
            else return 0;
        }
    }

    int current_ply = ply + (max_depth - depth_left);

    ordering.sort(moves, e.f, e.best_move, current_ply);

    int score = neg_inf;
    move best_move{};
    assert(moves.size() < 250);
    array_vector<move, 250> delay;
    for (move m : moves)
    {
        g.do_move<VColor>(m);
        ev_.do_move(m, c);
        trans_entry_mt& e = tt[g.get_hash()];
        {
            std::lock_guard<std::mutex> l(e.lock);
            if (e.nproc > 0)
            {
                delay.push_back(m);
                continue;
            }
        }
        // We want the maximum value, thats the better score.
        score = std::max(-negamax<invert_color(VColor)>(depth_left - 1, max_depth, -beta, -alpha, -c, searched_nodes,idx), score);

        // We improved the lowerbound 
        if (score > alpha)
        {
            alpha = score;
            best_move = m;
        }

        // If we have a score thats greater than our upperbound, we can prune 
        if (alpha >= beta)
        {
            best_move = m;
            g.undo_move<VColor>();
            ev_.undo_move(m, c);
            ordering.update_killer_move(m, current_ply);
            break;
        }
        ev_.undo_move(m, c);
        g.undo_move<VColor>();
    }

    for (move m : delay)
    {
        g.do_move<VColor>(m);
        ev_.do_move(m, c);
        trans_entry_mt& e = tt[g.get_hash()];
        {
            std::lock_guard<std::mutex> l(e.lock);
            if (e.nproc > 0)
            {
                delay.push_back(m);
                continue;
            }
        }
        // We want the maximum value, thats the better score.
        score = std::max(-negamax<invert_color(VColor)>(depth_left - 1, max_depth, -beta, -alpha, -c, searched_nodes, idx), score);

        // We improved the lowerbound 
        if (score > alpha)
        {
            alpha = score;
            best_move = m;
        }

        // If we have a score thats greater than our upperbound, we can prune 
        if (alpha >= beta)
        {
            best_move = m;
            g.undo_move<VColor>();
            ev_.undo_move(m, c);
            ordering.update_killer_move(m, current_ply);
            break;
        }
        ev_.undo_move(m, c);
        g.undo_move<VColor>();
    }

    move_flag flag;
    // Every move was worse than what we currently have. 
    if (score <= orig_alpha)
    {
        flag = move_flag::alpha;
        // Take random move because every move is bad, but still better than a null move
        best_move = moves[0];
    }
    // We achieved a cutoff
    else if (score >= beta)
    {
        flag = move_flag::beta;
    }
    // We improved the lowerbound
    else flag = move_flag::exact;
    {
        std::lock_guard<std::mutex> l(e.lock);
        tt.insert_mt(g.get_hash(), score, flag, depth_left, best_move, ply);
        e.nproc--;
        return score;
    }
}
