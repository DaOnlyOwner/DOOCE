#pragma once
#include "definitions.h"
#include "bitwise_ops.h"

enum class move_type : unsigned char
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
	unsigned int from, to;
	piece_type captured;
	piece_type moved;
	piece_type promo;

	move(uint from, uint to, piece_type captured, piece_type moved, piece_type promo)
		: from(from),to(to),captured(captured),moved(moved),promo(promo)
	{}

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

	piece_type get_captured_piece_type()
	{
		return captured;
	}

	piece_type get_promo_piece_type()
	{
		return promo;
	}
};

