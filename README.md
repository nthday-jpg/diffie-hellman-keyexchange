# Lab01: Số nguyên tố & Trao đổi khóa Diffie-Hellman

Bài tập môn Nhập môn mã hóa mật mã — Bộ môn Công nghệ Tri thức.

## Mục tiêu

Xây dựng chương trình sinh số nguyên tố an toàn và mô phỏng trao đổi khóa Diffie-Hellman, giúp Alice và Bob tính được cùng một bí mật mà không truyền trực tiếp khóa riêng.

Các mục dưới đây là yêu cầu của bài tập. Checklist tiến độ chỉ đánh dấu những phần đã cài đặt; các yêu cầu còn lại chưa được xem là hoàn thành.

## Tiến độ hiện tại

- [x] Kiểu `uint512_t` gồm 8 khối `uint64_t`, khối thấp nhất ở `digits[0]`.
- [x] Khởi tạo từ `uint64_t`, sao chép/gán và các phép so sánh `==`, `<`, `>`, `<=`, `>=`.
- [x] Các toán tử `+`, `-`, `*`, `/`, `%`; chia hoặc lấy dư cho 0 ném `std::domain_error`.
- [x] Lũy thừa mô-đun bằng bình phương–nhân trong `src/dh.cpp`, dùng nhân mô-đun tránh mất dữ liệu trung gian.
- [x] Xử lý số mũ 0, mô-đun 1 và từ chối mô-đun 0.
- [x] Kiểm thử riêng các phép nhân/chia/lấy dư và lũy thừa mô-đun với C++14; đối chiếu lũy thừa với Python `pow()`, gồm toán hạng 512 bit.
- [ ] Đưa các kiểm thử riêng vào repo để có thể chạy lại tự động.
- [ ] Hỗ trợ thay đổi kích thước số lớn sang 1024/2048 bit.
- [ ] Kiểm tra nguyên tố và sinh số nguyên tố an toàn.
- [ ] Sinh khóa riêng trong miền yêu cầu.
- [ ] Hoàn thiện và kiểm thử toàn bộ luồng trao đổi khóa Diffie-Hellman.
- [ ] Hoàn thiện báo cáo và hướng dẫn chạy chương trình đầy đủ.

**Lưu ý số học:** `+`, `-`, `*` trả kết quả modulo `2^512`, không giữ phần tràn. Khi tính lũy thừa mô-đun, chương trình dùng cộng/nhân đôi mô-đun riêng thay vì `(a * b) % mod` để tránh sai kết quả do mất phần cao của tích. Thuật toán hiện chưa có thời gian chạy cố định.

## 1. Chức năng cần triển khai

### 1.1. Xử lý số nguyên lớn

- Hỗ trợ tính toán với số nguyên có độ lớn tối thiểu **512 bit**, cho phép điều chỉnh kích thước (ví dụ 1024 hoặc 2048 bit).
- Đảm bảo các phép toán phục vụ thuật toán hoạt động đúng, đặc biệt phép nhân và lấy mô-đun; tránh tràn số hoặc mất dữ liệu trung gian.
- Nếu tự xây dựng kiểu số lớn, mô tả cách biểu diễn dữ liệu và cách các toán tử hoạt động trong báo cáo.

### 1.2. Lũy thừa mô-đun — `modular_exponentiation(base, exponent, mod)`

- Tự cài đặt phép tính `(base^exponent) % mod` bằng thuật toán hiệu quả, chẳng hạn bình phương và nhân.
- Thực hiện phép lấy mô-đun trong quá trình tính, không tính toàn bộ `base^exponent` trước.
- Xử lý đúng số mũ bằng 0 và kiểm tra điều kiện mô-đun hợp lệ.

### 1.3. Sinh số nguyên tố an toàn — `generate_safe_prime(bit_size)`

- Sinh số `p` có đúng kích thước bit yêu cầu; cấu hình bài tập phải đạt tối thiểu 512 bit.
- Đảm bảo `p` và `q = (p - 1) / 2` đều là số nguyên tố.
- Tự cài đặt logic sinh ứng viên và kiểm tra nguyên tố, ví dụ Miller–Rabin; **không gọi thư viện sinh hoặc kiểm tra số nguyên tố thay cho phần tự cài đặt**.
- Nếu dùng kiểm tra xác suất, nêu số vòng kiểm tra và giới hạn xác suất sai trong báo cáo; không coi một lần kiểm tra là chứng minh nguyên tố tuyệt đối.

### 1.4. Sinh khóa riêng — `generate_private_key(p)`

- Sinh độc lập khóa riêng `a` cho Alice và `b` cho Bob trong khoảng **`[2, p - 2]`**.
- Sử dụng nguồn ngẫu nhiên phù hợp cho mật mã; tránh seed cố định trong chế độ chạy thông thường và tránh thiên lệch khi đưa số ngẫu nhiên về khoảng yêu cầu.
- Không công khai khóa riêng trong luồng trao đổi.

### 1.5. Trao đổi khóa Diffie-Hellman

1. Sinh số nguyên tố an toàn `p` và chọn phần tử sinh `g` phù hợp.
2. Sinh khóa riêng `a`, `b`.
3. Tính giá trị công khai: `A = g^a mod p`, `B = g^b mod p`.
4. Trao đổi `A`, `B` (có thể mô phỏng trong cùng chương trình; đề bài không bắt buộc lập trình mạng).
5. Tính bí mật chung: `s_A = B^a mod p`, `s_B = A^b mod p`.
6. Hiển thị hai kết quả theo yêu cầu minh họa và xác nhận **`s_A == s_B`**.

