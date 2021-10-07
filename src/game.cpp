#include "game.h"
#include "attack_bitboards.h"

template<color VColor>
inline bitboard game::gen_attack_list()
{
	alist.clear();
	bitboard attacks =
		gen_attack_list_from_piece<VColor>(b.get_board(piece_type::bishop, VColor), piece_type::bishop, &gen::attacks_bishop)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::rook, VColor), piece_type::rook, &gen::attacks_rook)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::queen, VColor), piece_type::queen, &gen::attacks_queen)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::knight, VColor), piece_type::knight, &gen::attacks_knight)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::king, VColor), piece_type::king, &gen::attacks_king)
		| gen_attack_list_from_pawns<VColor>();

	// This is already ANDed by not_own_color_occ (in gen_attack_info_from_piece_type),
	// so I don't need to do that here again.
	return attacks;
}

template<color VColor>
bitboard game::gen_attack_list_from_pawns()
{
	bitboard attacks = 0ull;
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	uint idx = 0;
	while (own_pawns != 0ull)
	{
		idx = ops::num_trailing_zeros(own_pawns);
		bitboard only_pawn = ops::set_nth_bit(idx);
		attack_info ainfo;
		ainfo.attacks = gen::attack_pawns_left<VColor>(only_pawn,b);
		ainfo.attacks |= gen::attack_pawns_right<VColor>(only_pawn,b);
		if (ainfo.attacks != 0ull)
		{
			ainfo.from = idx;
			ainfo.ptype = piece_type::pawn;
			alist.add(ainfo);
			attacks |= ainfo.attacks;
		}
		ops::pop_lsb(own_pawns);
	}
	return attacks;
}

template<color VColor>
inline bitboard game::gen_attack_list_from_piece(bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint))
{
	uint idx = 0;
	bitboard attacks = 0ull;
	bitboard not_own_color_occ = b.get_board_of_side_not<VColor>();

	while (piece_occ != 0ull)
	{
		idx = ops::num_trailing_zeros(piece_occ);
		attack_info ainfo;
		ainfo.attacks = fn(b, idx) & not_own_color_occ;
		if (ainfo.attacks != 0ull)
		{
			ainfo.from = idx;
			ainfo.ptype = ptype;
			alist.add(ainfo);
			attacks |= ainfo.attacks;
		}
		ops::pop_lsb(piece_occ);
	}
	return attacks;
}

// This method is basically only used for castling.

template<color VColor>
bitboard game::gen_attack_bb_except_en_passant()
{
	bitboard attacks =
		gen_attack_bb_from_piece(b.get_board(piece_type::bishop, VColor), &gen::attacks_bishop) |
		gen_attack_bb_from_piece(b.get_board(piece_type::rook, VColor), &gen::attacks_rook) |
		gen_attack_bb_from_piece(b.get_board(piece_type::queen, VColor), &gen::attacks_queen) |
		gen_attack_bb_from_piece(b.get_board(piece_type::knight, VColor), &gen::attacks_knight) |
		gen_attack_bb_from_piece(b.get_board(piece_type::king, VColor), &gen::attacks_king);

	attacks |= gen::attack_pawns_left<VColor>(b.get_board(piece_type::pawn,VColor),b);
	attacks |= gen::attack_pawns_right<VColor>(b.get_board(piece_type::pawn, VColor), b);
	return attacks & b.get_board_of_side_not<VColor>();
}

inline bitboard game::gen_attack_bb_from_piece(bitboard piece_occ, bitboard(*fn)(const board&, uint))
{
	uint idx = 0;
	bitboard attacks = 0ULL;

	// loop until all pieces are handled
	while (piece_occ != 0ULL)
	{
		idx = ops::num_trailing_zeros(piece_occ);
		attacks |= fn(b, idx);

		ops::pop_lsb(piece_occ);
	}

	return attacks;
}

std::optional<piece_type> game::determine_capturing(color c, bitboard set_bit)
{
	bitboard not_set_bit = ~set_bit;

	bitboard p = b.get_board(piece_type::pawn, c);
	bitboard b_ = b.get_board(piece_type::bishop, c);
	bitboard n = b.get_board(piece_type::knight, c);
	bitboard r = b.get_board(piece_type::rook, c);
	bitboard q = b.get_board(piece_type::queen,c);

	if ((p & not_set_bit) != p) return { piece_type::pawn };
	else if ((b_ & not_set_bit) != b_) return piece_type::bishop;
	else if ((n & not_set_bit) != n) return piece_type::knight;
	else if ((r & not_set_bit) != r) return piece_type::rook;
	else if ((q & not_set_bit) != q) return piece_type::queen;
	else return {};
}

template<color VColor>
bool game::determine_promo(piece_type ptype, bitboard set_bit)
{
	if (ptype != piece_type::pawn) return false;
	if constexpr (VColor == color::white)
		return ops::has_bit_set_on_rank(set_bit, 8);
	else return ops::has_bit_set_on_rank(set_bit, 1);
}

template<color VColor>
bool game::add_when_legal(std::vector<move>& out, const move& m)
{
	bool legal = false;
	b.do_move<VColor>(m);
	if (!is_in_check<VColor>())
	{
		out.push_back(m);
		legal = true;
	}
	b.undo_move<VColor>(m);
	return legal;
}

