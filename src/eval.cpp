#include "eval.h"

namespace
{
	int material_difference(piece_type ptype, const board& b)
	{
		bitboard bbw = b.get_board(ptype, color::white);
		bitboard bbb = b.get_board(ptype, color::black);
		int w = ops::count_number_of_set_bits(bbw);
		int b_ = ops::count_number_of_set_bits(bbb);
		return w - b_;
	}
}


eval::eval(const game& g)
{
	const board& b = g.get_board();

	material_score = material_difference(piece_type::bishop, b) * bWt +
		material_difference(piece_type::knight, b) * nWt +
		material_difference(piece_type::rook, b) * rWt +
		material_difference(piece_type::pawn, b) * pWt +
		material_difference(piece_type::queen, b) * qWt;
}

void eval::do_move(const move& m, int c)
{
	material_score += weights[static_cast<int>(m.get_captured_piece_type())] * c;
}

void eval::undo_move(const move& m, int c)
{
	material_score -= weights[static_cast<int>(m.get_captured_piece_type())] * c;
}

int eval::operator()(const game& g)
{
	return material_score;
}


