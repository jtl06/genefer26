// Version-1 proof/certificate headers include a raw b_vec object.
// Keep its vector offset and total size compatible across 64-bit hosts.
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../src/b_vec.h"

template<size_t SIZE>
static bool check_layout()
{
	static_assert(alignof(b_vec<SIZE>) == 32);
	static_assert(sizeof(b_vec<SIZE>) == 32 + SIZE * 32);
	b_vec<SIZE> b;
	b.init(1000000, 2, 2);
	const auto * bytes = reinterpret_cast<const unsigned char *>(&b);
	for (size_t block = 0; block < SIZE; ++block)
	{
		for (size_t lane = 0; lane < 8; ++lane)
		{
			uint32_t value;
			std::memcpy(&value, bytes + 32 + (block * 8 + lane) * sizeof(value), sizeof(value));
			if (value != b[block][lane]) return false;
		}
	}
	return true;
}

int main()
{
	if (!check_layout<1>() || !check_layout<2>() || !check_layout<4>()) return 1;
	std::cout << "Version-1 base-vector layout checks passed\n";
}
