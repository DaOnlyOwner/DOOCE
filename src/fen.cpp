#include "fen.h"

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
				out.push_back(c);
			}
		}
		return out;
	}
}

/*std::string game::get_fen()
{
	auto board_fen = b.get_fen();
	std::string color_fen = start_color == color::white ? "w" : "b";
	std::string castle_fen;
	if (!start_info_white.has_moved_king && !start_info_white.has_moved_kingside_rook) castle_fen.push_back('K');
	if (!start_info_white.has_moved_king && !start_info_white.has_moved_queenside_rook) castle_fen.push_back('Q');
	if (!start_info_black.has_moved_king && !start_info_black.has_moved_kingside_rook) castle_fen.push_back('k');
	if (!start_info_black.has_moved_king && !start_info_black.has_moved_queenside_rook) castle_fen.push_back('q');
	return board_fen + " " + color_fen + " " + castle_fen;
}*/

//void parse_fen(const std::string& fen_str, board& b, game_context& context)
//{
//	// TODO: add missing FEN features (en-passant, moves since last pawn move, game round)
//
//	context.en_passantable_pawn = 0ull;
//	auto splitted = misc_tools::split(fen_str, ' ');
//	b = board(splitted[0], true);
//	context.start_color = (splitted[1] == "w" ? color::white : color::black);
//
//	game_info white{ true, true, true }, black{ true, true, true };
//	for (char c : splitted[2])
//	{
//		if (c == 'Q')
//		{
//			white.has_moved_king = false;
//			white.has_moved_queenside_rook = false;
//		}
//		else if (c == 'q')
//		{
//			black.has_moved_king = false;
//			black.has_moved_queenside_rook = false;
//		}
//		else if (c == 'K')
//		{
//			white.has_moved_king = false;
//			white.has_moved_kingside_rook = false;
//		}
//		else if (c == 'k')
//		{
//			black.has_moved_king = false;
//			black.has_moved_kingside_rook = false;
//		}
//	}
//	start_info_black = black;
//	start_info_white = white;
//}
//
//std::string board::get_fen() const
//{
//	std::string board_repr;
//	board_repr.resize(64, '.');
//	color c = color::white;
//	for (int i = 0; i < 6; i++)
//	{
//		piece_type pt = static_cast<piece_type>(i);
//		fill_board(board_repr, get_board_const(pt, c), pt, c);
//	}
//	c = color::black;
//	for (int i = 0; i < 6; i++)
//	{
//		piece_type pt = static_cast<piece_type>(i);
//		fill_board(board_repr, get_board_const(pt, c), pt, c);
//	}
//	std::string fen = "";
//	int counter = 0;
//	for (int y = 0; y < 8; y++)
//	{
//		for (int x = 0; x < 8; x++)
//		{
//			char c = board_repr[y * 8 + x];
//			if (c == '.')
//			{
//				counter++;
//				if (counter == 8)
//				{
//					fen.push_back('8');
//					counter = 0;
//				}
//			}
//			else
//			{
//				if (counter > 0) fen.push_back(counter + '0');
//				counter = 0;
//				fen.push_back(c);
//			}
//		}
//		if (counter > 0)
//		{
//			fen.push_back(counter + '0');
//			counter = 0;
//		}
//		fen.push_back('/');
//	}
//	fen.pop_back();
//	return fen;
//}

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
	auto& info_white = g.get_game_context().get_game_info(color::white);
	auto& info_black = g.get_game_context().get_game_info(color::black);

	if (!info_white.has_moved_king && !info_white.has_moved_kingside_rook) castle_fen.push_back('K');
	if (!info_white.has_moved_king && !info_white.has_moved_queenside_rook) castle_fen.push_back('Q');
	if (!info_black.has_moved_king && !info_black.has_moved_kingside_rook) castle_fen.push_back('k');
	if (!info_black.has_moved_king && !info_black.has_moved_queenside_rook) castle_fen.push_back('q');
	return board_fen + " " + color_fen + " " + castle_fen;
}

game fen::fen_to_game(const std::string& fen)
{
	// TODO: add missing FEN features (en-passant, moves since last pawn move, game round)
	game_context gc;
	gc.en_passantable_pawn = 0ull;
	auto splitted = misc_tools::split(fen, ' ');
	board b = fen::fen_to_board(splitted[0]);
	gc.turn = (splitted[1] == "w" ? color::white : color::black);

	game_info white{ true, true, true }, black{ true, true, true };
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
	return game(b,gc);
}