template<color VColor>
bool game::is_in_check()
{
	bitboard attacks = gen_attack_bb_except_en_passant<invert_color(VColor)>();
	return ((b.get_board(piece_type::king, VColor) & attacks) > 0);
}

template<color VColor>
inline void game::gen_move_pawn_push(std::vector<move>& out)
{
	bitboard cpy = b.get_board(piece_type::pawn, VColor);
	uint idx = 0;
	while (cpy != 0)
	{
		idx = ops::num_trailing_zeros(cpy);
		bitboard set_bit = ops::set_nth_bit(idx);
		move m{};
		m.set_from(idx);
		bitboard single = gen::move_pawns_single<VColor>(set_bit, b);
		if (single == 0ULL)
		{
			ops::pop_lsb(cpy);
			continue;
		}
		uint to = ops::num_trailing_zeros(single);
		m.set_to(to);
		m.set_moved_piece_type(piece_type::pawn);
		m.set_move_type(move_type::pawn_single);
		add_when_legal<VColor>(out, m);
		bitboard dbl = gen::move_pawns_dbl<VColor>(set_bit, b);
		if (dbl == 0ULL)
		{
			ops::pop_lsb(cpy);
			continue;
		}
		to = ops::num_trailing_zeros(dbl);
		m.set_to(to);
		m.set_move_type(move_type::pawn_double);
		add_when_legal<VColor>(out, m);
		ops::pop_lsb(cpy);
	}
}

template<color VColor>
void game::gen_move_en_passant(std::vector<move>& out, en_passant_fn efn, shift_func shift_white, shift_func shift_black)
{
	bitboard attack = efn(b, gc.en_passantable_pawn);
	if (attack != 0)
	{
		move m{};
		m.set_captured_piece_type(piece_type::pawn);
		m.set_moved_piece_type(piece_type::pawn);
		m.set_move_type(move_type::en_passant);
		if constexpr (VColor == color::white)
			m.set_from(ops::num_trailing_zeros(shift_white(attack)));
		else m.set_from(ops::num_trailing_zeros(shift_black(attack)));
		m.set_to(ops::num_trailing_zeros(attack));
		add_when_legal<VColor>(out, m);
	}
}

template<color VColor>
void game::gen_move_castling(std::vector<move>& out)
{
	constexpr color ecolor = invert_color(VColor);
	game_info ginfo = gc.get_game_info(VColor);
	auto [can_castle_kingside, can_castle_queenside] = can_castle<VColor>();
	move_type mtype;
	piece_type moved = (piece_type::king);

	if (can_castle_kingside)
	{
		mtype = (move_type::king_castle);
		move m(0, 0, moved, {}, {}, mtype);
		out.push_back(m);
	}
	if (can_castle_queenside)
	{
		mtype = (move_type::queen_castle);
		move m(0, 0, moved, {}, {}, mtype);
		out.push_back(m);
	}
}

template<color VColor>
std::vector<move> game::legal_moves()
{
	std::vector<move> out;
	out.reserve(250);

	// Attack pattern from previous is_king_in_check test
	for (int i = 0; i < alist.size; i++)
	{
		const attack_info& ainfo = alist.ainfo[i];
		gen_legal_moves_from_attack_list<VColor>(ainfo, out);
	}

	// en_passant
	gen_move_en_passant<VColor>(out,
		&gen::en_passant_left<VColor>, &ops::so_ea, &ops::no_ea);
	gen_move_en_passant<VColor>(out,
		&gen::en_passant_right<VColor>, &ops::so_we, &ops::no_we);

	// pawn push
	game::gen_move_pawn_push<VColor>(out);

	// castling
	game::gen_move_castling<VColor>(out);
	return out;
}

template<color VColor>
void game::push_promo_moves(std::vector<move>& out, move& m)
{
	m.set_promo_piece_type(piece_type::knight);
	bool legal = add_when_legal<VColor>(out, m);
	if (!legal) return;
	out.push_back(m);
	m.set_promo_piece_type(piece_type::queen);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::rook);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::bishop);
	out.push_back(m);
}

template<color VColor>
inline void game::gen_legal_moves_from_attack_list(const attack_info& ainfo, std::vector<move>& out)
{
	bitboard cpy = ainfo.attacks;
	uint idx = 0;
	while (cpy != 0)
	{
		move m{};
		idx = ops::num_trailing_zeros(cpy);
		bitboard set_bit = ops::set_nth_bit(idx);
		auto captured = determine_capturing(VColor, set_bit);
		m.set_from(ainfo.from);
		m.set_to(idx);
		m.set_moved_piece_type(ainfo.ptype);
		m.set_captured_piece_type(captured);
		bool promo = determine_promo<VColor>(ainfo.ptype, set_bit);
		if (promo)
		{
			move_type mtype = captured.has_value() ? move_type::promo_captures : move_type::promo;
			m.set_move_type(mtype);
			push_promo_moves<VColor>(out, m);
		}

		else
		{
			move_type mtype = captured.has_value() ? move_type::captures : move_type::quiet;
			m.set_move_type(mtype);
			add_when_legal<VColor>(out, m);
		}
		ops::pop_lsb(cpy);
	}
}

