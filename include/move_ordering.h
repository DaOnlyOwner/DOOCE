#pragma once
#include "move.h"
#include <vector>
#include "game.h"
#include "array_vector.h"
#include "eval.h"
#include "transposition_table.h"
#include <utility>
#include <algorithm>
#include <array>

class move_ordering
{
public:
	move_ordering()
	{
		for (int p_att = 0; p_att < 6; p_att++)
		{
			for (int p_vic = 0; p_vic < 6; p_vic++)
			{
				victim_attacker_score[p_vic][p_att] = victim_score[p_vic] + attacker_score[p_att];
			}
		}
	}

	//void sort_alt(const std::vector<move>& in, array_vector<std::pair<int, move>, 250>& out, move_flag flag, const move& best_move);

	void sort(std::vector<move>& to_sort, move_flag flag, const move& best_move, int ply);
	void sort_nokillers(std::vector<move>& to_sort, move_flag flag, const move& best_move);
	void update_killer_move(const move& m, int ply);

private:
	std::array<std::array<move, 2>, max_ply> killer_moves{};
	int score(const move& m, const move& best_move, move_flag flag, int ply);
	int score_nokillers(const move& m, const move& best_move, move_flag flag);

	int pv_move(const move& m, const move& best_move, move_flag flag);

	int killer_move(const move& m, int ply);

	int mvv_lva(const move& m);

	constexpr static int king_weight = eval::qWt + 100;
	constexpr static int victim_score[6] = { eval::pWt,eval::rWt,eval::bWt,eval::nWt,/* King weight */king_weight,eval::qWt};
	constexpr static int attacker_score[6] = { king_weight - eval::pWt,king_weight - eval::rWt,king_weight - eval::bWt,king_weight - 
		eval::nWt,king_weight - king_weight, king_weight - eval::qWt };
	int victim_attacker_score[6][6];
};