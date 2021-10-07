#define CATCH_CONFIG_MAIN  
#include "catch.hpp"
#include "game.h"
#include <chrono>
#include <map>
#include "perft.h"
#include "attack_bitboards.h"
#include "fen.h"


namespace
{
    // TODO: Write in Python a test function that goes through the whole tree and checks wether something is missing or is too much.

    template<color VColor>
    void check_branches_inner(game& g, int depth, int indent)
    {
        // stop recursion when search depth is reached
        if (depth == 0) { return; }

        std::vector<move> moves = g.legal_moves<VColor>();
        // sort the moves lexicographically by fen string
        std::vector<std::string> fen_strings;
        std::map<std::string, move> fen2move;
        for (const move& m : moves)
        {
            g.do_move<VColor>(m);
            auto fen = fen::board_to_fen(g.get_board());
            fen_strings.push_back(fen);
            fen2move[fen] = m;
            g.undo_move<VColor>();
        }
        std::sort(fen_strings.begin(), fen_strings.end());

        // go through the moves in the FEN order
        for (const std::string& fen : fen_strings)
        {
            const auto& m = fen2move[fen];
            g.do_move<VColor>(m);
            for (int i = 0; i < indent * 2; i++) { printf(" "); }
            printf("%s\n", fen.c_str());

            constexpr color opp_color = invert_color(VColor);
            check_branches_inner<opp_color>(g, depth - 1, indent + 1);
            g.undo_move<VColor>();
        }
    }

    void check_branches(const std::string& fen, int depth)
    {
        game g = fen::fen_to_game(fen);
        if (g.get_game_context().turn == color::white)
            check_branches_inner<color::white>(g, depth, 0);
        else check_branches_inner<color::black>(g, depth, 0);
    }

	void assert_perft_result(perft_results res, perft_results exp)
	{
		REQUIRE(res.captures == exp.captures);
		REQUIRE(res.nodes == exp.nodes);
		REQUIRE(res.en_passants == exp.en_passants);
		REQUIRE(res.castles == exp.castles);
		REQUIRE(res.promos == exp.promos);
	}

	void validate_position(game& g, int depth, perft_results exp)
	{
		auto res = perft(g,depth);
		assert_perft_result(res, exp);
	}
	void benchmark_perft(int depth)
	{
		game g;
		printf("========== BENCHMARK ==========\n");
		printf("Initial Perft depth = %i\n", depth);
		auto now = std::chrono::high_resolution_clock::now();
		auto res = perft(g,depth);
		auto then = std::chrono::high_resolution_clock::now();
		printf("Print so that it doesn't optimize the result away: %i\n", (int)res.nodes);
		printf("measured time in seconds: %f\n", std::chrono::duration_cast<std::chrono::milliseconds>(then - now).count() / 1000.f);
		printf("===============================\n\n");
	}
}

TEST_CASE("INIT")
{
	gen::init_all();
}

