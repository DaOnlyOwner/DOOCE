#pragma once
#include "definitions.h"
#include "move.h"
#include "board.h"
#include <random>

class zobrist_hash
{

public:
	zobrist_hash();
	zobrist_hash(const game_context& gc, const board& b);
	static u64 make_hash(const game_context& gc, const board& b); // For debugging purposes
	u64 get_hash() const;
	template<color c>
	void do_undo_move(bitboard ep_prev, uint ep_idx_after, const std::array<castling_info,2> ci_prev, const game_context& after_gc, const move& m);
private:
	static void init_all();
	static void init_piece_hash();
	static void init_side_to_move_hash();
	static void init_castling_rights_hash();
	static void init_ep_square_hash();


	void handle_quiet_move(uint ptype,uint color,uint from,uint to);

	static std::mt19937_64 gen;
	static std::uniform_int_distribution<u64> distr;
	static u64 piece_hash[2][6][64];
	static u64 black_to_move_hash;
	static u64 castling_rights_hash[2][2];
	static u64 ep_square_hash[8];
	static bool init;

	u64 hash = 0ULL;
};

