#include "dh_sample.h"
#include <stdexcept>

// a, b < mod. Tránh tràn khi tính (a + b) % mod.
static uint512_t add_mod(const uint512_t &a, const uint512_t &b, const uint512_t &mod)
{
    const uint512_t gap = mod - b;
    return a >= gap ? a - gap : a + b;
}

// Nhân bằng cộng và nhân đôi, không làm mất các bit cao của tích.
static uint512_t multiply_mod(uint512_t a, const uint512_t &b, const uint512_t &mod)
{
    uint512_t result{0};
    for (size_t i = 0; i < 512; ++i)
    {
        if ((b.digits[i / 64] >> (i % 64)) & 1)
        {
            result = add_mod(result, a, mod);
        }
        a = add_mod(a, a, mod);
    }
    return result;
}

uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exp, const uint512_t &mod)
{
    if (mod == uint512_t{0})
    {
        throw std::domain_error("modular_exponentiation: modulus is zero");
    }

    uint512_t res = uint512_t{1} % mod;
    uint512_t reduced_base = base % mod;
    uint512_t r_exp = exp;

    while (r_exp > uint512_t{0})
    {
        if (r_exp.digits[0] & 1)
        {
            res = multiply_mod(res, reduced_base, mod);
        }

        // Dịch phải một bit: r_exp = floor(r_exp / 2).
        for (size_t i = 0; i < 8; ++i)
        {
            r_exp.digits[i] >>= 1;
            if (i + 1 < 8)
            {
                r_exp.digits[i] |= r_exp.digits[i + 1] << 63;
            }
        }
        if (r_exp > uint512_t{0})
        {
            reduced_base = multiply_mod(reduced_base, reduced_base, mod);
        }
    }
    return res;
}
