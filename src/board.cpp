#include "board.h"

namespace
{
	// TODO: replace this function, no string serialization anymore
	void fill_board(std::string& out, bitboard b, piece_type pt, color c)
	{
		std::map<piece_type,char> ptToCharMap =
		{
			{piece_type::bishop, 'b'},
			{piece_type::rook,   'r'},
			{piece_type::queen,  'q'},
			{piece_type::king,   'k'},
			{piece_type::knight, 'n'},
			{piece_type::pawn,   'p'}
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

// TODO: declare the start formation in another way, no string serialization anymore
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

board::board(const std::string& start, bool is_fen) : boards{}
{
	std::string to_work_with = is_fen ? parse_fen_into_repr(start) : start;
	// Remove spaces from string: https://stackoverflow.com/questions/18589525/removing-all-spaces-from-a-string?noredirect=1&lq=1
	to_work_with.erase(std::remove_if(to_work_with.begin(), to_work_with.end(),
	    static_cast<int(*)(int)>(isspace)),to_work_with.end());

	// make sure the given board spec has exactly 64 fields
	if (to_work_with.size() != 64)
		throw std::runtime_error("Board has more / less than 64 fields");

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
	// TODO: rather transform this into a toString() function for debug printing when programming against the board struct

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
