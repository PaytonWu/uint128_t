// uint128_t.h
// An unsigned 128 bit integer type for C++
//
// Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// With much help from Auston Sterling
//
// Thanks to Stefan Deigmüller for finding
// a bug in operator*.
//
// Thanks to François Dessenne for convincing me
// to do a general rewrite of this class.

#if !defined(UINT128_H)
#define UINT128_H  // NOLINT(clang-diagnostic-unused-macros)
#pragma once

#if !defined(__cplusplus) || __cplusplus < 202002L
#   error "C++20 or above is required"
#endif

#include "endianness.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <cctype>
#include <compare>
#include <concepts>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

class uint128_t;

// Give uint128_t type traits
namespace std {
// This is probably not a good idea

template <>
struct is_integral<uint128_t> : true_type {
};
template <>
struct is_unsigned<uint128_t> : true_type {
};
template <>
struct is_arithmetic<uint128_t> : true_type {
};

}

class [[nodiscard]] uint128_t {
private:
#if defined(__BIG_ENDIAN__)
    uint64_t upper_{}, lower_{};
#endif
#if defined(__LITTLE_ENDIAN__)
    uint64_t lower_{}, upper_{};
#endif

public:
    uint128_t() = default;
    uint128_t(uint128_t const & rhs) = default;
    uint128_t(uint128_t && rhs) = default;
    auto operator=(uint128_t const & rhs) -> uint128_t & = default;
    auto operator=(uint128_t && rhs) -> uint128_t & = default;
    ~uint128_t() = default;

    // do not use prefixes (0x, 0b, etc.)
    // if the input string is too long, only right most characters are read
    constexpr uint128_t(std::string const & s, uint8_t const base) {
        init(s.c_str(), s.size(), base);
    }

    constexpr uint128_t(char const * s, std::size_t const len, uint8_t const base) {
        init(s, len, base);
    }

    constexpr explicit uint128_t(bool const b) : uint128_t{ static_cast<uint8_t>(b) } {
    }

    template <std::integral T>
    constexpr uint128_t(T const rhs)
#if defined(__BIG_ENDIAN__)
        : upper_{ 0 }, lower_{ static_cast<uint64_t>(rhs) }
#endif
#if defined(__LITTLE_ENDIAN__)
        : lower_{ static_cast<uint64_t>(rhs) }, upper_{ 0 }
#endif
    {
        if constexpr (std::is_signed_v<T>) {
            if (rhs < 0) {
                upper_ = -1;
            }
        }
    }

    constexpr uint128_t(std::unsigned_integral auto const upper_rhs, std::unsigned_integral auto const lower_rhs)
#if defined(__BIG_ENDIAN__)
        : upper_{ upper_rhs }, lower_{ lower_rhs }
#endif
#if defined(__LITTLE_ENDIAN__)
        : lower_{ lower_rhs }, upper_{ upper_rhs }
#endif
    {
    }

    constexpr uint128_t(std::integral auto const upper_rhs, std::integral auto const lower_rhs)
#if defined(__BIG_ENDIAN__)
        : upper_{ static_cast<uint64_t>(upper_rhs) }, lower_{ static_cast<uint64_t>(lower_rhs) }
#endif
#if defined(__LITTLE_ENDIAN__)
        : lower_{ static_cast<uint64_t>(lower_rhs) }, upper_{ static_cast<uint64_t>(upper_rhs) }
#endif
    {
    }

    template <std::integral T>
    constexpr auto operator=(T const rhs) -> uint128_t & {
        upper_ = 0;

        if constexpr (std::is_signed_v<T>) {
            if (rhs < 0) {
                upper_ = -1;
            }
        }

        lower_ = rhs;
        return *this;
    }

    constexpr auto operator=(bool const rhs) -> uint128_t & {
        upper_ = 0;
        lower_ = rhs;
        return *this;
    }

    // Typecast Operators
    constexpr explicit operator bool() const noexcept {
        return static_cast<bool>(upper_ | lower_);
    }

    constexpr operator uint8_t() const noexcept {
        return static_cast<uint8_t>(lower_);
    }

    constexpr operator uint16_t() const noexcept {
        return static_cast<uint16_t>(lower_);
    }

    constexpr operator uint32_t() const noexcept {
        return static_cast<uint32_t>(lower_);
    }

    constexpr operator uint64_t() const noexcept {
        return lower_;
    }

    // Bitwise Operators
    constexpr auto operator&(uint128_t const rhs) const noexcept -> uint128_t {
        return { upper_ & rhs.upper_, lower_ & rhs.lower_ };
    }

