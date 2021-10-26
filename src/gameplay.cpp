#include "gameplay.h"

std::optional<move_info> gameplay_conventional::pick_next_move()
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
    if (front.get_game_over_state() != game_over_state::running) return {};
    float think_time = 60 * 1000 * (time_mins / 35.f); // Assume 35 moves for a game for now.
    t.restart();
    auto [score, depth, searched_nodes] = iterative_deepening(think_time);
    move_info info;
    info.depth = depth;
    info.searched_nodes = searched_nodes;
    info.principal_variation = get_pv_(depth);
    info.score = score;
    ply++;

    // Apply the move
    const move& m = info.principal_variation[0];
    if (front.get_game_context().turn == color::white)
    {
        front.do_move<color::white>(m);
        ev.do_move<color::white>(m,front);
    }

    else
    {
        front.do_move<color::black>(m);
        ev.do_move<color::black>(m,front);
    }
    //printf("AFTER MOVE: %s", front.get_board().pretty().c_str());
    t.stop();
    return info;
}

bool gameplay_conventional::incoming_move(const move& m)
{
    if (!front.is_move_valid(m))
        return false;

    if (front.get_game_context().turn == color::white)
    {
        front.do_move<color::white>(m);
        ev.do_move<color::white>(m, front);
    }
    else
    {
        front.do_move<color::black>(m);
        ev.do_move<color::black>(m, front);
    }
    ply++;
    return true;
}

const game& gameplay_conventional::get_game() const
{
    return front;
}

game& gameplay_conventional::get_game()
{
    return front;
}
