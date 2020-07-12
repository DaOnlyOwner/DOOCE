#include "move_gen_helper.h"

move_gen_helper::move_lt move_gen_helper::init_knight_attacks()
{
	move_lt out;
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

		out[i] = b.to_ullong();
	}
	return out;
}

bitboard_constr get_diag(uint x, uint y)
{
	bitboard_constr bc(0);
	// This is init, so don't care about performance
	for (uint i = 0; i < 8; i++)
	{
		uint y_off = (y - x) + i;
		uint x_off = i;

		if (ops::contains(x_off, y_off) && (x != x_off && y != y_off))
			bc.set(ops::to_idx(x_off, y_off), true);
		else if (!ops::contains(x_off, y_off))
			break;
	}
	return bc;
}

bitboard_constr get_antidiag(uint x, uint y)
{
	bitboard_constr bc(0);
	// This is init, so don't care about performance
	for (uint i = 0; i < 8; i++)
	{
		uint y_off = (y + x) - i;
		uint x_off = i + 8;

		if (ops::contains(x_off, y_off) && (x != x_off && y != y_off))
			bc.set(ops::to_idx(x_off, y_off), true);
		else if (!ops::contains(x_off, y_off))
			break;
	}
	return bc;
}

// From ttps://www.chessprogramming.org/On_an_empty_Board#LineAttacks
bitboard_constr get_file(uint x, uint y)
{
	bitboard_constr bc(0x0101010101010101);
	uint idx = ops::to_idx(x, y);
	return (bc << (idx & 7)) ^ (bitboard_constr(1) << idx);
}

bitboard_constr get_rank(uint x, uint y)
{
	bitboard_constr bc(0xff);
	uint idx = ops::to_idx(x, y);
	return (bc << (idx & 56)) ^ (bitboard_constr(1) << idx);
}


move_gen_helper::hq_mask::lt move_gen_helper::init_hq_masks()
{
	hq_mask::lt out;
	for (uint x = 0; x < 8; x++)
	{
		for (uint y = 0; y < 8; y++)
		{
			uint idx = ops::to_idx(x, y);
			hq_mask mask;

			// Init diag and anti diag.
			auto diag = get_diag(x, y);
			auto antidiag = get_antidiag(x, y);
			auto rank = get_rank(x, y);
			auto file = get_file(x, y);
			mask.diagEx = diag.to_ullong();
			mask.antidiagEx = antidiag.to_ullong();
			mask.fileEx = file.to_ullong();
			mask.rankEx = rank.to_ullong();
			bitboard_constr mask_constr(0);
			mask_constr.set(idx, true);
			mask.mask = 2 * mask_constr.to_ullong();
			out[idx] = mask;
		}
	}
	return out;
}

move_gen_helper::move_lt move_gen_helper::init_king_attacks()
{
	move_lt out;
	for (uint x = 0; x < 8; x++)
	{
		for (uint y = 0; y < 8; y++)
		{
			bitboard_constr bc(0);
			uint idx = ops::to_idx(x, y);

			for (uint j = 0; j < 3; j++)
			{
				for (uint k = 0; k < 3; k++)
				{
					uint x_off = x + j;
					uint y_off = y + k;
					uint idx = ops::to_idx(x_off, y_off);
					if (ops::contains(x_off, y_off))
						bc.set(idx, true);
				}
				bc.set(idx, false);
				out[idx] = bc.to_ullong();
			}

		}
	}
	return out;
}

bitboard move_gen_helper::gen_attacks_bishop(bitboard occ, square s, const hq_mask::lt& masks)
{
	auto& mask = masks[sq_to_int(s)];
	return ops::hyperbola_quintessence(occ, mask.diagEx, mask.mask, _byteswap_uint64) |
		ops::hyperbola_quintessence(occ, mask.diagEx, mask.mask, _byteswap_uint64);
}

// From https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
constexpr inline unsigned char lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

bitboard move_gen_helper::gen_attacks_rook(bitboard occ, square s, const hq_mask::lt& masks)
{
	auto sq = sq_to_int(s);
	auto& hq_mask = masks[sq_to_int(s)];

	bitboard file_attacks = ops::hyperbola_quintessence(occ, hq_mask.fileEx, hq_mask.mask, _byteswap_uint64);

	auto slow_bitreverse = [](bitboard b)
	{
		bitboard rankx8 = b & 56;
		auto n = static_cast<unsigned char>(b >> rankx8);
		return ((lookup[n & 0b1111] << 4) | lookup[n >> 4]) << rankx8;
	};

	bitboard rank_attacks = ops::hyperbola_quintessence(occ, hq_mask.rankEx, hq_mask.mask, slow_bitreverse);
	return file_attacks | rank_attacks;
}

bitboard move_gen_helper::gen_attacks_queen(bitboard occ, square s, const hq_mask::lt& masks)
{
	return move_gen_helper::gen_attacks_rook(occ, s, masks) |
		move_gen_helper::gen_attacks_bishop(occ, s, masks);
}
