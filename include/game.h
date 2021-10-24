#pragma once
// TODO: Move to cpp file
#include "definitions.h"
#include "move.h"
#include "board.h"
#include <optional>
#include <string>
#include <vector>
#include "zobrist_hash.h"
#include "array_vector.h"
#include "misc_tools.h"

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

	template<color VColor, bool VOnlyCaptures = false>
	std::vector<move> legal_moves();

	template<color VColor>
	void do_move(const move& m);

	template<color VColor>
	void undo_move();

	void do_nullmove();
	void undo_nullmove();

	template<color VColor>
	bool is_in_check() const;

	template<color VColor>
	std::pair<bool,bool> can_castle() const;

	const game_context& get_game_context() const;

	const board& get_board() const;

	bool is_draw_by_rep() const;
	bool is_draw_by_halfmoves() const;

	uint get_ply() const;
	u64 get_hash() const;

	template<color VColor>
	void gen_piece_attacks_for_idx(uint idx, array_vector<piece_type, 6>& out) const;

	// The dooce algebraic notation:
	/*
	* - Specify only the from square and the to square, even if it's a capture
	* - If kingside castles write 00
	* - If queenside castles write 000
	* - if promotion write "from_square"+"to_square"+"=Q" for example
	*/
	// TODO: Refactor
	template<color VColor>
	std::optional<move> from_dooce_algebraic_notation(const std::string& m);

	std::string from_move_to_dooce_algebraic_notation(const move& m);

	bool is_move_valid(const move& m);

private:
	struct move_list_elements
	{
		move_list_elements(const move& m, const game_context& gc, const zobrist_hash& zh)
			: m(m),gc(gc),zh(zh){}
		move m;
		game_context gc;
		zobrist_hash zh;
	};

	game_context gc;
	board b;
	zobrist_hash zh;
	// Retrieving the game_context from the stack is much simpler.
	std::vector<move_list_elements> move_list;

	static bool init;

	template<color VColor, bool VOnlyCaptures>
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
	piece_type determine_capturing(color c, bitboard set_bit) const;

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
