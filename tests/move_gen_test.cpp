#define CATCH_CONFIG_MAIN  
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "game.h"
#include <chrono>

game_info ginfo{ true, true, true };

namespace
{
	void assert_perft_result(perft_results res, perft_results exp)
	{
		REQUIRE(res.nodes == exp.nodes);
		REQUIRE(res.captures == exp.captures);
		REQUIRE(res.en_passants == exp.en_passants);
		REQUIRE(res.castles == exp.castles);
		REQUIRE(res.promos == exp.promos);
	}

	void validate_position(game g, int depth, perft_results exp)
	{
		auto res = g.perft(depth);
		assert_perft_result(res, exp);
	}

	void benchmark_perft(int depth)
	{
		game g;
		printf("========== BENCHMARK ==========\n");
		printf("Initial Perft depth = %i\n", depth);
		auto now = std::chrono::high_resolution_clock::now();
		auto res = g.perft(depth);
		auto then = std::chrono::high_resolution_clock::now();
		printf("Print so that it doesn't optimize the result away: %i\n", (int)res.nodes);
		printf("measured time in seconds: %f\n", std::chrono::duration_cast<std::chrono::milliseconds>(then - now).count() / 1000.f);
		printf("===============================\n\n");
	}
}

TEST_CASE("INIT")
{
	board::init_all();
}

TEST_CASE("Move Generation Perft Position 2")
{
	// Position from chessprogramming.org
	std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
	game g(fen);

	SECTION("depth = 1")
	{
		validate_position(g, 1, perft_results{ 48, 8, 0, 2, 0 });
	}

	SECTION("depth = 2")
	{
		validate_position(g, 2, perft_results{ 2039, 351, 1, 91, 0 });
	}

	// SECTION("depth = 3")
	// {
	// 	validate_position(g, 3, perft_results{ 97862, 17102, 45, 3162, 0 });
	// }

	// SECTION("depth = 4")
	// {
	// 	validate_position(g, 4, perft_results{ 4085603, 757163, 1929, 128013, 15172 });
	// }
}

TEST_CASE("Move generation king")
{
	SECTION("King 1")
	{
		// TODO: transform position into FEN string
		game g(
			"..k....."
			"........"
			"........"
			"........"
			"........"
			".....K.."
			"........"
			"........", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 8, 0, 0, 0, 0 });
	}

	SECTION("King 2")
	{
		// TODO: transform position into FEN string
		game g(
			"..k....."
			"........"
			"........"
			"........"
			"........"
			"........"
			"....p..."
			".....K..", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 5, 1, 0, 0, 0 });
	}
}

TEST_CASE("Castling")
{
	SECTION("Queenside")
	{
		// TODO: transform position into FEN string
		game_info ginfo_castling;
		ginfo_castling.has_moved_kingside_rook = true;
		ginfo_castling.has_moved_king = false;
		ginfo_castling.has_moved_queenside_rook = false;
		game g(
			"r...k..."
			"........"
			"........"
			"........"
			"........"
			"........"
			"........"
			"....K...", ginfo, ginfo_castling, color::black, 0ull);
		validate_position(g, 1, perft_results{ 15, 0, 0, 1, 0 });
	}

	SECTION("Kingside")
	{
		// TODO: transform position into FEN string
		game_info ginfo_castling;
		ginfo_castling.has_moved_queenside_rook = true;
		ginfo_castling.has_moved_king = false;
		ginfo_castling.has_moved_kingside_rook = false;
		game g(
			"....k..r"
			"........"
			"........"
			"........"
			"........"
			"........"
			"........"
			"....K...", ginfo, ginfo_castling, color::black, 0ull);
		validate_position(g, 1, perft_results{ 14, 0, 0, 1, 0 });
	}
}

TEST_CASE("Move Generation pawn")
{
	SECTION("Pawn 1")
	{
		// TODO: transform position into FEN string
		game g(
			"..k....."
			"........"
			"p.p....."
			".P......"
			"........"
			"........"
			"........"
			"K.......", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 6, 2, 0, 0, 0 });
	}

	SECTION("Pawn 2")
	{
		// TODO: transform position into FEN string
		game g(
			"k......."
			"........"
			"........"
			"........"
			"..p....."
			"p......."
			"PPP....."
			"K.......", ginfo, ginfo, color::white, 0ull
		);
		validate_position(g, 1, perft_results{ 5, 1, 0, 0, 0 });
	}

	SECTION("Black to move - everything")
	{
		// TODO: transform position into FEN string
		game g(
			". . . . . . . ."
			". . . p . p p ."
			". . . . . P . ."
			". . p . . . P ."
			". P . P . . . ."
			". . . . . . . ."
			". . . . . . . ."
			"K . . . . . . k", ginfo, ginfo, color::black, 0ull
		);
		validate_position(g, 1, perft_results{ 10, 3, 0, 0, 0 });
	}

	SECTION("Overflow when attacking")
	{
		// TODO: transform position into FEN string
		game g(
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". p . . . . p ."
			"P . . . . . . P"
			". . . . . . . ."
			". . . . . . . ."
			"K . . . . . . k", ginfo, ginfo, color::white, 0ull
		);
		validate_position(g, 1, perft_results{ 7, 2, 0, 0, 0 });
	}
}

