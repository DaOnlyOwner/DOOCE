#pragma once
#include "definitions.h"

enum class move_type : unsigned char
{
	quiet=0,
	pawn_single,
	pawn_double,
	king_castle,
	queen_castle,
	captures,
	en_passant,
	knight_promo,
	bishop_promo,
	rook_promo,
	queen_promo,
	knight_promo_capture,
	bishop_promo_capture,
	rook_promo_capture,
	queen_promo_capture
};

struct move
{
	unsigned char from, to; // from and to are from bottom right = 0 to top left = 64.
	piece_type piece_moved;
	move_type type_of_move;
	piece piece_captured;
	bool was_en_passantable;
};

