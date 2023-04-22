#include "../include/uint128/uint128_t.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>


uint128_t::operator bool() const
{
    return static_cast<bool>(upper_ | lower_);
}

uint128_t::operator uint8_t() const
{
    return static_cast<uint8_t>(lower_);
}

uint128_t::operator uint16_t() const
{
    return static_cast<uint16_t>(lower_);
}

uint128_t::operator uint32_t() const
{
    return static_cast<uint32_t>(lower_);
}

uint128_t::operator uint64_t() const
{
    return lower_;
}

uint128_t uint128_t::operator&(uint128_t const & rhs) const
{
    return { upper_ & rhs.upper_, lower_ & rhs.lower_ };
}

uint128_t & uint128_t::operator&=(uint128_t const & rhs)
{
    upper_ &= rhs.upper_;
    lower_ &= rhs.lower_;
    return *this;
}

uint128_t uint128_t::operator|(uint128_t const & rhs) const
{
    return { upper_ | rhs.upper_, lower_ | rhs.lower_ };
}

uint128_t & uint128_t::operator|=(uint128_t const & rhs)
{
    upper_ |= rhs.upper_;
    lower_ |= rhs.lower_;
    return *this;
}

uint128_t uint128_t::operator^(uint128_t const & rhs) const
{
    return { upper_ ^ rhs.upper_, lower_ ^ rhs.lower_ };
}

uint128_t & uint128_t::operator^=(uint128_t const & rhs)
{
    upper_ ^= rhs.upper_;
    lower_ ^= rhs.lower_;
    return *this;
}

uint128_t uint128_t::operator~() const
{
    return { ~upper_, ~lower_ };
}

uint128_t uint128_t::operator<<(uint128_t const & rhs) const
{
    if (uint64_t const shift = rhs.lower_; static_cast<bool>(rhs.upper_) || (shift >= 128))
    {
        return uint128_0;
    }
    else
    {
        if (shift == 64)
        {
            return { lower_, 0 };
        }

        if (shift == 0)
        {
            return *this;
        }

        if (shift < 64)
        {
            return { (upper_ << shift) + (lower_ >> (64 - shift)), lower_ << shift };
        }

        if ((128 > shift) && (shift > 64))
        {
            return { lower_ << (shift - 64), 0 };
        }

        return uint128_0;
    }
}

uint128_t & uint128_t::operator<<=(uint128_t const & rhs)
{
    *this = *this << rhs;
    return *this;
}

uint128_t uint128_t::operator>>(uint128_t const & rhs) const
{
    if (uint64_t const shift = rhs.lower_; static_cast<bool>(rhs.upper_) || (shift >= 128))
    {
        return uint128_0;
    }
    else
    {
        if (shift == 64)
        {
            return { 0, upper_ };
        }
        if (shift == 0)
        {
            return *this;
        }
        if (shift < 64)
        {
            return { upper_ >> shift, (upper_ << (64 - shift)) + (lower_ >> shift) };
        }
        if ((128 > shift) && (shift > 64))
        {
            return { 0, (upper_ >> (shift - 64)) };
        }
        return uint128_0;
    }
}

uint128_t & uint128_t::operator>>=(uint128_t const & rhs)
{
    *this = *this >> rhs;
    return *this;
}

bool uint128_t::operator!() const
{
    return !static_cast<bool>(upper_ | lower_);
}

bool uint128_t::operator&&(uint128_t const & rhs) const
{
    return (static_cast<bool>(*this) && rhs);
}

bool uint128_t::operator||(uint128_t const & rhs) const
{
    return (static_cast<bool>(*this) || rhs);
}

uint128_t uint128_t::operator+(uint128_t const & rhs) const
{
    return { upper_ + rhs.upper_ + ((lower_ + rhs.lower_) < lower_), lower_ + rhs.lower_ };
}

uint128_t & uint128_t::operator+=(uint128_t const & rhs)
{
    upper_ += rhs.upper_ + ((lower_ + rhs.lower_) < lower_);
    lower_ += rhs.lower_;
    return *this;
}

uint128_t uint128_t::operator-(uint128_t const & rhs) const
{
    return { upper_ - rhs.upper_ - ((lower_ - rhs.lower_) > lower_), lower_ - rhs.lower_ };
}

uint128_t & uint128_t::operator-=(uint128_t const & rhs)
{
    *this = *this - rhs;
    return *this;
}

