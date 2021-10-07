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
	bitboard attacks_bishop(const board& b, uint bishop_idx);
	bitboard attacks_rook(const board& b, uint rook_idx);
	bitboard attacks_queen(const board& b, uint nat_idx);


	// ============================================================
	//   K I N G & K N I G H T  M O V E   G E N E R A T I O N
	// ============================================================

	bitboard attacks_king(const board&, uint king_idx); // The first argument is never used. This makes attack generation more convenient. 
	bitboard attacks_knight(const board&, uint knight_idx);

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

	// I need to pass pawns because sometimes I calculate for all pawns, sometimes for just one
	template<color VColor>
	bitboard attack_pawns_left(bitboard pawns, const board& b);
	
	template<color VColor>
	bitboard attack_pawns_right(bitboard pawns, const board& b);

	template<color VColor>
	bitboard move_pawns_single(bitboard pawns,const board& b);

	template<color VColor>
	bitboard move_pawns_dbl(bitboard pawns, const board& b);

	// This method expects as input an occupancy bitboard where only pawns (actually just one)  are set that can be captured en passant, i.e. that moved double.
	template<color VColor>
	bitboard en_passant_left(const board& b, bitboard pawns_on_en_passant_square);

	template<color VColor>
	bitboard en_passant_right(const board& b, bitboard pawns_on_en_passant_square);


}
