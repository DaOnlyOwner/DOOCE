#define CATCH_CONFIG_MAIN  
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "game.h"
#include <chrono>

game_info ginfo{ true,true,true };

TEST_CASE("INIT")
{
	board::init_all();
}


// Position from chessprogramming.org
TEST_CASE("Move Generation Perft Position 2")
{
	std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
	game g(fen);

	SECTION("depth = 1")
	{
		auto res = g.perft(1);
		REQUIRE(res.castles == 2);
		REQUIRE(res.captures == 8);
		REQUIRE(res.nodes == 48);
	}
	
	SECTION("depth = 2")
	{
		auto res = g.perft(2);
		REQUIRE(res.en_passants == 1);
		REQUIRE(res.captures == 351);
		REQUIRE(res.castles == 91);
		REQUIRE(res.nodes == 2039);
	}

	SECTION("depth = 3")
	{
		auto res = g.perft(3);
		REQUIRE(res.captures == 17102);
		REQUIRE(res.nodes == 97862);
		REQUIRE(res.en_passants == 45);
		REQUIRE(res.castles == 3162);
	}

	SECTION("depth = 4")
	{
		auto res = g.perft(4);
		REQUIRE(res.captures == 757163);
		REQUIRE(res.nodes == 4085603);
		REQUIRE(res.en_passants == 1929);
		REQUIRE(res.castles == 128013);
		REQUIRE(res.promos == 15172);
	}
}

TEST_CASE("Move generation king")
{
	SECTION("King 1")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.nodes == 8);
	}

	SECTION("King 2")
	{

		game g2(
			"..k....."
			"........"
			"........"
			"........"
			"........"
			"........"
			"....p..."
			".....K..", ginfo, ginfo, color::white, 0ULL
		);
		auto res = g2.perft(1);
		REQUIRE(res.captures == 1);
		REQUIRE(res.nodes == 5);
	}
}

TEST_CASE("Castling")
{
	SECTION("Queenside")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.castles == 1);
	}

	SECTION("Kingside")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.castles == 1);
	}
}

TEST_CASE("Move Generation pawn")
{
	SECTION("Pawn 1")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 2);
		REQUIRE(res.nodes == 6);
	}

	SECTION("Pawn 2")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 1);
		REQUIRE(res.nodes == 5);
	}

	SECTION("Black to move - everything")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 3);
		REQUIRE(res.nodes == 10);
	}

	SECTION("Overflow when attacking")
	{
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

		auto res = g.perft(1);
		REQUIRE(res.captures == 2);
		REQUIRE(res.nodes == 7);
	}
}

TEST_CASE("Move generation rook")
{
	SECTION("Rook 1")
	{
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

		auto res = g.perft(1);
		REQUIRE(res.captures == 1);
		REQUIRE(res.nodes == 16);
	
	}

	SECTION("Rook 2")
	{
		game g2(
			"R.....K."
			"........"
			"........"
			"........"
			"......k."
			"........"
			"........"
			"........", ginfo, ginfo, color::white, 0ULL
		);

		auto res = g2.perft(1);
		REQUIRE(res.captures == 0);
		REQUIRE(res.nodes == 17);
	}

	SECTION("capture in every direction")
	{
		game g3(
			"....k..K"
			"........"
			"........"
			"...p...."
			".p.R..p."
			"........"
			"...p...."
			"........", ginfo, ginfo, color::white, 0ULL
		);

		auto res = g3.perft(1);
		REQUIRE(res.captures == 4);
		REQUIRE(res.nodes == 11);
	}
}

TEST_CASE("Move Generation Bishop")
{
	SECTION("Bishop easy")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 2);
		REQUIRE(res.nodes == 11);
	}

	SECTION("Bishop on msb")
	{
		game g2(
			"B . . . . . . ."
			". . . k . . . ."
			". . . . . . . ."
			". . . p . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			"K . . . . . . .", ginfo, ginfo, color::white, 0ULL
		);
		auto res = g2.perft(1);
		REQUIRE(res.captures == 1);
		REQUIRE(res.nodes == 6);
	}

	SECTION("Bishop on lsb")
	{
		game g2(
			". . . . . . . k"
			". . . . . . . ."
			". . . . . . . ."
			". . . P . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			"K . . . . . . b", ginfo, ginfo, color::black, 0ULL
		);
		auto res = g2.perft(1);
		REQUIRE(res.captures == 1);
		REQUIRE(res.nodes == 7);
	}

	SECTION("Capture in every direction")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 4);
		REQUIRE(res.nodes == 12);
	}

}

