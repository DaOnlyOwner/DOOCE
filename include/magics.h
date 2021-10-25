#pragma once
#include "definitions.h"


namespace magic
{
	void init_magics();
	void print_magics();
	bitboard bishop_attacks(bitboard occ, int idx);
	bitboard rook_attacks(bitboard occ, int idx);
}