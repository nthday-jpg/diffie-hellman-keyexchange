#include "u512_sample.h"
#include <stdexcept>

// Chia dài theo bit; giữ riêng bit tràn của phần dư khi dịch trái.
void divmod(const uint512_t &a, const uint512_t &b,
            uint512_t &quotient, uint512_t &remainder)
{
    if (b == uint512_t{})
    {
        throw std::domain_error("uint512_t: division by zero");
    }

    quotient = uint512_t{};
    remainder = uint512_t{};
    for (size_t bit = 512; bit > 0; --bit)
    {
        const size_t index = bit - 1;
        const bool overflow = (remainder.digits[7] >> 63) != 0;
        uint64_t carry = (a.digits[index / 64] >> (index % 64)) & 1;
        for (size_t i = 0; i < 8; ++i)
        {
            const uint64_t next = remainder.digits[i] >> 63;
            remainder.digits[i] = (remainder.digits[i] << 1) | carry;
            carry = next;
        }

        if (overflow || remainder >= b)
        {
            remainder = remainder - b;
            quotient.digits[index / 64] |= uint64_t{1} << (index % 64);
        }
    }
}

uint512_t operator*(const uint512_t &a, const uint512_t &b)
{
    uint512_t result;
    for (size_t i = 0; i < 8; ++i)
    {
        uint128_t carry = 0;
        for (size_t j = 0; j < 8 - i; ++j)
        {
            const uint128_t product = static_cast<uint128_t>(a.digits[i]) *
                                      b.digits[j] + result.digits[i + j] + carry;
            result.digits[i + j] = static_cast<uint64_t>(product);
            carry = product >> 64;
        }
    }
    return result;
}

uint512_t operator/(const uint512_t &a, const uint512_t &b)
{
    uint512_t quotient, remainder;
    divmod(a, b, quotient, remainder);
    return quotient;
}

uint512_t operator%(const uint512_t &a, const uint512_t &b)
{
    uint512_t quotient, remainder;
    divmod(a, b, quotient, remainder);
    return remainder;
}

uint512_t operator+(const uint512_t &a, const uint512_t &b)
{
    uint512_t result;
    uint64_t carry = 0;

    for (size_t i = 0; i < 8; ++i)
    {
        const uint64_t sum = a.digits[i] + b.digits[i];
        const bool carry_from_sum = sum < a.digits[i];
        result.digits[i] = sum + carry;
        const bool carry_from_previous = result.digits[i] < sum;
        carry = carry_from_sum || carry_from_previous;
    }

    return result;
}

uint512_t operator-(const uint512_t &a, const uint512_t &b)
{
    uint512_t result;
    uint64_t borrow = 0;

    for (size_t i = 0; i < 8; ++i)
    {
        const uint64_t difference = a.digits[i] - b.digits[i];
        const bool borrow_from_difference = a.digits[i] < b.digits[i];
        result.digits[i] = difference - borrow;
        const bool borrow_from_previous = difference < borrow;
        borrow = borrow_from_difference || borrow_from_previous;
    }

    return result;
}

bool operator==(const uint512_t &a, const uint512_t &b)
{
    for (size_t i = 0; i < 8; ++i)
    {
        if (a.digits[i] != b.digits[i])
        {
            return false;
        }
    }
    return true;
}

bool operator<(const uint512_t &a, const uint512_t &b)
{
    // So sánh từ khối có trọng số cao nhất xuống thấp nhất.
    for (size_t i = 8; i > 0; --i)
    {
        if (a.digits[i - 1] != b.digits[i - 1])
        {
            return a.digits[i - 1] < b.digits[i - 1];
        }
    }
    return false;
}

bool operator>(const uint512_t &a, const uint512_t &b)
{
    return b < a;
}

bool operator>=(const uint512_t &a, const uint512_t &b)
{
    return !(a < b);
}

bool operator<=(const uint512_t &a, const uint512_t &b)
{
    return !(b < a);
}
