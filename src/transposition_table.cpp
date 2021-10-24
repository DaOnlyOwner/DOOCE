#include "transposition_table.h"

void insert_tt(TT& tt, u64 hash, i16 val, move_flag f, u8 depth, move best_move, u16 age)
{
	auto& entry = tt.get_no_checks(hash);
	if (entry.f != move_flag::none && (entry.depth < depth || entry.age > age))
	{
		entry = trans_entry(hash, val, f, depth, best_move, age);
	}

	else if (entry.f == move_flag::none)
	{
		entry = trans_entry(hash, val, f, depth, best_move, age);
	}
}