    constexpr auto operator&(std::integral auto const rhs) const noexcept -> uint128_t {
        return { 0, lower_ & static_cast<uint64_t>(rhs) };
    }

    constexpr auto operator&=(uint128_t const rhs) -> uint128_t & {
        upper_ &= rhs.upper_;
        lower_ &= rhs.lower_;
        return *this;
    }

    constexpr auto operator&=(std::integral auto const rhs) noexcept -> uint128_t & {
        upper_ = 0;
        lower_ &= rhs;
        return *this;
    }

    constexpr auto operator|(uint128_t const rhs) const noexcept -> uint128_t {
               return { upper_ | rhs.upper_, lower_ | rhs.lower_ };
    }

    constexpr auto operator|(std::integral auto const rhs) const -> uint128_t {
        return { upper_, lower_ | static_cast<uint64_t>(rhs) };
    }

    constexpr auto operator|=(uint128_t const rhs) noexcept -> uint128_t & {
        upper_ |= rhs.upper_;
        lower_ |= rhs.lower_;
        return *this;
    }

    constexpr auto operator|=(std::integral auto const rhs) noexcept -> uint128_t & {
        lower_ |= static_cast<uint64_t>(rhs);
        return *this;
    }

    constexpr auto operator^(uint128_t const rhs) const noexcept -> uint128_t {
        return { upper_ ^ rhs.upper_, lower_ ^ rhs.lower_ };
    }

    constexpr auto operator^(std::integral auto const rhs) const noexcept -> uint128_t {
        return { upper_, lower_ ^ static_cast<uint64_t>(rhs) };
    }

    constexpr auto operator^=(uint128_t const rhs) noexcept -> uint128_t & {
        upper_ ^= rhs.upper_;
        lower_ ^= rhs.lower_;
        return *this;
    }

    constexpr auto operator^=(std::integral auto const rhs) noexcept -> uint128_t & {
        lower_ ^= static_cast<uint64_t>(rhs);
        return *this;
    }

    constexpr auto operator~() const noexcept -> uint128_t {
        return { ~upper_, ~lower_ };
    }

    constexpr void export_bits(std::vector<uint8_t> & ret) const noexcept {
        convert_to_vector_big_endian(upper_, ret);
        convert_to_vector_big_endian(lower_, ret);
        assert(ret.size() == 16);
    }

    [[nodiscard]] constexpr auto export_bits_compact() const -> std::vector<uint8_t> {
        std::vector<uint8_t> ret;
        ret.reserve(16);
        export_bits(ret);
        assert(ret.size() == 16);

        int i = 0;
        while (i < 16 && ret[i] == 0) {
            ++i;
        }
        ret.erase(std::begin(ret), std::next(std::begin(ret), i));

        return ret;
    }

    [[nodiscard]] constexpr auto export_bits_compact(std::endian const endian) const -> std::vector<uint8_t> {
        auto res = export_bits_compact();
        if (endian == std::endian::little) {
            std::ranges::reverse(res);
        }

        return res;
    }

    // Bit Shift Operators
    constexpr auto operator<<(uint128_t const rhs) const noexcept -> uint128_t {
        if (uint64_t const shift = rhs.lower_; static_cast<bool>(rhs.upper_) || (shift >= 128)) {
            return { 0};
        } else {
            if (shift == 64) {
                return { lower_, 0u };
            }

            if (shift == 0) {
                return *this;
            }

            if (shift < 64) {
                return { (upper_ << shift) + (lower_ >> (64 - shift)), lower_ << shift };
            }

            if ((128 > shift) && (shift > 64)) {
                return { lower_ << (shift - 64), 0 };
            }

            return {0};
        }
    }

    constexpr auto operator<<(std::integral auto const rhs) const noexcept -> uint128_t {
        return *this << uint128_t{ rhs };
    }

    constexpr auto operator<<=(uint128_t const rhs) noexcept -> uint128_t & {
        *this = *this << rhs;
        return *this;
    }

    constexpr auto operator<<=(std::integral auto const rhs) noexcept -> uint128_t & {
        *this = *this << uint128_t{ rhs };
        return *this;
    }

    constexpr auto operator>>(uint128_t const rhs) const noexcept -> uint128_t {
        if (uint64_t const shift = rhs.lower_; static_cast<bool>(rhs.upper_) || (shift >= 128)) {
            return { 0 };
        } else {
            if (shift == 64) {
                return { 0u, upper_ };
            }
            if (shift == 0) {
                return *this;
            }
            if (shift < 64) {
                return { upper_ >> shift, (upper_ << (64 - shift)) + (lower_ >> shift) };
            }
            if ((128 > shift) && (shift > 64)) {
                return { 0, (upper_ >> (shift - 64)) };
            }
            return { 0 };
        }
    }

