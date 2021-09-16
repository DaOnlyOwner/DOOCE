#pragma once

#include <bitset>
#include <vector>
#include <array>
#include <cstdint>
#include "definitions.h"
#include "move.h"
#include "mailbox.h"
#include <string>


// starting positions

struct board
{



	board();
	board(const std::string& board);

	bitboard get_start(piece_type type, color player);


	static void init_knight_attacks();
	static void init_hq_masks();
	static void init_king_attacks();
	// Attack generation.
	inline bitboard gen_attacks_king(uint nat_idx, bitboard not_own_color_occ) { return not_own_color_occ & king_attacks[nat_idx]; };
	
	inline bitboard gen_attack_pawns_left_white(bitboard wpawns, bitboard black_occ)
	{
		return ops::no_we(wpawns) & black_occ;
	}

	inline bitboard gen_attack_pawns_left_black(bitboard bpawns, bitboard white_occ)
	{
		return ops::so_we(bpawns) & white_occ;
	}

	inline bitboard gen_attack_pawns_right_white(bitboard wpawns, bitboard black_occ)
	{
		return ops::no_ea(wpawns) & black_occ;
	}

	inline bitboard gen_attack_pawns_right_black(bitboard bpawns, bitboard white_occ)
	{
		return ops::so_ea(bpawns) & white_occ;
	}

	inline bitboard gen_move_pawns_single_white(bitboard wpawns, bitboard notOcc)
	{
		return ops::no(wpawns) & notOcc;
	}

	inline bitboard gen_move_pawns_single_black(bitboard bpawns, bitboard notOcc)
	{
		return ops::no(bpawns) & notOcc;
	}

	inline bitboard gen_move_pawns_dbl_white(bitboard wpawns, bitboard notOcc)
	{
		return ops::no<2>(ops::mask_rank(2) & wpawns) & notOcc & ops::no(notOcc); // & mask_rank -> only second rank pawns can move double, & notOcc masks out everything on the square 1 forward, ops::no(notOcc) masks out everything on the square the pawn wants to go (2 forward).
	}

	inline bitboard gen_move_pawns_dbl_black(bitboard bpawns, bitboard notOcc)
	{
		return ops::so<2>(ops::mask_rank(7) & bpawns) & notOcc & ops::so(notOcc);
	}

	// This method expects as input an occurancy bitboard where only pawns are set that can be captured en passant, i.e. that moved double.
	inline bitboard gen_en_passant_left_white(bitboard wpawns, bitboard bpawns_on_en_passant_square)
	{
		return (ops::no_we(wpawns & ops::mask_rank(5)) & ops::no(bpawns_on_en_passant_square)); // & ops::mask_rank(5) because we can only en passant pawns on rank 5, no_we because we capture to the left, so(bpawns_on_en_passant_square) to move the black pawns back so that it creates a mask on the capture square. 
	}

	inline bitboard gen_en_passant_right_white(bitboard wpawns, bitboard bpawns_on_en_passant_square)
	{
		return (ops::no_ea(wpawns & ops::mask_rank(5)) & ops::no(bpawns_on_en_passant_square)); 
	}

	inline bitboard gen_en_passant_left_black(bitboard bpawns, bitboard wpawns_on_en_passant_square)
	{
		return (ops::so_we(bpawns & ops::mask_rank(4)) & ops::so(wpawns_on_en_passant_square)); 
	}

	// inline bitboard gen_en_passant_left_black(bitboard bpawns, bitboard wpawns_on_en_passant_square)
	// {
	// 	return (ops::so_ea(bpawns & ops::mask_rank(4)) & ops::so(wpawns_on_en_passant_square)); 
	// }


