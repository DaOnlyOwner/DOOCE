#pragma once
#include "definitions.h"
#include "bitwise_ops.h"
#include <optional>
#include <cassert>

class move
{
public:
	move() = default;

	move(uint from, uint to, piece_type moved, piece_type captured, piece_type promo, move_type mtype);

	// TODO: Move the getter and setter to the cpp file

	void set_from(uint from);

	void set_to(uint to);

	void set_promo_piece_type(piece_type ptype);

	void set_captured_piece_type(piece_type ctype);

	void set_moved_piece_type(piece_type moved);

	void set_move_type(move_type mtype);

	bitboard get_from_as_bitboard() const;

	bitboard get_to_as_bitboard() const;

	uint get_from_as_idx() const;

	uint get_to_as_idx() const;

	move_type get_move_type() const;

	piece_type get_moved_piece_type() const;

	// Only call when checked that move_type == captures or promo_captures
	piece_type get_captured_piece_type() const;

	// Only call when move_type == promo or promo_captures
	piece_type get_promo_piece_type() const;
private:
	u32 data = 20668416; // from = 0, to = 0, capt = none, mtype = none, promo=none,moved=none
	static constexpr u32 from_bits = 63;
	static constexpr u32 not_from_bits = ~from_bits;
	static constexpr u32 from_shift = 0;

	static constexpr u32 to_shift = 6;
	static constexpr u32 to_bits = from_bits << to_shift;
	static constexpr u32 not_to_bits = ~to_bits;

	static constexpr u32 promo_shift = (6 + 6);
	static constexpr u32 promo_piece_type_bits = 7 << promo_shift;
	static constexpr u32 not_promo_piece_type_bits = ~promo_piece_type_bits;

	static constexpr u32 capt_shift = (6 + 6 + 3);
	static constexpr u32 capt_piece_type_bits = 7 << capt_shift;
	static constexpr u32 not_capt_piece_type_bits = ~capt_piece_type_bits;

	static constexpr u32 moved_shift = (6 + 6 + 3 + 3);
	static constexpr u32 moved_piece_type_bits = 7 << moved_shift;
	static constexpr u32 not_moved_piece_type_bits = ~moved_piece_type_bits;

	static constexpr u32 mtype_shift = (6 + 6 + 3 + 3 + 3);
	static constexpr u32 mtype_bits = 15 << mtype_shift;
	static constexpr u32 not_mtype_bits = ~mtype_bits;
	friend bool operator==(const move&, const move&);

};

bool operator==(const move& lhs, const move& rhs);


