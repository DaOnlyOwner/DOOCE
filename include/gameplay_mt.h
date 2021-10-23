#pragma once
#include "gameplay.h"
#include "transposition_table.h"
#include "eval.h"
#include <thread>
#include "move_ordering.h"

struct thread_loc
{
	game g;
	eval ev;
	int searched_nodes;
	int score;
	move_ordering ordering;
	bool time_up;
};

class gameplay_mt : public gameplay_conventional
{
public:
	gameplay_mt(float time_mins, color my_turn, const game& g, u64 cap_tt);

protected:
	virtual std::tuple<int, int, int> iterative_deepening(float ms) override;
	virtual std::vector<move> get_pv_(int depth) override;

private:
	template<color VColor>
	int negamax(int depth_left, int max_depth, int alpha, int beta, int color, int& searched_nodes, int idx);

	template<color VColor>
	int quiesence_search(int depth_left, int max_depth, int alpha, int beta, int c, int& searched_nodes, int idx);

	void renew_gamestate();

	void start_threads(int depth);
	void join_threads();

	TT_mt tt;

	std::vector<thread_loc> locals;
	std::vector<std::thread> workers;

};
