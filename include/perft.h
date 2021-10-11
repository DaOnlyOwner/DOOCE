#pragma once
#include "definitions.h"
#include "board.h"
#include "game.h"
#include <string>
#include <vector>


struct perft_results
{
	uint64_t nodes = 0ULL;
	uint64_t captures = 0ULL;
	uint64_t en_passants = 0ULL;
	uint64_t castles = 0ULL;
	uint64_t promos = 0ULL;
};

perft_results perft(game& g, int depth);
