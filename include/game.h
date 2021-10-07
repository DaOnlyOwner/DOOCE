#pragma once
#include "definitions.h"
#include "move.h"
#include "board.h"
#include <optional>
#include <string>
#include <vector>

class game
{
public:
	struct castle_info
	{
		bool kingside;
		bool queenside;
	};
	
	game();
	game(const board& b, const game_context& start_gc);
	game(const game& g) = default;
	game(game&& g) = default;
	game& operator=(const game&) = default;
	game& operator=(game&&) = default;
	~game() = default;

	template<color VColor>
	std::vector<move> legal_moves();

	template<color VColor>
	void do_move(const move& m);

	template<color VColor>
	void undo_move();

	template<color VColor>
	bool is_in_check() const;

	template<color VColor>
	castle_info can_castle() const;

	const game_context& get_game_context() const;

	const board& get_board() const;


private:
	game_context gc;
	board b;
	// Retrieving the game_context from the stack is much simpler.
	std::vector<std::pair<move,game_context>> move_list;

	// Cache 
	//attack_list alist;

	// Helper functions
	// gen attack list

	
	
	template<color VColor>
	void gen_attack_moves_from_piece(std::vector<move>& out, bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));

	template<color VColor>
	void gen_attack_moves_from_pawns(std::vector<move>& out);

	template<color VColor>
	void gen_attack_moves_from_pawns_inner(move& m, bitboard, std::vector<move>& out);

	bitboard gen_attack_bb_from_piece(bitboard piece_occ, bitboard(*fn)(const board&, uint)) const;

	// This method is basically only used for castling.
	template<color VColor>
	bitboard gen_attack_bb_except_en_passant() const;

	template<color VColor>
	void push_promo_moves(std::vector<move>& out, move& m);

	template<color VColor>
	void gen_legal_moves_from_attack_list(const attack_info& ainfo, std::vector<move>& out);

	template<color VColor>
	bool add_when_legal(std::vector<move>& out, const move& m);

	template<color VColor>
	bool determine_promo(bitboard set_bit) const;
	std::optional<piece_type> determine_capturing(color c, bitboard set_bit) const;

	template<color VColor>
	void gen_move_pawn_push(std::vector<move>& out);

	typedef bitboard(*en_passant_fn)(const board&, bitboard);
	typedef bitboard(*shift_func)(bitboard);

	template<color VColor>
	void gen_move_en_passant(std::vector<move>& out,
		en_passant_fn efn, shift_func shift_white, shift_func shift_black);

	template<color VColor>
	void gen_move_castling(std::vector<move>& out);
};

