#pragma once
#include <vector>
#include "move.h"
#include "board.h"
#include <optional>
#include <string>

struct game_info
{
	bool has_moved_king;
	bool has_moved_queenside_rook;
	bool has_moved_kingside_rook;
};



struct perft_results
{
	uint64_t nodes = 0ULL;
	uint64_t captures = 0ULL;
	uint64_t en_passants = 0ULL;
	uint64_t castles = 0ULL;
	uint64_t promos = 0ULL;
};

class game
{
public:

	game(); 
	game(const std::string& start_board, const game_info& start_info_white, const game_info& start_info_black, color start_color, bitboard start_en_passantable_pawn);
	game(const std::string& start_board);



	perft_results perft(int depth);

	// This method is basically only used for castling.
	template<color VColor>
	static bitboard gen_all_attacks_except_en_passant(/*bitboard en_passantable_pawn,*/ const board_info& info)
	{
		bitboard attacks =
			gen_all_attacks_from_sliding_piece_type(info.own_bishops, info.occ, &board::gen_attacks_bishop) |
			gen_all_attacks_from_sliding_piece_type(info.own_rooks, info.occ, &board::gen_attacks_rook) |
			gen_all_attacks_from_sliding_piece_type(info.own_queens, info.occ, &board::gen_attacks_queen) |
			gen_all_attacks_from_piece_type(info.own_knights, info.occ, &board::gen_attacks_knight) |
			gen_all_attacks_from_piece_type(info.own_king, info.occ, &board::gen_attacks_king);

		attacks |= board::gen_attack_pawns_left<VColor>(info.own_pawns, info.enemy_color_occ);
		attacks |= board::gen_attack_pawns_right<VColor>(info.own_pawns, info.enemy_color_occ);
		// The following is probably not needed because this method is used only for castling and en_passant attacks cannot put the king in check or attack the squares the king needs for castling.
		//attacks |= board::gen_en_passant_left<VColor>(info.own_pawns, en_passantable_pawn);
		//attacks |= board::gen_en_passant_right<VColor>(info.own_pawns, en_passantable_pawn);
		return attacks & info.not_own_color_occ;
	}

	// This method is used to precalc the attack patterns to test if the enemy king is in check
	template<color VColor>
	static bitboard gen_all_attack_pattern_except_en_passant(attack_pattern& pattern, int& size, const board_info& info)
	{
		size = 0;
		bitboard attacks =
			gen_attack_info_from_sliding_piece_type(info.own_bishops, info, pattern, size, piece_type::bishop, &board::gen_attacks_bishop) |
			gen_attack_info_from_sliding_piece_type(info.own_rooks, info, pattern, size, piece_type::rook, &board::gen_attacks_rook) |
			gen_attack_info_from_sliding_piece_type(info.own_queens, info, pattern, size, piece_type::queen, &board::gen_attacks_queen) |
			gen_attack_info_from_piece_type(info.own_knights, info, pattern, size, piece_type::knight, &board::gen_attacks_knight) |
			gen_attack_info_from_piece_type(info.own_king, info, pattern, size, piece_type::king, &board::gen_attacks_king) |
			gen_attack_info_from_pawns<VColor>(info, pattern,size);
		return attacks; // This is already ANDed by info.not_own_color_occ (in gen_attack_info_from_piece_type), so I don't need to do that here again. 
	}


	template<color VColor>
	static std::vector<move> gen_all_moves(const attack_pattern& pattern, int size, const board_info& info, bitboard en_passantable_pawn, const game_info& ginfo)
	{
		std::vector<move> out;
		out.reserve(250);
		
		// Attack pattern from previous is_king_in_check test
		for (int i = 0; i<size; i++)
		{
			game::gen_moves_from_attack_pattern<VColor>(pattern, i, info, out);
		}

		// en_passant
		game::gen_move_en_passant<VColor>(info, en_passantable_pawn, out,&board::gen_en_passant_left<VColor>,&ops::so_ea,&ops::no_ea);
		game::gen_move_en_passant<VColor>(info, en_passantable_pawn, out, &board::gen_en_passant_right<VColor>, &ops::so_we, &ops::no_we);

		// pawn push
		game::gen_move_pawn_push<VColor>(info, out);

		// castling
		game::gen_move_castling<VColor>(info, ginfo, out);
		return out;
	}
private:
	board b;
	game_info start_info_white;
	game_info start_info_black;
	color start_color;
	bitboard start_en_passantable_pawn;
	