    constexpr auto operator>>(std::integral auto const rhs) const noexcept -> uint128_t {
        return *this >> uint128_t{ rhs };
    }

    constexpr auto operator>>=(uint128_t const rhs) noexcept -> uint128_t & {
        *this = *this >> rhs;
        return *this;
    }

    constexpr auto operator>>=(std::integral auto const rhs) noexcept -> uint128_t & {
        *this = *this >> uint128_t{ rhs };
        return *this;
    }

    // Logical Operators
    constexpr bool operator!() const noexcept {
        return !static_cast<bool>(upper_ | lower_);
    }

    constexpr bool operator&&(uint128_t const rhs) const noexcept {
        return (static_cast<bool>(*this) && rhs);
    }

    constexpr bool operator||(uint128_t const rhs) const noexcept {
        return (static_cast<bool>(*this) || rhs);
    }

    constexpr bool operator&&(std::integral auto const rhs) const noexcept {
        return static_cast<bool>(*this && rhs);
    }

    constexpr bool operator||(std::integral auto const rhs) const noexcept {
        return static_cast<bool>(*this || rhs);
    }

    // Comparison Operators
    bool operator==(uint128_t const & rhs) const = default;
#if defined(__LITTLE_ENDIAN__)
    constexpr auto operator<=>(uint128_t const & rhs) const noexcept -> std::strong_ordering {
        if (upper_ == rhs.upper_) {
            return lower_ <=> rhs.lower_;
        }

        return upper_ <=> rhs.upper_;
    }
#elif defined(__BIG_ENDIAN__)
    constexpr auto operator<=>(uint128_t const & rhs) const ->std::strong_ordering = default;
#endif

    constexpr bool operator==(std::integral auto const rhs) const noexcept {
        return !upper_ && lower_ == static_cast<uint64_t>(rhs);
    }

    constexpr auto operator<=>(std::integral auto const rhs) const noexcept -> std::strong_ordering {
        if (upper_ == 0) {
            return lower_ <=> static_cast<uint64_t>(rhs);
        }

        return std::strong_ordering::greater;
    }

    // Arithmetic Operators
    constexpr auto operator+(uint128_t const rhs) const noexcept -> uint128_t {
        return { upper_ + rhs.upper_ + ((lower_ + rhs.lower_) < lower_), lower_ + rhs.lower_ };
    }

    constexpr auto operator+(std::integral auto const rhs) const noexcept -> uint128_t {
        return { upper_ + ((lower_ + static_cast<uint64_t>(rhs)) < lower_), lower_ + static_cast<uint64_t>(rhs) };
    }

    constexpr auto operator+=(uint128_t const rhs) noexcept -> uint128_t & {
        upper_ += rhs.upper_ + ((lower_ + rhs.lower_) < lower_);
        lower_ += rhs.lower_;
        return *this;
    }

    constexpr auto operator+=(std::integral auto const rhs) noexcept -> uint128_t & {
        return *this += uint128_t{ rhs };
    }

    constexpr auto operator-(uint128_t const rhs) const noexcept -> uint128_t {
        return { upper_ - rhs.upper_ - ((lower_ - rhs.lower_) > lower_), lower_ - rhs.lower_ };
    }

    constexpr auto operator-(std::integral auto const rhs) const noexcept -> uint128_t {
        return { static_cast<uint64_t>(upper_ - ((lower_ - rhs) > lower_)), static_cast<uint64_t>(lower_ - rhs) };
    }

    constexpr auto operator-=(uint128_t const rhs) noexcept -> uint128_t & {
        *this = *this - rhs;
        return *this;
    }

    constexpr auto operator-=(std::integral auto const rhs) noexcept -> uint128_t & {
        return *this = *this - uint128_t(rhs);
    }

