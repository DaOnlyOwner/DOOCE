#pragma once
#include "definitions.h"
#include "board.h"
#include "game.h"
#include <string>
#include <vector>

//void update_perft_results(const perft_results& res, perft_results& to_update);
//
//template<color VColor>
//perft_results perft_inner(int depth, game_info& ginfo_own, game_info& ginfo_enemy, bitboard en_passantable_pawn,
//	const attack_pattern& pattern, int size, const board_info& binfo, const move& m)
//{
//	constexpr color ecolor = invert_color(VColor);
//	if (depth == 0)
//	{
//		perft_results res{};
//		res.nodes = 1ULL;
//		switch (m.get_move_type())
//		{
//		case move_type::captures:
//			res.captures = 1ULL;
//			break;
//		case move_type::en_passant:
//			res.en_passants = 1ULL;
//			res.captures = 1ULL;
//			break;
//		case move_type::king_castle:
//			res.castles = 1ULL;
//			break;
//		case move_type::queen_castle:
//			res.castles = 1ULL;
//			break;
//		case move_type::promo:
//			res.promos = 1ULL;
//			break;
//		case move_type::promo_captures:
//			res.promos = 1ULL;
//			res.captures = 1ULL;
//			break;
//		case move_type::quiet:
//			break;
//		default:
//			break;
//		}
//		return res;
//	};
//
//	perft_results res_overall{};
//
//	std::vector<move> moves = game::gen_all_moves<VColor>(
//		pattern, size, binfo, en_passantable_pawn, ginfo_own);
//	for (const move& m : moves)
//	{
//		b.do_move<VColor>(m);
//		auto [ginfo_own_after, en_passantable_pawn_after] =
//			game::game_info_from_move<VColor>(m, ginfo_own);
//
//		board_info binfo_after;
//		extract_board<ecolor>(binfo_after);
//		attack_pattern pattern_after;
//		int size_after;
//		bitboard attacks = game::gen_all_attack_pattern_except_en_passant<ecolor>(
//			pattern_after, size_after, binfo_after);
//		// Test if king is in check and see if the move is legal
//		if (!b.is_king_in_check(VColor, attacks))
//		{
//			auto res = perft_inner<ecolor>(depth - 1, ginfo_enemy, ginfo_own_after,
//				en_passantable_pawn_after, pattern_after, size_after, binfo_after, m);
//			update_perft_results(res, res_overall);
//		}
//		/*else
//		{
//			printf("Attacks enemy: \n");
//			board::print_bitboard(attacks);
//			board::print_bitboard(king);
//		}*/
//		b.undo_move<VColor>(m);
//	}
//	return res_overall;
//}
