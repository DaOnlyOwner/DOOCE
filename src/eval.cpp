#include "eval.h"

#define FD_TEMPLATE_FUNC(name)\
template void eval::name<color::white>(const move& m, const game& g);\
template void eval::name<color::black>(const move& m, const game& g)

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

	int count_material(piece_type ptype, const board& b)
	{
		bitboard bb = b.get_board(ptype, color::white);
		return ops::count_number_of_set_bits(bb);
	}
}


eval::eval(const game& g)
{
	const board& b = g.get_board();

	for (int i = 0; i < 2; i++)
	{
		material[i] = count_material(piece_type::bishop, b) * bWt +
			count_material(piece_type::knight, b) * nWt +
			count_material(piece_type::rook, b) * rWt +
			count_material(piece_type::pawn, b) * pWt +
			count_material(piece_type::queen, b) * qWt;
	}
}

template<color VColor>
void eval::do_move(const move& m, const game& g)
{
	constexpr static int c = static_cast<int>(VColor);
	int mpiece = static_cast<int>(m.get_moved_piece_type());
	int cpiece = static_cast<int>(m.get_captured_piece_type());
	// apply the new position of m:
	int idx_from = 0;
	int idx_to = 0;
	if constexpr (VColor == color::white)
	{
		auto [x_from, y_from] = ops::from_idx(m.get_from_as_idx());
		y_from = 7 - y_from;
		idx_from = ops::to_idx(x_from, y_from);
		auto [x_to, y_to] = ops::from_idx(m.get_to_as_idx());
		y_to = 7 - y_to;
		idx_to = ops::to_idx(x_to, y_to);
	}
	else
	{
		idx_from = m.get_from_as_idx();
		idx_to = m.get_to_as_idx();
	}
	
	// moved piece from
	pt_score_opening[c] -= piece_tables_opening[mpiece][idx_from];

	// moved piece to
	pt_score_opening[c] += piece_tables_opening[mpiece][idx_to];
	
	// captured piece doesn't contribute to the pt score anymore.
	pt_score_opening[1 - c] -= piece_tables_opening[cpiece][idx_to];

	material[c] += weights[cpiece];
}

template<color VColor>
void eval::undo_move(const move& m, const game& g)
{
	constexpr static int c = static_cast<int>(VColor);
	int mpiece = static_cast<int>(m.get_moved_piece_type());
	int cpiece = static_cast<int>(m.get_captured_piece_type());
	// apply the new position of m:
	int idx_from = 0;
	int idx_to = 0;
	if constexpr (VColor == color::white)
	{
		auto [x_from, y_from] = ops::from_idx(m.get_from_as_idx());
		y_from = 7 - y_from;
		idx_from = ops::to_idx(x_from, y_from);
		auto [x_to, y_to] = ops::from_idx(m.get_to_as_idx());
		y_to = 7 - y_to;
		idx_to = ops::to_idx(x_to, y_to);
	}
	else
	{
		idx_from = m.get_from_as_idx();
		idx_to = m.get_to_as_idx();
	}

	// moved piece from
	pt_score_opening[c] += piece_tables_opening[mpiece][idx_from];

	// moved piece to
	pt_score_opening[c] -= piece_tables_opening[mpiece][idx_to];

	// put captured piece back
	pt_score_opening[1 - c] += piece_tables_opening[cpiece][idx_to];

	material[c] -= weights[cpiece];
}

int eval::operator()(const game& g, int c)
{
	int c_white = static_cast<int>(color::white);
	int c_black = static_cast<int>(color::black);
	int material_diff = material[c_white] - material[c_black];
	int pt_diff = pt_score_opening[c_white] - pt_score_opening[c_black];
	return (material_diff + pt_diff) * c;
}

FD_TEMPLATE_FUNC(do_move);
FD_TEMPLATE_FUNC(undo_move);
