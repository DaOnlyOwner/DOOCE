#pragma once
#include <bitset>
#include <array>
#include <cstdint>

typedef std::bitset<64> bitboard_constr;
typedef uint64_t bitboard;
typedef unsigned int uint;

static_assert(sizeof(unsigned long long) == sizeof(uint64_t), "unsigned long long needs to be 64 bit");

enum class piece_type : uint
{
	pawn=0, rook, bishop, knight, king, queen
};

struct attack_info
{
	bitboard attacks;
	piece_type ptype;
	unsigned int from;
};

typedef std::array<attack_info,64> attack_pattern;

enum class color : uint
{
	white=0, black
};

constexpr inline color invert_color(color c)
{
	return static_cast<color>(1 - static_cast<uint>(c));
}


struct piece
{
	color c;
	piece_type type;
};

enum class square : uint
{
	a1 = 63, b1=62, c1=61, d1=60, e1=59, f1=58, g1=57, h1=56,
	a2=55, b2=54, c2=53, d2=52, e2=51, f2=50, g2=49, h2=48,
	a3=47, b3=46, c3=45, d3=44, e3=43, f3=42, g3=41, h3=40,
	a4=39, b4=38, c4=37, d4=36, e4=35, f4=34, g4=33, h4=32,
	a5=31, b5=30, c5=29, d5=28, e5=27, f5=26, g5=25, h5=24,
	a6=23, b6=22, c6=21, d6=20, e6=19, f6=18, g6=17, h6=16,
	a7=15, b7=14, c7=13, d7=12, e7=11, f7=10, g7=9, h7=8,
	a8=7, b8=6, c8=5, d8=4, e8=3, f8=2, g8=1, h8=0
};

static inline uint sq_to_int(square s) { return static_cast<uint>(s); }
static inline square idx_to_sq(uint idx) { return static_cast<square>(idx); }
