#include "mailbox.h"
#include <map>

mailbox::mailbox()
	:mailbox(
		"rnbqkbnr"
		"pppppppp"
		"00000000"
		"00000000"
		"00000000"
		"00000000"
		"PPPPPPPP"
		"RNBQKBNR"
	)
{
	
}

mailbox::mailbox(const std::string& b)
{
	for (int i = 0; i < 64; i++)
	{
		char c = b[i];
		uint nat_idx = ops::flip_idx(i);
		std::map<char, piece> char_piece_map = {
			{'B',{color::white,piece_type::bishop}},
			{'b',{color::black,piece_type::bishop}},
			{'Q',{color::white,piece_type::queen}},
			{'q',{color::black,piece_type::queen}},
			{'K',{color::white,piece_type::king}},
			{'k',{color::black,piece_type::king}},
			{'N',{color::white,piece_type::knight}},
			{'n',{color::black,piece_type::knight}},
			{'P',{color::white,piece_type::pawn}},
			{'p',{color::black,piece_type::pawn}},
			{'R',{color::white,piece_type::rook}},
			{'r',{color::black,piece_type::rook}},
			{'0',{color::white,piece_type::none}}
		};
		repr[nat_idx] = char_piece_map[c];
	}
}

void mailbox::do_move(const move& m)
{
	auto p = repr[m.from];
	repr[m.from] = { color::white,piece_type::none };
	switch (m.type_of_move)
	{
	case move_type::bishop_promo:
		repr[m.to] = { p.c,piece_type::bishop};
		break;
	case move_type::bishop_promo_capture:
		repr[m.to] = { p.c, piece_type::bishop };
		break;
	case move_type::queen_promo:
		repr[m.to] = { p.c, piece_type::queen };
		break;
	case move_type::queen_promo_capture:
		repr[m.to] = { p.c,piece_type::queen };
		break;
	case move_type::rook_promo:
		repr[m.to] = { p.c,piece_type::rook };
		break;
	case move_type::rook_promo_capture:
		repr[m.to] = { p.c,piece_type::rook };
		break;
	case move_type::knight_promo:
		repr[m.to] = { p.c,piece_type::knight };
		break;
	case move_type::knight_promo_capture:
		repr[m.to] = { p.c,piece_type::knight };
		break;
	default:
		repr[m.to] = { p.c,p.type };
		break;
	}
}

void mailbox::undo_move(const move& m)
{
	auto p = repr[m.to];
	repr[m.to] = { m.piece_captured.c,m.piece_captured.type };
	bool was_promo = m.type_of_move == move_type::bishop_promo ||
		m.type_of_move == move_type::bishop_promo_capture ||
		m.type_of_move == move_type::queen_promo ||
		m.type_of_move == move_type::queen_promo_capture ||
		m.type_of_move == move_type::rook_promo ||
		m.type_of_move == move_type::rook_promo_capture ||
		m.type_of_move == move_type::knight_promo ||
		m.type_of_move == move_type::knight_promo_capture;
	if (was_promo)
		repr[m.from] = { p.c,piece_type::pawn };
	else repr[m.from] = { p.c,p.type };
}
