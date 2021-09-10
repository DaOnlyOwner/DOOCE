#include "mailbox.h"

void mailbox::do_move(const move& m)
{
	auto p = mailbox_board[m.from];
	mailbox_board[m.from] = { color::white,piece_type::none };
	mailbox_board[m.to] = { p.c,p.type };
}

void mailbox::undo_move(const move& m)
{
	auto p = mailbox_board[m.to];
	mailbox_board[m.to] = { m.piece_captured.c,m.piece_captured.type };
	mailbox_board[m.from] = { p.c,p.type };
}
