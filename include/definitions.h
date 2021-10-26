#pragma once
#include <bitset>
#include <array>
#include <cstdint>

typedef std::bitset<64> bitboard_constr;
typedef uint64_t bitboard;
typedef uint64_t u64;
typedef unsigned int uint;
typedef int16_t i16;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

inline constexpr i16 pos_inf = 31000;
inline constexpr i16 neg_inf = -31000;
inline constexpr int max_ply = 300;
inline constexpr i16 mate = 30000;

static_assert(sizeof(unsigned long long) == sizeof(uint64_t), "unsigned long long needs to be 64 bit");




enum class piece_type : uint
{
	pawn=0, rook, bishop, knight, king, queen,none
};

constexpr inline bool piece_has_value(piece_type pt)
{
	return pt != piece_type::none;
}

enum class color : uint
{
	white=0, black=1
};

enum class move_type : uint
{
	quiet = 0,
	pawn_single = 1,
	pawn_double = 2,
	king_castle = 3,
	queen_castle = 4,
	captures = 5,
	en_passant = 6,
	promo = 7,
	promo_captures = 8,
	none = 9
};

constexpr inline color invert_color(color c)
{
	return static_cast<color>(1 - static_cast<uint>(c));
}


struct piece
{
	color c;
	piece_type type;
	bool operator<(const piece& lh) const
	{
		int c_ = static_cast<int>(c);
		int t = static_cast<int>(type);
		int lc = static_cast<int>(lh.c);
		int lt = static_cast<int>(lh.type);
		return std::tie(c_,t) < std::tie(lc,lt);
	}

	bool operator==(const piece& lh) const
	{
		return lh.c == c && lh.type == type;
	}

};

// TODO: refactor addressing the bitboard starting with A1=0, A2=1, ..., H1=7, A2=8, ..., H8=63
enum class square : uint
{
	a8=63, b8=62, c8=61, d8=60, e8=59, f8=58, g8=57, h8=56,
	a7=55, b7=54, c7=53, d7=52, e7=51, f7=50, g7=49, h7=48,
	a6=47, b6=46, c6=45, d6=44, e6=43, f6=42, g6=41, h6=40,
	a5=39, b5=38, c5=37, d5=36, e5=35, f5=34, g5=33, h5=32,
	a4=31, b4=30, c4=29, d4=28, e4=27, f4=26, g4=25, h4=24,
	a3=23, b3=22, c3=21, d3=20, e3=19, f3=18, g3=17, h3=16,
	a2=15, b2=14, c2=13, d2=12, e2=11, f2=10, g2= 9, h2= 8,
	a1= 7, b1= 6, c1= 5, d1= 4, e1= 3, f1= 2, g1= 1, h1= 0
};

/*
enum class square : uint
{
	a1 = 63, b1 = 62, c1 = 61, d1 = 60, e1 = 59, f1 = 58, g1 = 57, h1 = 56,
	a2 = 55, b2 = 54, c2 = 53, d2 = 52, e2 = 51, f2 = 50, g2 = 49, h2 = 48,
	a3 = 47, b3 = 46, c3 = 45, d3 = 44, e3 = 43, f3 = 42, g3 = 41, h3 = 40,
	a4 = 39, b4 = 38, c4 = 37, d4 = 36, e4 = 35, f4 = 34, g4 = 33, h4 = 32,
	a5 = 31, b5 = 30, c5 = 29, d5 = 28, e5 = 27, f5 = 26, g5 = 25, h5 = 24,
	a6 = 23, b6 = 22, c6 = 21, d6 = 20, e6 = 19, f6 = 18, g6 = 17, h6 = 16,
	a7 = 15, b7 = 14, c7 = 13, d7 = 12, e7 = 11, f7 = 10, g7 = 9, h7 = 8,
	a8 = 7, b8 = 6, c8 = 5, d8 = 4, e8 = 3, f8 = 2, g8 = 1, h8 = 0
};*/

constexpr inline uint sq_to_int(square s) { return static_cast<uint>(s); }
constexpr inline square idx_to_sq(uint idx) { return static_cast<square>(idx); }
inline std::string sq_to_str(square sq)
{
	uint s = sq_to_int(sq);
	int y = s >> 3;
	int x = 7 - (s & 7);
	std::string out;
	out += ('a' + x);
	out += ('1' + y);
	return out;
}

inline std::string sq_idx_to_str(uint idx)
{
	int y = idx >> 3;
	int x = 7 - (idx & 7);
	std::string out;
	out += ('a' + x);
	out += ('1' + y);
	return out;
}

inline uint str_to_sq_idx(const std::string& sq)
{
	int x = 'h' - sq[0];
	int y = (sq[1] - '0')-1;
	return y * 8 + x;
}

inline square str_to_sq(const std::string& sq)
{
	return idx_to_sq(str_to_sq_idx(sq));
}


struct castling_info
{
	bool has_moved_king;
	bool has_moved_queenside_rook;
	bool has_moved_kingside_rook;
	bool queenside_right() const
	{
		return !has_moved_king && !has_moved_queenside_rook;
	}

	bool kingside_right() const
	{
		return !has_moved_king && !has_moved_kingside_rook;
	}
};

struct game_context
{
	std::array<castling_info, 2> castling_info_for_sides{};
	bitboard en_passantable_pawn = 0ULL;
	color turn = color::white;
	castling_info& get_castling_info(color c) { return castling_info_for_sides[static_cast<uint>(c)]; }
	uint fullmoves = 1;
	uint half_move_clock = 0;
	const castling_info& get_castling_info(color c) const { return castling_info_for_sides[static_cast<uint>(c)]; }

	void set_game_info(color c, const castling_info& info) { castling_info_for_sides[static_cast<uint>(c)] = info; }
};

struct hq_mask
{
	bitboard mask;
	bitboard diagEx;
	bitboard antidiagEx;
	bitboard fileEx;
	typedef std::array<hq_mask, 64> lt;
};

typedef std::array<bitboard, 64> move_lt;
