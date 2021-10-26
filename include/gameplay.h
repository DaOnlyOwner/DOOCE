#pragma once
#include "definitions.h"
#include <stdexcept>
#include "timer.h"
#include "move.h"
#include <vector>
#include <optional>
#include "game.h"
#include "eval.h"

#define DEPTH_QSEARCH 3


struct move_info
{
	int score = 0;
	std::vector<move> principal_variation;
	int depth = 0;
	int searched_nodes = 0;
};

class time_up : public std::exception
{

};

template<color VColor, typename TTable>
void get_pv_inner(std::vector<move>& out, game& g, const TTable& table, int depth)
{
    if (depth == 0) return;
    const auto& pv_node = table[g.get_hash()];
    if (pv_node.f != move_flag::none)
    {
        move best_move = pv_node.best_move;
        out.push_back(pv_node.best_move);
        g.do_move<VColor>(best_move);
        get_pv_inner<invert_color(VColor)>(out, g, table, depth - 1);
        g.undo_move<VColor>();
    }
}

template<typename TTable>
std::vector<move> get_pv(game& g, const TTable& table, int depth)
{
    std::vector<move> out;
    out.reserve(20);
    if (g.get_game_context().turn == color::white)
        get_pv_inner<color::white, TTable>(out, g, table, depth);
    else get_pv_inner<color::black, TTable>(out, g, table, depth);
    return out;
}

class gameplay
{
public:
	gameplay(float time_mins, color my_turn) :time_mins(time_mins),t(time_mins * 60.f * 1000.f),my_turn(my_turn) {}
	virtual std::optional<move_info> pick_next_move() = 0;
	virtual bool incoming_move(const move& m) = 0; 
	virtual const game& get_game() const = 0;
	virtual game& get_game() = 0;
	virtual ~gameplay() = default;

protected:
	float time_mins;
	timer t;
	color my_turn;
};

class gameplay_conventional : public gameplay
{
public:
	gameplay_conventional(float time_mins, color my_turn, const game& g) :gameplay(time_mins,my_turn), time_mins(time_mins), t(time_mins * 60.f * 1000.f), my_turn(my_turn),front(g),ev(g),move_timer(0) {}
	virtual std::optional<move_info> pick_next_move() override;
	virtual bool incoming_move(const move& m) override;
	virtual const game& get_game() const override;
	virtual game& get_game() override;
	virtual ~gameplay_conventional() override = default;

protected:
	virtual std::tuple<int, int, int> iterative_deepening(float think_time) = 0;
	virtual std::vector<move> get_pv_(int depth) = 0;
	float time_mins;
	timer t;
	color my_turn;
	game front;
	eval ev;
	int ply = 0;
	timer move_timer;
};




