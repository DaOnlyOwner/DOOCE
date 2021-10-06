#pragma once
#include "definitions.h"
#include "misc_tools.h"
#include "board.h"
#include "game.h"
#include <string>


namespace fen
{
	
	std::string board_to_fen(const board& b);
	board fen_to_board(const std::string& fen);

	std::string game_to_fen(const game& g);
	game fen_to_game(const std::string& fen);
}