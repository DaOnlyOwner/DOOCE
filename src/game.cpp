#include "game.h"
#include "bitwise_ops.h"
#include "misc_tools.h"


//// init game in start formation
//game::game()
//	: b(), start_info_white({ false,false,false }), start_info_black({ false,false,false }),
//	    start_color(color::white), start_en_passantable_pawn(0) {}
//
//// init game in a specific formation + context
//game::game(board b, game_context context)
///*: b(start_board), start_info_white(start_info_white), start_info_black(start_info_black),
//	start_color(start_color), start_en_passantable_pawn(start_en_passantable_pawn) {}*/
//{
//	// TODO: add proper initialization here ...
//}
//
//std::optional<piece_type> game::determine_capturing(const board_info& info, bitboard set_bit)
//{
//	bitboard not_set_bit = ~set_bit;
//	std::optional<piece_type> ptype;
//	if ((info.enemy_pawns & not_set_bit) != info.enemy_pawns) ptype = piece_type::pawn;
//	else if ((info.enemy_bishops & not_set_bit) != info.enemy_bishops) ptype = piece_type::bishop;
//	else if ((info.enemy_knights & not_set_bit) != info.enemy_knights) ptype = piece_type::knight;
//	else if ((info.enemy_rooks & not_set_bit) != info.enemy_rooks) ptype = piece_type::rook;
//	else if ((info.enemy_queens & not_set_bit) != info.enemy_queens) ptype = piece_type::queen;
//	return ptype;
//}
//
//void game::push_promo_moves(std::vector<move>& out, move& m)
//{
//	m.set_promo_piece_type(piece_type::knight);
//	out.push_back(m);
//	m.set_promo_piece_type(piece_type::queen);
//	out.push_back(m);
//	m.set_promo_piece_type(piece_type::rook);
//	out.push_back(m);
//	m.set_promo_piece_type(piece_type::bishop);
//	out.push_back(m);
//}