    constexpr auto operator*(uint128_t const rhs) const noexcept -> uint128_t {
        // split values into 4 32-bit parts
        uint64_t const top[4] = { upper_ >> 32, upper_ & 0xffffffff, lower_ >> 32, lower_ & 0xffffffff };
        uint64_t const bottom[4] = { rhs.upper_ >> 32, rhs.upper_ & 0xffffffff, rhs.lower_ >> 32, rhs.lower_ & 0xffffffff };
        uint64_t products[4][4];

        // multiply each component of the values
        for (int y = 3; y > -1; y--) {
            for (int x = 3; x > -1; x--) {
                products[3 - x][y] = top[x] * bottom[y];
            }
        }

        // first row
        uint64_t fourth32 = (products[0][3] & 0xffffffff);
        uint64_t third32 = (products[0][2] & 0xffffffff) + (products[0][3] >> 32);
        uint64_t second32 = (products[0][1] & 0xffffffff) + (products[0][2] >> 32);
        uint64_t first32 = (products[0][0] & 0xffffffff) + (products[0][1] >> 32);

        // second row
        third32 += (products[1][3] & 0xffffffff);
        second32 += (products[1][2] & 0xffffffff) + (products[1][3] >> 32);
        first32 += (products[1][1] & 0xffffffff) + (products[1][2] >> 32);

        // third row
        second32 += (products[2][3] & 0xffffffff);
        first32 += (products[2][2] & 0xffffffff) + (products[2][3] >> 32);

        // fourth row
        first32 += (products[3][3] & 0xffffffff);

        // move carry to next digit
        third32 += fourth32 >> 32;
        second32 += third32 >> 32;
        first32 += second32 >> 32;

        // remove carry from current digit
        fourth32 &= 0xffffffff;
        third32 &= 0xffffffff;
        second32 &= 0xffffffff;
        first32 &= 0xffffffff;

        // combine components
        return { (first32 << 32) | second32, (third32 << 32) | fourth32 };
    }

    constexpr auto operator*(std::integral auto const & rhs) const noexcept -> uint128_t {
        return *this * uint128_t{ rhs };
    }

    constexpr auto operator*=(uint128_t const rhs) noexcept -> uint128_t & {
        *this = *this * rhs;
        return *this;
    }

    constexpr auto operator*=(std::integral auto const rhs) noexcept -> uint128_t & {
        return *this = *this * uint128_t(rhs);
    }

private:
    [[nodiscard]] constexpr static auto divmod(uint128_t const lhs, uint128_t const rhs) -> std::pair<uint128_t, uint128_t> {
        // Save some calculations /////////////////////
        if (rhs == uint128_t{ 0 }) {
            throw std::domain_error("Error: division or modulus by 0");
        }

        if (rhs == uint128_t{ 1 }) {
            return { lhs, uint128_t{ 0 } };
        }

        if (lhs == rhs) {
            return { uint128_t{ 1}, uint128_t{ 0 } };
        }

        if ((lhs == uint128_t{ 0 }) || (lhs < rhs)) {
            return { uint128_t{ 0 }, lhs };
        }

        std::pair qr{ uint128_t{ 0 }, uint128_t{ 0 } };
        for (uint8_t x = lhs.bits(); x > 0; x--) {
            qr.first <<= uint128_t{ 1 };
            qr.second <<= uint128_t{ 1 };

            if ((lhs >> (x - 1U)) & 1) {
                ++qr.second;
            }

            if (qr.second >= rhs) {
                qr.second -= rhs;
                ++qr.first;
            }
        }
        return qr;
    }
    // do not use prefixes (0x, 0b, etc.)
    // if the input string is too long, only right most characters are read
    constexpr void init(char const * s, std::size_t len, uint8_t const base) {
        if (s == nullptr || !len || s[0] == '\x00') {
            lower_ = upper_ = 0;
            return;
        }

        while (len && *s && std::isspace(*s)) {
            ++s;
            len--;
        }

        // no prefixes
        switch (base) {
        case 16:
            init_hex(s, len);
            break;
        case 10:
            init_dec(s, len);
            break;
        case 8:
            init_oct(s, len);
            break;
        case 2:
            init_bin(s, len);
            break;
        default:
            // should probably throw error here
            break;
        }
    }

    constexpr void init_hex(char const * s, std::size_t const len) {
        // 2**128 = 0x100000000000000000000000000000000.
        constexpr std::size_t MAX_LEN = 32;

        lower_ = upper_ = 0;
        if (!s || !len) {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        std::size_t constexpr double_lower = sizeof(lower_) * 2;
        std::size_t const lower_len = (max_len >= double_lower) ? double_lower : max_len;
        std::size_t const upper_len = (max_len >= double_lower) ? (max_len - double_lower) : 0;

        std::stringstream lower_s, upper_s;
        upper_s << std::hex << std::string(s + starting_index, upper_len);
        lower_s << std::hex << std::string(s + starting_index + upper_len, lower_len);

        // should check for errors
        upper_s >> upper_;
        lower_s >> lower_;
    }

    constexpr void init_dec(char const * s, std::size_t const len) {
        // 2**128 = 340282366920938463463374607431768211456.
        constexpr std::size_t MAX_LEN = 39;

        lower_ = upper_ = 0;
        if (!s || !len) {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        s += starting_index;

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '9') && (i < max_len); ++s, ++i) {
            *this *= 10;
            *this += *s - '0';
        }
    }

