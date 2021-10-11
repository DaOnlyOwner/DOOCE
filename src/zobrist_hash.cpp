#include "zobrist_hash.h"
#include <limits>
#include "bitwise_ops.h"

// Passed a constant random number for reproducibility
std::mt19937_64 zobrist_hash::gen(89237509826);
std::uniform_int_distribution<u64> zobrist_hash::distr(
	std::numeric_limits<u64>::min(),
	std::numeric_limits<u64>::max()
);

bool zobrist_hash::init = false;

u64 zobrist_hash::piece_hash[2][6][64];
u64 zobrist_hash::black_to_move_hash;
u64 zobrist_hash::castling_rights_hash[2][2];
u64 zobrist_hash::ep_square_hash[8];

zobrist_hash::zobrist_hash() :
	zobrist_hash(game_context(),board())
{
}

zobrist_hash::zobrist_hash(const game_context& gc, const board& b)
{
	if (!init) init_all();
	uint ep_idx = ops::get_rank(ops::num_trailing_zeros(gc.en_passantable_pawn));
	hash ^= ep_square_hash[ep_idx] ^ (gc.turn == color::black ? black_to_move_hash : 0);
	// Apply piece hash
	for (int c = 0; c < 2; c++)
	{
		for (int p = 0; p < 6; p++)
		{
			bitboard cpy = b.get_board(static_cast<piece_type>(p), static_cast<color>(c));
			while (cpy != 0)
			{
				uint idx = ops::num_trailing_zeros(cpy);
				hash ^= piece_hash[c][p][idx];
				ops::pop_lsb(cpy);
			}
		}
	}

	// Apply castling rights hash
	for (int c = 0; c < 2; c++)
	{
		auto& ci = gc.get_castling_info(static_cast<color>(c));
		hash ^= castling_rights_hash[c][0] ^ castling_rights_hash[c][1];
	}
}

u64 zobrist_hash::get_hash()
{
	return hash;
}

void zobrist_hash::handle_quiet_move(piece_type ptype, color c, uint from, uint to)
{
	hash ^= piece_hash[static_cast<uint>(c)][static_cast<uint>(ptype)][from]
		^ piece_hash[static_cast<uint>(c)][static_cast<uint>(ptype)][to];
}

template<color c>
void zobrist_hash::do_undo_move(const move& m)
{
	constexpr uint c_idx = static_cast<uint>(c);
	switch (m.get_move_type())
	{
	case move_type::pawn_single:
	case move_type::pawn_double:
	case move_type::quiet:
		handle_quiet_move(m.get_moved_piece_type(),c,m.get_from_as_idx(),m.get_to_as_idx());
		break;
	case move_type::captures:
		handle_quiet_move(m.get_moved_piece_type(), c, m.get_from_as_idx(), m.get_to_as_idx());
		hash ^= piece_hash[c_idx][static_cast<uint>(m.get_captured_piece_type())][m.get_to_as_idx()];
	
	// TODO: Finish
		

	}
}

template void zobrist_hash::do_undo_move<color::white>(const move& m);
template void zobrist_hash::do_undo_move<color::black>(const move& m);

void zobrist_hash::init_piece_hash()
{
	for (int c = 0; c < 2; c++) // Iterate over the color
	{
		for (int p = 0; p < 6; p++) // Iterate over the piece_type
		{
			for (int sq = 0; sq < 64; sq++) // Iterate over the square
			{
				piece_hash[c][p][sq] = distr(gen);
			}
		}
	}
}

void zobrist_hash::init_castling_rights_hash()
{
	for (int c = 0; c < 2; c++)
	{
		castling_rights_hash[c][0] = distr(gen);
		castling_rights_hash[c][1] = distr(gen);
	}
}

void zobrist_hash::init_ep_square_hash()
{
	for (int sq = 0; sq < 8; sq++)
	{
		ep_square_hash[sq] = distr(gen);
	}
}

void zobrist_hash::init_side_to_move_hash()
{
	black_to_move_hash = distr(gen);
}


void zobrist_hash::init_all()
{
	init_piece_hash();
	init_side_to_move_hash();
	init_castling_rights_hash();
	init_ep_square_hash();
	init = true;
}
