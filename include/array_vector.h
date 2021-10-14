#pragma once
#include <array>
#include <type_traits>

template<typename T, int VSize>
class array_vector
{
public:
	typedef std::array<T, VSize> array_t;
	void push_back(T&& val)
	{
		data[size++] = std::move(val);
	}

	void push_back(const T& val)
	{
		data[size++] = val;
	}
	
	template<typename... TArgs>
	void emplace_back(TArgs&&... args)
	{
		data[size++] = T(std::forward<TArgs>(args)...);
	}

	template<typename... TArgs>
	void emplace_back(const TArgs& args ...)
	{
		data[size++] = T(args...);
	}

	void clear()
	{
		size = 0;
	}

	T* begin()
	{
		return data.begin();
	}

	T* end()
	{
		return data.begin() + size;
	}

private:
	int size = 0;
	array_t data;
};
