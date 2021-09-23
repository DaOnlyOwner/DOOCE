#define CATCH_CONFIG_MAIN  
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "game.h"

game_info ginfo{ true,true,true };

TEST_CASE("INIT")
{
	board::init_all();
}

TEST_CASE("Move generation king")
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

	game g2(
		"..k....."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		".....K..", ginfo, ginfo, color::white, 0ULL
	);
	res = g2.perft(1);
	REQUIRE(res.nodes == 5);
}

TEST_CASE("Move generation pawn")
{
	SECTION("Pawn attack")
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

	SECTION("Pawn double move")
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

}

TEST_CASE("Move generation rook")
{
	game g(
		"........"
		"........"
		"....R..."
		"........"
		"..k.p..."
		"........"
		"........"
		"....K..." , ginfo,ginfo,color::white,0ULL
	);

	auto res = g.perft(1);
	REQUIRE(res.captures == 1);
	REQUIRE(res.nodes == 16);

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

	res = g2.perft(1);
	REQUIRE(res.captures == 0);
	REQUIRE(res.nodes == 17);

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

	res = g3.perft(1);
	REQUIRE(res.captures == 4);
	REQUIRE(res.nodes == 11);
}

TEST_CASE("Move Generation Bishop")
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
	REQUIRE(res.nodes == 8);
}




TEST_CASE("Move Generation Perft", "[move_gen]")
{
	game g;

	// Check initial position
	SECTION("perft initial depth = 0") {
		auto res = g.perft(0);
		REQUIRE(res.nodes == 1);
	}

	SECTION("perft initial depth = 1") {
		auto res = g.perft(1);
		REQUIRE(res.nodes == 20);
		REQUIRE(res.castles == 0);
		REQUIRE(res.en_passants == 0);
		REQUIRE(res.promos == 0);
		REQUIRE(res.captures == 0);
	}

	SECTION("perft initial depth = 2") {
		auto res = g.perft(2);
		REQUIRE(res.nodes == 400);
		REQUIRE(res.captures == 0);
		REQUIRE(res.en_passants == 0);
		REQUIRE(res.castles == 0);
		REQUIRE(res.promos == 0);
	}

	
	SECTION("perft initial depth = 3") {
		auto res = g.perft(3);
		REQUIRE(res.captures == 34);
		REQUIRE(res.nodes == 8902);
	}
	/*
	SECTION("perft initial depth = 4") {
		auto res = g.perft(4);
		REQUIRE(res.nodes == 197281);
	}

	SECTION("perft initial depth = 5") {
		auto res = g.perft(5);
		REQUIRE(res.nodes == 4865609);
	}

	SECTION("perft initial depth = 6") {
		auto res = g.perft(6);
		REQUIRE(res.nodes == 119060324);
	}*/

}











