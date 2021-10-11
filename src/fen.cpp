#include "fen.h"
#include <stdexcept>
#include "bitwise_ops.h"
#include <cstdlib>

namespace
{
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
				// TODO: Check for invalid characters.
				out.push_back(c);
			}
		}
		return out;
	}
}

std::string fen::board_to_fen(const board& b)
{
	std::string board_repr;
	board_repr.resize(64, '.');
	color c = color::white;
	for (int i = 0; i < 6; i++)
	{
		piece_type pt = static_cast<piece_type>(i);
		fill_board(board_repr, b.get_board(pt, c), pt, c);
	}
	c = color::black;
	for (int i = 0; i < 6; i++)
	{
		piece_type pt = static_cast<piece_type>(i);
		fill_board(board_repr, b.get_board(pt, c), pt, c);
	}
	std::string fen = "";
	int counter = 0;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			char c = board_repr[y * 8 + x];
			if (c == '.')
			{
				counter++;
				if (counter == 8)
				{
					fen.push_back('8');
					counter = 0;
				}
			}
			else
			{
				if (counter > 0) fen.push_back(counter + '0');
				counter = 0;
				fen.push_back(c);
			}
		}
		if (counter > 0)
		{
			fen.push_back(counter + '0');
			counter = 0;
		}
		fen.push_back('/');
	}
	fen.pop_back();
	return fen;
}

board fen::fen_to_board(const std::string& fen)
{
	std::string repr = parse_fen_into_repr(fen);
	return board(repr);
}

// TODO Add missing fen parameters here
std::string fen::game_to_fen(const game& g)
{
	auto board_fen = fen::board_to_fen(g.get_board());
	std::string color_fen = g.get_game_context().turn == color::white ? "w" : "b";
	std::string castle_fen;
	std::string en_passant_fen;
	std::string fullmove;
	std::string half_move_clock;
	auto& gc = g.get_game_context();
	auto& info_white = gc.get_castling_info(color::white);
	auto& info_black = gc.get_castling_info(color::black);

	if (!info_white.has_moved_king && !info_white.has_moved_kingside_rook) castle_fen.push_back('K');
	if (!info_white.has_moved_king && !info_white.has_moved_queenside_rook) castle_fen.push_back('Q');
	if (!info_black.has_moved_king && !info_black.has_moved_kingside_rook) castle_fen.push_back('k');
	if (!info_black.has_moved_king && !info_black.has_moved_queenside_rook) castle_fen.push_back('q');
	if (castle_fen == "")
		castle_fen = "-";

	bitboard ep = gc.en_passantable_pawn;
	if (gc.turn == color::white)
		ep = ops::no(ep);
	else ep = ops::so(ep);
	en_passant_fen = ep == 0ULL ? "-" : sq_idx_to_str(ops::num_trailing_zeros(ep));

	fullmove = std::to_string(gc.fullmoves);
	half_move_clock = std::to_string(gc.half_move_clock);

	return board_fen + " " + color_fen + " " + castle_fen + " "
		+ en_passant_fen + " " + half_move_clock + " " + fullmove;
}

game fen::fen_to_game(const std::string& fen)
{
	// TODO: add missing FEN features (en-passant, moves since last pawn move, game round)
	game_context gc;
	gc.en_passantable_pawn = 0ull;
	auto splitted = misc_tools::split(fen, ' ');
	if (splitted.size() == 0)
		throw std::runtime_error("FEN has no board string");
	board b = fen::fen_to_board(splitted[0]);
	if (splitted.size() == 1)
	{
		return game(b, gc);
	}
	if (splitted.size() < 6) throw std::runtime_error("Fen string has not enough groups");
	gc.turn = (splitted[1] == "w" ? color::white : color::black);

	castling_info white{ true, true, true }, black{ true, true, true };
	for (char c : splitted[2])
	{
		if (c == 'Q')
		{
			white.has_moved_king = false;
			white.has_moved_queenside_rook = false;
		}
		else if (c == 'q')
		{
			black.has_moved_king = false;
			black.has_moved_queenside_rook = false;
		}
		else if (c == 'K')
		{
			white.has_moved_king = false;
			white.has_moved_kingside_rook = false;
		}
		else if (c == 'k')
		{
			black.has_moved_king = false;
			black.has_moved_kingside_rook = false;
		}
	}
	gc.set_game_info(color::black, black);
	gc.set_game_info(color::white, white);

	uint ep = str_to_sq_idx(splitted[3]);
	if (gc.turn == color::white)
		ep = ops::so(ep);
	else ep = ops::no(ep);
	gc.en_passantable_pawn = ep;
	gc.half_move_clock = atoi(splitted[4].c_str());
	gc.fullmoves = atoi(splitted[5].c_str());

	return game(b,gc);
}