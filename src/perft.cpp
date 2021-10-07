#include "perft.h"


namespace
{
	template<color VColor>
	perft_results perft_inner(game& g, int depth, const move& m)
	{
		constexpr color ecolor = invert_color(VColor);
		if (depth == 0)
		{
			perft_results res{};
			res.nodes = 1ULL;
			switch (m.get_move_type())
			{
			case move_type::captures:
				res.captures = 1ULL;
				break;
			case move_type::en_passant:
				res.en_passants = 1ULL;
				res.captures = 1ULL;
				break;
			case move_type::king_castle:
				res.castles = 1ULL;
				break;
			case move_type::queen_castle:
				res.castles = 1ULL;
				break;
			case move_type::promo:
				res.promos = 1ULL;
				break;
			case move_type::promo_captures:
				res.promos = 1ULL;
				res.captures = 1ULL;
				break;
			case move_type::quiet:
				break;
			default:
				break;
			}
			return res;
		};

		auto moves = g.legal_moves<VColor>();
		perft_results res{};
		for (const move& m : moves)
		{
			g.do_move<VColor>(m);
			update_perft_results(perft_inner<ecolor>(g, depth - 1, m), res);
			g.undo_move<VColor>();
		}
		return res;
	}
	void update_perft_results(const perft_results& res, perft_results& to_update)
	{
		to_update.captures += res.captures;
		to_update.castles += res.castles;
		to_update.en_passants += res.en_passants;
		to_update.promos += res.promos;
		to_update.nodes += res.nodes;
	}
}

perft_results perft(game& g, int depth)
{
	if (g.get_game_context().turn == color::white)
		return perft_inner<color::white>(g, depth, move{});
	else return perft_inner<color::black>(g, depth, move{});
}


// TODO: move perft into tests section, don't ship this with the chess library

//perft_results game::perft(int depth)
//{
//	board_info binfo;
//	attack_pattern pattern;
//	int size; move m{};
//
//	if (start_color == color::white)
//		game::extract_board<color::white>(binfo);
//	else
//		game::extract_board<color::black>(binfo);
//
//	if (start_color == color::white)
//		game::gen_all_attack_pattern_except_en_passant<color::white>(pattern, size, binfo);
//	else
//		game::gen_all_attack_pattern_except_en_passant<color::black>(pattern, size, binfo);
//
//	if (start_color == color::white)
//		return perft_inner<color::white>(depth, start_info_white, start_info_black,
//			start_en_passantable_pawn, pattern, size, binfo, m);
//	else
//		return perft_inner<color::black>(depth, start_info_black, start_info_white,
//			start_en_passantable_pawn, pattern, size, binfo, m);
//}
//
