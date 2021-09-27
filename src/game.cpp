#include "game.h"
#include "bitwise_ops.h"
#include "misc_tools.h"

// init game in start formation
game::game()
	: b(), start_info_white({ false,false,false }), start_info_black({ false,false,false }),
	    start_color(color::white), start_en_passantable_pawn(0) {}

// init game in a specific formation + context
game::game(board b, game_context context)
/*: b(start_board), start_info_white(start_info_white), start_info_black(start_info_black),
	start_color(start_color), start_en_passantable_pawn(start_en_passantable_pawn) {}*/
{
	// TODO: add proper initialization here ...
}

std::optional<piece_type> game::determine_capturing(const board_info& info, bitboard set_bit)
{
	bitboard not_set_bit = ~set_bit;
	std::optional<piece_type> ptype;
	if ((info.enemy_pawns & not_set_bit) != info.enemy_pawns) ptype = piece_type::pawn;
	else if ((info.enemy_bishops & not_set_bit) != info.enemy_bishops) ptype = piece_type::bishop;
	else if ((info.enemy_knights & not_set_bit) != info.enemy_knights) ptype = piece_type::knight;
	else if ((info.enemy_rooks & not_set_bit) != info.enemy_rooks) ptype = piece_type::rook;
	else if ((info.enemy_queens & not_set_bit) != info.enemy_queens) ptype = piece_type::queen;
	return ptype;
}

void game::push_promo_moves(std::vector<move>& out, move& m)
{
	m.set_promo_piece_type(piece_type::knight);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::queen);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::rook);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::bishop);
	out.push_back(m);
}

void game::init_knight_attacks()
{
	for (uint i = 0; i < 64; i++)
	{
		bitboard_constr b(0);
		auto [x, y] = ops::from_idx(i);

		std::pair<int, int> possible_pos[8];

		possible_pos[0] = { x - 1, y + 2 }; // top_left
		possible_pos[1] = { x + 1, y + 2 }; // top_right
		possible_pos[2] = { x - 2, y + 1 }; // midtop_left
		possible_pos[3] = { x + 2, y + 1 }; // midtop_right

		possible_pos[4] = { x - 2, y - 1 }; // midbot_left
		possible_pos[5] = { x + 2, y - 1 }; // midbot_right
		possible_pos[6] = { x - 1, y - 2 }; // bot_left
		possible_pos[7] = { x + 1, y - 2 }; // bot_right

		for (uint pos = 0; pos < 8; pos++)
		{
			auto [x, y] = possible_pos[pos];
			if (ops::contains(x, y)) b.set(ops::to_idx(x, y), true);
		}

		knight_attacks[i] = b.to_ullong();
	}
}

void game::init_hq_masks()
{
	for (uint x = 0; x < 8; x++)
	{
		for (uint y = 0; y < 8; y++)
		{
			uint idx = ops::to_idx(x, y);
			hq_mask mask;

			// Init diag and anti diag.
			auto diag = ops::get_diag(x, y);
			auto antidiag = ops::get_antidiag(x, y);
			auto rank = ops::get_rank(x, y);
			auto file = ops::get_file(x, y);
			mask.diagEx = diag.to_ullong();
			mask.antidiagEx = antidiag.to_ullong();
			mask.fileEx = file.to_ullong();
			bitboard_constr mask_constr(0);
			mask_constr.set(idx, true);
			mask.mask = mask_constr.to_ullong();
			hq_masks[idx] = mask;
		}
	}
}

void game::init_king_attacks()
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			bitboard_constr bc(0);
			uint idx = ops::to_idx(x, y);

			for (int j = -1; j <= 1; j++)
			{
				for (int k = -1; k <= 1; k++)
				{
					int x_off = x + j;
					int y_off = y + k;
					if (ops::contains(x_off, y_off) && !(x_off == x && y_off == y))
					{
						uint idx_inner = ops::to_idx((uint)x_off, (uint)y_off);
						bc.set(idx_inner, true);
					}
				}
			}
			king_attacks[idx] = bc.to_ullong();
		}
	}
}

void game::init_all()
{
	init_king_attacks();
	init_hq_masks();
	init_knight_attacks();
}