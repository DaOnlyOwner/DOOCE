#include "bitboard.h"

bitboard board::get_start(piece_type type, color player)
{
	switch (type)
	{
	case piece_type::pawn:
		if (player == color::white)
			return bitboard_constr{
				"00000000"
				"11111111"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
		}.to_ullong();
		if (player == color::black)
			return bitboard_constr{
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"11111111"
				"00000000"
		}.to_ullong();
	case piece_type::rook:
		if (player == color::white)
			return bitboard_constr{
				"10000001"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
		}.to_ullong();
		if (player == color::black)
			return bitboard_constr{
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"10000001"
		}.to_ullong();


	case piece_type::bishop:
		if (player == color::white)
			return bitboard_constr{
				"01000010"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
		}.to_ullong();
		if (player == color::black)
			return bitboard_constr{
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"01000010"
		}.to_ullong();
	case piece_type::knight:
		if (player == color::white)
			return bitboard_constr{
				"00100100"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
		}.to_ullong();
		if (player == color::black)
			return bitboard_constr{
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00100100"
		}.to_ullong();
	case piece_type::king:
		if (player == color::white)
			return bitboard_constr{
				"00001000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
		}.to_ullong();
		if (player == color::black)
			return bitboard_constr{
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00001000"
		}.to_ullong();
	case piece_type::queen:
		if (player == color::white)
			return bitboard_constr{
				"00010000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
		}.to_ullong();
		if (player == color::black)
			return bitboard_constr{
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00000000"
				"00010000"
		}.to_ullong();
	default:
		break;
	}
}

bitboard_constr get_diag(int x, int y)
{
	bitboard_constr bc(0);
	// This is init, so don't care about performance
	for (int i = 0; i < 8; i++)
	{
		int y_off = (y - x) + i;
		int x_off = i;

		if (board::contains(x_off, y_off) && (x != x_off && y != y_off))
			bc.set(board::to_idx(x_off, y_off), true);
		else if (!board::contains(x_off, y_off))
			break;
	}
	return bc;
}

bitboard_constr get_antidiag(int x, int y)
{
	bitboard_constr bc(0);
	// This is init, so don't care about performance
	for (int i = 0; i < 8; i++)
	{
		int y_off = (y + x) - i;
		int x_off = i + board_size;

		if (board::contains(x_off, y_off) && (x != x_off && y != y_off))
			bc.set(board::to_idx(x_off, y_off), true);
		else if(!board::contains(x_off, y_off))
			break;
	}
	return bc;
}

// From ttps://www.chessprogramming.org/On_an_empty_Board#LineAttacks
bitboard_constr get_file(int x, int y)
{
	bitboard_constr bc(0x0101010101010101);
	unsigned int idx = board::to_idx(x, y);
	return (bc << (idx & 7)) ^ (bitboard_constr(1) << idx);
}

bitboard_constr get_rank(int x, int y)
{
	bitboard_constr bc(0xff);
	unsigned int idx = board::to_idx(x, y);
	return (bc << (idx & 56)) ^ (bitboard_constr(1) << idx);
}


std::array<board::hq_mask, 64> board::init_hq_masks()
{
	std::array<board::hq_mask, 64> out;
	for (int x = 0; x < board_size; x++)
	{
		for (int y = 0; y < board_size; y++)
		{
			unsigned int idx = to_idx(x, y);
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

// attack generation

bitboard board::sliding_attacks(bitboard occ, bitboard attack_mask, bitboard mask)
{
	bitboard left, right;
	left = occ & attack_mask;
	right = _byteswap_uint64(left);
	left -= mask; // mask is already multiplied by 2!
	right -= _byteswap_uint64(mask);
	left ^= _byteswap_uint64(right);
	return left & attack_mask;
}

inline char board_to_slice(bitboard b, unsigned int slice)
{
	// basically moves the whole rank down,
	// so the most relevant bits are zeros, and only the last rank may not be filled with zeros.
	// Note that "slice" has to be the amount of ranks,
	// that we have to move down. 
	return static_cast<unsigned char>((b >> (8 * slice)));
}

inline bitboard slice_to_board(char c, unsigned int slice)
{
	bitboard b(0);
	return (b << (static_cast<bitboard>(c) * 8));
}


bitboard board::sliding_rank_attacks(bitboard occ, bitboard attack_mask, bitboard mask,
	unsigned int slice)
{
	unsigned char left, right;
	left = board_to_slice(occ & attack_mask, slice);
	right = reversed_lt[left];
	left -= mask; // mask is already multiplied by 2!
	right -= reversed_lt[mask];
	left ^= reversed_lt[right];
	return slice_to_board(left & attack_mask, slice);
}


bitboard board::gen_attacks_knight(bitboard occ, square s)
{
	
}

bitboard board::gen_attacks_bishop(bitboard occ, square s)
{
	auto& hq_mask = hq_masks[sq_to_int(s)];
	return sliding_attacks(occ, hq_mask.diagEx, hq_mask.mask) +
		sliding_attacks(occ, hq_mask.antidiagEx, hq_mask.mask);
}

bitboard board::gen_attacks_rook(bitboard occ, square s)
{
	unsigned int sq_int = sq_to_int(s);
	auto& hq_mask = hq_masks[sq_int];
	unsigned int slice = board_size - (sq_int / board_size);
	return sliding_rank_attacks(occ, hq_mask.diagEx, hq_mask.mask, slice) +
		sliding_rank_attacks(occ, hq_mask.antidiagEx, hq_mask.mask, slice);
}

bitboard board::gen_attacks_queen(bitboard occ, square s)
{
	return gen_attacks_bishop(occ, s) | gen_attacks_rook(occ, s);
}

std::array<bitboard, 64> board::init_knight_attacks()
{
	std::array<bitboard, 64> out;
	for (int i = 0; i < board_size * board_size; i++)
	{
		bitboard_constr b(0);
		auto [x, y] = from_idx(i);
		
		std::pair<int,int> possible_pos[max_horse_pos];

		possible_pos[0] = { x - 1, y + 2 }; // top_left
		possible_pos[1] = {x + 1, y + 2}; // top_right
		possible_pos[2] = {x - 2, y + 1}; // midtop_left
		possible_pos[3] = {x + 2, y + 1}; // midtop_right

		possible_pos[4] = {x - 2, y - 1}; // midbot_left
		possible_pos[5] = {x + 2, y - 1}; // midbot_right
		possible_pos[6] = {x - 1, y - 2}; // bot_left
		possible_pos[7] = {x + 1, y - 2}; // bot_right

		for (int pos = 0; pos < max_horse_pos; pos++)
		{
			auto [x, y] = possible_pos[pos];
			if (contains(x, y)) b.set(to_idx(x, y), true);
		}
		
		out[i] = b.to_ullong();
	}
	return out;
}