TEST_CASE("Move generation rook")
{
	SECTION("Rook 1")
	{
		// TODO: transform position into FEN string
		game g(
			"........"
			"........"
			"....R..."
			"........"
			"..k.p..."
			"........"
			"........"
			"....K...", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 16, 1, 0, 0, 0 });
	}

	SECTION("Rook 2")
	{
		// TODO: transform position into FEN string
		game g(
			"R.....K."
			"........"
			"........"
			"........"
			"......k."
			"........"
			"........"
			"........", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 17, 0, 0, 0, 0 });
	}

	SECTION("capture in every direction")
	{
		// TODO: transform position into FEN string
		game g(
			"....k..K"
			"........"
			"........"
			"...p...."
			".p.R..p."
			"........"
			"...p...."
			"........", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 11, 4, 0, 0, 0 });
	}
}

TEST_CASE("Move Generation Bishop")
{
	SECTION("Bishop easy")
	{
		// TODO: transform position into FEN string
		game g(
			". . . . . . . ."
			". . . k . . . ."
			". . . B . . . ."
			". . p . . . . ."
			". . . . . . . ."
			". . . . . . p ."
			". . . . . . . ."
			"K . . . . . . .", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 11, 2, 0, 0, 0 });
	}

	SECTION("Bishop on msb")
	{
		// TODO: transform position into FEN string
		game g(
			"B . . . . . . ."
			". . . k . . . ."
			". . . . . . . ."
			". . . p . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			"K . . . . . . .", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 6, 1, 0, 0, 0 });
	}

	SECTION("Bishop on lsb")
	{
		// TODO: transform position into FEN string
		game g(
			". . . . . . . k"
			". . . . . . . ."
			". . . . . . . ."
			". . . P . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			"K . . . . . . b", ginfo, ginfo, color::black, 0ULL
		);
		validate_position(g, 1, perft_results{ 7, 1, 0, 0, 0 });
	}

	SECTION("Capture in every direction")
	{
		// TODO: transform position into FEN string
		game g(
			". . . . . k . ."
			". p . . . P . ."
			". . . . . . . ."
			". . . b . . . ."
			". . . . P . . ."
			". P . . . p . ."
			". . . . . . . ."
			"K . . . . . . .", ginfo, ginfo, color::black, 0ULL
		);
		validate_position(g, 1, perft_results{ 12, 4, 0, 0, 0 });
	}
}

TEST_CASE("Check and Checkmates")
{
	SECTION("Backrank mate")
	{
		// TODO: transform position into FEN string
		game g(
			". . . Q . . . k"
			". . . . . . p p"
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . K . . .", ginfo, ginfo, color::black, 0ull
		);
		validate_position(g, 1, perft_results{});
		validate_position(g, 3, perft_results{});
	}

	SECTION("Check")
	{
		// TODO: transform position into FEN string
		game g(
			". . . . . . . ."
			". . . b . b . ."
			". . Q . k . Q ."
			". . . . p . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . .", ginfo, ginfo, color::black, 0ull
		);
		validate_position(g, 1, perft_results{ 1, 0, 0, 0, 0 });
	}

	SECTION("Stalemate")
	{
		// TODO: transform position into FEN string
		game g(
			"k . . . . . . ."
			". . . . . . . ."
			". K N . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . .", ginfo, ginfo, color::black, 0ull
		);
		validate_position(g, 1, perft_results{ 1, 0, 0, 0, 0 });
	}
}

TEST_CASE("Move Generation Queen")
{
	SECTION("Queen easy")
	{
		// TODO: transform position into FEN string
		game g(
			". . k . . . . ."
			". . . . . . . ."
			". . . . P . p ."
			". . . K . . . ."
			". . . . Q . . ."
			". . . . . . . ."
			". . p . p . . ."
			". . . . . . . .", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 24, 3, 0, 0, 0 });
	}
}

TEST_CASE("Move Generation Knight")
{
	// TODO: transform position into FEN string
	SECTION("Knight 1")
	{
		game g(
			"k . . . K . . ."
			". . . . . . . ."
			". . . . p . . ."
			". . . . . . . ."
			". . . N . . . ."
			". . . . . . . ."
			". . p . . . . ."
			". . . . . . . .", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 13, 2, 0, 0, 0 });
	}

	SECTION("Knight 2")
	{
		// TODO: transform position into FEN string
		game g(
			"N . . . K . . N"
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". p . . . . p ."
			". . . . . . . ."
			"N . . . k . . N", ginfo, ginfo, color::white, 0ULL
		);
		validate_position(g, 1, perft_results{ 13, 2, 0, 0, 0 });
	}
}

TEST_CASE("Move Generation Initial Perft", "[move_gen]")
{
	// initial game position
	game g;

	SECTION("depth = 0") {
		validate_position(g, 0, perft_results{ 1, 0, 0, 0, 0 });
	}

	SECTION("depth = 1") {
		validate_position(g, 1, perft_results{ 20, 0, 0, 0, 0 });
	}

	SECTION("depth = 2") {
		validate_position(g, 2, perft_results{ 400, 0, 0, 0, 0 });
	}

	SECTION("depth = 3") {
		validate_position(g, 3, perft_results{ 8902, 34, 0, 0, 0 });
	}

	SECTION("depth = 4") {
		validate_position(g, 4, perft_results{ 197281, 1576, 0, 0, 0 });
	}

	SECTION("depth = 5")
	{
		validate_position(g, 5, perft_results{ 4865609, 82719, 258, 0, 0 });
	}

    // TODO: this test takes 10 seconds, think of adding it again if test time doesn't matter too much
	/*SECTION("depth = 6") {
		validate_position(g, 6, perft_results{ 119060324ULL, 2812008ULL, 5248ULL, 0, 0 });
	}*/
}

TEST_CASE("Benchmark")
{
	benchmark_perft(5);

	// TODO: this test takes 10 seconds, think of adding it again if test time doesn't matter too much
	//benchmark_perft(6);
}
