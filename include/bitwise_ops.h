#pragma once
#include "definitions.h"

#ifdef __GNUC__
    #include <byteswap.h>
#elif _MSC_VER
    #define bswap_64(x) (_byteswap_uint64((x)))
#endif

namespace ops
{
	constexpr bitboard notAFile = 0xfefefefefefefefe;
	constexpr bitboard notHFile = 0x7f7f7f7f7f7f7f7f;
	constexpr bitboard mask_rank(int r)
	{
		r -= 1; // first rank == index 0 etc.
		bitboard b = 0b11111111;
		return b << (r * 8);
	}

	inline bool has_bit_set_on_rank(bitboard b, int r)
	{
		return (ops::mask_rank(r) ^ b) == b;
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

	inline square to_sq(uint x, uint y)
	{
		return static_cast<square>(y * 8 + x);
	}

	inline uint to_idx(uint x, uint y)
	{
		return y * 8 + x;
	}

	inline std::pair<uint, uint> from_sq(square sq)
	{
		uint s = sq_to_int(sq);
		int y = s  >> 3;
		int x = s & 7;
		return std::make_pair(x, y);
	}

	inline bool is_square_set(bitboard b, square sq)
	{
		uint idx = sq_to_int(sq);
		return static_cast<bool>((b >> idx) & 1);
	}

	constexpr inline bitboard set_square_bit(square sq)
	{
		uint idx = sq_to_int(sq);
		return 1ULL << idx;
	}

	inline bitboard set_nth_bit(uint n)
	{
		return 1ULL << n;
	}

	inline std::pair<uint, uint> from_idx(uint s)
	{
		int y = s >> 3;
		int x = s & 7;
		return std::make_pair(x, y);
	}

// Get the index of the least significant set bit.
#ifdef __GNUC__
    inline uint num_trailing_zeros(bitboard b)
	{
		return __builtin_ctzll(b);
	}
#elif _MSC_VER
	inline uint num_trailing_zeros(bitboard b)
	{
		unsigned long idx;
		_BitScanForward64(&idx, b);
		return idx;
	}
#endif

	inline uint flip_idx(uint idx)
	{
		return 63 - idx;
	}

	inline bool contains(int x, int y)
	{
		return x < 8 && y < 8 && x >= 0 && y >= 0;
	}

	inline bitboard hyperbola_quintessence(bitboard occ, bitboard attack_mask, bitboard slider)
	{
		bitboard left;
		left = occ & attack_mask;
		return ((left - 2 * slider) ^ bswap_64(bswap_64(left) - 2 * bswap_64(slider))) & attack_mask;
	}

	// http://timcooijmans.blogspot.com/2014/04/hyperbola-quintessence-for-rooks-along.html
	inline bitboard hyperbola_quintessence_for_ranks(bitboard occ, bitboard attack_mask, bitboard slider)
	{
		uint shift_down = ops::num_trailing_zeros(slider) & 56; // == ((slider) >> 3) << 3, slider >> 3 gets the rank, << 3 multiplies the rank by 8, the number of bits we have to shift down.
		constexpr bitboard first_rank_masked = ops::mask_rank(1);
		bitboard mapped = (((occ >> shift_down) & first_rank_masked) * 0x0101010101010101) & 0x8040201008040201; // & ops::mask_rank(1) -> See the comment on the page
		slider = ((slider >> shift_down) * 0x0101010101010101) & 0x8040201008040201;
		bitboard attacks = hyperbola_quintessence(mapped, attack_mask, slider);
		return ((attacks *0x0101010101010101 / 0x0100000000000000)) << shift_down;
	}

	inline void pop_lsb(bitboard& b)
	{
		 b &= (b - 1);
	}

}

