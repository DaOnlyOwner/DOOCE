#pragma once
#include "definitions.h"

namespace ops
{

	constexpr bitboard notAFile = 0xfefefefefefefefe;
	constexpr bitboard notHFile = 0x7f7f7f7f7f7f7f7f;
	constexpr bitboard mask_rank(int r)
	{
		bitboard b = 0b11111111;
		return b << (r * 8);
	}


	template<int VShiftAmount = 1>
	inline bitboard so(bitboard b)
	{
		return b >> (8 * VShiftAmount);
	}

	template<int VShiftAmount = 1>
	inline bitboard no(bitboard b)
	{
		return b << (8 * VShiftAmount);
	}

	inline bitboard ea(bitboard b)
	{
		// Shifting too far, so that we might end up at the "other" side again (wrapping),
		// is probably not intended. So we zero out all those moves.
		// Thus shifting by two or more positions at once is not efficiently possible
		return (b << 1) & notAFile;  
	}

	inline bitboard no_ea(bitboard b)
	{
		return (b << 9) & notAFile;
	}

	inline bitboard so_ea(bitboard b)
	{
		return (b >> 7) & notAFile;
	}

	inline bitboard we(bitboard b)
	{
		return (b >> 1) & notHFile;
	}

	inline bitboard so_we(bitboard b)
	{
		return (b >> 9) & notHFile;
	}

	inline bitboard no_we(bitboard b)
	{
		return (b << 7) & notHFile;
	}

	inline static square to_sq(uint x, uint y)
	{
		return static_cast<square>(y * 8 + x);
	}

	inline static uint to_idx(uint x, uint y)
	{
		return y * 8 + x;
	}

	inline static std::pair<uint, uint> from_sq(square sq)
	{
		uint s = sq_to_int(sq);
		int y = (s & 56) >> 3;
		int x = s & 7;
		return std::make_pair(x, y);
	}

	inline static std::pair<uint, uint> from_idx(uint s)
	{
		int y = (s & 56) >> 3;
		int x = s & 7;
		return std::make_pair(x, y);
	}


	inline static bool contains(int x, int y)
	{
		return x < 8 && y < 8 && x >= 0 && y >= 0;
	}

	// Note that "mask" has to be premultiplied by 2!	
	template<typename FnRev>
	bitboard hyperbola_quintessence(bitboard occ, bitboard attack_mask, bitboard mask, FnRev)
	{
		bitboard left, right;
		left = occ & attack_mask;
		right = _byteswap_uint64(left);
		left -= mask; // mask is already multiplied by 2!
		right -= _byteswap_uint64(mask);
		left ^= _byteswap_uint64(right);
		return left & attack_mask;
	}
}

