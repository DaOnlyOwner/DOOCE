#pragma once
#include "move.h"
#include "definitions.h"
#include "bitwise_ops.h"
#include <string>

struct mailbox
{

	mailbox();
	mailbox(const std::string& b);
	std::array<piece,64> repr;
	
	void do_move(const move& m);
	void undo_move(const move& m);
	



};