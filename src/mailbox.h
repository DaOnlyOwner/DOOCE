#pragma once
#include "move.h"
#include "definitions.h"
#include "bitwise_ops.h"

struct mailbox
{
	std::array<piece,64> repr;
	
	void do_move(const move& m);
	void undo_move(const move& m);
};