#pragma once
#include <vector>
#include "move.h"
#include "board.h"
#include <optional>

struct board_info
{
	bitboard own_bishops;
	bitboard own_king;
	bitboard own_knights;
	bitboard own_pawns;
	bitboard own_queens;
	bitboard own_rooks;
	bitboard enemy_bishops;
	bitboard enemy_king;
	bitboard enemy_knights;
	bitboard enemy_queens;
	bitboard enemy_pawns;
	bitboard enemy_rooks;
	bitboard own_color_occ;
	bitboard enemy_color_occ;
	bitboard not_own_color_occ;
	bitboard occ;
};

class game
{
public:
	game();

	// This method is basically only used for castling.
	template<color VColor>
	static bitboard gen_all_attacks_except_en_passant(/*bitboard en_passantable_pawn,*/ const board_info& info)
	{
		bitboard attacks = 
			gen_all_attacks_from_piece_type(info.own_bishops, info.occ, &board::gen_attacks_bishop) |
			gen_all_attacks_from_piece_type(info.own_rooks, info.occ,&board::gen_attacks_rook) |
			gen_all_attacks_from_piece_type(info.own_queens, info.occ, &board::gen_attacks_queen) |
			gen_all_attacks_from_piece_type(info.own_knights, info.occ,&board::gen_attacks_knight) |
			gen_all_attacks_from_piece_type(info.own_king, info.occ, &board::gen_attacks_king);

		attacks |= board::gen_attack_pawns_left<VColor>(info.own_pawns, info.enemy_color_occ);
		attacks |= board::gen_attack_pawns_right<VColor>(info.own_pawns, info.enemy_color_occ);
		// The following is probably not needed because this method is used only for castling and en_passant attacks cannot put the king in check or attack the squares the king needs for castling.
		//attacks |= board::gen_en_passant_left<VColor>(info.own_pawns, en_passantable_pawn);
		//attacks |= board::gen_en_passant_right<VColor>(info.own_pawns, en_passantable_pawn);
		return attacks & info.not_own_color_occ;
	}

	// This method is used to precalc the attack patterns to test if the enemy king is in check
	template<color VColor>
	static void gen_all_attack_info_except_en_passant(attack_pattern& pattern, int& size, const board_info& info)
	{
		size = 0;
		bitboard attacks =
			gen_attack_info_from_piece_type(info.own_bishops, info, pattern, size, piece_type::bishop, &board::gen_attacks_bishop) |
			gen_attack_info_from_piece_type(info.own_rooks, info, pattern, size, piece_type::rook, &board::gen_attacks_rook) |
			gen_attack_info_from_piece_type(info.own_queens, info, pattern, size, piece_type::queen, &board::gen_attacks_queen) |
			gen_attack_info_from_piece_type(info.own_knights, info, pattern, size, piece_type::knight, &board::gen_attacks_knight) |
			gen_attack_info_from_piece_type(info.own_king, info, pattern, size, piece_type::king, &board::gen_attacks_king) |
			gen_attack_info_from_pawns<VColor>(info, pattern,size);
		return attacks; // This is already ANDed by info.not_own_color_occ (in gen_attack_info_from_piece_type), so I don't need to do that here again. 
	}


	template<color VColor>
	static std::vector<move> gen_all_legal_moves(const attack_pattern& pattern, int size, const board_info& info, bitboard en_passantable_pawn)
	{
		std::vector<move> out;
		out.reserve(650);
		
		for (int i = 0; i<size; i++)
		{
			gen_moves_from_attack_pattern<VColor>(pattern, i, info, out);
		}

		// en_passant
		gen_move_en_passant_left<VColor>(info, en_passantable_pawn, out);
		gen_move_en_passant_right<VColor>(info, en_passantable_pawn, out);

		// pawn push
		gen_move_pawn_push<VColor>(info, out);

		// castling
	}
private:
	board b;
	
	static void push_promo_moves(std::vector<move>& out, move& m);


	template<color VColor>
	static void gen_move_pawn_push(const board_info& info, std::vector<move>& out)
	{
		bitboard cpy = info.own_pawns;
		uint idx = 0;
		while (cpy != 0)
		{
			idx = ops::num_trailing_zeros(cpy);
			bitboard set_bit = ops::set_nth_bit(idx);
			move m{};
			m.set_from(idx);
			m.set_to(board::gen_move_pawns_single<VColor>(set_bit, info.not_own_color_occ));
			m.set_moved_piece_type(piece_type::pawn);
			m.set_move_type(move_type::quiet);
			out.push_back(m);
			m.set_to(board::gen_move_pawns_dbl<VColor>(set_bit, info.not_own_color_occ));
			out.push_back(m);
		}
	}

	template<color VColor>
	static void gen_move_en_passant_left(const board_info& info, bitboard en_passantable_pawn, std::vector<move>& out)
	{
		bitboard attack = board::gen_en_passant_left(info.own_pawns, en_passantable_pawn);
		if (attack != 0)
		{
			move m{};
			m.set_captured_piece_type(piece_type::pawn);
			m.set_moved_piece_type(piece_type::pawn);
			m.set_move_type(move_type::en_passant);
			if constexpr (VColor == color::white)
				m.set_from(ops::num_trailing_zeros(ops::so_ea(attack)));
			else m.set_from(ops::num_trailing_zeros(ops::no_ea(attack)));
			m.set_to(ops::num_trailing_zeros(attack));
			out.push_back(m);
		}
	}

