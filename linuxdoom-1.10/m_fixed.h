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

fixed_t FixedDiv	(fixed_t a, fixed_t b);
fixed_t FixedDiv2	(fixed_t a, fixed_t b);



#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