    constexpr void init_oct(char const * s, std::size_t const len) {
        // 2**128 = 0o4000000000000000000000000000000000000000000.
        constexpr std::size_t MAX_LEN = 43;

        lower_ = upper_ = 0;
        if (!s || !len) {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        s += starting_index;

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '7') && (i < max_len); ++s, ++i) {
            *this *= 8;
            *this += *s - '0';
        }
    }

    constexpr void init_bin(char const * s, std::size_t const len) {
        // 2**128 = 0x100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.
        constexpr std::size_t MAX_LEN = 128;

        lower_ = upper_ = 0;
        if (!s || !len) {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        std::size_t constexpr eight_lower = sizeof(lower_) * 8;
        std::size_t const lower_len = (max_len >= eight_lower) ? eight_lower : max_len;
        std::size_t const upper_len = (max_len >= eight_lower) ? (max_len - eight_lower) : 0;
        s += starting_index;

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '1') && (i < upper_len); ++s, ++i) {
            upper_ <<= 1;
            upper_ |= *s - '0';
        }

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '1') && (i < lower_len); ++s, ++i) {
            lower_ <<= 1;
            lower_ |= *s - '0';
        }
    }

    constexpr static void convert_to_vector_big_endian(uint64_t const val, std::vector<uint8_t> & ret) {
        ret.push_back(static_cast<uint8_t>(val >> 56));
        ret.push_back(static_cast<uint8_t>(val >> 48));
        ret.push_back(static_cast<uint8_t>(val >> 40));
        ret.push_back(static_cast<uint8_t>(val >> 32));
        ret.push_back(static_cast<uint8_t>(val >> 24));
        ret.push_back(static_cast<uint8_t>(val >> 16));
        ret.push_back(static_cast<uint8_t>(val >> 8));
        ret.push_back(static_cast<uint8_t>(val));
    }

public:
    constexpr auto operator/(uint128_t const rhs) const -> uint128_t {
        return divmod(*this, rhs).first;
    }

    constexpr auto operator/(std::integral auto const rhs) const -> uint128_t {
        return *this / uint128_t{ rhs };
    }

    constexpr auto operator/=(uint128_t const rhs) -> uint128_t & {
        return *this = *this / rhs;
    }

    constexpr auto operator/=(std::integral auto const rhs) -> uint128_t & {
        return *this = *this / uint128_t(rhs);
    }

    constexpr auto operator%(uint128_t const rhs) const -> uint128_t {
        return divmod(*this, rhs).second;
    }

    constexpr auto operator%(std::integral auto const rhs) const -> uint128_t {
        return *this % uint128_t(rhs);
    }

    constexpr auto operator%=(uint128_t const rhs) -> uint128_t & {
        return *this = *this % rhs;
    }

    constexpr auto operator%=(std::integral auto const rhs) -> uint128_t & {
        return *this = *this % uint128_t{ rhs };
    }

    // Increment Operator
    constexpr auto operator++() noexcept -> uint128_t & {
        return *this += uint128_t{ 1 };
    }

    constexpr auto operator++(int) noexcept -> uint128_t {
        uint128_t const temp{ *this };
        ++*this;
        return temp;
    }

    // Decrement Operator
    constexpr auto operator--() noexcept -> uint128_t & {
        return *this -= uint128_t{ 1 };
    }

    constexpr auto operator--(int) noexcept -> uint128_t {
        uint128_t const temp{ *this };
        --*this;
        return temp;
    }

    // Nothing done since promotion doesn't work here
    constexpr auto operator+() const noexcept -> uint128_t {
        return *this;
    }

    // two's complement
    constexpr auto operator-() const noexcept -> uint128_t {
        return ~*this + uint128_t{ 1 };
    }

    // Get private values
    [[nodiscard]] constexpr auto upper() const noexcept -> uint64_t {
        return upper_;
    }

    [[nodiscard]] constexpr auto lower() const noexcept -> uint64_t {
        return lower_;
    }

    // Get bit size of value
    [[nodiscard]] constexpr auto bits() const -> uint8_t {
        if (upper_) {
            return 64 + static_cast<uint8_t>(std::bit_width(upper_));
        }

        return static_cast<uint8_t>(std::bit_width(lower_));
    }

    // Get string representation of value
    [[nodiscard]] constexpr auto str(uint8_t const base = 10, unsigned int const len = 0) const -> std::string {
        if ((base < 2) || (base > 16)) {
            throw std::invalid_argument("Base must be in the range [2, 16]");
        }

        std::string out;
        if (!(*this)) {
            out = "0";
        } else {
            std::pair qr{ *this, uint128_t{0} };
            do {
                qr = divmod(qr.first, base);
                out = "0123456789abcdef"[static_cast<uint8_t>(qr.second)] + out;
            } while (qr.first);
        }
        if (out.size() < len) {
            out = std::string(len - out.size(), '0') + out;
        }
        return out;
    }
};

