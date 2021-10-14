#include "game.h"
#include "attack_bitboards.h"

#define FD_ATTACK_TEMPLATE_FUNCTION(rt, name, ...) \
template rt game::##name##<color::white,true>( __VA_ARGS__ ); \
template rt game::##name##<color::black, true>(__VA_ARGS__);\
template rt game::##name##<color::white, false>(__VA_ARGS__);\
template rt game::##name##<color::black, false>(__VA_ARGS__)

#define FD_CONST_TEMPLATE_FUNCTION(rt, name, ...) \
template rt game::##name##<color::white>(  __VA_ARGS__ ) const;\
template rt game::##name##<color::black>(  __VA_ARGS__ ) const

#define FD_TEMPLATE_FUNCTION(rt, name, ...) \
template rt game::##name##<color::white>(  __VA_ARGS__ );\
template rt game::##name##<color::black>(  __VA_ARGS__ ) 


bool game::init = false;

template<color VColor, bool VOnlyCaptures>
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

			// If we only want to generate capturing moves than we test if there is a captured piece and add it when it's legal.
			// Otherwise we just add the move regardless wether it is a capture or not.
			if constexpr (VOnlyCaptures)
			{
				if (captured.has_value())
					add_when_legal<VColor>(out, m);
				ops::pop_lsb(attacks);
				continue;
			}
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
	castling_info ginfo = gc.get_castling_info(VColor);
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

template<color VColor, bool VOnlyCaptures>
std::vector<move> game::legal_moves()
{
	std::vector<move> out;
	out.reserve(250);

	// Pawn attacks
	//printf("%s\n",b.pretty().c_str());
	gen_attack_moves_from_pawns<VColor>(out);

	// Pieces
	gen_attack_moves_from_piece<VColor, VOnlyCaptures>(out, b.get_board(piece_type::bishop, VColor), piece_type::bishop, &gen::attacks_bishop);
	gen_attack_moves_from_piece<VColor, VOnlyCaptures>(out, b.get_board(piece_type::rook, VColor), piece_type::rook, &gen::attacks_rook);
	gen_attack_moves_from_piece<VColor, VOnlyCaptures>(out, b.get_board(piece_type::queen, VColor), piece_type::queen, &gen::attacks_queen);
	gen_attack_moves_from_piece<VColor, VOnlyCaptures>(out, b.get_board(piece_type::knight, VColor), piece_type::knight, &gen::attacks_knight);
	gen_attack_moves_from_piece<VColor, VOnlyCaptures>(out, b.get_board(piece_type::king, VColor),piece_type::king, &gen::attacks_king);

	// en_passant
	gen_move_en_passant<VColor>(out,
		&gen::en_passant_left<VColor>, &ops::so_ea, &ops::no_ea);
	gen_move_en_passant<VColor>(out,
		&gen::en_passant_right<VColor>, &ops::so_we, &ops::no_we);

	// pawn push
	if constexpr(!VOnlyCaptures)
		game::gen_move_pawn_push<VColor>(out);

	// castling
	if constexpr(!VOnlyCaptures)
		game::gen_move_castling<VColor>(out);
	return out;
}

template<color VColor>
void game::push_promo_moves(std::vector<move>& out, move& m)
{
	m.set_promo_piece_type(piece_type::knight);
	bool legal = add_when_legal<VColor>(out, m);
	if (!legal) return;
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

	move_list.emplace_back(m,gc,zh);
	constexpr square rook_queenside = VColor == color::white ? square::a1 : square::a8;
	constexpr square rook_kingside = VColor == color::white ? square::h1 : square::h8;

	constexpr square rook_queenside_opp = VColor == color::white ? square::a8 : square::a1;
	constexpr square rook_kingside_opp = VColor == color::white ? square::h8 : square::h1;

	auto ci_prev = gc.castling_info_for_sides;
	uint ep_prev = gc.en_passantable_pawn;

	castling_info& gi = gc.get_castling_info(VColor);
	gi.has_moved_king = (m.get_moved_piece_type() == piece_type::king || gi.has_moved_king);
	// Here and next stmt not accounting for castling, but that doesn't matter because then has_moved_king is set to true.
	gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook
		&& (idx_to_sq(m.get_from_as_idx()) == rook_kingside)) || gi.has_moved_kingside_rook);
	gi.has_moved_queenside_rook = ((m.get_moved_piece_type() == piece_type::rook
		&& (idx_to_sq(m.get_from_as_idx()) == rook_queenside)) || gi.has_moved_queenside_rook);

	if (m.get_move_type() == move_type::captures || m.get_move_type() == move_type::promo_captures)
	{
		castling_info& gi_opp = gc.get_castling_info(invert_color(VColor));
		// The rook has been captured without it moving -> set the flag to false
		gi_opp.has_moved_kingside_rook = (m.get_captured_piece_type() == piece_type::rook
			&& idx_to_sq(m.get_to_as_idx()) == rook_kingside_opp) || gi_opp.has_moved_kingside_rook; 
		gi_opp.has_moved_queenside_rook = (m.get_captured_piece_type() == piece_type::rook
			&& idx_to_sq(m.get_to_as_idx()) == rook_queenside_opp) || gi_opp.has_moved_queenside_rook;
	}

	gc.half_move_clock++;
	if (m.get_move_type() == move_type::pawn_double)
	{
		gc.en_passantable_pawn = m.get_to_as_bitboard();
	}
	else gc.en_passantable_pawn = 0ULL;
	if (m.get_moved_piece_type() == piece_type::pawn || m.get_move_type() == move_type::captures)
		gc.half_move_clock = 0;

	if constexpr (VColor == color::black)
		gc.fullmoves++;

	gc.turn = invert_color(gc.turn);
	b.do_move<VColor>(m);
	zh.do_undo_move<VColor>(ep_prev,ci_prev, gc, m);
}