	void update_perft_results(const perft_results& res, perft_results& to_update);

	template<color VColor>
	perft_results perft_inner(int depth, game_info& ginfo_own, game_info& ginfo_enemy, bitboard en_passantable_pawn, const attack_pattern& pattern, int size, const board_info& binfo, const move& m)
	{
		constexpr color ecolor = invert_color(VColor);
		if (depth == 0)
		{
			perft_results res{};
			res.nodes = 1ULL;
			switch (m.get_move_type())
			{
			case move_type::captures:
				res.captures = 1ULL;
				break;
			case move_type::en_passant:
				res.en_passants = 1ULL;
				break;
			case move_type::king_castle:
				res.castles = 1ULL;
				break;
			case move_type::promo:
				res.promos = 1ULL;
				break;
			case move_type::promo_captures:
				res.promos = 1ULL;
				res.captures = 1ULL;
				break;
			default:
				break;
			}
			return res;
		};
		perft_results res_overall{};
		std::vector<move> moves = game::gen_all_moves<VColor>(pattern, size, binfo, en_passantable_pawn, ginfo_own);
		for (const move& m : moves)
		{
			b.do_move<VColor>(m);
			auto [ginfo_own_after, en_passantable_pawn_after] = game::game_info_from_move<VColor>(m,ginfo_own);
			board_info binfo_after;
			game::extract_board<ecolor>(binfo_after);
			attack_pattern pattern_after;
			int size_after;
			bitboard attacks = game::gen_all_attack_pattern_except_en_passant<ecolor>(pattern_after, size_after, binfo_after);
			bitboard king = b.get_board(piece_type::king, VColor); // Test if our king is in check, and if not the move was legal.
			if (!board::is_king_in_check(king,attacks))
			{
				auto res = perft_inner<ecolor>(depth - 1, ginfo_enemy, ginfo_own_after, en_passantable_pawn_after, pattern_after, size_after, binfo_after, m);
				update_perft_results(res, res_overall);
			}
			b.undo_move<VColor>(m);
		}
		return res_overall;
	}



