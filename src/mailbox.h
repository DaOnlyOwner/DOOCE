#pragma once
#include "move.h"
#include "definitions.h"
#include "setwise_ops.h"

struct mailbox
{
	std::array<piece,64> mailbox_board;
	
	void do_move(const move& m);
	void undo_move(const move& m);
};