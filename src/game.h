#pragma once
#include <vector>
#include "move.h"
#include "board.h"

class game
{

public:
	game();

	template<color VColor>
	std::vector<move> gen_all_legal_moves(bool is_in_check)
	{
		std::vector<move> out;
		out.reserve(150);

		bitboard cpy;
		if constexpr (VColor == color::white)
			cpy = current_board.white_board;
		else cpy = current_board.black_board;
		bitboard all_occ = current_board.white_board & current_board.black_board;
		bitboard not_all_occ = ~all_occ;
		bitboard not_color_occ;
		bitboard all_attacks = 0;

		if constexpr (VColor == color::white)
			not_color_occ = ~current_board.white_board;
		else not_color_occ = ~current_board.black_board;

		uint idx = 0;
		while (cpy != 0)
		{
			idx += ops::num_trailing_zeros(cpy);
			auto p = current_board.mb_board.repr[idx];
			switch (p.type)
			{
			case piece_type::bishop:
				bitboard b_attacks = current_board.gen_attacks_bishop(all_occ, idx, not_color_occ);
				all_attacks |= b_attacks;
				make_move(b_attacks, idx, out, current_board.mb_board, VColor);
				break;
			case piece_type::king:
				bitboard k_attacks = current_board.gen_attacks_king(idx, not_color_occ);
				all_attacks |= k_attacks;
				make_move(k_attacks, idx, out, current_board.mb_board, VColor);
				break;
			case piece_type::knight:
				bitboard kn_attacks = current_board.gen_attacks_knight(idx, not_color_occ);
				all_attacks |= kn_attacks;
				make_move(kn_attacks, idx, out, current_board.mb_board, VColor);
				break;
			case piece_type::queen:
				bitboard q_attacks = current_board.gen_attacks_queen(all_occ, idx, not_color_occ);
				all_attacks |= q_attacks;
				make_move(q_attacks, idx, out, current_board.mb_board, VColor);
				break;
			case piece_type::rook:
				bitboard r_attacks = current_board.gen_attacks_rook(all_occ, idx, not_color_occ);
				all_attacks |= r_attacks;
				make_move(r_attacks, idx, out, current_board.mb_board, VColor);
				break;
			case piece_type::pawn:
				bitboard pawn_bit = ops::set_nth_bit(idx);
				if constexpr (VColor == color::white)
				{
					bitboard p_attacks = current_board.gen_attack_pawns_left_white(pawn_bit, current_board.black_board);
					p_attacks |= current_board.gen_attack_pawns_right_white(pawn_bit, current_board.black_board);
					all_attacks |= p_attacks;
					make_pawn_attack_move<color::white>(p_attacks, idx, out, current_board.mb_board);
					bitboard dbl_move = current_board.gen_move_pawns_dbl_white(pawn_bit, not_all_occ);
					make_move(dbl_move, idx, out, current_board.mb_board, color::white);
					bitboard pawn_single_move = current_board.gen_move_pawns_single_white(pawn_bit, not_all_occ);
					make_pawn_single_push_move<color::white>(pawn_single_move, idx, out);
				}
				else
				{
					bitboard p_attacks = current_board.gen_attack_pawns_left_black(pawn_bit, current_board.white_board);
					p_attacks |= current_board.gen_attack_pawns_right_black(pawn_bit, current_board.white_board);
					all_attacks |= p_attacks;
					make_pawn_attack_move<color::black>(p_attacks, idx, out, current_board.mb_board);
					bitboard pawn_dbl_move = current_board.gen_move_pawns_dbl_black(pawn_bit, not_all_occ);
					make_move(pawn_dbl_move, idx, out, current_board.mb_board, color::black);
					bitboard pawn_single_move = current_board.gen_move_pawns_single_black(pawn_bit, not_all_occ);
					make_pawn_single_push_move<color::black>(pawn_single_move,idx,out);
				}
				break;
			default:
				break;

			}
			cpy &= cpy - 1;
		}

		if constexpr (c = color::white)
		{
			if (!moved_king_white && !moved_rook_kingside_white && current_board.can_castle_kingside_white(all_occ, all_attacks))
			{
				move m;
				m.type_of_move = move_type::castling_kingside;
				out.push_back(m);
			}

			if (!moved_king_white && !moved_rook_queenside_white && current_board.can_castle_queenside_white(all_occ, all_attacks))
			{
				move m;
				m.type_of_move = move_type::castling_queenside;
				out.push_back(m);
			}



		}


	}

private:
	void make_move(bitboard attacks, uint from, std::vector<move>& out, const mailbox& mb_board, color c);
	
	template<color VColor>
	void make_pawn_single_push_move(bitboard attacks, uint from, std::vector<move>& out)
	{
		move m;
		m.from = from;
		m.to = ops::num_trailing_zeros(attacks);
		m.piece_captured = { color::white,piece_type::none };
		m.piece_moved = piece_type::pawn;
		if constexpr (c == color::white)
		{
			bool on_8th_rank = (ops::mask_rank(8) & attacks) > 0;
			if(on_8th_rank) add_promo_moves(m, out);
			else {
				m.type_of_move = move_type::quiet;
				out.push_back(m);
			}
		}

		else
		{
			bool on_1st_rank = (ops::mask_rank(1) & attacks) > 0; 
			if (on_1st_rank) add_promo_moves(m, out);
			else {
				m.type_of_move = move_type::quiet;
				out.push_back(m);
			}
		}
	}

	template<color VColor>
	void make_pawn_attack_move(bitboard attacks, uint from, std::vector<move>& out, const mailbox& mb_board)
	{
		uint idx = 0;
		while (attacks)
		{
			move m;
			idx = ops::num_trailing_zeros(attacks);
			m.from = from;
			m.to = idx;
			m.piece_moved = mb_board.repr[from].type;
			m.piece_captured = { VColor,mb_board.repr[idx].type };
			
			if constexpr (c == color::white)
			{
				bool on_8th_rank = (ops::mask_rank(8) & attacks) > 0;
				if (on_8th_rank) add_promo_capture_moves(m, out);
				else
				{
					m.type_of_move = move_type::captures;
					out.push_back(m);
				}
			}

			else
			{
				bool on_1st_rank = (ops::mask_rank(1) & attacks) > 0;
				if (on_1st_rank) add_promo_capture_moves(m, out);
				else
				{
					m.type_of_move = move_type::captures;
					out.push_back(m);
				}
			}
			attacks &= attacks - 1

		}
	}

	
	void add_promo_moves(move& m, std::vector<move>& out);
	void add_promo_capture_moves(move& m, std::vector<move>& out);

	std::vector<move> move_list;
	bool moved_king_white,
		moved_king_black,
		moved_rook_queenside_white,
		moved_rook_queenside_black,
		moved_rook_kingside_white,
		moved_rook_kingside_black;
	board current_board;
};


