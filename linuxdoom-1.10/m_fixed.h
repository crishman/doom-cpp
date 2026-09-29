// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Fixed point arithemtics, implementation.
//
//-----------------------------------------------------------------------------


#ifndef __M_FIXED__
#define __M_FIXED__


#include <cstdint>
#include <limits>


//
// Fixed point, 32bit as 16.16.
//
#define FRACBITS		16
#define FRACUNIT		(1<<FRACBITS)

typedef int fixed_t;

static_assert(std::numeric_limits<fixed_t>::digits == 31);
static_assert(std::numeric_limits<fixed_t>::min() == (-2147483647 - 1));
static_assert(FRACBITS == 16);

constexpr fixed_t FixedMul(fixed_t a, fixed_t b)
{
    const std::int64_t product = static_cast<std::int64_t>(a) * b;
    // Extract bits 16..47: floor the scaled product, then wrap to 32 bits.
    // Unsigned conversion/shifting also handles negative products in C++17.
    const std::uint32_t bits = static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(product) >> FRACBITS);
    // Map the high half into the signed range without out-of-range narrowing.
    return static_cast<fixed_t>(bits <= 0x7fffffffu
        ? static_cast<std::int64_t>(bits)
        : static_cast<std::int64_t>(bits) - 0x100000000LL);
}

static_assert(FixedMul(FRACUNIT, FRACUNIT) == FRACUNIT);
static_assert(FixedMul(-FRACUNIT, FRACUNIT) == -FRACUNIT);
static_assert(FixedMul(-FRACUNIT, -FRACUNIT) == FRACUNIT);
static_assert(FixedMul(FRACUNIT / 2, FRACUNIT / 2) == FRACUNIT / 4);
static_assert(FixedMul(1, 1) == 0);
static_assert(FixedMul(-1, 1) == -1);
static_assert(FixedMul(std::numeric_limits<fixed_t>::min(), FRACUNIT)
              == std::numeric_limits<fixed_t>::min());
static_assert(FixedMul(std::numeric_limits<fixed_t>::min(), -FRACUNIT)
              == std::numeric_limits<fixed_t>::min());
static_assert(FixedMul(std::numeric_limits<fixed_t>::max(), 2 * FRACUNIT) == -2);

namespace fixed_detail
{
// Invalid direct divisions remain runtime engine errors. Such calls cannot be
// used in a constant expression; valid calls need no engine dependencies.
[[noreturn]] void DivisionError(const char* message);

constexpr std::uint32_t Magnitude(fixed_t value)
{
    return value < 0 ? static_cast<std::uint32_t>(-static_cast<std::int64_t>(value))
                     : static_cast<std::uint32_t>(value);
}
}

constexpr fixed_t FixedDiv2(fixed_t a, fixed_t b)
{
    if (b == 0)
        fixed_detail::DivisionError("FixedDiv: divide by zero");

    // Preserve the original floating-point evaluation and truncation.
    const double quotient = static_cast<double>(a) / static_cast<double>(b) * FRACUNIT;
    if (quotient >= 2147483648.0 || quotient < -2147483648.0)
        fixed_detail::DivisionError("FixedDiv: quotient out of range");
    return static_cast<fixed_t>(quotient);
}

constexpr fixed_t FixedDiv(fixed_t a, fixed_t b)
{
    // This deliberately saturates early, matching the original gameplay rule.
    // Magnitudes are unsigned so INT_MIN and zero divisors are well-defined.
    if ((fixed_detail::Magnitude(a) >> 14) >= fixed_detail::Magnitude(b))
        return (a < 0) != (b < 0) ? std::numeric_limits<fixed_t>::min()
                                  : std::numeric_limits<fixed_t>::max();
    return FixedDiv2(a, b);
}

static_assert(FixedDiv(1, 3) == 21845);
static_assert(FixedDiv(-1, 3) == -21845);
static_assert(FixedDiv(1, -3) == -21845);
static_assert(FixedDiv(-1, -3) == 21845);
static_assert(FixedDiv(16383, 1) == 1073676288);
static_assert(FixedDiv(16384, 1) == std::numeric_limits<fixed_t>::max());
static_assert(FixedDiv(-16384, 1) == std::numeric_limits<fixed_t>::min());
static_assert(FixedDiv2(16384, 1) == 1073741824);
static_assert(FixedDiv(0, 0) == std::numeric_limits<fixed_t>::max());
static_assert(FixedDiv(-1, 0) == std::numeric_limits<fixed_t>::min());
static_assert(FixedDiv(std::numeric_limits<fixed_t>::min(),
                       std::numeric_limits<fixed_t>::min()) == FRACUNIT);
static_assert(FixedDiv(0, std::numeric_limits<fixed_t>::min()) == 0);
static_assert(FixedDiv2(std::numeric_limits<fixed_t>::min(), FRACUNIT)
              == std::numeric_limits<fixed_t>::min());



#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
