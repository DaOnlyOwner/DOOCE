#pragma once
#include "definitions.h"
#include "game.h"
#include "transposition_table.h"
#include "eval.h"
#include <stdexcept>
#include "timer.h"
#include "move.h"
#include <vector>

struct move_info
{
	int score = 0;
	std::vector<move> principal_variation;
	int depth = 0;
};

class gameplay
{
public:
	gameplay(float time_mins, color my_turn) :time_mins(time_mins),t(time_mins * 60.f * 1000.f),my_turn(my_turn) {}
	virtual move_info pick_next_move() = 0;
	virtual bool incoming_move(const move& m) = 0; 
	virtual const game& get_game() const = 0;
	virtual game& get_game() = 0;
	void start() { t.restart(); }

protected:
	float time_mins;
	timer t;
	color my_turn;
};

class gameplay_st : public gameplay
{
public:
	gameplay_st(float time_mins, color my_turn, const game& g, u64 cap_tt);
	virtual move_info pick_next_move() override;
	virtual bool incoming_move(const move& m) override;
	virtual const game& get_game() const override;
	virtual game& get_game() override;

private:
	TT tt;
	game g;
	eval ev;
	template<color VColor>
	int negamax(const timer& t, int depth_left, int alpha, int beta, int color);
	int quiesence_search();
	std::pair<int,int> iterative_deepening(float ms);
	int ply = 0;
};


