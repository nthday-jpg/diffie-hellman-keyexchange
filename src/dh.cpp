#include "dh_sample.h"
#include <stdexcept>

namespace
{
constexpr size_t primality_rounds = 64;

void shift_right_one(uint512_t &value)
{
    for (size_t i = 0; i < 8; ++i)
    {
        value.digits[i] >>= 1;
        if (i + 1 < 8)
            value.digits[i] |= value.digits[i + 1] << 63;
    }
}

size_t bit_length(const uint512_t &value)
{
    for (size_t i = 512; i > 0; --i)
        if ((value.digits[(i - 1) / 64] >> ((i - 1) % 64)) & 1)
            return i;
    return 0;
}

uint512_t random_bits(prng_ctx *ctx, size_t bits)
{
    uint512_t value{0};
    for (size_t i = 0; i < (bits + 63) / 64; ++i)
        value.digits[i] = prng_next_u64(ctx);
    if (bits % 64)
        value.digits[bits / 64] &= (uint64_t{1} << (bits % 64)) - 1;
    return value;
}

// Lấy mẫu đều trong [0, bound); lấy dư trực tiếp theo bound sẽ gây thiên lệch.
uint512_t random_below(prng_ctx *ctx, const uint512_t &bound)
{
    const size_t bits = bit_length(bound - uint512_t{1});
    uint512_t value;
    do
    {
        value = random_bits(ctx, bits);
    } while (value >= bound);
    return value;
}

// Chia thử để loại các số có ước nguyên tố nhỏ trước khi tính lũy thừa tốn kém.
bool passes_small_primes(const uint512_t &n)
{
    for (uint64_t divisor = 2; divisor <= 997; ++divisor)
    {
        bool prime = true;
        for (uint64_t d = 2; d * d <= divisor; ++d)
            if (divisor % d == 0)
            {
                prime = false;
                break;
            }
        if (prime)
        {
            uint64_t remainder = 0;
            for (size_t i = 8; i > 0; --i)
                remainder = ((uint128_t(remainder) << 64) | n.digits[i - 1]) % divisor;
            if (remainder == 0)
                return n == uint512_t{divisor};
        }
    }
    return n >= uint512_t{2};
}
}

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

namespace
{
// Bên gọi đã lọc bằng phép chia thử. Một hợp số còn lại vượt qua 64 vòng
// với cơ sở ngẫu nhiên độc lập, phân bố đều có xác suất tối đa 4^-64 = 2^-128.
bool is_probable_prime(prng_ctx *ctx, const uint512_t &n)
{
    if (n <= uint512_t{3})
        return n >= uint512_t{2};
    if (!(n.digits[0] & 1))
        return false;

    const uint512_t minus_one = n - uint512_t{1};
    uint512_t d = minus_one;
    size_t s = 0;
    while (!(d.digits[0] & 1))
    {
        shift_right_one(d);
        ++s;
    }
    for (size_t round = 0; round < primality_rounds; ++round)
    {
        const uint512_t a = random_below(ctx, n - uint512_t{3}) + uint512_t{2};
        uint512_t x = modular_exponentiation(a, d, n);
        if (x == uint512_t{1} || x == minus_one)
            continue;
        bool passed = false;
        for (size_t r = 1; r < s; ++r)
        {
            x = multiply_mod(x, x, n);
            if (x == minus_one)
            {
                passed = true;
                break;
            }
            if (x == uint512_t{1})
                return false;
        }
        if (!passed)
            return false;
    }
    return true;
}
}

uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize)
{
    if (!ctx)
        throw std::invalid_argument("generate_safe_prime: null PRNG context");
    if (bitsize < 3 || bitsize > 512)
        throw std::invalid_argument("generate_safe_prime: bit size must be in [3, 512]");

    for (;;)
    {
        uint512_t q = random_bits(ctx, bitsize - 1);
        q.digits[(bitsize - 2) / 64] |= uint64_t{1} << ((bitsize - 2) % 64);
        q.digits[0] |= 1;
        // q có tối đa 511 bit, nên 2*q+1 không thể làm tràn uint512_t.
        const uint512_t p = q + q + uint512_t{1};
        if (passes_small_primes(q) && passes_small_primes(p) &&
            is_probable_prime(ctx, q) && is_probable_prime(ctx, p))
            return p;
    }
}

uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p)
{
    if (!ctx)
        throw std::invalid_argument("generate_private_key: null PRNG context");
    if (p < uint512_t{5})
        throw std::invalid_argument("generate_private_key: p must be at least 5");
    return random_below(ctx, p - uint512_t{3}) + uint512_t{2};
}