uint128_t uint128_t::operator*(uint128_t const & rhs) const
{
    // split values into 4 32-bit parts
    uint64_t const top[4] = { upper_ >> 32, upper_ & 0xffffffff, lower_ >> 32, lower_ & 0xffffffff };
    uint64_t const bottom[4] = { rhs.upper_ >> 32, rhs.upper_ & 0xffffffff, rhs.lower_ >> 32, rhs.lower_ & 0xffffffff };
    uint64_t products[4][4];

    // multiply each component of the values
    for (int y = 3; y > -1; y--)
    {
        for (int x = 3; x > -1; x--)
        {
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

uint128_t & uint128_t::operator*=(uint128_t const & rhs)
{
    *this = *this * rhs;
    return *this;
}

static void convert_to_vector_big_endian(uint64_t const & val, std::vector<uint8_t> & ret)
{
    ret.push_back(static_cast<uint8_t>(val >> 56));
    ret.push_back(static_cast<uint8_t>(val >> 48));
    ret.push_back(static_cast<uint8_t>(val >> 40));
    ret.push_back(static_cast<uint8_t>(val >> 32));
    ret.push_back(static_cast<uint8_t>(val >> 24));
    ret.push_back(static_cast<uint8_t>(val >> 16));
    ret.push_back(static_cast<uint8_t>(val >> 8));
    ret.push_back(static_cast<uint8_t>(val));
}

void uint128_t::export_bits(std::vector<uint8_t> & ret) const
{
    convert_to_vector_big_endian(upper_, ret);
    convert_to_vector_big_endian(lower_, ret);
    assert(ret.size() == 16);
}

std::vector<uint8_t> uint128_t::export_bits_compact() const
{
    std::vector<uint8_t> ret;
    ret.reserve(16);
    export_bits(ret);
    assert(ret.size() == 16);

    int i = 0;
    while (i < 16 && ret[i] == 0)
    {
        ++i;
    }
    ret.erase(std::begin(ret), std::next(std::begin(ret), i));
    return ret;
}

std::vector<uint8_t> uint128_t::export_bits_compact(std::endian const endian) const
{
    auto res = export_bits_compact();
    if (endian == std::endian::little)
    {
        std::ranges::reverse(res);
    }

    return res;
}

std::pair<uint128_t, uint128_t> uint128_t::divmod(uint128_t const & lhs, uint128_t const & rhs) const
{
    // Save some calculations /////////////////////
    if (rhs == uint128_0)
    {
        throw std::domain_error("Error: division or modulus by 0");
    }

    if (rhs == uint128_1)
    {
        return { lhs, uint128_0 };
    }

    if (lhs == rhs)
    {
        return { uint128_1, uint128_0 };
    }

    if ((lhs == uint128_0) || (lhs < rhs))
    {
        return { uint128_0, lhs };
    }

    std::pair qr{ uint128_0, uint128_0 };
    for (uint8_t x = lhs.bits(); x > 0; x--)
    {
        qr.first <<= uint128_1;
        qr.second <<= uint128_1;

        if ((lhs >> (x - 1U)) & 1)
        {
            ++qr.second;
        }

        if (qr.second >= rhs)
        {
            qr.second -= rhs;
            ++qr.first;
        }
    }
    return qr;
}

uint128_t uint128_t::operator/(uint128_t const & rhs) const
{
    return divmod(*this, rhs).first;
}

uint128_t & uint128_t::operator/=(uint128_t const & rhs)
{
    *this = *this / rhs;
    return *this;
}

uint128_t uint128_t::operator%(uint128_t const & rhs) const
{
    return divmod(*this, rhs).second;
}

uint128_t & uint128_t::operator%=(uint128_t const & rhs)
{
    *this = *this % rhs;
    return *this;
}

uint128_t & uint128_t::operator++()
{
    return *this += uint128_1;
}

uint128_t uint128_t::operator++(int)
{
    uint128_t temp(*this);
    ++ * this;
    return temp;
}

uint128_t & uint128_t::operator--()
{
    return *this -= uint128_1;
}

uint128_t uint128_t::operator--(int)
{
    uint128_t temp(*this);
    -- * this;
    return temp;
}

uint128_t uint128_t::operator+() const
{
    return *this;
}

uint128_t uint128_t::operator-() const
{
    return ~*this + uint128_1;
}

uint64_t uint128_t::upper() const
{
    return upper_;
}

uint64_t uint128_t::lower() const
{
    return lower_;
}

uint8_t uint128_t::bits() const
{
    if (upper_)
    {
        return 64 + static_cast<uint8_t>(std::bit_width(upper_));
    }

    return static_cast<uint8_t>(std::bit_width(lower_));
}

std::string uint128_t::str(uint8_t const base, unsigned int const len) const
{
    if ((base < 2) || (base > 16))
    {
        throw std::invalid_argument("Base must be in the range [2, 16]");
    }

    std::string out;
    if (!(*this))
    {
        out = "0";
    }
    else
    {
        std::pair qr{ *this, uint128_0 };
        do
        {
            qr = divmod(qr.first, base);
            out = "0123456789abcdef"[static_cast<uint8_t>(qr.second)] + out;
        } while (qr.first);
    }
    if (out.size() < len)
    {
        out = std::string(len - out.size(), '0') + out;
    }
    return out;
}

uint128_t operator<<(bool const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(uint8_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(uint16_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(uint32_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(uint64_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(int8_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(int16_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(int32_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator<<(int64_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) << rhs;
}

uint128_t operator>>(bool const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(uint8_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(uint16_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(uint32_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(uint64_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(int8_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(int16_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(int32_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

uint128_t operator>>(int64_t const & lhs, uint128_t const & rhs)
{
    return uint128_t(lhs) >> rhs;
}

std::ostream & operator<<(std::ostream & stream, uint128_t const & rhs)
{
    if (stream.flags() & std::ostream::oct)
    {
        stream << rhs.str(8);
    }
    else if (stream.flags() & std::ostream::dec)
    {
        stream << rhs.str(10);
    }
    else if (stream.flags() & std::ostream::hex)
    {
        stream << rhs.str(16);
    }
    return stream;
}
