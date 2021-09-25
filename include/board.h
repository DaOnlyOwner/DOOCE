#pragma once

#include <bitset>
#include <vector>
#include <array>
#include <cstdint>
#include "definitions.h"
#include "move.h"
#include <string>

struct board_info
{
	bitboard own_bishops;
	bitboard own_king;
	bitboard own_knights;
	bitboard own_pawns;
	bitboard own_queens;
	bitboard own_rooks;
	bitboard enemy_bishops;
	bitboard enemy_king;
	bitboard enemy_knights;
	bitboard enemy_queens;
	bitboard enemy_pawns;
	bitboard enemy_rooks;
	bitboard own_color_occ;
	bitboard enemy_color_occ;
	bitboard not_own_color_occ;
	bitboard occ;
	bitboard not_occ;
};

struct board
{

	board();
	board(const std::string& start, bool fen=false);
	void print() const ;
	std::string get_fen() const;

	static void print_bitboard(bitboard b);

	static void init_all();
	static void init_knight_attacks();
	static void init_hq_masks();
	static void init_king_attacks();
	// Attack generation.
	static inline bitboard gen_attacks_king(uint nat_idx) { return king_attacks[nat_idx]; };

	template<color VColor>
	static inline bitboard gen_attack_pawns_left(bitboard own_pawns, bitboard enemy_occ)
	{
		if constexpr (VColor == color::white)
			return ops::no_we(own_pawns) & enemy_occ;
		else return ops::so_we(own_pawns) & enemy_occ;
	}

	template<color VColor>
	static inline bitboard gen_attack_pawns_right(bitboard own_pawns, bitboard enemy_occ)
	{
		if constexpr (VColor == color::white)
			return ops::no_ea(own_pawns) & enemy_occ;
		else return ops::so_ea(own_pawns) & enemy_occ;
	}

	template<color VColor>
	static inline bitboard gen_move_pawns_single(bitboard own_pawns, bitboard notOcc)
	{
		if constexpr (VColor == color::white)
			return ops::no(own_pawns) & notOcc;
		else return ops::so(own_pawns)& notOcc;
	}

	template<color VColor>
	static inline bitboard gen_move_pawns_dbl(bitboard own_pawns, bitboard notOcc)
	{
		if constexpr (VColor == color::white)
			return ops::no<2>(ops::mask_rank(2) & own_pawns) & notOcc & ops::no(notOcc); // & mask_rank -> only second rank pawns can move double, & notOcc masks out everything on the square 1 forward, ops::no(notOcc) masks out everything on the square the pawn wants to go (2 forward).
		else return ops::so<2>(ops::mask_rank(7) & own_pawns) & notOcc & ops::so(notOcc);
	}


	// This method expects as input an occurancy bitboard where only pawns are set that can be captured en passant, i.e. that moved double.
	template<color VColor>
	static inline bitboard gen_en_passant_left(bitboard own_pawns, bitboard pawns_on_en_passant_square)
	{
		if constexpr (VColor == color::white)
			return (ops::no_we(own_pawns & ops::mask_rank(5)) & ops::no(pawns_on_en_passant_square)); // & ops::mask_rank(5) because we can only en passant pawns on rank 5, no_we because we capture to the left, so(bpawns_on_en_passant_square) to move the black pawns back so that it creates a mask on the capture square. 
		else return (ops::so_we(own_pawns & ops::mask_rank(4)) & ops::so(pawns_on_en_passant_square));
	}

	template<color VColor>
	static inline bitboard gen_en_passant_right(bitboard own_pawns, bitboard pawns_on_en_passant_square)
	{
		if constexpr (VColor == color::white)
			return (ops::no_ea(own_pawns & ops::mask_rank(5)) & ops::no(pawns_on_en_passant_square));
		else return (ops::so_ea(own_pawns & ops::mask_rank(4)) & ops::so(pawns_on_en_passant_square));
	}

