#include "board.h"
#include "bitwise_ops.h"
#include <cstring>
#include <map>
#include <cstdio>
#include <ctype.h>

std::array<bitboard, 64> board::knight_attacks;
std::array<bitboard, 64> board::king_attacks;
std::array<board::hq_mask, 64> board::hq_masks;

namespace
{
	bitboard_constr get_diag(uint x, uint y)
	{
		bitboard_constr bc(0);
		// This is init, so don't care about performance
		for (int i = -8; i <= 8; i++)
		{
			int y_off = y + i;
			int x_off = x + i;

			if (ops::contains(x_off, y_off) && (x != x_off && y != y_off))
				bc.set(ops::to_idx(x_off, y_off), true);
		}
		return bc;
	}

	bitboard_constr get_antidiag(uint x, uint y)
	{
		bitboard_constr bc(0);
		// This is init, so don't care about performance
		for (int i = -8; i <= 8; i++)
		{
			int y_off = y + i;
			int x_off = x - i;

			if (ops::contains(x_off, y_off) && (x != x_off && y != y_off))
				bc.set(ops::to_idx(x_off, y_off), true);
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

	std::string parse_fen_into_repr(const std::string& str)
	{
		std::string out;
		out.reserve(64);
		for (int i = 0; i < str.size(); i++)
		{
			char c = str[i];
			if (c == '/')
			{
				continue;
			}
			else if (isdigit(c))
			{
				int spaces = c - '0';
				for (int k = 0; k < spaces; k++) out.push_back('.');
			}
			else
			{
				out.push_back(c);
			}
		}
		return out;
	}

	void fill_board(std::string& out, bitboard b, piece_type pt, color c)
	{
		std::map<piece_type,char> ptToCharMap =
		{
			{piece_type::bishop, 'b'},
			{piece_type::rook, 'r'},
			{piece_type::queen, 'q'},
			{piece_type::king, 'k'},
			{piece_type::knight, 'n'},
			{piece_type::pawn, 'p'}
		};
		while (b != 0)
		{
			uint idx = ops::num_trailing_zeros(b);
			char piece_notation = ptToCharMap[pt];
			out[ops::flip_idx(idx)] = (c == color::white ? toupper(piece_notation) : piece_notation);	
			ops::pop_lsb(b);
		}
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

board::board(const std::string& start, bool fen)
	:boards{}
{
	std::string to_work_with;
	if(fen)
	{
		to_work_with = parse_fen_into_repr(start);
	}
	else to_work_with = start;
	// Remove spaces from string: https://stackoverflow.com/questions/18589525/removing-all-spaces-from-a-string?noredirect=1&lq=1
	to_work_with.erase(std::remove_if(to_work_with.begin(), to_work_with.end(),
	    static_cast<int(*)(int)>(isspace)),to_work_with.end());

	if (to_work_with.size() != 64)
		throw std::exception("Board has more / less than 64 fields");
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
	for (int i = 0; i<to_work_with.size(); i++)
	{
		char c = to_work_with[i];
		if (c == '.' || c==' ') continue;
		bitboard* b = charBitboardMap[c];
		*b |= ops::set_nth_bit(ops::flip_idx(i));
	}
}


void board::print() const
{
	std::string board_repr;
	board_repr.resize(64, '.');
	color c = color::white;
	for (int i = 0; i < 6; i++)
	{
		piece_type pt = static_cast<piece_type>(i);
		fill_board(board_repr, get_board_const(pt, c), pt, c);
	}
	c = color::black;
	for (int i = 0; i < 6; i++)
	{
		piece_type pt = static_cast<piece_type>(i);
		fill_board(board_repr, get_board_const(pt, c), pt, c);
	}

	for (int y = 0; y < 8; y++)
	{
		printf("%i  ", 8 - y);
		for (int x = 0; x < 8; x++)
		{
			char c = board_repr[ops::to_idx(x,y)];
			printf("%c ", c);
		}
		printf("\n");
	}
	printf("\n");
	printf("   a b c d e f g h\n\n");
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
			bitboard_constr mask_constr(0);
			mask_constr.set(idx, true);
			mask.mask = mask_constr.to_ullong();
			hq_masks[idx] = mask;
		}
	}
}

void board::init_king_attacks()
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

void board::init_all()
{
	init_king_attacks();
	init_hq_masks();
	init_knight_attacks();
}
