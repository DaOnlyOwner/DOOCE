#include "pybind11/pybind11.h"
#include "pybind11/stl_bind.h"
#include "game.h"
#include "board.h"
#include "definitions.h"
#include "fen.h"
#include "move.h"
#include <vector>
#include "perft.h"

// https://stackoverflow.com/questions/2653214/stringification-of-a-macro-value
#define xstr(a) str(a)
#define str(a) #a

namespace py = pybind11;

#define MAKE_VALUE(enum_, val) value(xstr(val), enum_##::##val)
#define MAKE_SQUARE_VALUE(val) MAKE_VALUE(square,val)
#define MAKE_PIECE_VALUE(val) MAKE_VALUE(piece_type, val)
#define MAKE_MOVE_VALUE(val) MAKE_VALUE(move_type, val)

typedef move move_; // It somehow doesn't compile when I do this without the underscore

PYBIND11_MAKE_OPAQUE(std::vector<move_>);

PYBIND11_MODULE(dooce, m)
{

	py::bind_vector<std::vector<move_>>(m, "MoveVector");
	py::enum_<color>(m, "Color", py::arithmetic())
		.value("white", color::white)
		.value("black", color::black);

	py::enum_<square>(m, "Square", py::arithmetic())
		.MAKE_SQUARE_VALUE(a1)
		.MAKE_SQUARE_VALUE(b1)
		.MAKE_SQUARE_VALUE(c1)
		.MAKE_SQUARE_VALUE(d1)
		.MAKE_SQUARE_VALUE(e1)
		.MAKE_SQUARE_VALUE(f1)
		.MAKE_SQUARE_VALUE(g1)
		.MAKE_SQUARE_VALUE(h1)
		.MAKE_SQUARE_VALUE(a2)
		.MAKE_SQUARE_VALUE(b2)
		.MAKE_SQUARE_VALUE(c2)
		.MAKE_SQUARE_VALUE(d2)
		.MAKE_SQUARE_VALUE(e2)
		.MAKE_SQUARE_VALUE(f2)
		.MAKE_SQUARE_VALUE(g2)
		.MAKE_SQUARE_VALUE(h2)
		.MAKE_SQUARE_VALUE(a3)
		.MAKE_SQUARE_VALUE(b3)
		.MAKE_SQUARE_VALUE(c3)
		.MAKE_SQUARE_VALUE(d3)
		.MAKE_SQUARE_VALUE(e3)
		.MAKE_SQUARE_VALUE(f3)
		.MAKE_SQUARE_VALUE(g3)
		.MAKE_SQUARE_VALUE(h3)
		.MAKE_SQUARE_VALUE(a4)
		.MAKE_SQUARE_VALUE(b4)
		.MAKE_SQUARE_VALUE(c4)
		.MAKE_SQUARE_VALUE(d4)
		.MAKE_SQUARE_VALUE(e4)
		.MAKE_SQUARE_VALUE(f4)
		.MAKE_SQUARE_VALUE(g4)
		.MAKE_SQUARE_VALUE(h4)
		.MAKE_SQUARE_VALUE(a5)
		.MAKE_SQUARE_VALUE(b5)
		.MAKE_SQUARE_VALUE(c5)
		.MAKE_SQUARE_VALUE(d5)
		.MAKE_SQUARE_VALUE(e5)
		.MAKE_SQUARE_VALUE(f5)
		.MAKE_SQUARE_VALUE(g5)
		.MAKE_SQUARE_VALUE(h5)
		.MAKE_SQUARE_VALUE(a6)
		.MAKE_SQUARE_VALUE(b6)
		.MAKE_SQUARE_VALUE(c6)
		.MAKE_SQUARE_VALUE(d6)
		.MAKE_SQUARE_VALUE(e6)
		.MAKE_SQUARE_VALUE(f6)
		.MAKE_SQUARE_VALUE(g6)
		.MAKE_SQUARE_VALUE(h6)
		.MAKE_SQUARE_VALUE(a7)
		.MAKE_SQUARE_VALUE(b7)
		.MAKE_SQUARE_VALUE(c7)
		.MAKE_SQUARE_VALUE(d7)
		.MAKE_SQUARE_VALUE(e7)
		.MAKE_SQUARE_VALUE(f7)
		.MAKE_SQUARE_VALUE(g7)
		.MAKE_SQUARE_VALUE(h7)
		.MAKE_SQUARE_VALUE(a8)
		.MAKE_SQUARE_VALUE(b8)
		.MAKE_SQUARE_VALUE(c8)
		.MAKE_SQUARE_VALUE(d8)
		.MAKE_SQUARE_VALUE(e8)
		.MAKE_SQUARE_VALUE(f8)
		.MAKE_SQUARE_VALUE(g8)
		.MAKE_SQUARE_VALUE(h8);

	py::enum_<piece_type>(m, "PieceType", py::arithmetic())
		.MAKE_PIECE_VALUE(rook)
		.MAKE_PIECE_VALUE(bishop)
		.MAKE_PIECE_VALUE(queen)
		.MAKE_PIECE_VALUE(king)
		.MAKE_PIECE_VALUE(pawn)
		.MAKE_PIECE_VALUE(knight);

	py::enum_<move_type>(m, "MoveType", py::arithmetic())
		.MAKE_MOVE_VALUE(quiet)
		.MAKE_MOVE_VALUE(captures)
		.MAKE_MOVE_VALUE(promo)
		.MAKE_MOVE_VALUE(pawn_single)
		.MAKE_MOVE_VALUE(pawn_double)
		.MAKE_MOVE_VALUE(promo_captures)
		.MAKE_MOVE_VALUE(king_castle)
		.MAKE_MOVE_VALUE(queen_castle)
		.MAKE_MOVE_VALUE(en_passant);

	m.def("fen_to_board", &fen::fen_to_board);
	m.def("board_to_fen", &fen::board_to_fen);
	m.def("fen_to_game", &fen::fen_to_game);
	m.def("game_to_fen", &fen::game_to_fen);

	m.def("invert_color", &invert_color);

	m.def("perft", &perft);

	py::class_<perft_results>(m, "PerftResults")
		.def(py::init<>())
		.def_readonly("nodes", &perft_results::nodes)
		.def_readonly("captures", &perft_results::captures)
		.def_readonly("en_passants", &perft_results::en_passants)
		.def_readonly("promos", &perft_results::promos)
		.def_readonly("castles", &perft_results::castles);

	py::class_<move>(m, "Move")
		.def(py::init<>())
		.def("set_from", &move::set_from)
		.def("set_to", &move::set_to)
		.def("get_from_as_bitboard", &move::get_from_as_bitboard)
		.def("get_from_as_square", [](const move& m) {return idx_to_sq(m.get_from_as_idx()); })
		.def("get_to_as_bitboard", &move::get_to_as_bitboard)
		.def("get_to_as_square", [](const move& m) {return idx_to_sq(m.get_to_as_idx()); })
		.def("set_move_type", &move::get_move_type)
		.def("get_move_type", &move::get_move_type)
		.def("set_moved_piece_type", &move::set_moved_piece_type)
		.def("get_moved_piece_type", &move::get_moved_piece_type)
		.def("set_promo_piece_type", &move::set_promo_piece_type)
		.def("get_promo_piece_type", &move::get_promo_piece_type)
		.def("set_captured_piece_type", &move::set_captured_piece_type)
		.def("get_captured_piece_type", &move::get_captured_piece_type)
		.def("__repr__", [](const move& m) {
		return "from: " + sq_idx_to_str(m.get_from_as_idx()) + ", " +
			"to: " + sq_idx_to_str(m.get_to_as_idx());
			});

	py::class_<castling_info>(m, "CastlingInfo")
		.def(py::init<>())
		.def_readonly("has_moved_king", &castling_info::has_moved_king)
		.def_readonly("has_moved_queenside_rook", &castling_info::has_moved_queenside_rook)
		.def_readonly("has_moved_kingside_rook", &castling_info::has_moved_kingside_rook);

	py::class_<game_context>(m, "GameContext")
		.def(py::init<>())
		.def_readonly("half_move_clock", &game_context::half_move_clock)
		.def_readonly("fullmoves", &game_context::fullmoves)
		.def("get_castling_info", py::overload_cast<color>(&game_context::get_castling_info))
		.def_readonly("turn", &game_context::turn)
		.def_readonly("en_passantable_pawn", &game_context::en_passantable_pawn);

	py::class_<board>(m, "Board")
		.def(py::init<>(), "Constructs a board in starting position")
		.def(py::init<const std::string&>(), "Constructs a new board based on the given representation.\n"
			"The representation has to look like the first part of a fen string, but with dots ('.') between the pieces. Spaces don't matter.\n"
			"If you want to convert from fen strings use the fen_to_board function")
		.def("get_board", py::overload_cast<piece_type, color>(&board::get_board), "Retrieves the bitboard of the given piece and color")
		.def("get_board_of_side", [](const board& b, color c) {return c == color::white ?
			b.get_board_of_side<color::white>() : b.get_board_of_side<color::black>(); }, "Retrieves the whole bitboard (all pieces) of the given color")
		.def("get_board_of_side_not", [](const board& b, color c) {return c == color::white ?
			b.get_board_of_side_not<color::white>() : b.get_board_of_side_not<color::black>(); }, "the same as ~get_board_of_side()")
		.def("get_whole_board", &board::get_whole_board, "Retrieves the whole bitboard (all pieces, both colors) ")
		.def("__repr__", [](const board& b) {return fen::board_to_fen(b); });

	py::class_<game>(m, "Game")
		.def(py::init<>(), "Constructs a new game in initial starting condition")
		.def(py::init<const board&, const game_context&>(), "Constructs a new game from the given board and game context")
		.def("legal_moves", [](game& g)
			{return g.get_game_context().turn == color::white ? g.legal_moves<color::white>() : g.legal_moves<color::black>(); },
			"Generate all legal moves for the current position and the current side")
		.def("do_move", [](game& g, const move& m)
			{
				if (g.get_game_context().turn == color::white) g.do_move<color::white>(m);
				else g.do_move<color::black>(m);
			}, "Apply the move to the current position for the current side")
		.def("undo_move", [](game& g)
			{
				if (g.get_game_context().turn == color::white) g.undo_move<color::black>();
				else g.undo_move<color::white>();
			}, "Undo the last move")
		.def("can_castle", [](game& g)
			{
				return g.get_game_context().turn == color::white ? g.can_castle<color::white>() : g.can_castle<color::black>();
			}, "Can the current side castle?")
		.def("is_in_check", [](game& g)
			{
				return g.get_game_context().turn == color::white ? g.is_in_check<color::white>() : g.is_in_check<color::black>();
			}, "Is the king of the current side in check?")
		.def("get_board", &game::get_board);
}
