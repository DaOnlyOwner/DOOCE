#define CATCH_CONFIG_MAIN  
#include "catch.hpp"
#include "gameplay.h"

TEST_CASE("Convert from string to squares")
{
	REQUIRE(sq_idx_to_str(str_to_sq_idx("a2")) == "a2");
}

TEST_CASE("Debug")
{
	game g;
	auto a = g.from_dooce_algebraic_notation<color::white>("a2a4");
	REQUIRE(a.has_value());
	a = g.from_dooce_algebraic_notation<color::white>("h3a4");
	REQUIRE(!a.has_value());

	a = g.from_dooce_algebraic_notation<color::white>("a2a5");
	REQUIRE(!a.has_value());



	//gameplay_st gp(5,game(), 1 << 15); // 1 << 27 for performance tests
	//auto mi = gp.pick_next_move();

	//printf("Reached depth: %i\n", mi.depth);
	//printf("First move: From: %s, To: %s\n", sq_idx_to_str(mi.principal_variation[0].get_from_as_idx()).c_str(), sq_idx_to_str(mi.principal_variation[0].get_to_as_idx()).c_str());
	//printf("Score: %i\n", mi.score);
}


