#include "dh_sample.h"
#include <iostream>
#include <iomanip>
#include <exception>

// In số lớn ở hệ thập lục phân, từ khối cao nhất đến khối thấp nhất.
static void print_hex(const char *label, const uint512_t &value)
{
    const auto flags = std::cout.flags();
    const auto fill = std::cout.fill();
    std::cout << label << "0x" << std::hex;
    bool started = false;
    for (size_t i = 8; i > 0; --i)
    {
        if (!started)
        {
            if (value.digits[i - 1] == 0 && i > 1)
                continue;
            std::cout << value.digits[i - 1];
            started = true;
        }
        else
            std::cout << std::setfill('0') << std::setw(16) << value.digits[i - 1];
    }
    std::cout.flags(flags);
    std::cout.fill(fill);
    std::cout << '\n';
}

int main()
{
    try
    {
        constexpr size_t bit_size = 512;

        // Khởi tạo PRNG bằng entropy hệ điều hành, không dùng seed cố định.
        uint8_t seed[32];
        randombytes(seed, sizeof(seed));
        prng_ctx ctx;
        prng_seed(&ctx, seed, sizeof(seed));

        std::cout << "Dang sinh so nguyen to an toan " << bit_size
                  << " bit..." << std::endl;
        const uint512_t p = generate_safe_prime(&ctx, bit_size);
        const uint512_t q = (p - uint512_t{1}) / uint512_t{2};

        // Với p = 2q + 1, hai điều kiện loại các phần tử có bậc 1, 2 hoặc q.
        uint512_t g;
        do
        {
            g = generate_private_key(&ctx, p);
        } while (modular_exponentiation(g, uint512_t{2}, p) == uint512_t{1} ||
                 modular_exponentiation(g, q, p) == uint512_t{1});

        // Alice và Bob giữ riêng a, b; chỉ trao đổi các giá trị công khai A, B.
        const uint512_t a = generate_private_key(&ctx, p);
        const uint512_t b = generate_private_key(&ctx, p);
        const uint512_t A = modular_exponentiation(g, a, p);
        const uint512_t B = modular_exponentiation(g, b, p);
        const uint512_t alice_secret = modular_exponentiation(B, a, p);
        const uint512_t bob_secret = modular_exponentiation(A, b, p);

        print_hex("p = ", p);
        print_hex("g = ", g);
        print_hex("A = ", A);
        print_hex("B = ", B);
        // Hiển thị bí mật chung theo yêu cầu minh họa của đề bài.
        print_hex("Bi mat chung cua Alice = ", alice_secret);
        print_hex("Bi mat chung cua Bob   = ", bob_secret);
        const bool matched = alice_secret == bob_secret;
        std::cout << "Bi mat chung trung khop: " << (matched ? "CO" : "KHONG") << '\n';
        return matched ? 0 : 1;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Loi trao doi khoa Diffie-Hellman: " << error.what() << '\n';
        return 1;
    }
}
