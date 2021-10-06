#pragma once
#include "attack_bitboards.h"

namespace
{
	std::array<bitboard, 64> knight_attacks;
	std::array<bitboard, 64> king_attacks;
	std::array<hq_mask, 64> hq_masks;
}

void gen::init_knight_attacks()
{
	for (uint i = 0; i < 64; i++)
	{
		bitboard_constr b(0);
		auto [x, y] = ops::from_idx(i);

		std::pair<int, int> possible_pos[8];

		possible_pos[0] = { x - 1, y + 2 }; // top_left
		possible_pos[1] = { x + 1, y + 2 }; // top_right
		possible_pos[2] = { x - 2, y + 1 }; // midtop_left
		possible_pos[3] = { x + 2, y + 1 }; // midtop_right

		possible_pos[4] = { x - 2, y - 1 }; // midbot_left
		possible_pos[5] = { x + 2, y - 1 }; // midbot_right
		possible_pos[6] = { x - 1, y - 2 }; // bot_left
		possible_pos[7] = { x + 1, y - 2 }; // bot_right

		for (uint pos = 0; pos < 8; pos++)
		{
			auto [x, y] = possible_pos[pos];
			if (ops::contains(x, y)) b.set(ops::to_idx(x, y), true);
		}

		knight_attacks[i] = b.to_ullong();
	}
}

void gen::init_hq_masks()
{
	for (uint x = 0; x < 8; x++)
	{
		for (uint y = 0; y < 8; y++)
		{
			uint idx = ops::to_idx(x, y);
			hq_mask mask;

			// Init diag and anti diag.
			auto diag = ops::get_diag(x, y);
			auto antidiag = ops::get_antidiag(x, y);
			auto rank = ops::get_rank(x, y);
			auto file = ops::get_file(x, y);
			mask.diagEx = diag.to_ullong();
			mask.antidiagEx = antidiag.to_ullong();
			mask.fileEx = file.to_ullong();
			bitboard_constr mask_constr(0);
			mask_constr.set(idx, true);
			mask.mask = mask_constr.to_ullong();
			hq_masks[idx] = mask;
		}
	}
}

void gen::init_king_attacks()
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			bitboard_constr bc(0);
			uint idx = ops::to_idx(x, y);

			for (int j = -1; j <= 1; j++)
			{
				for (int k = -1; k <= 1; k++)
				{
					int x_off = x + j;
					int y_off = y + k;
					if (ops::contains(x_off, y_off) && !(x_off == x && y_off == y))
					{
						uint idx_inner = ops::to_idx((uint)x_off, (uint)y_off);
						bc.set(idx_inner, true);
					}
				}
			}
			king_attacks[idx] = bc.to_ullong();
		}
	}
}

// ============================================================
//        S L I D E R S   D R A W   G E N E R A T I O N
// ============================================================
bitboard gen::gen_attacks_bishop(const board& b, uint bishop_idx)
{
	bitboard occ = b.get_whole_board();
	auto& mask = hq_masks[bishop_idx];
	return (ops::hyperbola_quintessence(occ, mask.diagEx, mask.mask) |
		ops::hyperbola_quintessence(occ, mask.antidiagEx, mask.mask));
}

bitboard gen::gen_attacks_rook(const board& b, uint rook_idx)
{
	bitboard occ = b.get_whole_board();
	constexpr bitboard whole_diag_ex = 9241421688590303745ULL; // Diagonal Ex 
	auto& hq_mask = hq_masks[rook_idx];

	bitboard file_attacks = ops::hyperbola_quintessence(occ, hq_mask.fileEx, hq_mask.mask);
	bitboard rank_attacks = ops::hyperbola_quintessence_for_ranks(occ, whole_diag_ex, hq_mask.mask);
	return (file_attacks | rank_attacks);
}

bitboard gen::gen_attacks_queen(const board& b, uint nat_idx)
{
	return gen_attacks_bishop(b, nat_idx) | gen_attacks_rook(b, nat_idx);
}

// King attack bits
bitboard gen::gen_attacks_king(uint king_idx)
{
	return king_attacks[king_idx];
}

bitboard gen::gen_attacks_knight(uint knight_idx)
{
	return (knight_attacks[knight_idx]);
}

// Misc functions
bool gen::is_square_attacked(bitboard attacks, square sq)
{
	return static_cast<bool>(attacks & ops::set_square_bit(sq));
}

void gen::init_all()
{
	init_king_attacks();
	init_hq_masks();
	init_knight_attacks();
}

template<color VColor>
bool gen::can_castle_kingside(const board& b, bitboard attacks)
{
	// info: this function assumes that king and rook have not moved yet

	bitboard occ = b.get_whole_board(); // TODO: try to cache this using a member attribute

	if constexpr (VColor == color::white)
		return !ops::is_square_set(occ, square::f1) &&
		!ops::is_square_set(occ, square::g1) &&
		!is_square_attacked(attacks, square::f1) &&
		!is_square_attacked(attacks, square::g1) &&
		!is_square_attacked(attacks, square::e1);
	else
		return !ops::is_square_set(occ, square::f8) &&
		!ops::is_square_set(occ, square::g8) &&
		!is_square_attacked(attacks, square::f8) &&
		!is_square_attacked(attacks, square::g8) &&
		!is_square_attacked(attacks, square::e8);
}

