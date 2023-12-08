// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(UINT128_T_INCLUDE_UINT128_DETAILS_UINT128_STORAGE)
#define UINT128_T_INCLUDE_UINT128_DETAILS_UINT128_STORAGE

#pragma once

#include <bit>
#include <compare>
#include <concepts>
#include <cstdint>

namespace uint128::details {

struct uint128_little_endian_storage {
protected:
    std::uint64_t lower_{ 0 };
    std::uint64_t upper_{ 0 };

public:
    constexpr uint128_little_endian_storage() = default;

    constexpr uint128_little_endian_storage(std::unsigned_integral auto const high, std::unsigned_integral auto const low) noexcept
        : lower_{ low }, upper_{ high } {
    }

    template <std::integral INT>
    constexpr uint128_little_endian_storage(INT const value) noexcept
        : lower_{ static_cast<std::uint64_t>(value) }, upper_{ std::is_signed_v<INT> && value < 0 ? static_cast<std::uint64_t>(-1) : static_cast<std::uint64_t>(0) } {
        static_assert(sizeof(INT) <= sizeof(std::uint64_t));
    }

    constexpr auto operator==(uint128_little_endian_storage const & rhs) const noexcept -> bool = default;
    constexpr auto operator<=>(uint128_little_endian_storage const & rhs) const noexcept -> std::strong_ordering {
        if (upper_ == rhs.upper_)
        {
            return lower_ <=> rhs.lower_;
        }

        return upper_ <=> rhs.upper_;
    }
};

struct uint128_big_endian_storage {
protected:
    std::uint64_t upper_{ 0 };
    std::uint64_t lower_{ 0 };

public:
    constexpr uint128_big_endian_storage() = default;

    constexpr uint128_big_endian_storage(std::unsigned_integral auto const high, std::unsigned_integral auto const low) noexcept
        : upper_{ high }, lower_{ low } {
    }

    template <std::integral INT>
    constexpr uint128_big_endian_storage(INT const value) noexcept
        : upper_{ std::is_signed_v<INT> && value < 0 ? static_cast<std::uint64_t>(-1) : static_cast<std::uint64_t>(0) }, lower_{ static_cast<std::uint64_t>(value) } {
        static_assert(sizeof(INT) <= sizeof(std::uint64_t));
    }

    constexpr auto operator==(uint128_big_endian_storage const & rhs) const -> bool = default;
    constexpr auto operator<=>(uint128_big_endian_storage const & rhs) const noexcept -> std::strong_ordering = default;
};

struct uint128_storage : std::conditional_t<std::endian::native == std::endian::little, uint128_little_endian_storage, uint128_big_endian_storage> {
    static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

    using base_type = std::conditional_t<std::endian::native == std::endian::little, uint128_little_endian_storage, uint128_big_endian_storage>;

    using base_type::base_type;
};

}


#endif //UINT128_T_INCLUDE_UINT128_DETAILS_UINT128_STORAGE