	template<color VColor>
	static std::pair<game_info, bitboard> game_info_from_move(const move& m, const game_info& previous_info)
	{
		constexpr square rook_queenside = VColor == color::white ? square::a1 : square::a8;
		constexpr square rook_kingside = VColor == color::white ? square::h1 : square::h8;
		game_info gi;
		gi.has_moved_king = (m.get_moved_piece_type() == piece_type::king || previous_info.has_moved_king);
		gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook && (idx_to_sq(m.get_from_as_idx()) == rook_kingside)) || previous_info.has_moved_kingside_rook); // Here and next stmt not accounting for castling, but that doesn't matter because then has_moved_king is set to true.
		gi.has_moved_kingside_rook = ((m.get_moved_piece_type() == piece_type::rook && (idx_to_sq(m.get_from_as_idx()) == rook_queenside)) || previous_info.has_moved_queenside_rook);
		bitboard en_passantable_pawn = 0ULL;
		if (m.get_move_type() == move_type::pawn_double)
			en_passantable_pawn = m.get_to();		

		return std::make_pair(gi, en_passantable_pawn);
	}

	static void push_promo_moves(std::vector<move>& out, move& m);

	template<color VColor>
	static void gen_move_castling(const board_info& info, const game_info& ginfo, std::vector<move>& out)
	{
		constexpr color ecolor = invert_color(VColor);
		bitboard attacks = game::gen_all_attacks_except_en_passant<ecolor>(info);
		bool can_castle_kingside = board::can_castle_kingside<VColor>(info.occ, attacks) && !ginfo.has_moved_king && !ginfo.has_moved_kingside_rook;
		bool can_castle_queenside = board::can_castle_queenside<VColor>(info.occ, attacks) && !ginfo.has_moved_king && !ginfo.has_moved_queenside_rook;
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
	static void gen_move_pawn_push(const board_info& info, std::vector<move>& out)
	{
		bitboard cpy = info.own_pawns;
		uint idx = 0;
		while (cpy != 0)
		{
			idx = ops::num_trailing_zeros(cpy);
			bitboard set_bit = ops::set_nth_bit(idx);
			move m{};
			m.set_from(idx);
			uint to = ops::num_trailing_zeros_with_zero_check(board::gen_move_pawns_single<VColor>(set_bit, info.not_own_color_occ));
			if (to == 64)
			{
				ops::pop_lsb(cpy);
				continue;
			}
			m.set_to(to);
			m.set_moved_piece_type(piece_type::pawn);
			m.set_move_type(move_type::pawn_single);
			out.push_back(m);
			to = ops::num_trailing_zeros_with_zero_check(board::gen_move_pawns_dbl<VColor>(set_bit, info.not_own_color_occ));
			if (to == 64)
			{
				ops::pop_lsb(cpy);
				continue;
			}
			m.set_to(to);
			m.set_move_type(move_type::pawn_double);
			out.push_back(m);
			ops::pop_lsb(cpy);
		}
	}

	template<color VColor, typename EnPassantFunc, typename ShiftFuncWhite, typename ShiftFuncBlack>
	static void gen_move_en_passant(const board_info& info, bitboard en_passantable_pawn, std::vector<move>& out, EnPassantFunc en_passant_fn, ShiftFuncWhite shift_white, ShiftFuncBlack shift_black)
	{
		bitboard attack = en_passant_fn(info.own_pawns, en_passantable_pawn);
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
			out.push_back(m);
		}
	}

	template<color VColor>
	static void gen_moves_from_attack_pattern(const attack_pattern& pattern, int i, const board_info& info, std::vector<move>& out)
	{
		const attack_info& ainfo = pattern[i];
		bitboard cpy = ainfo.attacks;
		uint idx = 0;
		while (cpy != 0)
		{
			move m{};
			idx = ops::num_trailing_zeros(cpy);
			bitboard set_bit = ops::set_nth_bit(idx);
			auto captured = determine_capturing(info, set_bit);
			m.set_from(ainfo.from);
			m.set_to(idx);
			m.set_moved_piece_type(ainfo.ptype);
			m.set_captured_piece_type(captured);
			bool promo = determine_promo<VColor>(ainfo.ptype, info, set_bit);
			if (promo)
			{
				move_type mtype = captured.has_value() ? move_type::promo_captures : move_type::promo;
				m.set_move_type(mtype);
				push_promo_moves(out, m);
			}

			else
			{
				move_type mtype = captured.has_value() ? move_type::captures : move_type::quiet;
				m.set_move_type(mtype);
				out.push_back(m);
			}
			ops::pop_lsb(cpy);
		}
	}

	template<color VColor>
	static bool determine_promo(piece_type ptype, const board_info& info, bitboard set_bit)
	{
		if (ptype != piece_type::pawn) return {};
		if constexpr (VColor == color::white)
			return ops::has_bit_set_on_rank(set_bit, 8);
		else return ops::has_bit_set_on_rank(set_bit, 1);
	}

	static std::optional<piece_type> determine_capturing(const board_info& info, bitboard set_bit)
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

	template<color VColor>
	static bitboard gen_attack_info_from_pawns(const board_info& info, attack_pattern& pattern, int& size)
	{
		bitboard attacks = 0ULL;
		bitboard own_pawns = info.own_pawns;
		uint idx = 0;
		while (own_pawns != 0ULL)
		{
			idx = ops::num_trailing_zeros(own_pawns);
			bitboard only_pawn = ops::set_nth_bit(idx);
			attack_info ainfo;
			ainfo.attacks = board::gen_attack_pawns_left<VColor>(only_pawn, info.enemy_color_occ);
			ainfo.attacks |= board::gen_attack_pawns_right<VColor>(only_pawn, info.enemy_color_occ);
			if (ainfo.attacks != 0ULL)
			{
				ainfo.from = idx;
				ainfo.ptype = piece_type::pawn;
				pattern[size++] = ainfo;
				attacks |= ainfo.attacks;
			}
			ops::pop_lsb(own_pawns);
		}

		return attacks;
	}


	template<typename GenFn>
	static bitboard gen_attack_info_from_sliding_piece_type(bitboard piece_occ, const board_info& info, attack_pattern& pattern, int& size, piece_type ptype, GenFn fn)
	{
		uint idx = 0;
		bitboard attacks = 0ULL;
		while (piece_occ != 0ULL)
		{
			idx = ops::num_trailing_zeros(piece_occ);
			attack_info ainfo;
			ainfo.attacks = fn(info.occ, idx) & info.not_own_color_occ;
			if (ainfo.attacks != 0ULL)
			{
				ainfo.from = idx;
				ainfo.ptype = ptype;
				pattern[size++] = ainfo;
				attacks |= ainfo.attacks;
			}
			ops::pop_lsb(piece_occ);
		}
		return attacks;
	}

	template<typename GenFn>
	static bitboard gen_attack_info_from_piece_type(bitboard piece_occ, const board_info& info, attack_pattern& pattern, int& size, piece_type ptype, GenFn fn)
	{
		uint idx = 0;
		bitboard attacks = 0ULL;
		while (piece_occ != 0ULL)
		{
			idx = ops::num_trailing_zeros(piece_occ);
			attack_info ainfo;
			ainfo.attacks = fn(idx) & info.not_own_color_occ;
			if (ainfo.attacks != 0ULL)
			{
				ainfo.from = idx;
				ainfo.ptype = ptype;
				pattern[size++] = ainfo;
				attacks |= ainfo.attacks;
			}
			ops::pop_lsb(piece_occ);
		}
		return attacks;
	}


	template<typename GenFn>
	static bitboard gen_all_attacks_from_piece_type(bitboard piece_occ, bitboard occ, GenFn fn)
	{
		uint idx = 0;
		bitboard attacks = 0ULL;
		while (piece_occ != 0ULL)
		{
			idx = ops::num_trailing_zeros(piece_occ);
			attacks |= fn(idx);
			ops::pop_lsb(piece_occ);
		}
		return attacks;
	}

	template<typename GenFn>
	static bitboard gen_all_attacks_from_sliding_piece_type(bitboard piece_occ, bitboard occ, GenFn fn)
	{
		uint idx = 0;
		bitboard attacks = 0ULL;
		while (piece_occ != 0ULL)
		{
			idx = ops::num_trailing_zeros(piece_occ);
			attacks |= fn(occ, idx);
			piece_occ &= piece_occ - 1;
		}
		return attacks;
	}


	template<color VColor>
	void extract_board(board_info& info)
	{
		constexpr color ecolor = invert_color(VColor);
		info.own_bishops = b.get_board(piece_type::bishop, VColor);
		info.own_king = b.get_board(piece_type::king, VColor);
		info.own_knights = b.get_board(piece_type::knight, VColor);
		info.own_pawns = b.get_board(piece_type::pawn, VColor);
		info.own_queens = b.get_board(piece_type::queen, VColor);
		info.own_rooks = b.get_board(piece_type::rook, VColor);

		info.enemy_bishops = b.get_board(piece_type::bishop, ecolor);
		info.enemy_king = b.get_board(piece_type::king, ecolor);
		info.enemy_knights = b.get_board(piece_type::knight, ecolor);
		info.enemy_pawns = b.get_board(piece_type::pawn, ecolor);
		info.enemy_queens = b.get_board(piece_type::queen, ecolor);
		info.enemy_rooks = b.get_board(piece_type::rook, ecolor);

		info.own_color_occ = info.own_king | info.own_knights | info.own_bishops | info.own_queens | info.own_rooks | info.own_pawns;
		info.enemy_color_occ = info.enemy_bishops | info.enemy_king | info.enemy_knights | info.enemy_queens | info.enemy_rooks | info.enemy_pawns;
		info.not_own_color_occ = ~info.own_color_occ;
		info.occ = info.own_color_occ | info.enemy_color_occ;
	}
};

