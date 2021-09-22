#include "game.h"
#include "bitwise_ops.h"

#if 0 
void game::add_promo_moves(move& m, std::vector<move>& out)
{
	m.type_of_move = move_type::bishop_promo;
	out.push_back(m);
	m.type_of_move = move_type::queen_promo;
	out.push_back(m);
	m.type_of_move = move_type::rook_promo;
	out.push_back(m);
	m.type_of_move = move_type::knight_promo;
	out.push_back(m);
}

void game::add_promo_capture_moves(move& m, std::vector<move>& out)
{
	m.type_of_move = move_type::bishop_promo_capture;
	out.push_back(m);
	m.type_of_move = move_type::queen_promo_capture;
	out.push_back(m);
	m.type_of_move = move_type::rook_promo_capture;;
	out.push_back(m);
	m.type_of_move = move_type::knight_promo_capture;;
	out.push_back(m);
}

void game::make_move(bitboard attacks, uint from, std::vector<move>& out, const mailbox& mb_board, color c)
{
	bitboard cpy = attacks;
	uint idx = 0;
	while (cpy != 0)
	{
		idx += ops::num_trailing_zeros(cpy);
		move m;
		m.from = from;
		m.to = idx;
		m.piece_moved = mb_board.repr[from].type;
		m.piece_captured = { c,mb_board.repr[idx].type };
		if (m.piece_captured.type != piece_type::none)
		{
			m.type_of_move = move_type::captures;
		}

		else m.type_of_move = move_type::quiet;
		out.push_back(m);
		cpy &= cpy - 1;
	}
}
#endif



uint64_t game::perft(int depth)
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
	if(start_color == color::white)
		return perft_inner<color::white>(depth, start_info_white, start_info_black, start_en_passantable_pawn, pattern, size, binfo);
	else return perft_inner<color::black>(depth, start_info_black, start_info_white, start_en_passantable_pawn, pattern, size, binfo);
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
