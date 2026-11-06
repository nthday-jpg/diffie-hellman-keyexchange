#include "dh_sample.h"

uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exp, const uint512_t &mod);
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize);
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p);