template bool gen::can_castle_kingside<color::white>(const board& b, bitboard attacks);
template bool gen::can_castle_kingside<color::black>(const board& b, bitboard attacks);


template<color VColor>
bool gen::can_castle_queenside(const board& b, bitboard attacks)
{
	// info: this function assumes that king and rook have not moved yet

	bitboard occ = b.get_whole_board(); // TODO: try to cache this using a member attribute

	if constexpr (VColor == color::white)
		return !ops::is_square_set(occ, square::d1) &&
		!ops::is_square_set(occ, square::c1) &&
		!ops::is_square_set(occ, square::b1) &&
		!is_square_attacked(attacks, square::c1) &&
		!is_square_attacked(attacks, square::d1) &&
		!is_square_attacked(attacks, square::e1);
	else
		return !ops::is_square_set(occ, square::d8) &&
		!ops::is_square_set(occ, square::c8) &&
		!ops::is_square_set(occ, square::b8) &&
		!is_square_attacked(attacks, square::c8) &&
		!is_square_attacked(attacks, square::d8) &&
		!is_square_attacked(attacks, square::e8);
}

template bool gen::can_castle_queenside<color::white>(const board& b, bitboard attacks);
template bool gen::can_castle_queenside<color::black>(const board& b, bitboard attacks);

template<color VColor>
bitboard gen::gen_attack_pawns_left(const board& b)
{
	constexpr color opp = invert_color(VColor);
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	bitboard enemy_occ = b.get_board_of_side<opp>(); // TODO: try to cache this using a member attribute

	if constexpr (VColor == color::white)
		return ops::no_we(own_pawns) & enemy_occ;
	else return ops::so_we(own_pawns) & enemy_occ;
}

template bitboard gen::gen_attack_pawns_left<color::white>(const board& b);
template bitboard gen::gen_attack_pawns_left<color::black>(const board& b);

template<color VColor>
bitboard gen::gen_attack_pawns_right(const board& b)
{
	constexpr color opp = invert_color(VColor);
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	bitboard enemy_occ = b.get_board_of_side<opp>(); // TODO: try to cache this using a member attribute

	if constexpr (VColor == color::white)
		return ops::no_ea(own_pawns) & enemy_occ;
	else return ops::so_ea(own_pawns) & enemy_occ;
}

template bitboard gen::gen_attack_pawns_right<color::white>(const board& b);
template bitboard gen::gen_attack_pawns_right<color::black>(const board& b);

template<color VColor>
bitboard gen::gen_move_pawns_single(const board& b)
{
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	bitboard not_occ = ~(b.get_whole_board()); // TODO: try to cache this using a member attribute

	if constexpr (VColor == color::white)
		return ops::no(own_pawns) & not_occ;
	else return ops::so(own_pawns) & not_occ;
}

template bitboard gen::gen_move_pawns_single<color::white>(const board& b);
template bitboard gen::gen_move_pawns_single<color::black>(const board& b);

template<color VColor>
bitboard gen::gen_move_pawns_dbl(const board& b)
{
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	bitboard not_occ = ~b.get_whole_board();

	if constexpr (VColor == color::white)
		// info: '& mask_rank' -> only second rank pawns can move double, & notOcc masks out
		//       everything on the square 1 forward, ops::no(notOcc) masks out everything on
		//       the square the pawn wants to go (2 forward).
		return ops::no<2>(ops::mask_rank(2) & own_pawns) & not_occ & ops::no(not_occ);
	else return ops::so<2>(ops::mask_rank(7) & own_pawns) & not_occ & ops::so(not_occ);
}

template bitboard gen::gen_move_pawns_dbl<color::white>(const board& b);
template bitboard gen::gen_move_pawns_dbl<color::black>(const board& b);

template<color VColor>
bitboard gen::gen_en_passant_left(const board& b, bitboard pawns_on_en_passant_square)
{
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);

	if constexpr (VColor == color::white)
		// info: '& ops::mask_rank(5)' because we can only en passant pawns on rank 5, no_we because we capture to the left,
		// so(bpawns_on_en_passant_square) to move the black pawns back so that it creates a mask on the capture square.
		return (ops::no_we(own_pawns & ops::mask_rank(5)) & ops::no(pawns_on_en_passant_square));
	else return (ops::so_we(own_pawns & ops::mask_rank(4)) & ops::so(pawns_on_en_passant_square));
}

template bitboard gen::gen_en_passant_left<color::white>(const board& b, bitboard pawns_on_en_passant_square);
template bitboard gen::gen_en_passant_left<color::black>(const board& b, bitboard pawns_on_en_passant_square);

template<color VColor>
inline bitboard gen::gen_en_passant_right(const board& b, bitboard pawns_on_en_passant_square)
{
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);

	if constexpr (VColor == color::white)
		return (ops::no_ea(own_pawns & ops::mask_rank(5)) & ops::no(pawns_on_en_passant_square));
	else return (ops::so_ea(own_pawns & ops::mask_rank(4)) & ops::so(pawns_on_en_passant_square));
}

template bitboard gen::gen_en_passant_right<color::white>(const board& b, bitboard pawns_on_en_passant_square);
template bitboard gen::gen_en_passant_right<color::black>(const board& b, bitboard pawns_on_en_passant_square);