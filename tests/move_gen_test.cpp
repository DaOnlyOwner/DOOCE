#define CATCH_CONFIG_MAIN  
#include "catch.hpp"
#include "game.h"



TEST_CASE("Move Generation Perft", "[move_gen]")
{
	// Init
	board::init_hq_masks();
	board::init_king_attacks();
	board::init_knight_attacks();


	// Check initial position
	SECTION("perft initial positions") {
		game g;
		auto res = g.perft(0);
		REQUIRE(res == 1);
		res = g.perft(1);
		REQUIRE(res == 20);
	}

}

TEST_CASE("Move Generation Knight", "[move_gen]")
{

}







