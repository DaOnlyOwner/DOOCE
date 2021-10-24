#include "gameplay_st.h"

gameplay_st::gameplay_st(float time_mins, color my_turn, const game& g, u64 cap_tt) :
    gameplay_conventional(time_mins, my_turn,g), tt(cap_tt), move_timer(0)
{
}

std::vector<move> gameplay_st::get_pv_(int depth)
{
    return get_pv(front, tt, depth);
}


std::tuple<int, int, int> gameplay_st::iterative_deepening(float ms)
{
    int depth = 3;
    int score = 0;
    timer t(ms);
    int searched_nodes = 0;
    g = front;
    move_timer = timer(ms);
    move_timer.restart();
    while (true)
    {
        try
        {
            int searched_nodes_ = 0;
            if (g.get_game_context().turn == color::white)
            {
                t.action_now();
                score = negamax<color::white>(depth, depth, neg_inf, pos_inf, searched_nodes_,false);
            }
            else
            {
                t.action_now();
                score = -negamax<color::black>(depth, depth, neg_inf, pos_inf, searched_nodes_,false); // Dont perform null moves at the start
            }
            printf("Iteration took: %f time of span,d=%i\n", t.action_took_time_of_span(), depth);
            searched_nodes = searched_nodes_;
            depth++;
        }
        catch (time_up)
        {
            printf("Iteration was not finished and took: %f time of span,d=%i\n", t.action_took_time_of_span(), depth);
            // Reset the evaluation
            ev = eval(front);
            depth--;
            break;
        }
    }
    printf("\n");
    return { score, depth,searched_nodes };
}

//rnb1k1nr/pppp1ppp/4p3/8/1bBqPB2/2NP2P1/PPP2P1P/R2QK1NR w KQkq - 3 7
template<color VColor>
int gameplay_st::quiesence_search(int depth_left, int max_depth, int alpha, int beta, int& searched_nodes)
{
    constexpr int c = VColor == color::white ? 1 : -1;
    if (move_timer.time_is_up()) throw time_up();
    searched_nodes++;
    trans_entry& e = tt[g.get_hash()];
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
    if (g.is_draw_by_rep() || g.is_draw_by_halfmoves()) return 0;
    int stand_pat = ev(g,c);
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
        ev.do_move<VColor>(m,g);
        int score = -quiesence_search<invert_color(VColor)>(depth_left - 1, max_depth, -beta, -alpha, searched_nodes);
        ev.undo_move<VColor>(m,g);
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
int gameplay_st::negamax(int depth_left, int max_depth, int alpha, int beta, int& searched_nodes, bool null_move)
{
    constexpr int c = VColor == color::white ? 1 : -1;
    constexpr int R = 2;
    if (move_timer.time_is_up()) throw time_up();
    searched_nodes++;
    //if (t.time_is_up()) throw time_up();
    int orig_alpha = alpha;
    trans_entry& e = tt[g.get_hash()];
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


    if (g.is_draw_by_rep() || g.is_draw_by_halfmoves()) return 0;

    if (depth_left == 0)
    {
        int score = quiesence_search<VColor>(DEPTH_QSEARCH, max_depth, alpha, beta, searched_nodes);
        searched_nodes--;
        return score;
    }
    bool in_check = g.is_in_check<VColor>();
    // Do a NULL move
    if (null_move && ply > 0 && depth_left >= (R + 1) && !in_check && g.get_board().has_pieces_except_pawn_king<VColor>())
    {
        g.do_nullmove();
        int score_nullmove = -negamax<invert_color(VColor)>(depth_left - 1 - R, max_depth, -beta, -beta + 1, searched_nodes, false);
        g.undo_nullmove();
        if (score_nullmove >= beta) return beta;
    }


    auto moves = g.legal_moves<VColor>();
    assert(moves.size() <= 250);
    //bool is_in_check = g.is_in_check<VColor>();
    //depth_left = is_in_check ? depth_left + 1 : depth_left;
    if (moves.size() == 0)
    {
        //printf("Fen: %s", fen::game_to_fen(g).c_str());
        if (in_check) return pos_inf * c;
        else return 0;
    }


    int current_ply = ply + (max_depth - depth_left);

    ordering.sort(moves, e.f, e.best_move, current_ply);

    int score = neg_inf;
    move best_move{};
    for (move m : moves)
    {
        g.do_move<VColor>(m);
        ev.do_move<VColor>(m,g);
        // We want the maximum value, thats the better score.
        score = std::max(-negamax<invert_color(VColor)>(depth_left - 1, max_depth, -beta, -alpha, searched_nodes,true), score);

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
            ev.undo_move<VColor>(m,g);
            ordering.update_killer_move(m, current_ply);
            break;
        }
        ev.undo_move<VColor>(m,g);
        g.undo_move<VColor>();
    }

    move_flag flag;
    // Every move was worse than what we currently have. 
    // The retriever might have an even worse move, so this represents an upperbound
    if (score <= orig_alpha)
    {
        flag = move_flag::alpha;
        // Take random move because every move is bad, but still better than a null move
        best_move = moves[0];
    }
    // We achieved a cutoff
    // The retriever might have a better move still, so this is at least a lower bound 
    else if (score >= beta)
    {
        flag = move_flag::beta;
    }
    // We improved the lowerbound
    else flag = move_flag::exact;
    insert_tt(tt,g.get_hash(), score, flag, depth_left, best_move, ply);
    return score;
}
