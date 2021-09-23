#include "game.h"
#include "bitwise_ops.h"

game::game()
	: b(), start_info_white({ false,false,false }), start_info_black({ false,false,false }), start_color(color::white), start_en_passantable_pawn(0) {}

game::game(const std::string& start_board, const game_info& start_info_white, const game_info& start_info_black, color start_color, bitboard start_en_passantable_pawn)
	: b(start_board), start_info_white(start_info_white), start_info_black(start_info_black), start_color(start_color), start_en_passantable_pawn(start_en_passantable_pawn) {}

game::game(const std::string& start_board) 
	: b(start_board), start_info_white({ false,false,false }), start_info_black({ false,false,false }), start_color(color::white), start_en_passantable_pawn(0) {}

perft_results game::perft(int depth)
{
	board_info binfo;
	if (start_color == color::white)
		game::extract_board<color::white>(binfo);
	else game::extract_board<color::black>(binfo);
	attack_pattern pattern;
	int size;
	if(start_color == color::white)
		game::gen_all_attack_pattern_except_en_passant<color::white>(pattern, size, binfo);
	else game::gen_all_attack_pattern_except_en_passant<color::black>(pattern, size, binfo);
	move m{};
	if(start_color == color::white)
		return perft_inner<color::white>(depth, start_info_white, start_info_black, start_en_passantable_pawn, pattern, size, binfo,m);
	else return perft_inner<color::black>(depth, start_info_black, start_info_white, start_en_passantable_pawn, pattern, size, binfo,m);
}


std::optional<piece_type> game::determine_capturing(const board_info& info, bitboard set_bit)
{
	bitboard not_set_bit = ~set_bit;
	std::optional<piece_type> ptype;
	if ((info.enemy_pawns & not_set_bit) != info.enemy_pawns) ptype = piece_type::pawn;
	else if ((info.enemy_bishops & not_set_bit) != info.enemy_bishops) ptype = piece_type::bishop;
	else if ((info.enemy_knights & not_set_bit) != info.enemy_knights) ptype = piece_type::knight;
	else if ((info.enemy_rooks & not_set_bit) != info.enemy_rooks) ptype = piece_type::rook;
	else if ((info.enemy_queens & not_set_bit) != info.enemy_queens) ptype = piece_type::queen;
	return ptype;
}

void game::push_promo_moves(std::vector<move>& out, move& m)
{
	m.set_promo_piece_type(piece_type::knight);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::queen);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::rook);
	out.push_back(m);
	m.set_promo_piece_type(piece_type::bishop);
	out.push_back(m);
}

void game::update_perft_results(const perft_results& res, perft_results& to_update)
{
	to_update.captures += res.captures;
	to_update.castles += res.castles;
	to_update.en_passants += res.en_passants;
	to_update.promos += res.promos;
	to_update.nodes += res.nodes;
}

