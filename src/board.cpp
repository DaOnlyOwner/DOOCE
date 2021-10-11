#include "board.h"
#include <map>
#include <stdexcept>
#include <algorithm>

void fill_board(std::string& out, bitboard b, piece_type pt, color c)
{
	std::map<piece_type, char> ptToCharMap =
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

board::board(const std::string& start) : boards{}
{
	std::string to_work_with = start;
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
	recalculate_boards();
}

std::string board::pretty() const
{
	std::string board_repr;
	std::string out;
	board_repr.resize(64, '.');
	color c = color::white;
	for (int i = 0; i < 6; i++)
	{
		piece_type pt = static_cast<piece_type>(i);
		fill_board(board_repr, get_board(pt, c), pt, c);
	}
	c = color::black;
	for (int i = 0; i < 6; i++)
	{
		piece_type pt = static_cast<piece_type>(i);
		fill_board(board_repr, get_board(pt, c), pt, c);
	}

	for (int y = 0; y < 8; y++)
	{
		out += std::to_string(8 - y) + " ";
		for (int x = 0; x < 8; x++)
		{
			char c = board_repr[ops::to_idx(x,y)];
			out += ' ';
			out += c;
		}
		out+="\n";
	}
	out+="\n";
	out += "   a b c d e f g h";
	return out;
}

std::string board::get_bitboard(bitboard b)
{
	std::string out;
	bitboard mask = 1ULL << 63ULL;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (b & mask)
				out+="1 ";
			else out += "0 ";
			mask >>= 1;
		}
		out += "\n";
	}
	out += "\n";
	return out;
}

bitboard& board::get_board(piece_type ptype, color c)
{
	return boards[static_cast<uint>(ptype)][static_cast<uint>(c)];
}

bitboard board::get_whole_board() const
{
	return whole_board;
}

void board::recalculate_boards()
{
	black_side = 0ULL;
	white_side = 0ULL;
	for (int i = 0; i < 6; i++)
		black_side |= boards[i][static_cast<uint>(color::black)];
	for (int i = 0; i < 6; i++)
		white_side |= boards[i][static_cast<uint>(color::white)];
	whole_board = white_side | black_side;
	not_black_side = ~black_side;
	not_white_side = ~white_side;
}

inline void board::handle_quiet_move(bitboard& own, bitboard from, bitboard to)
{
	own = (own | to) & ~from;
}

const bitboard& board::get_board(piece_type ptype, color c) const
{
	return boards[static_cast<uint>(ptype)][static_cast<uint>(c)];
}

template<color VColor>
bitboard board::get_board_of_side() const
{
	if constexpr (VColor == color::white) return white_side;
	else return black_side;
}

template<color VColor>
bitboard board::get_board_of_side_not() const
{
	if constexpr (VColor == color::white) return not_white_side;
	else return not_black_side;
}


template<color VColor>
inline void board::do_move(const move& m)
{
	constexpr color ecolor = invert_color(VColor);
	bitboard& own = get_board(m.get_moved_piece_type(), VColor);
	bitboard from = m.get_from_as_bitboard();
	bitboard to = m.get_to_as_bitboard();

	switch (m.get_move_type())
	{
	case move_type::promo:
	{
		own &= ~from;
		bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
		promo_board |= to;
	}
	break;

	case move_type::promo_captures:
	{
		own &= ~from;
		bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
		promo_board |= to;
		bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
		enemy &= ~to;
	}
	break;

	case move_type::captures:
	{
		handle_quiet_move(own, from, to);
		bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
		enemy &= ~to;
	}
	break;

	case move_type::pawn_single:
	case move_type::pawn_double:
	case move_type::quiet:
		handle_quiet_move(own, from, to);
		break;
	case move_type::en_passant:
	{
		handle_quiet_move(own, from, to);
		bitboard& enemy = get_board(piece_type::pawn, ecolor);
		if constexpr (VColor == color::white)
			enemy &= ~ops::so(to);
		else enemy &= ~ops::no(to);
	}
	break;
	case move_type::king_castle:
		// own == king
	{
		constexpr auto rook_from_square = VColor == color::white ? square::h1 : square::h8;
		constexpr auto rook_to_square = VColor == color::white ? square::f1 : square::f8;
		constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
		constexpr auto king_to_square = VColor == color::white ? square::g1 : square::g8;
		constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
		constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
		constexpr bitboard king_from = ops::set_square_bit(king_from_square);
		constexpr bitboard king_to = ops::set_square_bit(king_to_square);

		bitboard& rooks = get_board(piece_type::rook, VColor);
		handle_quiet_move(rooks, rook_from, rook_to);
		handle_quiet_move(own, king_from, king_to);
	}
	break;
	// own == king
	case move_type::queen_castle:
	{
		constexpr auto rook_from_square = VColor == color::white ? square::a1 : square::a8;
		constexpr auto rook_to_square = VColor == color::white ? square::d1 : square::d8;
		constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
		constexpr auto king_to_square = VColor == color::white ? square::c1 : square::c8;
		constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
		constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
		constexpr bitboard king_from = ops::set_square_bit(king_from_square);
		constexpr bitboard king_to = ops::set_square_bit(king_to_square);

		bitboard& rooks = get_board(piece_type::rook, VColor);
		handle_quiet_move(rooks, rook_from, rook_to);
		handle_quiet_move(own, king_from, king_to);
	}
	break;
	}
	recalculate_boards();
}