	static inline bitboard gen_attacks_knight(uint nat_idx) { return (knight_attacks[nat_idx]); };
	static inline bitboard gen_attacks_bishop(bitboard occ, uint natural_idx)
	{
		auto& mask = hq_masks[natural_idx];
		/*printf("Test: masks \n");
		print_bitboard(occ);
		print_bitboard(mask.diagEx);
		print_bitboard(mask.antidiagEx);
		auto a = ops::hyperbola_quintessence(occ, mask.diagEx, mask.mask);
		auto b = ops::hyperbola_quintessence(occ, mask.antidiagEx, mask.mask);
		print_bitboard(a);
		print_bitboard(b);*/
		return (ops::hyperbola_quintessence(occ, mask.diagEx, mask.mask) |
			ops::hyperbola_quintessence(occ, mask.antidiagEx, mask.mask));
	}
	static inline bitboard gen_attacks_rook(bitboard occ, uint natural_idx)
	{
		constexpr bitboard whole_diag_ex = 9241421688590303745ULL; // Diagonal Ex 

		auto& hq_mask = hq_masks[natural_idx];

		bitboard file_attacks = ops::hyperbola_quintessence(occ, hq_mask.fileEx, hq_mask.mask);
		bitboard rank_attacks = ops::hyperbola_quintessence_for_ranks(occ, whole_diag_ex, hq_mask.mask);
		return (file_attacks | rank_attacks);
	}
	static inline bitboard gen_attacks_queen(bitboard occ, uint nat_idx)
	{
		return gen_attacks_bishop(occ, nat_idx) | gen_attacks_rook(occ, nat_idx);
	}

	static inline bool is_square_attacked(bitboard attacks, square sq)
	{
		return static_cast<bool>(attacks & ops::set_square_bit(sq));
	}

	inline bool is_king_in_check(color c, bitboard attacks)
	{
		square king_square = idx_to_sq(ops::num_trailing_zeros(
			get_board(piece_type::king, c)));
		return is_square_attacked(attacks, king_square);
	}

	// TODO: Here add the king square to is_square_attacked.
	template<color VColor>
	static inline bool can_castle_kingside(bitboard occ, bitboard attacks)
	{
		if constexpr (VColor == color::white)
			return !ops::is_square_set(occ, square::f1) &&
				!ops::is_square_set(occ, square::g1) &&
				!is_square_attacked(attacks, square::f1) &&
				!is_square_attacked(attacks, square::g1) &&
				!is_square_attacked(attacks, square::e1);
		
		else
			return !ops::is_square_set(occ, square::f8) &&
			!ops::is_square_set(occ, square::g8) &&
			!is_square_attacked(attacks, square::f8) &&
			!is_square_attacked(attacks, square::g8) &&
			!is_square_attacked(attacks, square::e8);

	}

	template<color VColor>
	static inline bool can_castle_queenside(bitboard occ, bitboard attacks)
	{
		if constexpr (VColor == color::white)
			return !ops::is_square_set(occ, square::d1) &&
			!ops::is_square_set(occ, square::c1) &&
			!ops::is_square_set(occ, square::b1) &&
			!is_square_attacked(attacks, square::c1) &&
			!is_square_attacked(attacks, square::d1) &&
			!is_square_attacked(attacks, square::e1);
		else
			return !ops::is_square_set(occ, square::d8) &&
			!ops::is_square_set(occ, square::c8) &&
			!ops::is_square_set(occ, square::b8) &&
			!is_square_attacked(attacks, square::c8) &&
			!is_square_attacked(attacks, square::d8) &&
			!is_square_attacked(attacks, square::e8);

	}

	bitboard& get_board(piece_type ptype, color c)
	{
		return boards[static_cast<uint>(ptype)][static_cast<uint>(c)];
	}

	const bitboard& get_board_const(piece_type ptype, color c) const
	{
		return boards[static_cast<uint>(ptype)][static_cast<uint>(c)];
	}


	inline void handle_quiet_move(bitboard& own, bitboard from, bitboard to)
	{
		own = (own | to) & ~from;
	}