template<color VColor>
void game::undo_move()
{
	auto& [m,old_gc,zh_] = move_list.back();
	//auto& [m, old_gc] = move_list.back();
	move_list.pop_back();
	gc = old_gc; 
	zh = zh_;
	b.undo_move<VColor>(m);
}

template<color VColor>
std::pair<bool,bool> game::can_castle() const
{
	constexpr color opp = invert_color(VColor);
	auto& ginfo = gc.get_castling_info(VColor);
	bitboard attacks = gen_attack_bb_except_en_passant<opp>();
	attacks |= gen::attack_pawns_castle<opp>(b.get_board(piece_type::pawn,opp)); // This always adds the attacked fields from pawns.  
	bool can_castle_kingside = gen::can_castle_kingside<VColor>(b, attacks)
		&& !ginfo.has_moved_king && !ginfo.has_moved_kingside_rook;
	bool can_castle_queenside = gen::can_castle_queenside<VColor>(b, attacks)
		&& !ginfo.has_moved_king && !ginfo.has_moved_queenside_rook;
	return std::make_pair(can_castle_kingside,can_castle_queenside);
}

bool game::is_last_move_threefold_repetition() const
{
	const auto& to_check = move_list[move_list.size() - 1];
	if (to_check.m.get_moved_piece_type() == piece_type::pawn
		|| to_check.m.get_move_type() == move_type::captures)
		return false;
	int count = 0;
	for (int j = move_list.size()-2; j >= 0; j--)
	{
		const auto& compare = move_list[j];
		if (compare.zh.get_hash() == compare.zh.get_hash())
			count++;
		if (count == 3)
			return true;
		if (compare.m.get_moved_piece_type() == piece_type::pawn
			|| compare.m.get_move_type() == move_type::captures)
			return false;
	}
	return false;
}

uint game::get_ply() const
{
	return move_list.size();
}

template<color VColor>
void game::gen_piece_attacks_for_idx(uint idx, array_vector<piece_type, 6>& out) const
{
	bitboard set_bit = ~ops::set_nth_bit(idx);
	bitboard b = gen_attack_bb_from_piece(b.get_board(piece_type::bishop, VColor), &gen::attacks_bishop);
	if (b & set_bit > 0) out.push_back(piece_type::bishop);
	bitboard r = gen_attack_bb_from_piece(b.get_board(piece_type::rook, VColor), &gen::attacks_rook);
	if (r & set_bit > 0) out.push_back(piece_type::bishop);
	bitboard q = gen_attack_bb_from_piece(b.get_board(piece_type::queen, VColor), &gen::attacks_queen);
	if (q & set_bit > 0) out.push_back(piece_type::queen);
	bitboard n = gen_attack_bb_from_piece(b.get_board(piece_type::knight, VColor), &gen::attacks_knight);
	if (n & set_bit > 0) out.push_back(piece_type::knight);
	bitboard k = gen_attack_bb_from_piece(b.get_board(piece_type::king, VColor), &gen::attacks_king);
	if (k & set_bit > 0) out.push_back(piece_type::king);

	bitboard p = gen::attack_pawns_left<VColor>(b.get_board(piece_type::pawn, VColor), b);
	p |= gen::attack_pawns_right<VColor>(b.get_board(piece_type::pawn, VColor), b);
	if (p & set_bit > 0) out.push_back(piece_type::pawn);
}

// The dooce algebraic notation:
/*
* - Specify only the from square and the to square, even if it's a capture
* - If kingside castles write 00
* - If queenside castles write 000
* - if promotion write "from_square"+"to_square"+"=Q" for examples
*/
// TODO: Refactor

