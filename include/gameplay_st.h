#pragma once
#include "gameplay.h"
#include "game.h"
#include "transposition_table.h"
#include "eval.h"
#include "move_ordering.h"

class gameplay_st : public gameplay_conventional
{
public:
	gameplay_st(float time_mins, color my_turn, const game& g, u64 cap_tt);

protected:
	virtual std::tuple<int, int, int> iterative_deepening(float ms) override;
	virtual std::vector<move> get_pv_(int depth) override;
private:
	TT tt;
	game g;
	move_ordering ordering;
	timer move_timer;
	template<color VColor>
	int negamax(int depth_left, int max_depth, int alpha, int beta, int color, int& searched_nodes);

	template<color VColor>
	int quiesence_search(int depth_left, int max_depth, int alpha, int beta, int c, int& searched_nodes);
};