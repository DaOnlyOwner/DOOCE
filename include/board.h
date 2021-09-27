#pragma once

#include "definitions.h"
#include "bitwise_ops.h"
#include "move.h"
#include "fen.h"

#include <bitset>
#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <cstring>
#include <map>
#include <cstdio>
#include <ctype.h>
#include <algorithm>
#include <stdexcept>

struct board
{
	board();
	board(const std::string& start, bool fen = false);

	bitboard boards[6][2];
	
	void print() const;
	static void print_bitboard(bitboard b);

	// ============================================================
	//              B I T B O A R D   H E L P E R S
	// ============================================================

	inline bitboard& get_board(piece_type ptype, color c)
	{
		return boards[static_cast<uint>(ptype)][static_cast<uint>(c)];
	}

	inline bitboard get_boards_of_side(color c)
	{
		return get_board(piece_type::king, c)
		    | get_board(piece_type::queen, c)
			| get_board(piece_type::rook, c)
			| get_board(piece_type::bishop, c)
			| get_board(piece_type::knight, c)
			| get_board(piece_type::pawn, c);
	}

	const bitboard& get_board_const(piece_type ptype, color c) const
	{
		return boards[static_cast<uint>(ptype)][static_cast<uint>(c)];
	}

	// ============================================================
	//           A P P L Y   /   R E V E R T   M O V E
	// ============================================================

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
			case move_type::king_castle: // own == king
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
};
