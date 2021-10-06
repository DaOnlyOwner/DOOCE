#pragma once
#include "definitions.h"
#include "bitwise_ops.h"
#include "board.h"

namespace gen
{
	
	// Init attack masks
	void init_all();
	void init_knight_attacks();
	void init_hq_masks();
	void init_king_attacks();

	// ============================================================
	//        S L I D E R S   D R A W   G E N E R A T I O N
	// ============================================================
	bitboard gen_attacks_bishop(const board& b, uint bishop_idx);
	bitboard gen_attacks_rook(const board& b, uint rook_idx);
	bitboard gen_attacks_queen(const board& b, uint nat_idx);


	// ============================================================
	//   K I N G & K N I G H T  M O V E   G E N E R A T I O N
	// ============================================================

	bitboard gen_attacks_king(uint king_idx);
	bitboard gen_attacks_knight(uint knight_idx);

	// ============================================================
	//							M I S C
	// ============================================================

	bool is_square_attacked(bitboard attacks, square sq);

	template<color VColor>
	bool can_castle_kingside(const board& b, bitboard attacks);

	template<color VColor>
	bool can_castle_queenside(const board& b, bitboard attacks);


	// ============================================================
	//           P A W N   M O V E   G E N E R A T I O N
	// ============================================================


	template<color VColor>
	bitboard gen_attack_pawns_left(const board& b);
	
	template<color VColor>
	bitboard gen_attack_pawns_right(const board& b);

	template<color VColor>
	bitboard gen_move_pawns_single(const board& b);

	template<color VColor>
	bitboard gen_move_pawns_dbl(const board& b);

	// This method expects as input an occupancy bitboard where only pawns (actually just one)  are set that can be captured en passant, i.e. that moved double.
	template<color VColor>
	bitboard gen_en_passant_left(const board& b, bitboard pawns_on_en_passant_square);

	template<color VColor>
	bitboard gen_en_passant_right(const board& b, bitboard pawns_on_en_passant_square);


}