template<color VColor>
void game::do_move(const move& m)
{

	constexpr square rook_queenside = VColor == color::white ? square::a1 : square::a8;
	constexpr square rook_kingside = VColor == color::white ? square::h1 : square::h8;
	game_info& gi = gc.get_game_info(VColor);
	gi.has_moved_king = (m.get_moved_piece_type() == piece_type::king || gi.has_moved_king);
	// Here and next stmt not accounting for castling, but that doesn't matter because then has_moved_king is set to true.
	gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook
		&& (idx_to_sq(m.get_from_as_idx()) == rook_kingside)) || gi.has_moved_kingside_rook);
	gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook
		&& (idx_to_sq(m.get_from_as_idx()) == rook_queenside)) || gi.has_moved_queenside_rook);
	if (m.get_move_type() == move_type::pawn_double)
		gc.en_passantable_pawn = m.get_to();

	gc.turn = invert_color(gc.turn);
	move_list.push_back(std::make_pair(m, gc));
	b.do_move<VColor>(m);
}

template<color VColor>
void game::undo_move()
{
	auto& [m,old_gc] = move_list.back();
	move_list.pop_back();
	gc = old_gc; 
	b.undo_move<VColor>(m);
}

template<color VColor>
game::castle_info game::can_castle()
{
	bitboard attacks = gen_attack_bb_except_en_passant<ecolor>();
	bool can_castle_kingside = gen::can_castle_kingside<VColor>(b, attacks)
		&& !ginfo.has_moved_king && !ginfo.has_moved_kingside_rook;
	bool can_castle_queenside = gen::can_castle_queenside<VColor>(b, attacks)
		&& !ginfo.has_moved_king && !ginfo.has_moved_queenside_rook;
	castle_info ci;
	ci.kingside = can_castle_kingside;
	ci.queenside = can_castle_queenside;
	return ci;
}

game::game() : b(), gc()
{
	move_list.reserve(9000);
}

game::game(const board& b, const game_context& gc) : b(b),gc(gc)
{
	move_list.reserve(9000);
}

const game_context& game::get_game_context() const
{
	return gc;
}

const board& game::get_board() const
{
	return b;
}

template bitboard game::gen_attack_bb_except_en_passant<color::white>();
template bitboard game::gen_attack_bb_except_en_passant<color::black>();

template bitboard game::gen_attack_list_from_piece<color::white>(bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));
template bitboard game::gen_attack_list_from_piece<color::black>(bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));

template bitboard game::gen_attack_list_from_pawns<color::white>();
template bitboard game::gen_attack_list_from_pawns<color::black>();

template bitboard game::gen_attack_list<color::white>();
template bitboard game::gen_attack_list<color::black>();

template bool game::determine_promo<color::white>(piece_type ptype,bitboard set_bit);
template bool game::determine_promo<color::black>(piece_type ptype,bitboard set_bit);

template bool game::add_when_legal<color::white>(std::vector<move>& out, const move& m);
template bool game::add_when_legal<color::black>(std::vector<move>& out, const move& m);

template bool game::is_in_check<color::white>();
template bool game::is_in_check<color::black>();

template void game::push_promo_moves<color::white>(std::vector<move>& out, move& m);
template void game::push_promo_moves<color::black>(std::vector<move>& out, move& m);

template void game::gen_legal_moves_from_attack_list<color::white>(const attack_info& ainfo, std::vector<move>& out);
template void game::gen_legal_moves_from_attack_list<color::black>(const attack_info& ainfo, std::vector<move>& out);

template void game::gen_move_pawn_push<color::white>(std::vector<move>& out);
template void game::gen_move_pawn_push<color::black>(std::vector<move>& out);

template void game::gen_move_en_passant<color::white>(std::vector<move>& out, en_passant_fn, shift_func, shift_func);
template void game::gen_move_en_passant<color::black>(std::vector<move>& out, en_passant_fn, shift_func, shift_func);

template void game::gen_move_castling<color::white>(std::vector<move>& out);
template void game::gen_move_castling<color::black>(std::vector<move>& out);

template std::vector<move> game::legal_moves<color::white>();
template std::vector<move> game::legal_moves<color::black>();

template void game::do_move<color::white>(const move& m);
template void game::do_move<color::black>(const move& m);

template void game::undo_move<color::white>();
template void game::undo_move<color::black>();

template game::castle_info game::can_castle<color::white>();
template game::castle_info game::can_castle<color::black>();

//// init game in start formation
//game::game()
//	: b(), start_info_white({ false,false,false }), start_info_black({ false,false,false }),
//	    start_color(color::white), start_en_passantable_pawn(0) {}
//
//// init game in a specific formation + context
//game::game(board b, game_context context)
///*: b(start_board), start_info_white(start_info_white), start_info_black(start_info_black),
//	start_color(start_color), start_en_passantable_pawn(start_en_passantable_pawn) {}*/
//{
//	// TODO: add proper initialization here ...
//}
//
//


