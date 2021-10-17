#pragma once

#include "definitions.h"
#include "bitwise_ops.h"
#include "move.h"
#include <optional>

#include <string>
#include <array>


void fill_board(std::string& out, bitboard b, piece_type pt, color c);

typedef std::array<std::array<piece, 8>, 8> mailbox;

class board
{
public:
	board();
	board(const std::string& start);
	board(const board& b) = default;
	board(board&& b) = default;
	board& operator=(const board& b) = default;
	board& operator=(board&& b) = default;

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
	mailbox as_mailbox() const;
	
	template<color VColor>
	bitboard get_board_of_side_not() const;

	std::pair<std::optional<piece_type>, color> get_occupation_of_idx(uint idx) const;

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
	void fill_mailbox(color c, piece_type pt, mailbox& mb) const;
};