template<color VColor>
inline std::optional<move> game::from_dooce_algebraic_notation(const std::string& m)
{
	std::vector<move> moves = legal_moves<VColor>();
	if (m == "00")
	{
		move mv;
		mv.set_from(0);
		mv.set_to(0);
		mv.set_moved_piece_type(piece_type::king);
		mv.set_move_type(move_type::king_castle);
		if (std::find(moves.begin(), moves.end(), mv) != moves.end())
		{
			return mv;
		}
		else return {};
	}

	else if (m == "000")
	{
		move mv;
		mv.set_from(0);
		mv.set_to(0);
		mv.set_moved_piece_type(piece_type::king);
		mv.set_move_type(move_type::queen_castle);
		if (std::find(moves.begin(), moves.end(), mv) != moves.end())
		{
			return mv;
		}
		else return {};
	}


	std::string from;
	from.push_back(m[0]);
	from.push_back(m[1]);
	uint from_idx = str_to_sq_idx(from);
	bitboard from_set = ops::set_nth_bit(from_idx);
	std::string to;
	to.push_back(m[0]);
	to.push_back(m[1]);
	uint to_idx = str_to_sq_idx(to);
	bitboard to_set = ops::set_nth_bit(to_idx);
	auto [move_ptype, c] = b.get_occupation_of_idx(to_idx);
	if (!move_ptype.has_value()) return {};
	if (c != gc.turn) return {};
	auto captured_ptype = determine_capturing(c, ops::set_nth_bit(to_idx));
	bool promo;
	std::optional<piece_type> promo_ptype;
	promo = determine_promo<VColor>(ops::set_nth_bit(to_idx));
	// No promo specified

	if (promo)
	{
		std::map<char, piece_type> char_to_pt = { { 'Q',piece_type::queen },{ 'R',piece_type::rook },{ 'N',piece_type::knight },{ 'B', piece_type::bishop } };
		if (m.size() < 6) return {};
		promo_ptype = char_to_pt[m[5]];
	}

	move_type mtype;
	if (captured_ptype.has_value()) mtype = move_type::captures;
	if (move_ptype.value() == piece_type::pawn && !captured_ptype.has_value())
	{

		bitboard epl = gen::en_passant_left<VColor>(b, gc.en_passantable_pawn);
		bitboard epr = gen::en_passant_right<VColor>(b, gc.en_passantable_pawn);
		if (epl == to_idx || epr == to_idx) mtype = move_type::en_passant;
		else
		{
			// determine if pawn push
			bitboard to_ = gen::move_pawns_single<VColor>(from_set, b);
			if (to_ == to_set) mtype = move_type::pawn_single;
			else mtype = move_type::pawn_double;
		}
	}

	if (promo && !captured_ptype.has_value())
	{
		mtype = move_type::promo;
	}

	else if (promo && captured_ptype.has_value())
	{
		mtype = move_type::promo_captures;
	}

	move mv;
	mv.set_from(from_idx);
	mv.set_to(to_idx);
	mv.set_captured_piece_type(captured_ptype);
	mv.set_promo_piece_type(promo_ptype);
	mv.set_move_type(mtype);
	mv.set_moved_piece_type(move_ptype.value());
	if (std::find(moves.begin(), moves.end(), mv) != moves.end())
	{
		return mv;
	}
	else return {};
}

game::game() : b(), gc(), zh()
{
	if (!init) { gen::init_all(); init = true; }
	move_list.reserve(9000);
}

game::game(const board& b, const game_context& gc) : b(b),gc(gc), zh(gc, b)
{
	if (!init) { gen::init_all(); init = true; }
	move_list.reserve(9000);
}

game::game(const std::string& board_repr, const game_context& gc) : b(board_repr), gc(gc), zh(gc, b)
{
	if (!init) { gen::init_all(); init = true; }
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

// TODO: Use FD_TEMPLATE_FUNCTION for that

FD_TEMPLATE_FUNCTION(std::optional<move>, from_dooce_algebraic_notation, const std::string&);

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

FD_ATTACK_TEMPLATE_FUNCTION(void, gen_attack_moves_from_piece, std::vector<move>& out, bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));

template void game::gen_move_pawn_push<color::white>(std::vector<move>& out);
template void game::gen_move_pawn_push<color::black>(std::vector<move>& out);

template void game::gen_move_en_passant<color::white>(std::vector<move>& out, en_passant_fn, shift_func, shift_func);
template void game::gen_move_en_passant<color::black>(std::vector<move>& out, en_passant_fn, shift_func, shift_func);

template void game::gen_move_castling<color::white>(std::vector<move>& out);
template void game::gen_move_castling<color::black>(std::vector<move>& out);

FD_ATTACK_TEMPLATE_FUNCTION(std::vector<move>, legal_moves);

template void game::do_move<color::white>(const move& m);
template void game::do_move<color::black>(const move& m);

template void game::undo_move<color::white>();
template void game::undo_move<color::black>();

template std::pair<bool,bool> game::can_castle<color::white>() const;
template std::pair<bool, bool> game::can_castle<color::black>() const;