template<color VColor>
inline void board::undo_move(const move& m)
{
	constexpr color ecolor = static_cast<color>(1 - static_cast<uint>(VColor));
	bitboard& own = get_board(m.get_moved_piece_type(), VColor);
	bitboard from = m.get_from_as_bitboard();
	bitboard to = m.get_to_as_bitboard();

	switch (m.get_move_type())
	{
	case move_type::promo:
	{
		own |= from;
		bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
		promo_board &= ~to;
	}
	break;

	case move_type::promo_captures:
	{
		own |= from;
		bitboard& promo_board = get_board(m.get_promo_piece_type(), VColor);
		promo_board &= ~to;
		bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
		enemy |= to;
	}
	break;

	case move_type::captures:
	{
		handle_quiet_move(own, to, from);
		bitboard& enemy = get_board(m.get_captured_piece_type(), ecolor);
		enemy |= to;
	}
	break;

	case move_type::pawn_single:
	case move_type::pawn_double:
	case move_type::quiet:
		handle_quiet_move(own, to, from);
		break;
	case move_type::en_passant:
	{
		handle_quiet_move(own, to, from);
		bitboard& enemy = get_board(piece_type::pawn, ecolor);
		if constexpr (VColor == color::white)
			enemy |= ops::so(to);
		else enemy |= ops::no(to);
	}
	break;
	case move_type::king_castle: // own == king
	{
		constexpr auto rook_from_square = VColor == color::white ? square::h1 : square::h8;
		constexpr auto rook_to_square = VColor == color::white ? square::f1 : square::f8;
		constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
		constexpr auto king_to_square = VColor == color::white ? square::g1 : square::g8;
		constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
		constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
		constexpr bitboard king_from = ops::set_square_bit(king_from_square);
		constexpr bitboard king_to = ops::set_square_bit(king_to_square);

		bitboard& rooks = get_board(piece_type::rook, VColor);
		handle_quiet_move(rooks, rook_to, rook_from);
		handle_quiet_move(own, king_to, king_from);
	}
	break;
	// own == king
	case move_type::queen_castle:
	{
		constexpr auto rook_from_square = VColor == color::white ? square::a1 : square::a8;
		constexpr auto rook_to_square = VColor == color::white ? square::d1 : square::d8;
		constexpr auto king_from_square = VColor == color::white ? square::e1 : square::e8;
		constexpr auto king_to_square = VColor == color::white ? square::c1 : square::c8;
		constexpr bitboard rook_from = ops::set_square_bit(rook_from_square);
		constexpr bitboard rook_to = ops::set_square_bit(rook_to_square);
		constexpr bitboard king_from = ops::set_square_bit(king_from_square);
		constexpr bitboard king_to = ops::set_square_bit(king_to_square);

		bitboard& rooks = get_board(piece_type::rook, VColor);
		handle_quiet_move(rooks, rook_to, rook_from);
		handle_quiet_move(own, king_to, king_from);
	}
	break;
	}
	recalculate_boards();
}

template bitboard board::get_board_of_side<color::white>() const;
template bitboard board::get_board_of_side<color::black>() const;

template bitboard board::get_board_of_side_not<color::white>() const;
template bitboard board::get_board_of_side_not<color::black>() const;

template void board::do_move<color::white>(const move& m);
template void board::do_move<color::black>(const move& m);

template void board::undo_move<color::white>(const move& m);
template void board::undo_move<color::black>(const move& m);