	inline bitboard gen_attacks_knight(uint nat_idx, bitboard not_own_color_occ) { return (not_own_color_occ & knight_attacks[nat_idx]); };
	inline bitboard gen_attacks_bishop(bitboard occ, uint natural_idx, bitboard not_own_color_occ)
	{
		auto& mask = hq_masks[natural_idx];
		return not_own_color_occ & (ops::hyperbola_quintessence(occ, mask.diagEx, mask.mask) |
			ops::hyperbola_quintessence(occ, mask.antidiagEx, mask.mask));
	}
	inline bitboard gen_attacks_rook(bitboard occ, uint natural_idx, bitboard not_own_color_occ)
	{
		auto& hq_mask = hq_masks[natural_idx];

		bitboard file_attacks = ops::hyperbola_quintessence(occ, hq_mask.fileEx, hq_mask.mask);
		bitboard rank_attacks = ops::hyperbola_quintessence_for_ranks(occ, hq_mask.rankEx, hq_mask.mask);
		return not_own_color_occ &(file_attacks | rank_attacks);
	}
	inline bitboard gen_attacks_queen(bitboard occ, uint nat_idx, bitboard not_own_color_occ)
	{
		auto& hq_mask = hq_masks[nat_idx];
		bitboard attacks = ops::hyperbola_quintessence(occ, hq_mask.diagEx, hq_mask.mask);
		attacks |= ops::hyperbola_quintessence(occ, hq_mask.antidiagEx, hq_mask.mask);
		attacks |= ops::hyperbola_quintessence(occ, hq_mask.fileEx, hq_mask.mask);
		attacks |= ops::hyperbola_quintessence_for_ranks(occ, hq_mask.rankEx, hq_mask.mask);
		return attacks & not_own_color_occ;
	}

	inline uint is_square_attacked(bitboard attacks, square sq)
	{
		return attacks & (~ops::set_square_bit(sq));
	}

	inline bool can_castle_kingside_white(bitboard occ, bitboard attacks)
	{
		return ~ops::get_bit_from_sq(occ, square::f1) &
			~ops::get_bit_from_sq(occ, square::g1) &
			~is_square_attacked(attacks,square::f1) &
			~is_square_attacked(attacks,square::g1);
	}

	inline bool can_castle_kingside_black(bitboard occ, bitboard attacks)
	{
		return ~ops::get_bit_from_sq(occ, square::f8)&
			~ops::get_bit_from_sq(occ, square::g8)&
			~is_square_attacked(attacks, square::f8)&
			~is_square_attacked(attacks, square::g8);
	}

	inline bool can_castle_queenside_white(bitboard occ, bitboard attacks)
	{
		return ~ops::get_bit_from_sq(occ, square::d1)&
			~ops::get_bit_from_sq(occ, square::c1)&
			~ops::get_bit_from_sq(occ, square::b1)&
			~is_square_attacked(attacks, square::b1)&
			~is_square_attacked(attacks, square::c1)&
			~is_square_attacked(attacks, square::d1);
	}

	inline bool can_castle_queenside_black(bitboard occ, bitboard attacks)
	{
		return ~ops::get_bit_from_sq(occ, square::d8) &
			~ops::get_bit_from_sq(occ, square::c8) &
			~ops::get_bit_from_sq(occ, square::b8) &
			~is_square_attacked(attacks, square::b8) &
			~is_square_attacked(attacks, square::c8) &
			~is_square_attacked(attacks, square::d8);
	}

	// Returns true if the last move was a double pawn push, i.e. the pawn is now on an en passantable square.
	bool do_move_black(const move& m);
	bool do_move_white(const move& m);

	void undo_move_black(const move& m);
	void undo_move_white(const move& m);

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

	bitboard white_board;
	bitboard black_board;
	bitboard en_passantable_pawns_white;
	bitboard en_passantable_pawns_black;

	mailbox mb_board;
	
	static std::array<bitboard, 64> knight_attacks; // Knights
	static std::array<bitboard, 64> king_attacks;
	static std::array<hq_mask, 64> hq_masks; // Sliding pieces


	/*
	// From here: https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
	std::array<unsigned char, 256> reversed_lt
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
	};*/

};


