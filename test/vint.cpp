// Scalar-oracle checks for the host vector operations used by the GPU backend.
// Run on ARM64 and x86 (including builds with -mavx2 / -mavx512f when supported).
#include "../src/vint.h"
#include <cstdlib>
#include <iostream>
#include <random>

static void check(bool value)
{
	if (!value) { std::cerr << "Host vector check failed\n"; std::exit(EXIT_FAILURE); }
}

int main()
{
	std::mt19937_64 rng(26);
	for (unsigned round = 0; round < 4096; ++round)
	{
		uint32_t a[8], b[8]; uint64_t x[8], shift[8]; int32_t si[8];
		for (size_t i = 0; i < 8; ++i)
		{
			a[i] = uint32_t(rng()); b[i] = uint32_t(rng());
			x[i] = rng(); shift[i] = (round + i) % 64;
			si[i] = (i % 2 == 0) ? int32_t(a[i] >> 1) : -int32_t(a[i] >> 1);
		}
		if (round == 0) { a[0] = 0; a[1] = UINT32_MAX; si[0] = INT32_MIN; si[1] = INT32_MAX; }
		const UInt32_8 va(a), vb(b);
		const UInt64_8 vx(x), vs(shift);
		const UInt32_8 sum = va + vb, product = va * vb, maximum = va.max(vb), less = va < vb;
		const UInt64_8 rotated = vx.rotl(vs), extended = UInt32_8_to_UInt64_8(va);
		const UInt64_8 signed_extended = Int32_8_to_UInt64_8(Int32_8(si));
		const UInt32_8 truncated = UInt64_8_to_UInt32_8(vx);
		for (size_t i = 0; i < 8; ++i)
		{
			check(sum[i] == uint32_t(a[i] + b[i]));
			check(product[i] == uint32_t(uint64_t(a[i]) * b[i]));
			check(maximum[i] == std::max(a[i], b[i]));
			check(less[i] == (a[i] < b[i] ? UINT32_MAX : 0));
			check(rotated[i] == (shift[i] == 0 ? x[i] : (x[i] << shift[i]) | (x[i] >> (64 - shift[i]))));
			check(extended[i] == a[i]);
			check(signed_extended[i] == uint64_t(si[i]));
			check(truncated[i] == uint32_t(x[i]));
		}
		for (int bit = 0; bit < 32; ++bit)
		{
			uint8_t mask = 0;
			for (size_t i = 0; i < 8; ++i) mask |= uint8_t(((a[i] >> bit) & 1u) << i);
			check(va.get_bit_mask(bit) == mask);
		}
		check(va.min() == *std::min_element(a, a + 8));
		check(va.max() == *std::max_element(a, a + 8));
		check(va.is_equal(va));
	}
	check(UInt32_8(uint32_t(0)).is_zero());
	check(Int32_8(int32_t(-1)).is_true());
	std::cout << "Host vector checks passed\n";
}
