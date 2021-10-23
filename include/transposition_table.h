#pragma once
#include <vector>
#include "definitions.h"
#include "move.h"
#include <atomic>
#include <mutex>

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

struct trans_entry_mt
{
	std::mutex lock{};
	u64 hash = 0;
	i16 val = 0;
	move_flag f = move_flag::none;
	u8 depth = 0;
	move best_move{};
	u16 age = 0;
	u8 nproc = 0;

	trans_entry_mt() = default;
	trans_entry_mt(u64 hash, i16 val, move_flag f, u8 depth, move best_move, u16 age)
		:hash(hash), val(val), f(f), depth(depth), best_move(best_move), age(age),nproc(0)
	{}

};


template<typename T>
class trans_table
{

public:
	trans_table(u64 cap):cap(cap)
	{
		if (cap == 0) throw std::runtime_error("No capacity for tt");
		if ((cap & (cap - 1)) != 0)
			throw std::runtime_error("Capacity is not a power of two");
		container = new T[cap];
	}

	~trans_table()
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

	void insert(u64 hash, i16 val, move_flag f, u8 depth, move best_move, u16 age)
	{
		auto& entry = container[idx(hash)];
		if (entry.f != move_flag::none && (entry.depth < depth || entry.age > age))
		{
			entry = trans_entry(hash, val, f, depth, best_move, age);
		}

		else if (entry.f == move_flag::none)
		{
			entry = trans_entry(hash, val, f, depth, best_move, age);
		}
	}

	void insert_mt(u64 hash, i16 val, move_flag f, u8 depth, move best_move, u16 age)
	{
		auto& entry = container[idx(hash)];
		if (entry.f != move_flag::none && (entry.depth < depth || entry.age > age))
		{
			entry.hash = hash;
			entry.val = val;
			entry.hash = hash;
			entry.best_move = best_move;
			entry.age = age;
			entry.f = f;
		}

		else if (entry.f == move_flag::none)
		{
			entry.hash = hash;
			entry.val = val;
			entry.hash = hash;
			entry.best_move = best_move;
			entry.age = age;
			entry.f = f;
		}
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
typedef trans_table<trans_entry> TT;
typedef trans_table<trans_entry_mt> TT_mt;