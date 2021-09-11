#include "game.h"
#include "bitwise_ops.h"

void game::add_promo_moves(move& m, std::vector<move>& out)
{
	m.type_of_move = move_type::bishop_promo;
	out.push_back(m);
	m.type_of_move = move_type::queen_promo;
	out.push_back(m);
	m.type_of_move = move_type::rook_promo;
	out.push_back(m);
	m.type_of_move = move_type::knight_promo;
	out.push_back(m);
}

void game::make_move(bitboard attacks, uint from, std::vector<move>& out, const mailbox& mb_board, color c)
{
	bitboard cpy = attacks;
	uint idx = 0;
	while (cpy != 0)
	{
		idx += ops::num_trailing_zeros(cpy);
		move m;
		m.from = from;
		m.to = idx;
		m.piece_moved = mb_board.repr[from].type;
		m.piece_captured = { c,mb_board.repr[idx].type };
		if (m.piece_captured.type != piece_type::none)
		{
			m.type_of_move = move_type::captures;
		}

		else m.type_of_move = move_type::quiet;
		out.push_back(m);
		cpy &= cpy - 1;
	}
}
