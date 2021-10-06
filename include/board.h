#pragma once

#include "definitions.h"
#include "bitwise_ops.h"
#include "move.h"

#include <string>



void fill_board(std::string& out, bitboard b, piece_type pt, color c);

class board
{
public:
	board();
	board(const std::string& start);

	// ============================================================
	//							P R I N T
	// ============================================================
	std::string pretty() const;
	static std::string get_bitboard(bitboard b);

	// ============================================================
	//              B I T B O A R D   H E L P E R S
	// ============================================================

	bitboard& get_board(piece_type ptype, color c);
	const bitboard& get_board(piece_type ptype, color c) const;
	template<color VColor>
	bitboard get_board_of_side() const;
	bitboard get_whole_board() const;
	
	template<color VColor>
	bitboard get_board_of_side_not() const;

	void recalculate_boards();

	// ============================================================
	//           A P P L Y   /   R E V E R T   M O V E
	// ============================================================

	template<color VColor>
	void do_move(const move& m);

	template<color VColor>
	void undo_move(const move& m);
private:
	bitboard boards[6][2];
	bitboard whole_board;
	bitboard black_side, white_side;
	bitboard not_black_side, not_white_side;

	inline void handle_quiet_move(bitboard& own, bitboard from, bitboard to);
};