//class game
//{
//public:
//
//	game();
//	game::game(board b, game_context context);
//
//	// TODO: remove those old constructors, they are obsolete now
//	/*game(const std::string& start_board, const game_info& start_info_white, 
//		const game_info& start_info_black, color start_color, bitboard start_en_passantable_pawn);*/
//	//game(const std::string& fen);
//
//	perft_results perft(int depth);
//	game_info get_start_info(color c) const 
//	{
//		if (c == color::white)
//			return start_info_white;
//		else return start_info_black;
//	}
//
//	color get_start_color() const
//	{
//		return start_color;
//	}
//
//	board& get_board()
//	{
//		return b;
//	}
//
//	const board& get_board_const() const
//	{
//		return b;
//	}
//
//	bitboard get_start_en_passantable_pawn() const 
//	{
//		return start_en_passantable_pawn;
//	}
//
//	// TODO: move this to board.h
//	//template<color VColor>
//	//void extract_board()
//	//{
//	//	constexpr color ecolor = invert_color(VColor);
//	//	info.own_bishops = b.get_board_const(piece_type::bishop, VColor);
//	//	info.own_king = b.get_board_const(piece_type::king, VColor);
//	//	info.own_knights = b.get_board_const(piece_type::knight, VColor);
//	//	info.own_pawns = b.get_board_const(piece_type::pawn, VColor);
//	//	info.own_queens = b.get_board_const(piece_type::queen, VColor);
//	//	info.own_rooks = b.get_board_const(piece_type::rook, VColor);
//
//	//	info.enemy_bishops = b.get_board_const(piece_type::bishop, ecolor);
//	//	info.enemy_king = b.get_board_const(piece_type::king, ecolor);
//	//	info.enemy_knights = b.get_board_const(piece_type::knight, ecolor);
//	//	info.enemy_pawns = b.get_board_const(piece_type::pawn, ecolor);
//	//	info.enemy_queens = b.get_board_const(piece_type::queen, ecolor);
//	//	info.enemy_rooks = b.get_board_const(piece_type::rook, ecolor);
//
//	//	info.own_color_occ = info.own_king | info.own_knights | info.own_bishops
//	//		| info.own_queens | info.own_rooks | info.own_pawns;
//	//	info.enemy_color_occ = info.enemy_bishops | info.enemy_king | info.enemy_knights
//	//		| info.enemy_queens | info.enemy_rooks | info.enemy_pawns;
//	//	info.not_own_color_occ = ~info.own_color_occ;
//	//	info.occ = info.own_color_occ | info.enemy_color_occ;
//	//	info.not_occ = ~info.occ;
//	//}
//
//	// This method is basically only used for castling.
//	template<color VColor>
//	bitboard gen_all_own_attacks_except_en_passant(const board_info& info)
//	{
//		bitboard attacks =
//			gen_all_attacks_from_sliding_piece_type(info.own_bishops, info.occ, &board::gen_attacks_bishop) |
//			gen_all_attacks_from_sliding_piece_type(info.own_rooks, info.occ, &board::gen_attacks_rook) |
//			gen_all_attacks_from_sliding_piece_type(info.own_queens, info.occ, &board::gen_attacks_queen) |
//			gen_all_attacks_from_piece_type(info.own_knights, info.occ, &board::gen_attacks_knight) |
//			gen_all_attacks_from_piece_type(info.own_king, info.occ, &board::gen_attacks_king);
//
//		attacks |= board::gen_attack_pawns_left<VColor>(info.own_pawns, info.enemy_color_occ);
//		attacks |= board::gen_attack_pawns_right<VColor>(info.own_pawns, info.enemy_color_occ);
//		return attacks & info.not_own_color_occ;
//	}
//
//	template<color VColor>
//	bitboard gen_all_enemy_attacks_except_en_passant(const board_info& info)
//	{
//		bitboard attacks =
//			gen_all_attacks_from_sliding_piece_type(info.enemy_bishops, info.occ, &board::gen_attacks_bishop) |
//			gen_all_attacks_from_sliding_piece_type(info.enemy_rooks, info.occ, &board::gen_attacks_rook) |
//			gen_all_attacks_from_sliding_piece_type(info.enemy_queens, info.occ, &board::gen_attacks_queen) |
//			gen_all_attacks_from_piece_type(info.enemy_knights, info.occ, &board::gen_attacks_knight) |
//			gen_all_attacks_from_piece_type(info.enemy_king, info.occ, &board::gen_attacks_king);
//
//		attacks |= board::gen_attack_pawns_left<VColor>(info.enemy_pawns, info.own_color_occ);
//		attacks |= board::gen_attack_pawns_right<VColor>(info.enemy_pawns, info.own_color_occ);
//		return attacks & (~info.enemy_color_occ);
//	}
//
//	// This method is used to precalc the attack patterns to test if the enemy king is in check
//	template<color VColor>
//	bitboard gen_all_attack_pattern_except_en_passant(
//		attack_pattern& pattern, int& size, const board_info& info)
//	{
//		size = 0;
//
//		bitboard attacks =
//			gen_attack_info_from_sliding_piece_type(info.own_bishops, info,
//			    pattern, size, piece_type::bishop, &board::gen_attacks_bishop)
//			| gen_attack_info_from_sliding_piece_type(info.own_rooks, info,
//			    pattern, size, piece_type::rook, &board::gen_attacks_rook)
//			| gen_attack_info_from_sliding_piece_type(info.own_queens, info,
//			    pattern, size, piece_type::queen, &board::gen_attacks_queen)
//			| gen_attack_info_from_piece_type(info.own_knights, info,
//			    pattern, size, piece_type::knight, &board::gen_attacks_knight)
//			| gen_attack_info_from_piece_type(info.own_king, info,
//			    pattern, size, piece_type::king, &board::gen_attacks_king)
//			| gen_attack_info_from_pawns<VColor>(info, pattern, size);
//
//		// This is already ANDed by info.not_own_color_occ (in gen_attack_info_from_piece_type),
//		// so I don't need to do that here again.
//		return attacks;
//	}
//
//	template<color VColor>
//	std::vector<move> gen_all_legal_moves(const attack_pattern& pattern, int size,
//		const board_info& info, bitboard en_passantable_pawn, const game_info& ginfo, board& b)
//	{
//		// generate all possible moves (unvalidated)
//		constexpr color ecolor = invert_color(VColor);
//		auto moves = gen_all_moves<VColor>(pattern, size, info, en_passantable_pawn, ginfo);
//
//		// initialize legal moves array
//		std::vector<move> moves_legal;
//		moves_legal.reserve(moves.size());
//
//		// go through each of the moves
//		for (const move& m : moves)
//		{
//			// simulate the move
//			b.do_move<VColor>(m);
//
//			// compute the enemy attacks (bitboard)
//			board_info binfo_after;
//			extract_board<ecolor>(binfo_after);
//			bitboard attacks = game::gen_all_enemy_attacks_except_en_passant<ecolor>(binfo_after);
//
//			// only append the move if the king is not in check after applying it
//			if (!b.is_king_in_check(VColor, attacks))
//				moves_legal.push_back(m);
//
//			// revert the simulated move
//			b.undo_move<VColor>(m);
//		}
//
//		return moves_legal;
//	}
//
//	template<color VColor>
//	std::vector<move> gen_all_legal_moves(bitboard en_passantable_pawn, const game_info& ginfo)
//	{
//		board_info binfo;
//		extract_board<VColor>(binfo);
//		constexpr color ecolor = invert_color(VColor);
//		attack_pattern ap;
//		int size = 0;
//		gen_all_attack_pattern_except_en_passant<VColor>(ap, size, info);
//		return gen_all_legal_moves<VColor>(ap, size, binfo, en_passantable_pawn, ginfo, b);
//	}
//
//	template<color VColor>
//	std::vector<move> gen_all_moves(const attack_pattern& pattern, int size,
//	    const board_info& info, bitboard en_passantable_pawn, const game_info& ginfo)
//	{
//		//board::print_bitboard(pattern[0].attacks);
//		std::vector<move> out;
//		out.reserve(250);
//		
//		// Attack pattern from previous is_king_in_check test
//		for (int i = 0; i<size; i++)
//		{
//			game::gen_moves_from_attack_pattern<VColor>(pattern, i, info, out);
//		}
//
//		// en_passant
//		game::gen_move_en_passant<VColor>(info, en_passantable_pawn, out,
//		    &board::gen_en_passant_left<VColor>,&ops::so_ea,&ops::no_ea);
//		game::gen_move_en_passant<VColor>(info, en_passantable_pawn, out,
//		    &board::gen_en_passant_right<VColor>, &ops::so_we, &ops::no_we);
//
//		// pawn push
//		game::gen_move_pawn_push<VColor>(info, out);
//
//		// castling
//		game::gen_move_castling<VColor>(info, ginfo, out);
//		return out;
//	}
//private:
//	board b;
//	game_info start_info_white;
//	game_info start_info_black;
//	color start_color;
//	bitboard start_en_passantable_pawn;
//	game_context gc;
//
//	inline bool is_king_in_check(color c, bitboard attacks)
//	{
//		// TODO: check if this does the right thing
//		return (b.get_board(piece_type::king, c) & attacks) > 0;
//
//		/*square king_square = idx_to_sq(ops::num_trailing_zeros(
//			get_board(piece_type::king, c)));
//		return is_square_attacked(attacks, king_square);*/
//	}
//
//	template<color VColor>
//	std::pair<game_info, bitboard> game_info_from_move(const move& m, const game_info& previous_info)
//	{
//		constexpr square rook_queenside = VColor == color::white ? square::a1 : square::a8;
//		constexpr square rook_kingside = VColor == color::white ? square::h1 : square::h8;
//		game_info gi;
//		gi.has_moved_king = (m.get_moved_piece_type() == piece_type::king || previous_info.has_moved_king);
//		// Here and next stmt not accounting for castling, but that doesn't matter because then has_moved_king is set to true.
//		gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook
//		    && (idx_to_sq(m.get_from_as_idx()) == rook_kingside)) || previous_info.has_moved_kingside_rook);
//		gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook
//		    && (idx_to_sq(m.get_from_as_idx()) == rook_queenside)) || previous_info.has_moved_queenside_rook);
//		bitboard en_passantable_pawn = 0ULL;
//		if (m.get_move_type() == move_type::pawn_double)
//			en_passantable_pawn = m.get_to();		
//
//		return std::make_pair(gi, en_passantable_pawn);
//	}
//
//	void push_promo_moves(std::vector<move>& out, move& m);
//
//	template<color VColor>
//	void gen_move_castling(const board_info& info, const game_info& ginfo, std::vector<move>& out)
//	{
//		constexpr color ecolor = invert_color(VColor);
//		bitboard attacks = game::gen_all_enemy_attacks_except_en_passant<ecolor>(info);
//		bool can_castle_kingside = board::can_castle_kingside<VColor>(info.occ, attacks)
//		    && !ginfo.has_moved_king && !ginfo.has_moved_kingside_rook;
//		bool can_castle_queenside = board::can_castle_queenside<VColor>(info.occ, attacks)
//		    && !ginfo.has_moved_king && !ginfo.has_moved_queenside_rook;
//		move_type mtype;
//		piece_type moved = (piece_type::king);
//
//		if (can_castle_kingside)
//		{
//			mtype = (move_type::king_castle);
//			move m(0, 0, moved, {}, {}, mtype);
//			out.push_back(m);
//		}
//		if (can_castle_queenside)
//		{
//			mtype = (move_type::queen_castle);
//			move m(0, 0, moved, {}, {}, mtype);
//			out.push_back(m);
//		}
//	}
//
//	template<color VColor>
//	void gen_move_pawn_push(const board_info& info, std::vector<move>& out)
//	{
//		bitboard cpy = info.own_pawns;
//		uint idx = 0;
//		while (cpy != 0)
//		{
//			idx = ops::num_trailing_zeros(cpy);
//			bitboard set_bit = ops::set_nth_bit(idx);
//			move m{};
//			m.set_from(idx);
//			bitboard single = board::gen_move_pawns_single<VColor>(set_bit, info.not_occ);
//			if (single == 0ULL)
//			{
//				ops::pop_lsb(cpy);
//				continue;
//			}
//			uint to = ops::num_trailing_zeros(single);
//			m.set_to(to);
//			m.set_moved_piece_type(piece_type::pawn);
//			m.set_move_type(move_type::pawn_single);
//			out.push_back(m);
//			bitboard dbl = board::gen_move_pawns_dbl<VColor>(set_bit, info.not_occ);
//			if (dbl == 0ULL)
//			{
//				ops::pop_lsb(cpy);
//				continue;
//			}
//			to = ops::num_trailing_zeros(dbl);
//			m.set_to(to);
//			m.set_move_type(move_type::pawn_double);
//			out.push_back(m);
//			ops::pop_lsb(cpy);
//		}
//	}
//
//	template<color VColor, typename EnPassantFunc, typename ShiftFuncWhite, typename ShiftFuncBlack>
//	void gen_move_en_passant(const board_info& info, bitboard en_passantable_pawn, std::vector<move>& out,
//	    EnPassantFunc en_passant_fn, ShiftFuncWhite shift_white, ShiftFuncBlack shift_black)
//	{
//		bitboard attack = en_passant_fn(info.own_pawns, en_passantable_pawn);
//		if (attack != 0)
//		{
//			move m{};
//			m.set_captured_piece_type(piece_type::pawn);
//			m.set_moved_piece_type(piece_type::pawn);
//			m.set_move_type(move_type::en_passant);
//			if constexpr (VColor == color::white)
//				m.set_from(ops::num_trailing_zeros(shift_white(attack)));
//			else m.set_from(ops::num_trailing_zeros(shift_black(attack)));
//			m.set_to(ops::num_trailing_zeros(attack));
//			out.push_back(m);
//		}
//	}
//
//	template<color VColor>
//	void gen_moves_from_attack_pattern(const attack_pattern& pattern, int i,
//	    const board_info& info, std::vector<move>& out)
//	{
//		const attack_info& ainfo = pattern[i];
//		bitboard cpy = ainfo.attacks;
//		uint idx = 0;
//		while (cpy != 0)
//		{
//			move m{};
//			idx = ops::num_trailing_zeros(cpy);
//			bitboard set_bit = ops::set_nth_bit(idx);
//			auto captured = determine_capturing(info, set_bit);
//			m.set_from(ainfo.from);
//			m.set_to(idx);
//			m.set_moved_piece_type(ainfo.ptype);
//			m.set_captured_piece_type(captured);
//			bool promo = determine_promo<VColor>(ainfo.ptype, info, set_bit);
//			if (promo)
//			{
//				move_type mtype = captured.has_value() ? move_type::promo_captures : move_type::promo;
//				m.set_move_type(mtype);
//				push_promo_moves(out, m);
//			}
//
//			else
//			{
//				move_type mtype = captured.has_value() ? move_type::captures : move_type::quiet;
//				m.set_move_type(mtype);
//				out.push_back(m);
//			}
//			ops::pop_lsb(cpy);
//		}
//	}
//
//	template<color VColor>
//	bool determine_promo(piece_type ptype, const board_info& info, bitboard set_bit)
//	{
//		if (ptype != piece_type::pawn) return {};
//		if constexpr (VColor == color::white)
//			return ops::has_bit_set_on_rank(set_bit, 8);
//		else return ops::has_bit_set_on_rank(set_bit, 1);
//	}
//
//	std::optional<piece_type> determine_capturing(const board_info& info, bitboard set_bit);
//
//	template<color VColor>
//	bitboard gen_attack_info_from_pawns(const board_info& info, attack_pattern& pattern, int& size)
//	{
//		bitboard attacks = 0ULL;
//		bitboard own_pawns = info.own_pawns;
//		uint idx = 0;
//		while (own_pawns != 0ULL)
//		{
//			idx = ops::num_trailing_zeros(own_pawns);
//			bitboard only_pawn = ops::set_nth_bit(idx);
//			attack_info ainfo;
//			ainfo.attacks = board::gen_attack_pawns_left<VColor>(only_pawn, info.enemy_color_occ);
//			ainfo.attacks |= board::gen_attack_pawns_right<VColor>(only_pawn, info.enemy_color_occ);
//			if (ainfo.attacks != 0ULL)
//			{
//				ainfo.from = idx;
//				ainfo.ptype = piece_type::pawn;
//				pattern[size++] = ainfo;
//				attacks |= ainfo.attacks;
//			}
//			ops::pop_lsb(own_pawns);
//		}
//
//		return attacks;
//	}
//
//	template<typename GenFn>
//	bitboard gen_attack_info_from_sliding_piece_type(bitboard piece_occ, const board_info& info,
//	    attack_pattern& pattern, int& size, piece_type ptype, GenFn fn)
//	{
//		uint idx = 0;
//		bitboard attacks = 0ULL;
//		while (piece_occ != 0ULL)
//		{
//			idx = ops::num_trailing_zeros(piece_occ);
//			attack_info ainfo;
//			ainfo.attacks = fn(info.occ, idx) & info.not_own_color_occ;
//			if (ainfo.attacks != 0ULL)
//			{
//				ainfo.from = idx;
//				ainfo.ptype = ptype;
//				pattern[size++] = ainfo;
//				attacks |= ainfo.attacks;
//			}
//			ops::pop_lsb(piece_occ);
//		}
//		return attacks;
//	}
//
//	template<typename GenFn>
//	bitboard gen_attack_info_from_piece_type(bitboard piece_occ, const board_info& info,
//	    attack_pattern& pattern, int& size, piece_type ptype, GenFn fn)
//	{
//		uint idx = 0;
//		bitboard attacks = 0ULL;
//		while (piece_occ != 0ULL)
//		{
//			idx = ops::num_trailing_zeros(piece_occ);
//			attack_info ainfo;
//			ainfo.attacks = fn(idx) & info.not_own_color_occ;
//			if (ainfo.attacks != 0ULL)
//			{
//				ainfo.from = idx;
//				ainfo.ptype = ptype;
//				pattern[size++] = ainfo;
//				attacks |= ainfo.attacks;
//			}
//			ops::pop_lsb(piece_occ);
//		}
//		return attacks;
//	}
//
//	template<typename GenFn>
//	bitboard gen_all_attacks_from_piece_type(bitboard piece_occ, bitboard occ, GenFn fn)
//	{
//		uint idx = 0;
//		bitboard attacks = 0ULL;
//		while (piece_occ != 0ULL)
//		{
//			idx = ops::num_trailing_zeros(piece_occ);
//			attacks |= fn(idx);
//			ops::pop_lsb(piece_occ);
//		}
//		return attacks;
//	}
//
//	template<typename GenFn>
//	bitboard gen_all_attacks_from_sliding_piece_type(bitboard piece_occ, bitboard occ, GenFn fn)
//	{
//		uint idx = 0;
//		bitboard attacks = 0ULL;
//
//		// loop until all pieces are handled
//		while (piece_occ != 0ULL)
//		{
//			idx = ops::num_trailing_zeros(piece_occ);
//			attacks |= fn(occ, idx);
//
//			// piece is handled -> set the piece bit at idx to 0
//			piece_occ &= piece_occ - 1;
//		}
//
//		return attacks;
//	}
//};

