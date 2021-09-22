#include "board.h"
#include "bitwise_ops.h"
#include <cstring>
#include <map>
#include <cstdio>

std::array<bitboard, 64> board::knight_attacks;
std::array<bitboard, 64> board::king_attacks;
std::array<board::hq_mask, 64> board::hq_masks;

namespace
{
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
}

board::board() :board(
	"rnbqkbnr"
	"pppppppp"
	"........"
	"........"
	"........"
	"........"
	"PPPPPPPP"
	"RNBQKBNR"
) {}

board::board(const std::string& start)
	:boards{}
{
	std::map<char, bitboard*> charBitboardMap =
	{
		{'N',&get_board(piece_type::knight,color::white)},
		{'K',&get_board(piece_type::king,color::white)},
		{'Q',&get_board(piece_type::queen,color::white)},
		{'B',&get_board(piece_type::bishop,color::white)},
		{'R',&get_board(piece_type::rook,color::white)},
		{'P',&get_board(piece_type::pawn,color::white)},

		{'n',&get_board(piece_type::knight,color::black)},
		{'k',&get_board(piece_type::king,color::black)},
		{'q',&get_board(piece_type::queen,color::black)},
		{'b',&get_board(piece_type::bishop,color::black)},
		{'r',&get_board(piece_type::rook,color::black)},
		{'p',&get_board(piece_type::pawn,color::black)}
	};
	for (int i = 0; i<start.size(); i++)
	{
		char c = start[i];
		if (c == '.') continue;
		bitboard* b = charBitboardMap[c];
		*b |= ops::set_nth_bit(ops::flip_idx(i));
	}
}

void board::print_bitboard(bitboard b)
{
	bitboard mask = 1ULL << 63ULL;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (b & mask)
				printf("1 ");
			else printf("0 ");
			mask >>= 1;
		}
		printf("\n");
	}
	printf("\n");
}

void board::init_knight_attacks()
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


void board::init_hq_masks()
{
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
			bitboard m = mask_constr.to_ullong();
			mask.mask = m;
			hq_masks[idx] = mask;
		}
	}
}

void board::init_king_attacks()
{
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
				king_attacks[idx] = bc.to_ullong();
			}

		}
	}
}