TEST_CASE("Check and Checkmates")
{
	SECTION("Backrank mate")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 0);
		REQUIRE(res.nodes == 0);
		res = g.perft(3);
		REQUIRE(res.captures == 0);
		REQUIRE(res.nodes == 0);
	}

	SECTION("Check")
	{
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

		auto res = g.perft(1);
		REQUIRE(res.captures == 0);
		REQUIRE(res.nodes == 1);
	}

	SECTION("Stalemate")
	{
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

		auto res = g.perft(1);
		REQUIRE(res.captures == 0);
		REQUIRE(res.nodes == 0);
	}

}

TEST_CASE("Move Generation Queen")
{
	SECTION("Queen easy")
	{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 3);
		REQUIRE(res.nodes == 24);
	}
}

TEST_CASE("Move Generation Knight")
{
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
		auto res = g.perft(1);
		REQUIRE(res.captures == 2);
		REQUIRE(res.nodes == 13);
	}

	SECTION("Knight 2")
	{
		game g2(
			"N . . . K . . N"
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". . . . . . . ."
			". p . . . . p ."
			". . . . . . . ."
			"N . . . k . . N", ginfo, ginfo, color::white, 0ULL
		);
		auto res = g2.perft(1);
		REQUIRE(res.captures == 2);
		REQUIRE(res.nodes == 13);
	}
}


TEST_CASE("Move Generation Initial Perft", "[move_gen]")
{
	game g;


	// Check initial position
	SECTION("depth = 0") {
		auto res = g.perft(0);
		REQUIRE(res.nodes == 1);
	}

	SECTION("depth = 1") {
		auto res = g.perft(1);
		REQUIRE(res.nodes == 20);
		REQUIRE(res.castles == 0);
		REQUIRE(res.en_passants == 0);
		REQUIRE(res.promos == 0);
		REQUIRE(res.captures == 0);
	}

	SECTION("depth = 2") {
		auto res = g.perft(2);
		REQUIRE(res.nodes == 400);
		REQUIRE(res.captures == 0);
		REQUIRE(res.en_passants == 0);
		REQUIRE(res.castles == 0);
		REQUIRE(res.promos == 0);
	}

	SECTION("depth = 3") {
		auto res = g.perft(3);
		REQUIRE(res.nodes == 8902);
		REQUIRE(res.captures == 34);
	}
	
	SECTION("depth = 4") {
		auto res = g.perft(4);
		REQUIRE(res.nodes == 197281);
		REQUIRE(res.captures == 1576);
	}

	SECTION("depth = 5")
	{
		auto res = g.perft(5);
		REQUIRE(res.en_passants == 258);
		REQUIRE(res.captures == 82719);
		REQUIRE(res.nodes == 4865609);
	}

	SECTION("depth = 6") {
		auto res = g.perft(6);
		REQUIRE(res.nodes == 119060324ULL);
		REQUIRE(res.captures == 2812008ULL);
		REQUIRE(res.en_passants == 5248ULL);
	}

}

namespace
{
	void benchmark_perft(int depth)
	{
		game g;
		printf("========== BENCHMARK ==========\n");
		printf("Initial Perft depth = %i\n",depth);
		auto now = std::chrono::high_resolution_clock::now();
		auto res = g.perft(depth);
		auto then = std::chrono::high_resolution_clock::now();
		printf("Print so that it doesn't optimize the result away: %i\n", (int)res.nodes);
		printf("measured time in seconds: %f\n", std::chrono::duration_cast<std::chrono::milliseconds>(then - now).count() / 1000.f);
		printf("===============================\n\n");
	}
}

TEST_CASE("Benchmark")
{
	benchmark_perft(5);
	//benchmark_perft(6);
}













