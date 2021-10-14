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
            auto fen = fen::game_to_fen(g);
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
		REQUIRE(res.en_passants == exp.en_passants);
		REQUIRE(res.captures == exp.captures);
		REQUIRE(res.nodes == exp.nodes);
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

TEST_CASE("Debug")
{
    //benchmark_perft(6);
    //benchmark_perft(5);
    //check_branches("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4);
    //check_branches("r3k2N/p1p1q1b1/bn1ppnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQq - 0 2", 1);
    //check_branches("r3k2r/p1p1qpb1/bN2pnp1/3P4/1p2P3/2N2Q2/PPPBBPpP/R3K2R b KQkq - 0 2", 1);
}

#if 1
// Positions from chessprogramming.org
TEST_CASE("Perft Position 2")
{
    // Position from chessprogramming.org
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    auto g = fen::fen_to_game(fen);

    SECTION("depth = 1")
    {
        validate_position(g, 1, perft_results{ 48, 8, 0, 2, 0 });
    }

    SECTION("depth = 2")
    {
        validate_position(g, 2, perft_results{ 2039, 351, 1, 91, 0 });
    }

    SECTION("depth = 3")
    {
        //check_branches(fen, 3);
        validate_position(g, 3, perft_results{ 97862, 17102, 45, 3162, 0 });
    }

    SECTION("depth = 4")
    {
        validate_position(g, 4, perft_results{ 4085603, 757163, 1929, 128013, 15172 });
    }

    SECTION("depth = 5")
    {
        validate_position(g, 5, perft_results{ 193690690 , 35043416 , 73365 ,4993637 ,8392 });
    }

}

 TEST_CASE("Perft Position 3")
 {
    game g = fen::fen_to_game("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
 	SECTION("depth = 1")
 	{
 		validate_position(g, 1, perft_results{ 14,1,0,0,0 });
 	}

 	SECTION("depth = 2")
 	{
 		validate_position(g, 2, perft_results{ 191,14,0,0,0 });
 	}

 	SECTION("depth = 3")
 	{
 		validate_position(g, 3, perft_results{ 2812,209,2,0,0 });
 	}

    SECTION("depth = 6")
    {
        validate_position(g, 6, perft_results{ 11030083 ,940350 ,33325 ,0 ,7552 });
    }

 }

 TEST_CASE("Perft Position 4")
 {
    game g = fen::fen_to_game("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
 	SECTION("depth = 1")
 	{
 		validate_position(g, 1, perft_results{ 6,0,0,0,0 });
 	}

 	SECTION("depth = 2")
 	{
 		validate_position(g, 2, perft_results{ 264,87,0,6,48 });
 	}

    SECTION("depth = 5")
    {
        validate_position(g, 5, perft_results{ 15833292 , 2046173 , 6512 ,0,329464 });
    }
 }

 TEST_CASE("Perft Position 5")
 {
     SECTION("depth = 3")
     {
         auto g = fen::fen_to_game("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
         auto res = perft(g, 3);
         REQUIRE(res.nodes == 62379);
     }
 }

 TEST_CASE("Perft Position 6")
 {
    game g = fen::fen_to_game("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
 	SECTION("depth = 1")
 	{
 		auto res = perft(g,1);
 		REQUIRE(res.nodes == 46);
 	}

 	SECTION("depth = 4")
 	{
 		auto res = perft(g,4);
 		REQUIRE(res.nodes == 3894594);
 	}
 }

 std::array<castling_info, 2> ci = { true,true,true,true,true,true };
 game_context gc{ ci, 0ULL,color::white,0,1 };
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

	SECTION("depth = 6") {
		validate_position(g, 6, perft_results{ 119060324ULL, 2812008ULL, 5248ULL, 0, 0 });
	}
}

 TEST_CASE("Benchmark")
 {
     // This takes exactly 11 sec with Hyperbola Quintessence
 	benchmark_perft(6);
 }
#endif