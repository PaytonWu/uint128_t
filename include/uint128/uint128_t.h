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

#include "uint128_t_config.h"

#include <bit>
#include <concepts>
#include <cstdint>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "endianness.h"

class UINT128_T_API uint128_t;

// Give uint128_t type traits
namespace std
{
    // This is probably not a good idea

    template <>
    struct is_integral<uint128_t> : true_type
    {
    };
    template <>
    struct is_unsigned<uint128_t> : true_type
    {
    };
    template <>
    struct is_arithmetic<uint128_t> : true_type
    {
    };
}

class uint128_t
{
private:
#ifdef __BIG_ENDIAN__
    uint64_t upper_{}, lower_{};
#endif
#ifdef __LITTLE_ENDIAN__
    uint64_t lower_{}, upper_{};
#endif

public:
    // Constructors
    constexpr uint128_t() = default;
    constexpr uint128_t(uint128_t const & rhs) = default;
    constexpr uint128_t(uint128_t && rhs) = default;

    // do not use prefixes (0x, 0b, etc.)
    // if the input string is too long, only right most characters are read
    uint128_t(std::string const & s, uint8_t base);
    uint128_t(char const * s, std::size_t len, uint8_t base);

    constexpr uint128_t(bool const b) : uint128_t{ static_cast<uint8_t>(b) }
    {
    }

    template <std::integral T>
    constexpr uint128_t(T const rhs)
#ifdef __BIG_ENDIAN__
        : upper_{ 0 }, lower_{ static_cast<uint64_t>(rhs) }
#endif
#ifdef __LITTLE_ENDIAN__
        : lower_{ static_cast<uint64_t>(rhs) }, upper_{ 0 }
#endif
    {
        if (std::is_signed_v<T>)
        {
            if (rhs < 0)
            {
                upper_ = -1;
            }
        }
    }

    constexpr uint128_t(std::integral auto const & upper_rhs, std::integral auto const & lower_rhs)
#ifdef __BIG_ENDIAN__
        : UPPER(upper_rhs), LOWER(lower_rhs)
#endif
#ifdef __LITTLE_ENDIAN__
        : lower_(lower_rhs), upper_(upper_rhs)
#endif
    {
    }

    // Destructor
    constexpr ~uint128_t() = default;

    //  RHS input args only

    // Assignment Operator
    constexpr uint128_t & operator=(uint128_t const & rhs) = default;
    constexpr uint128_t & operator=(uint128_t && rhs) = default;

    template <std::integral T>
    constexpr uint128_t & operator=(T const rhs)
    {
        upper_ = 0;

        if (std::is_signed_v<T>)
        {
            if (rhs < 0)
            {
                upper_ = -1;
            }
        }

        lower_ = rhs;
        return *this;
    }

    constexpr uint128_t & operator=(bool const rhs)
    {
        upper_ = 0;
        lower_ = rhs;
        return *this;
    }

    // Typecast Operators
    explicit operator bool() const;
    operator uint8_t() const;
    operator uint16_t() const;
    operator uint32_t() const;
    operator uint64_t() const;

    // Bitwise Operators
    uint128_t operator&(uint128_t const & rhs) const;

    void export_bits(std::vector<uint8_t> & ret) const;
    [[nodiscard]] std::vector<uint8_t> export_bits_compact() const;
    [[nodiscard]] std::vector<uint8_t> export_bits_compact(std::endian endian) const;

    uint128_t operator&(std::integral auto const & rhs) const
    {
        return { 0, lower_ & static_cast<uint64_t>(rhs) };
    }

    uint128_t & operator&=(uint128_t const & rhs);

    uint128_t & operator&=(std::integral auto const & rhs)
    {
        upper_ = 0;
        lower_ &= rhs;
        return *this;
    }

    uint128_t operator|(uint128_t const & rhs) const;

    uint128_t operator|(std::integral auto const & rhs) const
    {
        return { upper_, lower_ | static_cast<uint64_t>(rhs) };
    }

    uint128_t & operator|=(uint128_t const & rhs);

    uint128_t & operator|=(std::integral auto const & rhs)
    {
        lower_ |= static_cast<uint64_t>(rhs);
        return *this;
    }

    uint128_t operator^(uint128_t const & rhs) const;

    uint128_t operator^(std::integral auto const & rhs) const
    {
        return { upper_, lower_ ^ static_cast<uint64_t>(rhs) };
    }

    uint128_t & operator^=(uint128_t const & rhs);

    uint128_t & operator^=(std::integral auto const & rhs)
    {
        lower_ ^= static_cast<uint64_t>(rhs);
        return *this;
    }

    uint128_t operator~() const;

    // Bit Shift Operators
    uint128_t operator<<(uint128_t const & rhs) const;