	// Returns true if the last move was a double pawn push, i.e. the pawn is now on an en passantable square.
	template<color VColor>
	void do_move(const move& m)
	{
		constexpr color ecolor = invert_color(VColor);
		bitboard& own = get_board(m.get_moved_piece_type(),VColor);
		bitboard from = m.get_from();
		bitboard to = m.get_to();

		switch (m.get_move_type())
		{
		case move_type::promo:
		{
			own &= ~from;
			bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
			promo_board |= to;
		}
		break;

		case move_type::promo_captures:
		{
			own &= ~from;
			bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
			promo_board |= to;
			bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
			enemy &= ~to;
		}
		break;

		case move_type::captures:
		{
			handle_quiet_move(own, from, to);
			bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
			enemy &= ~to;
		}
		break;

		case move_type::pawn_single:
		case move_type::pawn_double:
		case move_type::quiet:
			handle_quiet_move(own, from, to);
			break;
		case move_type::en_passant:
		{
			handle_quiet_move(own, from, to);
			bitboard& enemy = get_board(piece_type::pawn, ecolor);
			if constexpr (VColor == color::white)
				enemy &= ~ops::so(to);
			else enemy &= ~ops::no(to);
		}
		break;
		case move_type::king_castle:
			// own == king
		{
			constexpr auto rook_from_square = VColor == color::white ? square::h1 : square::h8;
			constexpr auto rook_to_square = VColor == color::white ? square::f1 : square::f8;
			constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
			constexpr auto king_to_square = VColor == color::white ? square::g1 : square::g8;
			constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
			constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
			constexpr bitboard king_from = ops::set_square_bit(king_from_square);
			constexpr bitboard king_to = ops::set_square_bit(king_to_square);

			bitboard& rooks = get_board(piece_type::rook, VColor);
			handle_quiet_move(rooks, rook_from, rook_to);
			handle_quiet_move(own, king_from, king_to);			
		}
		break;
		// own == king
		case move_type::queen_castle:
		{
			constexpr auto rook_from_square = VColor == color::white ? square::a1 : square::a8;
			constexpr auto rook_to_square = VColor == color::white ? square::d1 : square::d8;
			constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
			constexpr auto king_to_square = VColor == color::white ? square::c1 : square::c8;
			constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
			constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
			constexpr bitboard king_from = ops::set_square_bit(king_from_square);
			constexpr bitboard king_to = ops::set_square_bit(king_to_square);

			bitboard& rooks = get_board(piece_type::rook, VColor);
			handle_quiet_move(rooks, rook_from, rook_to);
			handle_quiet_move(own, king_from, king_to);
		}
		break;
		}
	}

	template<color VColor>
	void undo_move(const move& m)
	{
		constexpr color ecolor = static_cast<color>(1 - static_cast<uint>(VColor));
		bitboard& own = get_board(m.get_moved_piece_type(), VColor);
		bitboard from = m.get_from();
		bitboard to = m.get_to();

		switch (m.get_move_type())
		{
		case move_type::promo:
		{
			own |= from;
			bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
			promo_board &= ~to;
		}
		break;

		case move_type::promo_captures:
		{
			own |= from;
			bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
			promo_board &= ~to;
			bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
			enemy |= to;
		}
		break;

		case move_type::captures:
		{
			handle_quiet_move(own, to, from);
			bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
			enemy |= to;
		}
		break;

		case move_type::pawn_single:
		case move_type::pawn_double:
		case move_type::quiet:
			handle_quiet_move(own, to, from);
			break;
		case move_type::en_passant:
		{
			handle_quiet_move(own, to, from);
			bitboard& enemy = get_board(piece_type::pawn, ecolor);
			if constexpr (VColor == color::white)
				enemy |= ops::so(to);
			else enemy |= ops::no(to);
		}
		break;
		case move_type::king_castle:
			// own == king
		{
			constexpr auto rook_from_square = VColor == color::white ? square::h1 : square::h8;
			constexpr auto rook_to_square = VColor == color::white ? square::f1 : square::f8;
			constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
			constexpr auto king_to_square = VColor == color::white ? square::g1 : square::g8;
			constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
			constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
			constexpr bitboard king_from = ops::set_square_bit(king_from_square);
			constexpr bitboard king_to = ops::set_square_bit(king_to_square);

			bitboard& rooks = get_board(piece_type::rook, VColor);
			handle_quiet_move(rooks, rook_to, rook_from);
			handle_quiet_move(own, king_to, king_from);
		}
		break;
		// own == king
		case move_type::queen_castle:
		{
			constexpr auto rook_from_square = VColor == color::white ? square::a1 : square::a8;
			constexpr auto rook_to_square = VColor == color::white ? square::d1 : square::d8;
			constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
			constexpr auto king_to_square = VColor == color::white ? square::c1 : square::c8;
			constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
			constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
			constexpr bitboard king_from = ops::set_square_bit(king_from_square);
			constexpr bitboard king_to = ops::set_square_bit(king_to_square);

			bitboard& rooks = get_board(piece_type::rook, VColor);
			handle_quiet_move(rooks, rook_to, rook_from);
			handle_quiet_move(own, king_to, king_from);
		}
		break;
		}
	}

	struct hq_mask
	{
		bitboard mask;
		bitboard diagEx;
		bitboard antidiagEx;
		bitboard fileEx;
		typedef std::array<hq_mask, 64> lt;
	};
	typedef std::array<bitboard, 64> move_lt;

	bitboard boards[6][2];
	
	
	static std::array<bitboard, 64> knight_attacks; // Knights
	static std::array<bitboard, 64> king_attacks;
	static std::array<hq_mask, 64> hq_masks; // Sliding pieces
};


