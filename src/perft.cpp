#include "perft.h"

// TODO: move perft into tests section, don't ship this with the chess library

perft_results game::perft(int depth)
{
	board_info binfo;
	attack_pattern pattern;
	int size; move m{};

	if (start_color == color::white)
		game::extract_board<color::white>(binfo);
	else
		game::extract_board<color::black>(binfo);

	if (start_color == color::white)
		game::gen_all_attack_pattern_except_en_passant<color::white>(pattern, size, binfo);
	else
		game::gen_all_attack_pattern_except_en_passant<color::black>(pattern, size, binfo);

	if (start_color == color::white)
		return perft_inner<color::white>(depth, start_info_white, start_info_black,
			start_en_passantable_pawn, pattern, size, binfo, m);
	else
		return perft_inner<color::black>(depth, start_info_black, start_info_white,
			start_en_passantable_pawn, pattern, size, binfo, m);
}

void game::update_perft_results(const perft_results& res, perft_results& to_update)
{
	to_update.captures += res.captures;
	to_update.castles += res.castles;
	to_update.en_passants += res.en_passants;
	to_update.promos += res.promos;
	to_update.nodes += res.nodes;
}