TEST_CASE("Debug")
{
    check_branches("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5);
}

#if 0 
#if 0 
game_info ginfo{true, true, true};

namespace
{

	// void print_results(perft_results res)
	// {
	// 	printf("Total: %i\n", res.nodes);
	// }



}

TEST_CASE("INIT")
{
	game::init_all();
}

//// Positions from chessprogramming.org
//TEST_CASE("Perft Position 2")
//{
//	// Position from chessprogramming.org
//	std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
//	game g(fen);
//
//	// SECTION("depth = 1")
//	// {
//	// 	validate_position(g, 1, perft_results{ 48, 8, 0, 2, 0 });
//	// }
//
//	// SECTION("depth = 2")
//	// {
//	// 	validate_position(g, 2, perft_results{ 2039, 351, 1, 91, 0 });
//	// }
//
//	 SECTION("depth = 3")
//	 {
//		check_branches(fen, 3);
//	 	// validate_position(g, 3, perft_results{ 97862, 17102, 45, 3162, 0 });
//	 }
//
//	//  SECTION("depth = 4")
//	//  {
//	//  	validate_position(g, 4, perft_results{ 4085603, 757163, 1929, 128013, 15172 });
//	//  }
//}

// TEST_CASE("Perft Position 3")
// {
// 	game g("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
// 	SECTION("depth = 1")
// 	{
// 		validate_position(g, 1, perft_results{ 14,1,0,0,0 });
// 	}

// 	SECTION("depth = 2")
// 	{
// 		validate_position(g, 2, perft_results{ 191,14,0,0,0 });
// 	}

// 	SECTION("depth = 3")
// 	{
// 		validate_position(g, 3, perft_results{ 2812,209,2,0,0 });
// 	}

// }

// TEST_CASE("Perft Position 4")
// {
// 	game g("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
// 	SECTION("depth = 1")
// 	{
// 		validate_position(g, 1, perft_results{ 6,0,0,0,0 });
// 	}

// 	SECTION("depth = 2")
// 	{
// 		validate_position(g, 2, perft_results{ 264,87,0,6,48 });
// 	}
// }

// TEST_CASE("Perft Position 6")
// {
// 	game g("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
// 	SECTION("depth = 1")
// 	{
// 		auto res = g.perft(1);
// 		REQUIRE(res.nodes == 46);
// 	}

// 	SECTION("depth = 2")
// 	{
// 		auto res = g.perft(2);
// 		REQUIRE(res.nodes == 2079);
// 	}
// }



#endif


game_context gc{ {{true,true,true},{true,true,true}}, 0ULL,color::white };
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
            "........", gc
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
            ".....K..", gc
        );
        validate_position(g, 1, perft_results{ 5, 1, 0, 0, 0 });
    }
}

#if 0
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
        validate_position(g, 1, perft_results{ 16, 0, 0, 1, 0 });
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
        validate_position(g, 1, perft_results{ 15, 0, 0, 1, 0 });
    }
}
#endif

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
            "K.......", gc
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
            "K.......", gc
        );
        validate_position(g, 1, perft_results{ 5, 1, 0, 0, 0 });
    }

    SECTION("Black to move - everything")
    {
        // TODO: transform position into FEN string
        auto gc2 = gc;
        gc2.turn = color::black;
        game g(
            ". . . . . . . ."
            ". . . p . p p ."
            ". . . . . P . ."
            ". . p . . . P ."
            ". P . P . . . ."
            ". . . . . . . ."
            ". . . . . . . ."
            "K . . . . . . k", gc2
        );
        validate_position(g, 1, perft_results{ 10, 3, 0, 0, 0 });
    }

    SECTION("Shift outside of board")
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
            "K . . . . . . k", gc
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
            "....K...", gc
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
            "........", gc
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
            "........", gc
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
            "K . . . . . . .", gc
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
            "K . . . . . . .", gc
        );
        validate_position(g, 1, perft_results{ 6, 1, 0, 0, 0 });
    }

    auto gc2 = gc;
    gc2.turn = color::black;
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
            "K . . . . . . b", gc2
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
            "K . . . . . . .", gc2
        );
        validate_position(g, 1, perft_results{ 12, 4, 0, 0, 0 });
    }
}

TEST_CASE("Check and Checkmates")
{
    auto gc2 = gc;
    gc2.turn = color::black;
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
            ". . . . K . . .", gc2
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
            ". . . . . . . .", gc2
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
            ". . . . . . . .", gc2
        );
        validate_position(g, 1, perft_results{ 0, 0, 0, 0, 0 });
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
            ". . . . . . . .", gc
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
            ". . . . . . . .", gc
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
            "N . . . k . . N", gc
        );
        validate_position(g, 1, perft_results{ 13, 2, 0, 0, 0 });
    }
}

TEST_CASE("Initial Perft", "[move_gen]")
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
	SECTION("depth = 6") {
		validate_position(g, 6, perft_results{ 119060324ULL, 2812008ULL, 5248ULL, 0, 0 });
	}
}

 TEST_CASE("Benchmark")
 {
 	benchmark_perft(5);
 	// TODO: this test takes 10 seconds, think of adding it again if test time doesn't matter too much
 	//benchmark_perft(6);
 }
#endif