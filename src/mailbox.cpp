#include "mailbox.h"

void mailbox::do_move(const move& m)
{
	auto p = repr[m.from];
	repr[m.from] = { color::white,piece_type::none };
	switch (m.type_of_move)
	{
	case move_type::bishop_promo:
		repr[m.to] = { p.c,piece_type::bishop};
	case move_type::bishop_promo_capture:
		repr[m.to] = { p.c, piece_type::bishop };
	case move_type::queen_promo:
		repr[m.to] = { p.c, piece_type::queen };
	case move_type::queen_promo_capture:
		repr[m.to] = { p.c,piece_type::queen };
	case move_type::rook_promo:
		repr[m.to] = { p.c,piece_type::rook };
	case move_type::rook_promo_capture:
		repr[m.to] = { p.c,piece_type::rook };
	case move_type::knight_promo:
		repr[m.to] = { p.c,piece_type::knight };
	case move_type::knight_promo_capture:
		repr[m.to] = { p.c,piece_type::knight };
	default:
		repr[m.to] = { p.c,p.type };
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
