#pragma once
#include "setwise_ops.h"
#include "definitions.h"
#include <array>

namespace move_gen_helper
{
	struct hq_mask
	{
		bitboard mask;
		bitboard diagEx;
		bitboard antidiagEx;
		bitboard fileEx;
		bitboard rankEx;
		typedef std::array<hq_mask, 64> lt;
	};
	typedef std::array<bitboard, 64> move_lt;

	move_lt init_knight_attacks();
	hq_mask::lt init_hq_masks();
	move_lt init_king_attacks();

	// Attack generation.
	inline bitboard gen_attacks_king(square s, const move_lt& lt) { return lt[sq_to_int(s)]; }
	inline bitboard gen_attacks_knight(square s, const move_lt& lt) { return lt[sq_to_int(s)]; }
	
	template<color VColor>
	bitboard gen_move_pawns_single(bitboard occ)
	{
		return ops::no(occ);
	}

	template<>
	bitboard gen_move_pawns_single<color::black>(bitboard occ)
	{
		return ops::so(occ);
	}

	template<color VColor>
	bitboard gen_move_pawns_double(bitboard occ)
	{
		// mask everything except for the second rank
		return ops::no(ops::mask_rank(1) & occ);
	}

	template<>
	bitboard gen_move_pawns_double<color::black>(bitboard occ)
	{
		return ops::so(ops::mask_rank(6) & occ);
	}

	template<color VColor>
	bitboard gen_attack_pawns_left(bitboard occ)
	{
		return ops::no_we(occ);
	}

	template<>
	bitboard gen_attack_pawns_left<color::black>(bitboard occ)
	{
		return ops::so_we(occ);
	}

	template<color VColor>
	bitboard gen_attack_pawns_right(bitboard occ)
	{
		return ops::no_ea(occ);
	}

	template<>
	bitboard gen_attack_pawns_right<color::black>(bitboard occ)
	{
		return ops::so_ea(occ);
	}


	// Sliding pieces
	bitboard gen_attacks_bishop(bitboard occ, square s, const hq_mask::lt& masks);
	bitboard gen_attacks_rook(bitboard occ, square s, const hq_mask::lt& masks);
	bitboard gen_attacks_queen(bitboard occ, square s, const hq_mask::lt& masks);
}
