#define CATCH_CONFIG_MAIN  
#include "catch.hpp"
#include "gameplay.h"

TEST_CASE("Debug")
{
	gameplay_st gp(5,game(), 1 << 15); // 1 << 27 for performance tests
	auto mi = gp.pick_next_move();

	printf("Reached depth: %i\n", mi.depth);
	printf("First move: From: %s, To: %s\n", sq_idx_to_str(mi.principal_variation[0].get_from_as_idx()).c_str(), sq_idx_to_str(mi.principal_variation[0].get_to_as_idx()).c_str());
	printf("Score: %i\n", mi.score);
}


