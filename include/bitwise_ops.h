#pragma once
#include "definitions.h"

namespace ops
{



	// From here: https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
	constexpr std::array<unsigned char, 256> reversed_lt
	{
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
	};

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

	// Basically gets the index of the least significant set bit 
	inline uint num_trailing_zeros(bitboard b)
	{
		unsigned long idx;
		_BitScanForward64(&idx, b);
		return idx;
	}

	inline uint flip_idx(uint idx)
	{
		return 63 - idx;
	}

	inline bool contains(int x, int y)
	{
		return x < 8 && y < 8 && x >= 0 && y >= 0;
	}

	inline bitboard rev_bits(bitboard b)
	{
		return reversed_lt[b];
	}

	inline bitboard hyperbola_quintessence(bitboard occ, bitboard attack_mask, bitboard slider)
	{
		bitboard left;
		left = occ & attack_mask;
		return ((left - 2*slider) ^ _byteswap_uint64(_byteswap_uint64(left) - 2*_byteswap_uint64(slider))) & attack_mask;
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