**Lưu ý chọn `g`:** không mặc định `g = 2` luôn là phần tử sinh. Với `p = 2q + 1`, để chọn phần tử sinh của toàn nhóm nhân modulo `p`, chọn `2 <= g <= p - 2` và kiểm tra `g^2 mod p != 1`, `g^q mod p != 1`.

## 2. Những điều cần đảm bảo

- [ ] Mã nguồn dùng **C/C++**, nếu dùng C++ thì chỉ dùng **C++14 trở xuống** và thư viện phù hợp phiên bản.
- [ ] Các chức năng cốt lõi được tự cài đặt theo yêu cầu, không thay thế bằng lời gọi thư viện mật mã có sẵn.
- [ ] Tính toán đúng với số lớn tối thiểu 512 bit và không tràn dữ liệu trung gian.
- [ ] Kiểm tra cả `p` và `(p - 1) / 2`, không chỉ kiểm tra riêng `p`.
- [ ] Khóa riêng luôn thuộc `[2, p - 2]`; tham số `g` được lựa chọn và giải thích rõ.
- [ ] Alice và Bob luôn tính được cùng bí mật trong các lần chạy hợp lệ.
- [ ] Có kiểm thử lũy thừa mô-đun bằng ví dụ nhỏ, số nguyên tố/hợp số đã biết, điều kiện safe prime, miền khóa riêng và toàn bộ luồng trao đổi khóa.
- [ ] Chương trình biên dịch và chạy được theo hướng dẫn; phần không biên dịch hoặc chạy lỗi sẽ không có điểm theo quy định đề bài.

**Giới hạn bảo mật:** 512 bit là mức tối thiểu của bài tập, không an toàn cho triển khai thực tế. Diffie-Hellman thuần túy không xác thực danh tính và có thể bị tấn công trung gian. Việc in bí mật chung chỉ phục vụ minh họa; không áp dụng cách này trong hệ thống thực tế.

## 3. Mã nguồn và cách chạy hiện có

Repo cung cấp thư viện sinh số giả ngẫu nhiên (PRNG) dùng **SHAKE128**, một XOF (Extendable-Output Function). Chạy chương trình minh họa PRNG bằng:

```sh
make run-prng
```

Lệnh trên chạy minh họa PRNG, **không phải chương trình trao đổi khóa hoàn chỉnh**. Có thể dùng các hàm trong thư viện làm cơ sở xây dựng số lớn như `uint512_t`, `uint1024_t`, `uint2048_t`. Tính an toàn của bộ sinh còn phụ thuộc nguồn entropy và cách khởi tạo seed, không chỉ phụ thuộc SHAKE128.

Khi hoàn thành chương trình, cần bổ sung hướng dẫn biên dịch/chạy Diffie-Hellman, phụ thuộc cần cài đặt và vị trí thay đổi kích thước bit, số vòng kiểm tra nguyên tố cùng các tham số liên quan. Cần rà soát cấu hình biên dịch để bảo đảm không sử dụng tính năng sau C++14.

## 4. Nội dung báo cáo

1. **Xử lý số lớn:** cách biểu diễn, các phép toán; mô tả chi tiết nếu tự xây dựng kiểu dữ liệu.
2. **Kiểm tra nguyên tố:** thuật toán đã tìm hiểu, thuật toán lựa chọn và cách triển khai.
3. **Số ngẫu nhiên (không bắt buộc, tính điểm cộng):** vai trò trong mật mã và cách tạo số ngẫu nhiên an toàn trên máy tính.
4. **Diffie-Hellman:** nguyên lý và một ứng dụng thực tế.
5. **Mô tả chương trình:** các thành phần đã cài đặt, cách biên dịch/chạy, cách đổi tham số và kết quả kiểm thử.
6. **Tự đánh giá:** ưu điểm, hạn chế và hướng cải thiện.

## 5. Quy định và cấu trúc bài nộp

- Nhóm tối đa **4 sinh viên**.
- Thời gian thực hiện: **2 tuần** từ khi bài tập được đăng trên trang môn học.
- Nộp file **ZIP**; mã số sinh viên được sắp theo thứ tự trong danh sách lớp.

```text
<MSSV1-MSSV2-MSSV3-MSSV4>.zip
├── project_01_source/
│   └── *.cpp
└── project_01_report/
    └── *.pdf
```

Đính kèm đầy đủ tệp nguồn, header và cấu hình cần thiết để biên dịch chương trình.

## Tài liệu tham khảo

- [Diffie-Hellman — tài liệu môn mật mã](https://www.math.auckland.ac.nz/~sgal018/crypto-book/ch20.pdf)
- [Safe prime](https://planetmath.org/SafePrime)
- [Kiểm tra Miller–Rabin](https://crypto.stanford.edu/pbc/notes/numbertheory/millerrabin.html)
- [Minh họa Diffie-Hellman](https://www.geeksforgeeks.org/implementation-diffie-hellman-algorithm/)
- [Thư viện chuẩn C++ — lưu ý phiên bản sử dụng](https://en.cppreference.com/w/cpp/standard_library)
