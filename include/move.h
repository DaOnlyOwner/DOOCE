#pragma once
#include "definitions.h"
#include "bitwise_ops.h"
#include <optional>

enum class move_type : uint
{
	quiet=0,
	pawn_single=1,
	pawn_double=2,
	king_castle=3,
	queen_castle=4,
	captures=5,
	en_passant=6,
	promo=7,
	promo_captures=8
};

struct move
{
	uint from, to;
	piece_type moved;
	std::optional<piece_type> captured; // TODO: don't make this optional, only use this if the mtype indicates it's a capture, otherwise ignore
	std::optional<piece_type> promo;    // TODO: don't make this optional, only use this if the mtype indicates it's a promo, otherwise ignore
	move_type mtype;

	move() = default;

	move(uint from, uint to, piece_type moved, piece_type captured, piece_type promo, move_type mtype)
		:from(from),to(to),moved(moved),captured(captured),promo(promo),mtype(mtype){}

	// TODO: remove all those getters / setters, this struct is supposed to contain no logic, just data

	void set_from(uint from)
	{
		this->from = from;
	}

	void set_to(uint to)
	{
		this->to = to;
	}

	void set_promo_piece_type(const std::optional<piece_type>& ptype)
	{
		promo = ptype;
	}

	void set_captured_piece_type(const std::optional<piece_type>& ctype)
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

	bitboard get_from() const
	{
		return ops::set_nth_bit(from);
	}

	bitboard get_to() const
	{
		return ops::set_nth_bit(to);
	}

	uint get_from_as_idx() const
	{
		return from;
	}

	uint get_to_as_idx() const
	{
		return to;
	}

	move_type get_move_type() const
	{
		return mtype;
	}

	piece_type get_moved_piece_type() const
	{
		return moved;
	}

	// Only call when checked that move_type == captures or promo_captures
	piece_type get_captured_piece_type() const
	{
		return captured.value();
	}

	// Only call when move_type == promo or promo_captures
	piece_type get_promo_piece_type() const
	{
		return promo.value();
	}
};
