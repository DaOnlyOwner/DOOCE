#pragma once
#include <gameplay.h>
#include <memory>
#include <map>
#include "definitions.h"
#include "GLFW/glfw3.h"
#include "timer.h"
#include <utility>
#include "imgui.h"
#define BOARD_SIZE 640


struct image
{
	int w, h;
	GLuint id;
};

struct point
{
	int x=-1, y=-1;
	bool operator==(const point& lhs) const
	{
		return x == lhs.x && y == lhs.y;
	}

	bool is_valid() const
	{
		return x >= 0 && y >= 0;
	}

	void set_invalid()
	{
		x = -1; y = -1;
	}

};

struct rect
{
	point min, max;
	bool contains_point(const point& p)
	{
		return min.x <= p.x && min.y <= p.y && max.x >= p.x && max.y >= p.y;
	}
};


class frontend
{
public:
	frontend();
	void render();
private:
	constexpr static uint tt_size_exp_default = 20;
	constexpr static float minutes_to_think_default = 5;
	constexpr static char* from_fen_default = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
	constexpr static int sq_size = BOARD_SIZE / 8;

	std::map<piece, image> pieceToImage;
	std::unique_ptr<gameplay> gp;

	void render_board(ImDrawList* dl);
	void render_pieces(ImDrawList* dl);
	void render_gui();

	std::pair<ImVec2, ImVec2> get_min_max(int x, int y, piece pt, ImVec2 offset);

	bool clicked_on_piece();
	void update_click();
	std::pair<point,point> update_let_go();
	void update_drag();
	void update_game(const point& from_,const point& to);

	int tt_size_exponent = tt_size_exp_default;
	float minutes_to_think = minutes_to_think_default;
	std::string from_fen = from_fen_default;
	std::map<piece_type, float> piece_offsets_x = { {piece_type::bishop,15},{piece_type::knight,15},{piece_type::pawn ,25},{piece_type::king , 15},{piece_type::rook , 25},{piece_type::queen , 13 } };
	std::map<piece_type, float> piece_offsets_y = { {piece_type::bishop,33},{piece_type::knight,33},{piece_type::pawn ,39},{piece_type::king , 34},{piece_type::rook , 39},{piece_type::queen , 37 } };

	point from{};
	point delta = { 0,0 };
	point last_mp = { 0,0 };

	timer fen_error_timer;
	timer tt_error_timer;
	timer mtt_error_timer;
	timer move_invalid_timer;

};