    uint128_t operator<<(std::integral auto const rhs) const
    {
        return *this << uint128_t(rhs);
    }

    uint128_t & operator<<=(uint128_t const & rhs);

    uint128_t & operator<<=(std::integral auto const rhs)
    {
        *this = *this << uint128_t{ rhs };
        return *this;
    }

    uint128_t operator>>(uint128_t const & rhs) const;

    uint128_t operator>>(std::integral auto const & rhs) const
    {
        return *this >> uint128_t(rhs);
    }

    uint128_t & operator>>=(uint128_t const & rhs);

    uint128_t & operator>>=(std::integral auto const & rhs)
    {
        *this = *this >> uint128_t(rhs);
        return *this;
    }

    // Logical Operators
    bool operator!() const;
    bool operator&&(uint128_t const & rhs) const;
    bool operator||(uint128_t const & rhs) const;

    bool operator&&(std::integral auto const & rhs) const
    {
        return static_cast<bool>(*this && rhs);
    }

    bool operator||(std::integral auto const & rhs) const
    {
        return static_cast<bool>(*this || rhs);
    }

    // Comparison Operators
    bool operator==(uint128_t const & rhs) const;

    bool operator==(std::integral auto const rhs) const
    {
        return (!upper_ && (lower_ == static_cast<uint64_t>(rhs)));
    }

    bool operator!=(uint128_t const & rhs) const;

    bool operator!=(std::integral auto const & rhs) const
    {
        return (upper_ || (lower_ != static_cast<uint64_t>(rhs)));
    }

    bool operator>(uint128_t const & rhs) const;

    bool operator>(std::integral auto const & rhs) const
    {
        return (upper_ || (lower_ > static_cast<uint64_t>(rhs)));
    }

    bool operator<(uint128_t const & rhs) const;

    bool operator<(std::integral auto const & rhs) const
    {
        return (!upper_) ? (lower_ < static_cast<uint64_t>(rhs)) : false;
    }

    bool operator>=(uint128_t const & rhs) const;

    bool operator>=(std::integral auto const & rhs) const
    {
        return ((*this > rhs) || (*this == rhs));
    }

    bool operator<=(uint128_t const & rhs) const;

    bool operator<=(std::integral auto const & rhs) const
    {
        return ((*this < rhs) || (*this == rhs));
    }

    // Arithmetic Operators
    uint128_t operator+(uint128_t const & rhs) const;

    uint128_t operator+(std::integral auto const & rhs) const
    {
        return { upper_ + ((lower_ + static_cast<uint64_t>(rhs)) < lower_), lower_ + static_cast<uint64_t>(rhs) };
    }

    uint128_t & operator+=(uint128_t const & rhs);

    uint128_t & operator+=(std::integral auto const & rhs)
    {
        return *this += uint128_t(rhs);
    }

    uint128_t operator-(uint128_t const & rhs) const;

    uint128_t operator-(std::integral auto const & rhs) const
    {
        return { static_cast<uint64_t>(upper_ - ((lower_ - rhs) > lower_)), static_cast<uint64_t>(lower_ - rhs) };
    }

    uint128_t & operator-=(uint128_t const & rhs);

    uint128_t & operator-=(std::integral auto const & rhs)
    {
        return *this = *this - uint128_t(rhs);
    }

    uint128_t operator*(uint128_t const & rhs) const;

    uint128_t operator*(std::integral auto const & rhs) const
    {
        return *this * uint128_t(rhs);
    }

    uint128_t & operator*=(uint128_t const & rhs);

    uint128_t & operator*=(std::integral auto const & rhs)
    {
        return *this = *this * uint128_t(rhs);
    }

private:
    [[nodiscard]] std::pair<uint128_t, uint128_t> divmod(uint128_t const & lhs, uint128_t const & rhs) const;
    // do not use prefixes (0x, 0b, etc.)
    // if the input string is too long, only right most characters are read
    void init(char const * s, std::size_t len, uint8_t base);
    void init_hex(char const * s, std::size_t len);
    void init_dec(char const * s, std::size_t len);
    void init_oct(char const * s, std::size_t len);
    void init_bin(char const * s, std::size_t len);

public:
    uint128_t operator/(uint128_t const & rhs) const;

    uint128_t operator/(std::integral auto const & rhs) const
    {
        return *this / uint128_t(rhs);
    }

    uint128_t & operator/=(uint128_t const & rhs);

    uint128_t & operator/=(std::integral auto const & rhs)
    {
        return *this = *this / uint128_t(rhs);
    }

    uint128_t operator%(uint128_t const & rhs) const;

    uint128_t operator%(std::integral auto const & rhs) const
    {
        return *this % uint128_t(rhs);
    }

