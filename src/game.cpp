#include "game.h"
#include "attack_bitboards.h"

template<color VColor>
void game::gen_attack_moves_from_piece(std::vector<move>& out, bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint))
{
	uint from = 0;
	bitboard attacks = 0ull;
	bitboard not_own_color_occ = b.get_board_of_side_not<VColor>();

	move m{};
	m.set_moved_piece_type(ptype);
	while (piece_occ != 0ull)
	{
		from = ops::num_trailing_zeros(piece_occ);
		bitboard attacks = fn(b, from) & not_own_color_occ;
		m.set_from(from);
		while (attacks != 0)
		{
			uint to = ops::num_trailing_zeros(attacks);
			m.set_to(to);
			auto captured = determine_capturing(VColor, ops::set_nth_bit(to));
			m.set_captured_piece_type(captured);
			move_type mtype = captured.has_value() ? move_type::captures : move_type::quiet;
			m.set_move_type(mtype);
			add_when_legal<VColor>(out, m);
			ops::pop_lsb(attacks);
		}
		ops::pop_lsb(piece_occ);
	}
}

template<color VColor>
void game::gen_attack_moves_from_pawns(std::vector<move>& out)
{
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	uint idx = 0;
	while (own_pawns != 0ull)
	{
		idx = ops::num_trailing_zeros(own_pawns);
		bitboard only_pawn = ops::set_nth_bit(idx);
		auto attack_left = gen::attack_pawns_left<VColor>(only_pawn, b);
		auto attack_right = gen::attack_pawns_right<VColor>(only_pawn, b);
		move m{};
		m.set_from(idx);
		m.set_moved_piece_type(piece_type::pawn);
		if (attack_left != 0ull)
			gen_attack_moves_from_pawns_inner<VColor>(m, attack_left, out);

		if (attack_right != 0ull)
			gen_attack_moves_from_pawns_inner<VColor>(m, attack_right, out);

		ops::pop_lsb(own_pawns);
	}
}

template<color VColor>
void game::gen_attack_moves_from_pawns_inner(move& m, bitboard attack, std::vector<move>& out)
{
	m.set_to(ops::num_trailing_zeros(attack));
	auto captured = determine_capturing(VColor, attack);
	bool promo = determine_promo<VColor>(attack);
	move_type mtype;

	m.set_captured_piece_type(captured);
	if (captured.has_value() && promo)
	{
		m.set_move_type(move_type::promo_captures);
		push_promo_moves<VColor>(out, m);
	}
	else if (captured.has_value())
	{
		m.set_move_type(move_type::captures);
		add_when_legal<VColor>(out, m);
	}
	else
	{
		printf("This text should never be printed. Something went wrong in gen_attack_moves_from_pawns_inner()!\n");
	}
	// This should never happen
	/*else if (promo)
	{
		printf("This text should never be printed. Something went wrong in gen_attack_moves_from_pawns_inner()!\n");
		m.set_move_type(move_type::promo);
		push_promo_moves<VColor>(out, m);
	}
	else
	{
		printf("This text should never be printed. Something went wrong in gen_attack_moves_from_pawns_inner()!\n");
	}*/
}

// This method is basically only used for castling.

template<color VColor>
bitboard game::gen_attack_bb_except_en_passant() const
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

bitboard game::gen_attack_bb_from_piece(bitboard piece_occ, bitboard(*fn)(const board&, uint)) const
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

std::optional<piece_type> game::determine_capturing(color c, bitboard set_bit) const
{
	color ec = invert_color(c);
	bitboard not_set_bit = ~set_bit;

	bitboard p = b.get_board(piece_type::pawn, ec);
	bitboard b_ = b.get_board(piece_type::bishop, ec);
	bitboard n = b.get_board(piece_type::knight, ec);
	bitboard r = b.get_board(piece_type::rook, ec);
	bitboard q = b.get_board(piece_type::queen, ec);

	if ((p & not_set_bit) != p) return { piece_type::pawn };
	else if ((b_ & not_set_bit) != b_) return piece_type::bishop;
	else if ((n & not_set_bit) != n) return piece_type::knight;
	else if ((r & not_set_bit) != r) return piece_type::rook;
	else if ((q & not_set_bit) != q) return piece_type::queen;
	else return {};
}

