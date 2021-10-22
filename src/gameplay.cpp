#include "gameplay.h"
#include <algorithm>
#include "fen.h"

namespace
{

    template<color VColor, typename TTable>
    void get_pv_inner(std::vector<move>& out, game& g, const TTable& table)
    {
        auto pv_node = table[g.get_hash()];
        if (pv_node.f != decltype(pv_node)::flag::none)
        {
            move best_move = pv_node.best_move;
            out.push_back(pv_node.best_move);
            g.do_move<VColor>(best_move);
            get_pv_inner<invert_color(VColor)>(out, g, table);
            g.undo_move<VColor>();
        }
    }

    template<typename TTable>
    std::vector<move> get_pv(game& g, const TTable& table)
    {
        std::vector<move> out;
        out.reserve(20);
        if (g.get_game_context().turn == color::white)
            get_pv_inner<color::white,TTable>(out, g, table);
        else get_pv_inner<color::black,TTable>(out, g, table);
        return out;
    }
}

gameplay_st::gameplay_st(float time_mins, color my_turn, const game& g, u64 cap_tt):
gameplay(time_mins,my_turn),g(g),tt(cap_tt)
{
}

std::optional<move_info> gameplay_st::pick_next_move()
{
//    //rnbqkbnr/2pppppp/B7/pp6/8/4P3/PPPP1PPP/RNBQK1NR w KQkq b6 0 3
//    if (my_turn != g.get_game_context().turn) return {};
//    int depth = 0;
//    auto score = negamax<color::black>(timer(10000), depth, neg_inf, pos_inf, 1);
//    move_info info;
//    info.depth = depth;
//    info.score = score;
//    info.principal_variation = get_pv(g, tt);
//
// //   if (g.get_game_context().turn == color::white) g.do_move<color::white>(info.principal_variation[0]);
////    else g.do_move<color::black>(info.principal_variation[0]);
//    return info;
    float think_time = 60*1000*(time_mins / 35.f); // Assume 35 moves for a game for now.
    t.restart();
    auto [score,depth] = iterative_deepening(think_time);
    move_info info;
    info.depth = depth;
    info.principal_variation = get_pv(g, tt);
    info.score = score;
    ply++;
    
    // Apply the move
    if (g.get_game_context().turn == color::white) g.do_move<color::white>(info.principal_variation[0]);
    else g.do_move<color::black>(info.principal_variation[0]);

    t.stop();
    return info;
}

const game& gameplay_st::get_game() const
{
    return g;
}

game& gameplay_st::get_game()
{
    return g;
}


bool gameplay_st::incoming_move(const move& m)
{
    if (!g.is_move_valid(m) || my_turn == g.get_game_context().turn)
         return false;
 

    if (g.get_game_context().turn == color::white)
    {
        g.do_move<color::white>(m);
    }
    else g.do_move<color::black>(m);
    ply++;
    return true;
}

std::pair<int, int> gameplay_st::iterative_deepening(float ms)
{
    int depth = 3;
    int score = 0;
    timer t(ms);
    while (true)
    {
        // If time is up or the last iteration took more than 85% of the time.
        if (t.time_is_up() || t.action_took_time_of_span() > 0.60f)
        {
            depth--;
            break;
        }
        if (g.get_game_context().turn == color::white)
        {
            t.action_now();
            score = negamax<color::white>(t, depth, neg_inf, pos_inf, 1);
        }
        else
        {
            t.action_now();
            score = -negamax<color::black>(t, depth, neg_inf, pos_inf, -1);
        }
        depth++;
    }
    return std::make_pair(score, depth);
}

// See https://en.wikipedia.org/wiki/Negamax
template<color VColor>
int gameplay_st::negamax(const timer& t, int depth_left, int alpha, int beta, int c)
{
    //if (t.time_is_up()) throw time_up();
    int orig_alpha = alpha;
    trans_entry& e = tt[g.get_hash()];
    // e.depth stores the values for a search to depth e.depth.
    // If e.depth < depth_left, then we actually want the evaluation of a depth much deeper than whats saved, so we can't use the entry
    if (e.f != trans_entry::flag::none && e.depth >= depth_left)
    {
        switch (e.f)
        {
        case trans_entry::flag::exact:
            return e.val;
        case trans_entry::flag::alpha:
            alpha = std::max((int)e.val, alpha);
            break;
        case trans_entry::flag::beta:
            beta = std::min((int)e.val, beta);
            break;
        }
        if (alpha >= beta)
        {
            return e.val;
        }
    }


    auto moves = g.legal_moves<VColor>();
    //bool is_in_check = g.is_in_check<VColor>();
    //depth_left = is_in_check ? depth_left + 1 : depth_left;
    if (moves.size() == 0)
    {
        //printf("Fen: %s", fen::game_to_fen(g).c_str());
        if (g.is_in_check<VColor>()) return pos_inf * c;
        else return 0;
    }

    if (g.is_draw_by_rep() || g.is_draw_by_halfmoves()) return 0;

    if (depth_left == 0)
    {
        int score = ev(g) * c;
        return score;
    }

    //std::sort(moves.begin(),moves.end(),)

    int score = neg_inf;
    move best_move{};
    for (move m : moves)
    {
        g.do_move<VColor>(m);
        // We want the maximum value, thats the better score.
        score = std::max(-negamax<invert_color(VColor)>(t,depth_left - 1, -beta, -alpha, -c), score);
        
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
            break;
        }
        g.undo_move<VColor>();
    }

    trans_entry::flag flag;
    // Every move was worse than what we currently have. 
    // The retriever might have an even worse move, so this represents an upperbound
    if (score <= orig_alpha)
    {
        flag = trans_entry::flag::alpha;
        // Take random move because every move is bad, but still better than a null move
        best_move = moves[0];
    }
    // We achieved a cutoff
    // The retriever might have a better move still, so this is at least a lower bound 
    else if (score >= beta)
    {
        flag = trans_entry::flag::beta;
    }
    // We improved the lowerbound
    else flag = trans_entry::flag::exact;
    tt.insert(g.get_hash(), score, flag, depth_left, best_move, ply);
    return score;
}
