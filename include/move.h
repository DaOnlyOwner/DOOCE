#pragma once
#include "definitions.h"
#include "bitwise_ops.h"

enum class move_type : uint
{
	quiet=0,
	//pawn_single,
	//pawn_double,
	king_castle,
	queen_castle,
	captures,
	en_passant,
	promo,
	promo_captures
};

struct move
{
	uint from, to;
	piece_type moved;
	//std::optional<piece_type> captured;
	//std::optional<piece_type> promo;
	piece_type captured;
	piece_type promo;
	move_type mtype;


	void set_from(uint from)
	{
		this->from = from;
	}

	void set_to(uint to)
	{
		this->to = to;
	}

	void set_promo_piece_type(const piece_type ptype)
	{
		promo = ptype;
	}

	void set_captured_piece_type(const piece_type ctype)
	{
		captured = ctype;
	}

	void set_moved_piece_type(piece_type moved)
	{
		this->moved = moved;
	}

	void set_move_type(move_type mtype)
	{
		this->mtype = mtype;
	}

	bitboard get_from()
	{
		return ops::set_nth_bit(from);
	}

	bitboard get_to()
	{
		return ops::set_nth_bit(to);
	}

	piece_type get_moved_piece_type()
	{
		return moved;
	}

	// Only call when checked that move_type == captures or promo_captures
	piece_type get_captured_piece_type()
	{
		return captured;
	}

	// Only call when move_type == promo or promo_captures
	piece_type get_promo_piece_type()
	{
		return promo;
	}
};