template<color VColor>
bool game::determine_promo(bitboard set_bit) const
{
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
bool game::is_in_check() const
{
	bitboard attacks = gen_attack_bb_except_en_passant<invert_color(VColor)>();
	return ((b.get_board(piece_type::king, VColor) & attacks) > 0);
}

template<color VColor>
void game::gen_move_pawn_push(std::vector<move>& out)
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
		bool promo = determine_promo<VColor>(single);
		m.set_moved_piece_type(piece_type::pawn);
		if (promo)
		{
			m.set_move_type(move_type::promo);
			push_promo_moves<VColor>(out, m);
		}
		else
		{
			m.set_move_type(move_type::pawn_single);
			add_when_legal<VColor>(out, m);
		}
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
	castling_info ginfo = gc.get_game_info(VColor);
	auto [can_castle_kingside, can_castle_queenside] = can_castle<VColor>();
	move_type mtype;
	piece_type moved = piece_type::king;

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

	// Pawn attacks
	printf("%s\n",b.pretty().c_str());
	gen_attack_moves_from_pawns<VColor>(out);

	// Pieces
	gen_attack_moves_from_piece<VColor>(out, b.get_board(piece_type::bishop, VColor), piece_type::bishop, &gen::attacks_bishop);
	gen_attack_moves_from_piece<VColor>(out, b.get_board(piece_type::rook, VColor), piece_type::rook, &gen::attacks_rook);
	gen_attack_moves_from_piece<VColor>(out, b.get_board(piece_type::queen, VColor), piece_type::queen, &gen::attacks_queen);
	gen_attack_moves_from_piece<VColor>(out, b.get_board(piece_type::knight, VColor), piece_type::knight, &gen::attacks_knight);
	gen_attack_moves_from_piece<VColor>(out, b.get_board(piece_type::king, VColor),piece_type::king, &gen::attacks_king);

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
void game::do_move(const move& m)
{
	move_list.push_back(std::make_pair(m, gc));
	constexpr square rook_queenside = VColor == color::white ? square::a1 : square::a8;
	constexpr square rook_kingside = VColor == color::white ? square::h1 : square::h8;

	constexpr square rook_queenside_opp = VColor == color::white ? square::a8 : square::a1;
	constexpr square rook_kingside_opp = VColor == color::white ? square::h8 : square::h1;

	castling_info& gi = gc.get_game_info(VColor);
	gi.has_moved_king = (m.get_moved_piece_type() == piece_type::king || gi.has_moved_king);
	// Here and next stmt not accounting for castling, but that doesn't matter because then has_moved_king is set to true.
	gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook
		&& (idx_to_sq(m.get_from_as_idx()) == rook_kingside)) || gi.has_moved_kingside_rook);
	gi.has_moved_queenside_rook = ((m.get_moved_piece_type() == piece_type::rook
		&& (idx_to_sq(m.get_from_as_idx()) == rook_queenside)) || gi.has_moved_queenside_rook);

	if (m.get_move_type() == move_type::captures || m.get_move_type() == move_type::promo_captures)
	{
		castling_info& gi_opp = gc.get_game_info(invert_color(VColor));
		// The rook has been captured without it moving -> set the flag to false
		gi_opp.has_moved_kingside_rook = (m.get_captured_piece_type() == piece_type::rook
			&& idx_to_sq(m.get_to_as_idx()) == rook_kingside_opp) || gi_opp.has_moved_kingside_rook; 
		gi_opp.has_moved_queenside_rook = (m.get_captured_piece_type() == piece_type::rook
			&& idx_to_sq(m.get_to_as_idx()) == rook_queenside_opp) || gi_opp.has_moved_queenside_rook;
	}

	gc.half_move_clock++;
	if (m.get_move_type() == move_type::pawn_double)
	{
		gc.en_passantable_pawn = m.get_to();
	}
	else gc.en_passantable_pawn = 0ULL;
	if (m.get_moved_piece_type() == piece_type::pawn || m.get_move_type() == move_type::captures)
		gc.half_move_clock = 0;

	if constexpr (VColor == color::black)
		gc.fullmoves++;

	gc.turn = invert_color(gc.turn);
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
std::pair<bool,bool> game::can_castle() const
{
	constexpr color opp = invert_color(VColor);
	auto& ginfo = gc.get_game_info(VColor);
	bitboard attacks = gen_attack_bb_except_en_passant<opp>();
	attacks |= gen::attack_pawns_castle<opp>(b.get_board(piece_type::pawn,opp)); // This always adds the attacked fields from pawns.  
	bool can_castle_kingside = gen::can_castle_kingside<VColor>(b, attacks)
		&& !ginfo.has_moved_king && !ginfo.has_moved_kingside_rook;
	bool can_castle_queenside = gen::can_castle_queenside<VColor>(b, attacks)
		&& !ginfo.has_moved_king && !ginfo.has_moved_queenside_rook;
	return std::make_pair(can_castle_kingside,can_castle_queenside);
}

game::game() : b(), gc()
{
	move_list.reserve(9000);
}

game::game(const board& b, const game_context& gc) : b(b),gc(gc)
{
	move_list.reserve(9000);
}

game::game(const std::string& board_repr, const game_context& gc) : b(board_repr), gc(gc)
{}


const game_context& game::get_game_context() const
{
	return gc;
}

const board& game::get_board() const
{
	return b;
}

template bitboard game::gen_attack_bb_except_en_passant<color::white>() const;
template bitboard game::gen_attack_bb_except_en_passant<color::black>() const;

template bool game::determine_promo<color::white>(bitboard set_bit) const;
template bool game::determine_promo<color::black>(bitboard set_bit) const;

template bool game::add_when_legal<color::white>(std::vector<move>& out, const move& m);
template bool game::add_when_legal<color::black>(std::vector<move>& out, const move& m);

template bool game::is_in_check<color::white>() const;
template bool game::is_in_check<color::black>() const;

template void game::push_promo_moves<color::white>(std::vector<move>& out, move& m);
template void game::push_promo_moves<color::black>(std::vector<move>& out, move& m);

template void game::gen_attack_moves_from_pawns<color::white>(std::vector<move>& out);
template void game::gen_attack_moves_from_pawns<color::black>(std::vector<move>& out);

template void game::gen_attack_moves_from_piece<color::white>
(std::vector<move>& out, bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));
template void game::gen_attack_moves_from_piece<color::black>
(std::vector<move>& out, bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));

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

template std::pair<bool,bool> game::can_castle<color::white>() const;
template std::pair<bool, bool> game::can_castle<color::black>() const;