// useful values
inline constexpr uint128_t uint128_0{ 0 };
inline constexpr uint128_t uint128_1{ 1 };

// lhs type T as first argument
// If the output is not a bool, casts to type T

// Bitwise Operators
constexpr auto operator&(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t {
    return rhs & lhs;
}

template <std::integral T>
constexpr auto operator&=(T & lhs, uint128_t const rhs) noexcept -> T & {
    return lhs = static_cast<T>(rhs & lhs);
}

constexpr auto operator|(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t {
    return rhs | lhs;
}

template <std::integral T>
constexpr auto operator|=(T & lhs, uint128_t const rhs) noexcept -> T & {
    return lhs = static_cast<T>(rhs | lhs);
}

constexpr auto operator^(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t {
    return rhs ^ lhs;
}

template <std::integral T>
constexpr auto operator^=(T & lhs, uint128_t const rhs) noexcept -> T & {
    return lhs = static_cast<T>(rhs ^ lhs);
}

// Bit shift operators
constexpr auto operator<<(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t {
    return uint128_t{ lhs } << rhs;
}

template <std::integral T>
constexpr auto operator<<=(T & lhs, uint128_t const rhs) noexcept -> T & {
    return lhs = static_cast<T>(uint128_t{ lhs } << rhs);
}

constexpr auto operator>>(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t {
    return uint128_t{ lhs } >> rhs;
}

template <std::integral T>
constexpr auto operator>>=(T & lhs, uint128_t const rhs) noexcept -> T & {
    return lhs = static_cast<T>(uint128_t{ lhs } >> rhs);
}

// Arithmetic Operators
constexpr auto operator+(std::integral auto const lhs, uint128_t const rhs) -> uint128_t {
    return rhs + lhs;
}

template <std::integral T>
constexpr auto operator+=(T & lhs, uint128_t const rhs) -> T & {
    return lhs = static_cast<T>(rhs + lhs);
}

constexpr auto operator-(std::integral auto const lhs, uint128_t const rhs) -> uint128_t {
    return -(rhs - lhs);
}

template <std::integral T>
constexpr auto operator-=(T & lhs, uint128_t const rhs) -> T & {
    return lhs = static_cast<T>(-(rhs - lhs));
}

constexpr auto operator*(std::integral auto const lhs, uint128_t const rhs) -> uint128_t {
    return rhs * lhs;
}

template <std::integral T>
constexpr auto operator*=(T & lhs, uint128_t const rhs) -> T & {
    return lhs = static_cast<T>(rhs * lhs);
}

constexpr auto operator/(std::integral auto const lhs, uint128_t const rhs) -> uint128_t {
    return uint128_t(lhs) / rhs;
}

template <std::integral T>
constexpr auto operator/=(T & lhs, uint128_t const rhs) -> T & {
    return lhs = static_cast<T>(uint128_t(lhs) / rhs);
}

constexpr auto operator%(std::integral auto const lhs, uint128_t const rhs) -> uint128_t {
    return uint128_t{ lhs } % rhs;
}

template <std::integral T>
constexpr auto operator%=(T & lhs, uint128_t const rhs) -> T & {
    return lhs = static_cast<T>(uint128_t{ lhs } % rhs);
}

// IO Operator
inline std::ostream & operator<<(std::ostream & stream, uint128_t const rhs) {
    if (stream.flags() & std::ios_base::oct) {
        stream << rhs.str(8);
    } else if (stream.flags() & std::ios_base::dec) {
        stream << rhs.str(10);
    } else if (stream.flags() & std::ios_base::hex) {
        stream << rhs.str(16);
    }
    return stream;
}

#endif
