#include "game.h"
#include "attack_bitboards.h"

template<color VColor>
inline bitboard game::gen_attack_list()
{
	alist.clear();
	bitboard attacks =
		gen_attack_list_from_piece<VColor>(b.get_board(piece_type::bishop, VColor), piece_type::bishop, &gen::attacks_bishop)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::rook, VColor), piece_type::rook, &gen::attacks_rook)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::queen, VColor), piece_type::queen, &gen::attacks_queen)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::knight, VColor), piece_type::knight, &gen::attacks_knight)
		| gen_attack_list_from_piece<VColor>(b.get_board(piece_type::king, VColor), piece_type::king, &gen::attacks_king)
		| gen_attack_list_from_pawns<VColor>();

	// This is already ANDed by not_own_color_occ (in gen_attack_info_from_piece_type),
	// so I don't need to do that here again.
	return attacks;
}

template<color VColor>
bitboard game::gen_attack_list_from_pawns()
{
	bitboard attacks = 0ull;
	bitboard own_pawns = b.get_board(piece_type::pawn, VColor);
	uint idx = 0;
	while (own_pawns != 0ull)
	{
		idx = ops::num_trailing_zeros(own_pawns);
		bitboard only_pawn = ops::set_nth_bit(idx);
		attack_info ainfo;
		ainfo.attacks = gen::attack_pawns_left<invert_color(VColor)>(b);
		ainfo.attacks |= gen::attack_pawns_right<invert_color(VColor)>(b);
		if (ainfo.attacks != 0ull)
		{
			ainfo.from = idx;
			ainfo.ptype = piece_type::pawn;
			alist.add(ainfo);
			attacks |= ainfo.attacks;
		}
		ops::pop_lsb(own_pawns);
	}
	return attacks;
}

template<color VColor>
inline bitboard game::gen_attack_list_from_piece(bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint))
{
	uint idx = 0;
	bitboard attacks = 0ull;
	bitboard not_own_color_occ = b.get_board_of_side_not<VColor>();

	while (piece_occ != 0ull)
	{
		idx = ops::num_trailing_zeros(piece_occ);
		attack_info ainfo;
		ainfo.attacks = fn(b, idx) & not_own_color_occ;
		if (ainfo.attacks != 0ull)
		{
			ainfo.from = idx;
			ainfo.ptype = ptype;
			alist.add(ainfo);
			attacks |= ainfo.attacks;
		}
		ops::pop_lsb(piece_occ);
	}
	return attacks;
}

template bitboard game::gen_attack_list_from_piece<color::white>(bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));
template bitboard game::gen_attack_list_from_piece<color::black>(bitboard piece_occ, piece_type ptype, bitboard(*fn)(const board&, uint));

template bitboard game::gen_attack_list_from_pawns<color::white>();
template bitboard game::gen_attack_list_from_pawns<color::black>();

template bitboard game::gen_attack_list<color::white>();
template bitboard game::gen_attack_list<color::black>();

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
