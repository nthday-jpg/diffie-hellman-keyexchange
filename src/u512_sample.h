#ifndef U512_H
#define U512_H

#define uint128_t __uint128_t
#include <stdint.h>
#include <stddef.h>
#include <iostream>

using namespace std;

struct uint512_t
{
    // Little-endian theo khối: digits[0] chứa 64 bit thấp nhất.
    uint64_t digits[8];

    uint512_t(uint64_t value = 0) : digits{}
    {
        for (size_t i = 0; i < 8; i++)
        {
            digits[i] = 0;
        }
    }
};

// Cộng/trừ lấy kết quả modulo 2^512 (bỏ carry/borrow cuối cùng).
uint512_t operator+(const uint512_t &a, const uint512_t &b);
uint512_t operator-(const uint512_t &a, const uint512_t &b);
bool operator==(const uint512_t &a, const uint512_t &b);
bool operator<(const uint512_t &a, const uint512_t &b);
bool operator>(const uint512_t &a, const uint512_t &b);
bool operator>=(const uint512_t &a, const uint512_t &b);
bool operator<=(const uint512_t &a, const uint512_t &b);

#endif
