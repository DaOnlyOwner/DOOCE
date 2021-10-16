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
		hash ^= ci.kingside_right() ? castling_rights_hash[c][0] : 0ULL;
		hash ^= ci.queenside_right() ? castling_rights_hash[c][1] : 0ULL;
	}
}

u64 zobrist_hash::get_hash() const
{
	return hash;
}

void zobrist_hash::handle_quiet_move(uint ptype, uint c, uint from, uint to)
{
	hash ^= piece_hash[c][ptype][from]
		^ piece_hash[c][ptype][to];
}

template<color VColor>
void zobrist_hash::do_undo_move(uint ep_previous,const std::array<castling_info,2> ci_previous, const game_context& gc_after, const move& m)
{
	constexpr uint c_idx = static_cast<uint>(VColor);
	uint from = m.get_from_as_idx();
	uint to = m.get_to_as_idx();
	uint ptype = static_cast<uint>(m.get_moved_piece_type());
	switch (m.get_move_type())
	{
	case move_type::pawn_single:
	case move_type::pawn_double:
	case move_type::quiet:
	{
		handle_quiet_move(ptype, c_idx, from, to);
	}
	break;
	case move_type::captures:
	{
		handle_quiet_move(ptype, c_idx, from, to);
		hash ^= piece_hash[1 - c_idx][ptype][to];
	}
	break;
	case move_type::en_passant:
	{
		handle_quiet_move(ptype, c_idx, from, to);
		if constexpr (c == color::white)
			hash ^= piece_hash[1 - c_idx][ptype][to + 8];
		else hash ^= piece_hash[1 - c_idx][ptype][to - 8];
	}
	break;

	case move_type::promo_captures:
		// We fall through to case move_type::promo which handles the promotion part.
	{
		hash ^= piece_hash[1 - c_idx][ptype][to];
	}
	case move_type::promo:
	{

		hash ^= piece_hash[c_idx][ptype][from];
		// Now promote to piece -> xor with the hash of the new piece
		hash ^= piece_hash[c_idx][static_cast<uint>(m.get_promo_piece_type())][to];
	}
	break;
	case move_type::king_castle:
	{
		constexpr uint rook_from_square = sq_to_int(c == color::white ? square::h1 : square::h8);
		constexpr uint rook_to_square = sq_to_int(c == color::white ? square::f1 : square::f8);
		constexpr uint king_from_square = sq_to_int(c == color::white ? square::e1 : square::e8);
		constexpr uint king_to_square = sq_to_int(c == color::white ? square::g1 : square::g8);
		handle_quiet_move(static_cast<uint>(piece_type::rook), c_idx, rook_from_square, rook_to_square);
		handle_quiet_move(static_cast<uint>(piece_type::king), c_idx, king_from_square, king_to_square);
	}
	break;
	case move_type::queen_castle:
	{
		constexpr auto rook_from_square = sq_to_int(c == color::white ? square::a1 : square::a8);
		constexpr auto rook_to_square = sq_to_int(c == color::white ? square::d1 : square::d8);
		constexpr auto king_from_square = sq_to_int(c == color::white ? square::e1 : square::e8);
		constexpr auto king_to_square = sq_to_int(c == color::white ? square::c1 : square::c8);
		handle_quiet_move(static_cast<uint>(piece_type::rook), c_idx, rook_from_square, rook_to_square);
		handle_quiet_move(static_cast<uint>(piece_type::king), c_idx, king_from_square, king_to_square);
	}
	break;
	}

	uint ep_idx = ops::get_rank(ops::num_trailing_zeros(ep_previous));
	hash ^= ep_square_hash[ep_idx]; // Remove the previous hash
	ep_idx = ops::get_rank(ops::num_trailing_zeros(gc_after.en_passantable_pawn));
	hash ^= ep_square_hash[ep_idx];
	auto& ci_after = gc_after.castling_info_for_sides;

	for (int c = 0; c < 2; c++)
	{
		if (ci_previous[c].kingside_right() != ci_after[c].kingside_right())
		{
			hash ^= castling_rights_hash[c][0];
		}

		if (ci_previous[c].queenside_right() != ci_after[c].queenside_right())
		{
			hash ^=	castling_rights_hash[c][1];
		}
	}
	hash ^= black_to_move_hash;
}

template void zobrist_hash::do_undo_move<color::black>
(uint ep_prev, const std::array<castling_info, 2> ci_prev, const game_context& after_gc, const move& m);
template void zobrist_hash::do_undo_move<color::white>
(uint ep_prev, const std::array<castling_info, 2> ci_prev, const game_context& after_gc, const move& m);



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
