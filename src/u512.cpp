#include "u512_sample.h"

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