    uint128_t & operator%=(uint128_t const & rhs);

    uint128_t & operator%=(std::integral auto const & rhs)
    {
        return *this = *this % uint128_t(rhs);
    }

    // Increment Operator
    uint128_t & operator++();
    uint128_t operator++(int);

    // Decrement Operator
    uint128_t & operator--();
    uint128_t operator--(int);

    // Nothing done since promotion doesn't work here
    uint128_t operator+() const;

    // two's complement
    uint128_t operator-() const;

    // Get private values
    [[nodiscard]] uint64_t upper() const;
    [[nodiscard]] uint64_t lower() const;

    // Get bit size of value
    [[nodiscard]] uint8_t bits() const;

    // Get string representation of value
    [[nodiscard]] std::string str(uint8_t base = 10, unsigned int len = 0) const;
};

// useful values
inline constexpr uint128_t uint128_0{ 0 };
inline constexpr uint128_t uint128_1{ 1 };

// lhs type T as first argument
// If the output is not a bool, casts to type T

// Bitwise Operators
uint128_t operator&(std::integral auto const & lhs, uint128_t const & rhs)
{
    return rhs & lhs;
}

template <std::integral T>
T & operator&=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(rhs & lhs);
}

uint128_t operator|(std::integral auto const & lhs, uint128_t const & rhs)
{
    return rhs | lhs;
}

template <std::integral T>
T & operator|=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(rhs | lhs);
}

uint128_t operator^(std::integral auto const & lhs, uint128_t const & rhs)
{
    return rhs ^ lhs;
}

template <std::integral T>
T & operator^=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(rhs ^ lhs);
}

// Bit shift operators
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(bool const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(uint8_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(uint16_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(uint32_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(uint64_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(int8_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(int16_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(int32_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator<<(int64_t const & lhs, uint128_t const & rhs);

template <std::integral T>
T & operator<<=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(uint128_t(lhs) << rhs);
}

UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(bool const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(uint8_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(uint16_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(uint32_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(uint64_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(int8_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(int16_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(int32_t const & lhs, uint128_t const & rhs);
UINT128_T_EXTERN_C UINT128_T_API uint128_t operator>>(int64_t const & lhs, uint128_t const & rhs);

template <std::integral T>
T & operator>>=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(uint128_t(lhs) >> rhs);
}

// Comparison Operators
bool operator==(std::integral auto const & lhs, uint128_t const & rhs)
{
    return (!rhs.upper() && (static_cast<uint64_t>(lhs) == rhs.lower()));
}

bool operator!=(std::integral auto const & lhs, uint128_t const & rhs)
{
    return (rhs.upper() | (static_cast<uint64_t>(lhs) != rhs.lower()));
}

bool operator>(std::integral auto const & lhs, uint128_t const & rhs)
{
    return (!rhs.upper()) && (static_cast<uint64_t>(lhs) > rhs.lower());
}

bool operator<(std::integral auto const & lhs, uint128_t const & rhs)
{
    if (rhs.upper())
    {
        return true;
    }
    return (static_cast<uint64_t>(lhs) < rhs.lower());
}

bool operator>=(std::integral auto const & lhs, uint128_t const & rhs)
{
    if (rhs.upper())
    {
        return false;
    }
    return (static_cast<uint64_t>(lhs) >= rhs.lower());
}

bool operator<=(std::integral auto const & lhs, uint128_t const & rhs)
{
    if (rhs.upper())
    {
        return true;
    }
    return (static_cast<uint64_t>(lhs) <= rhs.lower());
}

// Arithmetic Operators
uint128_t operator+(std::integral auto const & lhs, uint128_t const & rhs)
{
    return rhs + lhs;
}

template <std::integral T>
T & operator+=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(rhs + lhs);
}

uint128_t operator-(std::integral auto const & lhs, uint128_t const & rhs)
{
    return -(rhs - lhs);
}

template <std::integral T>
T & operator-=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(-(rhs - lhs));
}

uint128_t operator*(std::integral auto const & lhs, uint128_t const & rhs)
{
    return rhs * lhs;
}

template <std::integral T>
T & operator*=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(rhs * lhs);
}

uint128_t operator/(std::integral auto const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) / rhs;
}

template <std::integral T>
T & operator/=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(uint128_t(lhs) / rhs);
}

uint128_t operator%(std::integral auto const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) % rhs;
}

template <std::integral T>
T & operator%=(T & lhs, uint128_t const & rhs)
{
    return lhs = static_cast<T>(uint128_t(lhs) % rhs);
}

// IO Operator
UINT128_T_EXTERN_C UINT128_T_API std::ostream & operator<<(std::ostream & stream, uint128_t const & rhs);

#endif
