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
		REQUIRE(res.nodes == 8902);
		REQUIRE(res.captures == 34);
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










