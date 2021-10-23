#include "move_ordering.h"


//void move_ordering::sort_alt(const std::vector<move>& in, array_vector<std::pair<int, move>, 250>& out, move_flag flag, const move& best_move)
//{
//	for (const auto& m : in)
//	{
//		out.push_back({ score(m,best_move,flag),m });
//	}
//
//	std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
//		return a.first < b.first;
//		});
//}

void move_ordering::sort(std::vector<move>& to_sort, move_flag flag, const move& best_move, int ply)
{
	std::sort(to_sort.begin(), to_sort.end(), [&](const auto& a, const auto& b) {
		return score(a,best_move,flag,ply) > score(b,best_move,flag,ply);
		});
}

void move_ordering::sort_nokillers(std::vector<move>& to_sort, move_flag flag, const move& best_move)
{
	std::sort(to_sort.begin(), to_sort.end(), [&](const auto& a, const auto& b) {
		return score_nokillers(a, best_move, flag) > score_nokillers(b, best_move, flag);
		});
}

int move_ordering::score(const move& m, const move& best_move, move_flag flag, int ply)
{
	return pv_move(m, best_move, flag) + killer_move(m,ply) + mvv_lva(m);
}

int move_ordering::score_nokillers(const move& m, const move& best_move, move_flag flag)
{
	return pv_move(m, best_move, flag) + mvv_lva(m);
}

int move_ordering::pv_move(const move& m, const move& best_move, move_flag flag)
{
	// Beta cutoff or at least improved lower bound
	if ((flag == move_flag::beta || flag == move_flag::exact) && m == best_move) return king_weight + 1001;
	return 0;
}

void move_ordering::update_killer_move(const move& m, int ply)
{
	auto& m1 = killer_moves[ply][0];
	if (m1.get_moved_piece_type() == piece_type::none)
	{
		m1 = m;
		return;
	}

	killer_moves[ply][1] = m1;
	killer_moves[ply][0] = m;
}

int move_ordering::killer_move(const move& m, int ply)
{
	return (static_cast<int>(killer_moves[ply][0] == m || killer_moves[ply][1] == m) * 50); 
}

int move_ordering::mvv_lva(const move& m)
{
	auto mt = m.get_move_type();
	if (mt != move_type::captures && mt != move_type::en_passant && mt != move_type::promo_captures) return 0;
	auto vic = m.get_captured_piece_type();
	auto att = m.get_moved_piece_type();
	return victim_attacker_score[static_cast<int>(vic)][static_cast<int>(att)];
}
