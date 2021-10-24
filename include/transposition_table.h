#pragma once
#include <vector>
#include "definitions.h"
#include "move.h"

enum class move_flag : u8
{
	alpha,beta,exact,none
};
struct trans_entry
{
	u64 hash=0;
	i16 val=0;
	move_flag f = move_flag::none;
	u8 depth = 0;
	move best_move{};
	u16 age = 0;

	trans_entry() = default;
	trans_entry(u64 hash,i16 val, move_flag f, u8 depth, move best_move, u16 age)
		:hash(hash),val(val),f(f),depth(depth),best_move(best_move),age(age)
	{}

};

struct pv_entry
{
	u64 hash;
	move best_move;
	u16 age;
};


template<typename T>
class cache
{

public:
	cache(u64 cap):cap(cap)
	{
		if (cap == 0) throw std::runtime_error("No capacity for tt");
		if ((cap & (cap - 1)) != 0)
			throw std::runtime_error("Capacity is not a power of two");
		container = new T[cap];
	}

	~cache()
	{
		delete[] container;
	}

	T& operator[](u64 hash)
	{
		auto& entry = container[idx(hash)];
		return entry.hash == hash ? entry : nullentry;
	}

	const T& operator[](u64 hash) const
	{
		auto& entry = container[idx(hash)];
		return entry.hash == hash ? entry : nullentry;
	}

	const T& get_no_checks(u64 hash) const
	{
		return container[idx(hash)];
	}

	T& get_no_checks(u64 hash)
	{
		return container[idx(hash)];
	}

private:
	T nullentry{};
	T* container;
	u64 cap;

	u64 idx(u64 hash) const
	{
		return hash & (cap - 1);
	}

};

typedef cache<trans_entry> TT;
typedef cache<pv_entry> PV_T;

void insert_tt(TT& tt, u64 hash, i16 val, move_flag f, u8 depth, move best_move, u16 age);

