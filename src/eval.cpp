#include "eval.h"

namespace
{
	i64 material_difference(piece_type ptype, const board& b)
	{
		bitboard bbw = b.get_board(ptype, color::white);
		bitboard bbb = b.get_board(ptype, color::black);
		int w = ops::count_number_of_set_bits(bbw);
		int b_ = ops::count_number_of_set_bits(bbb);
		return bbw - bbb;
	}
}


i64 eval::operator()(const game& g)
{
	i64 material_score;
	
	const board& b = g.get_board();

	material_score = material_difference(piece_type::bishop, b)* bWt +
		material_difference(piece_type::knight, b) * nWt +
		material_difference(piece_type::rook, b) * rWt +
		material_difference(piece_type::pawn, b) * pWt +
		material_difference(piece_type::queen, b) * qWt +
		material_difference(piece_type::king, b) * kWt;
}