	template<color VColor>
	static void gen_move_en_passant_right(const board_info& info, bitboard en_passantable_pawn, std::vector<move>& out)
	{
		bitboard attack = board::gen_en_passant_left(info.own_pawns, en_passantable_pawn);
		if (attack != 0)
		{
			move m{};
			m.set_captured_piece_type(piece_type::pawn);
			m.set_moved_piece_type(piece_type::pawn);
			m.set_move_type(move_type::en_passant);
			if constexpr (VColor == color::white)
				m.set_from(ops::num_trailing_zeros(ops::so_we(attack)));
			else m.set_from(ops::num_trailing_zeros(ops::no_we(attack)));
			m.set_to(ops::num_trailing_zeros(attack));
			out.push_back(m);
		}
	}

	template<color VColor>
	static void gen_moves_from_attack_pattern(const attack_pattern& pattern, int i, const board_info& info, std::vector<move>& out)
	{
		const attack_info& ainfo = pattern[i];
		bitboard cpy = ainfo.attacks;
		uint idx = 0;
		while (cpy != 0)
		{
			move m{};
			idx = ops::num_trailing_zeros(cpy);
			bitboard set_bit = ops::set_nth_bit(idx);
			auto captured = determine_capturing(info, set_bit);
			m.set_from(from);
			m.set_to(to);
			m.set_moved_piece_type(ainfo.ptype);
			m.set_captured_piece_type(captured);
			bool promo = determine_promo<VColor>(ainfo.ptype, info, set_bit);
			if (promo)
			{
				move_type mtype = captured.has_value() ? move_type::promo_captures : move_type::promo;
				m.set_move_type(mtype);
				push_promo_moves(out, m);
			}

			else
			{
				move_type mtype = captured.has_value() ? move_type::captures : move_type::quiet;
				m.set_move_type(mtype);
				out.push_back(m);
			}
		}
	}

	template<color VColor>
	static bool determine_promo(const attack_info& ainfo, const board_info& info, bitboard set_bit)
	{
		if (ainfo.ptype != piece_type::pawn) return {};
		if constexpr (VColor == color::white)
			return ops::has_bit_set_on_rank(set_bit, 8);
		else return ops::has_bit_set_on_rank(set_bit, 1);
	}

	static std::optional<piece_type> determine_capturing(const board_info& info, bitboard set_bit)
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

	template<color VColor>
	static bitboard gen_attack_info_from_pawns(const board_info& info, attack_pattern& pattern, int& size)
	{
		bitboard attacks = 0ULL;
		bitboard own_pawns = info.own_pawns;
		while (own_pawns != 0)
		{
			idx += ops::num_trailing_zeros(own_pawns);
			bitboard only_pawn = ops::set_nth_bit(idx);
			attack_info ainfo;
			ainfo.attacks = board::gen_attack_pawns_left<VColor>(only_pawn, info.enemy_color_occ);
			ainfo.attacks |= board::gen_attack_pawns_right<VColor>(only_pawn, info.enemy_color_occ);
			ainfo.from = idx;
			ainfo.ptype = piece_type::pawn;
			pattern[size++] = ainfo;
			attacks |= ainfo.attacks;
		}

		return attacks;
	}


	template<typename GenFn>
	static bitboard gen_attack_info_from_piece_type(bitboard piece_occ, const board_info& info, attack_pattern& pattern, int& size, piece_type ptype, GenFn fn)
	{
		uint idx = 0;
		bitboard attacks = 0ULL;
		while (piece_occ != 0)
		{
			idx = ops::num_trailing_zeros(piece_occ);
			attack_info ainfo;
			ainfo.attacks = fn(info.occ, idx) & info.not_own_color_occ;
			ainfo.from = idx;
			ainfo.ptype = ptype;
			pattern[size++] = pa;
			piece_occ &= piece_occ - 1;
			attacks |= ainfo.attacks;
		}
		return attacks;
	}


	template<typename GenFn>
	static bitboard gen_all_attacks_from_piece_type(bitboard piece_occ, bitboard occ, GenFn fn)
	{
		uint idx = 0;
		bitboard attacks = 0;
		while (piece_occ != 0)
		{
			idx = ops::num_trailing_zeros(piece_occ);
			attacks |= fn(occ, idx);
			piece_occ &= piece_occ - 1;
		}
		return attacks;
	}

	template<color VColor>
	static void extract_board(board_info& info)
	{
		constexpr color ecolor = invert_color(VColor);
		info.own_bishops = b.get_board(piece_type::bishop, VColor);
		info.own_king = b.get_board(piece_type::king, VColor);
		info.own_knights = b.get_board(piece_type::knight, VColor);
		info.own_pawns = b.get_board(piece_type::pawn, VColor);
		info.own_queens = b.get_board(piece_type::queen, VColor);
		info.own_rooks = b.get_board(piece_type::rook, VColor);

		info.enemy_bishops = b.get_board(piece_type::bishop, ecolor);
		info.enemy_king = b.get_board(piece_type::king, ecolor);
		info.enemy_knights = b.get_board(piece_type::knight, ecolor);
		info.enemy_pawns = b.get_board(piece_type::pawn, ecolor);
		info.enemy_queens = b.get_board(piece_type::queen, ecolor);
		info.enemy_rooks = b.get_board(piece_type::rook, ecolor);

		info.own_color_occ = info.own_king | info.own_knights | info.own_bishops | info.own_queens | info.own_rooks | info.own_pawns;
		info.enemy_color_occ = info.enemy_bishops | info.enemy_king | info.enemy_knights | info.enemy_queens | info.enemy_rooks | info.enemy_pawns;
		info.not_own_color_occ = ~info.own_color_occ;
		info.occ = info.own_color_occ | info.enemy_color_occ;
	}
};


#if 0
class game
{

public:
	game();

	template<color VColor>
	std::vector<move> gen_all_legal_moves(bitboard* all_attacks_out)
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

			*all_attacks_out = all_attacks;
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
			attacks &= attacks - 1;
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

#endif
