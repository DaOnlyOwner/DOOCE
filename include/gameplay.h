#pragma once
#include "definitions.h"
#include "game.h"
#include "transposition_table.h"
#include "eval.h"
#include <stdexcept>
#include "timer.h"
#include "move.h"
#include <vector>

class time_up : std::exception
{
	std::string what()
	{
		return "Times up!";
	}
};

struct move_info
{
	int score = 0;
	std::vector<move> principal_variation;
	int depth = 0;
};

class gameplay
{
public:
	gameplay(int time_mins) :time_mins(time_mins) {}
	virtual move_info pick_next_move() = 0;
	virtual void incoming_move(const move& m) = 0; 

private:
	int time_mins; 
};

class gameplay_st : gameplay
{
public:
	gameplay_st(int time_mins, const game& g);
	virtual move_info pick_next_move() override;
	virtual void incoming_move(const move& m) override;

private:
	TT tt;
	game g;
	eval ev;
	template<color VColor>
	int negamax(const timer& t, int depth_left, int alpha, int beta, int color);
	int quiesence_search();
	std::pair<int,int> iterative_deepening(int ms);
	int ply = 0;
};


