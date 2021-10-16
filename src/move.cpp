#include "move.h"

bool operator==(const move& lhs, const move& rhs)
{
	return lhs.data == rhs.data;
}

move::move(uint from, uint to, piece_type moved, piece_type captured, piece_type promo, move_type mtype)
{
	set_from(from);
	set_to(to);
	set_moved_piece_type(moved);
	set_captured_piece_type(captured);
	set_promo_piece_type(promo);
	set_move_type(mtype);
}

void move::set_from(uint from)
{
	assert(from < 64);
	data &= not_from_bits;
	data |= from;
	assert(get_from_as_idx() == from);
}

void move::set_to(uint to)
{
	assert(to < 64);
	data &= not_to_bits;
	data |= static_cast<u32>(to) << to_shift;
	assert(get_to_as_idx() == to);
}

void move::set_promo_piece_type(piece_type ptype)
{
	u32 i = static_cast<u32>(ptype);
	data &= not_promo_piece_type_bits;
	data |= i << promo_shift;
	assert(get_promo_piece_type() == ptype);
}

void move::set_captured_piece_type(piece_type ctype)
{
	u32 i = static_cast<u32>(ctype);
	data &= not_capt_piece_type_bits;
	data |= i << capt_shift;
	assert(get_captured_piece_type() == ctype);
}

void move::set_moved_piece_type(piece_type moved)
{
	u32 i = static_cast<u32>(moved);
	data &= not_moved_piece_type_bits;
	data |= i << moved_shift;
	assert(get_moved_piece_type() == moved);
}

void move::set_move_type(move_type mtype)
{
	u32 i = static_cast<u32>(mtype);
	data &= not_mtype_bits;
	data |= i << mtype_shift;
	assert(get_move_type() == mtype);
}

bitboard move::get_from_as_bitboard() const
{
	return ops::set_nth_bit(get_from_as_idx());
}

bitboard move::get_to_as_bitboard() const
{
	return ops::set_nth_bit(get_to_as_idx());
}

uint move::get_from_as_idx() const
{
	return data & from_bits;
}

uint move::get_to_as_idx() const
{
	return (data >> to_shift) & from_bits;
}

move_type move::get_move_type() const
{
	return static_cast<move_type>((data >> mtype_shift) & 15);
}

piece_type move::get_moved_piece_type() const
{
	return static_cast<piece_type>((data >> moved_shift) & 7);
}

// Only call when checked that move_type == captures or promo_captures

piece_type move::get_captured_piece_type() const
{
	return static_cast<piece_type>((data >> capt_shift) & 7);
}

// Only call when move_type == promo or promo_captures

piece_type move::get_promo_piece_type() const
{
	return static_cast<piece_type>((data >> promo_shift) & 7);
}
