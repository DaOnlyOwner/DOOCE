#pragma once
#include "definitions.h"
#include "move.h"
#include "board.h"
#include <optional>
#include <string>
#include <vector>
#include "zobrist_hash.h"

class game
{
public:
	
	game();
	game(const board& b, const game_context& start_gc);
	game(const std::string& board_repr, const game_context& start_gc);
	game(const game& g) = default;
	game(game&& g) = default;
	game& operator=(const game&) = default;
	game& operator=(game&&) = default;
	~game() = default;

	template<color VColor>
	std::vector<move> legal_moves();

	template<color VColor>
	void do_move(const move& m);

	template<color VColor>
	void undo_move();

	template<color VColor>
	bool is_in_check() const;

	template<color VColor>
	std::pair<bool,bool> can_castle() const;

	const game_context& get_game_context() const;

	const board& get_board() const;

	bool is_threefold_repetition() const;

private:
	struct move_list_elements
	{
		move_list_elements(const move& m, const game_context& gc, const zobrist_hash& zh, bool is_threefold)
			: m(m),gc(gc),zh(zh),is_threefold_rep(is_threefold){}
		move m;
		game_context gc;
		zobrist_hash zh;
		bool is_threefold_rep;
	};


	game_context gc;
	board b;
	zobrist_hash zh;
	bool is_threefold_rep = false;
	// Retrieving the game_context from the stack is much simpler.
	std::vector<move_list_elements> move_list;

	static bool init;

	bool is_last_move_threefold_repetition() const;

	template<color VColor>
	void gen_attack_moves_from_piece(std::vector<move>& out, bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));

	template<color VColor>
	void gen_attack_moves_from_pawns(std::vector<move>& out);

	template<color VColor>
	void gen_attack_moves_from_pawns_inner(move& m, bitboard, std::vector<move>& out);

	bitboard gen_attack_bb_from_piece(bitboard piece_occ, bitboard(*fn)(const board&, uint)) const;

	// This method is basically only used for castling.
	template<color VColor>
	bitboard gen_attack_bb_except_en_passant() const;

	template<color VColor>
	void push_promo_moves(std::vector<move>& out, move& m);

	template<color VColor>
	bool add_when_legal(std::vector<move>& out, const move& m);

	template<color VColor>
	bool determine_promo(bitboard set_bit) const;
	std::optional<piece_type> determine_capturing(color c, bitboard set_bit) const;

	template<color VColor>
	void gen_move_pawn_push(std::vector<move>& out);

	typedef bitboard(*en_passant_fn)(const board&, bitboard);
	typedef bitboard(*shift_func)(bitboard);

	template<color VColor>
	void gen_move_en_passant(std::vector<move>& out,
		en_passant_fn efn, shift_func shift_white, shift_func shift_black);

	template<color VColor>
	void gen_move_castling(std::vector<move>& out);
};
