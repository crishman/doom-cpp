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
// $Log:$
//
// DESCRIPTION:
//	Endianess handling, swapping 16bit and 32bit.
//
//-----------------------------------------------------------------------------

#include "m_swap.h"

static_assert(SwapSHORT(0x1234u) == 0x3412u);
static_assert(SwapLONG(0x12345678u) == 0x78563412u);
static_assert(SwapSHORT(0) == 0);
static_assert(SwapLONG(0) == 0);
static_assert(SwapSHORT(0xffffu) == 0xffffu);
static_assert(SwapLONG(0xffffffffu) == 0xffffffffu);

// Exhaust every 16-bit input, and check each bit of the 32-bit permutation.
constexpr bool CheckByteSwaps()
{
    for (std::uint32_t value = 0; value <= 0xffffu; ++value)
    {
        const auto swapped = SwapSHORT(static_cast<std::uint16_t>(value));
        if (swapped != (value % 256u) * 256u + value / 256u
            || SwapSHORT(swapped) != value)
            return false;
    }
    for (unsigned bit = 0; bit < 32; ++bit)
    {
        const std::uint32_t value = std::uint32_t{1} << bit;
        const unsigned destination = (3 - bit / 8) * 8 + bit % 8;
        if (SwapLONG(value) != (std::uint32_t{1} << destination)
            || SwapLONG(SwapLONG(value)) != value)
            return false;
    }
    return true;
}
static_assert(CheckByteSwaps());

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
static_assert(SHORT(0x3412u) == 0x1234);
static_assert(LONG(0x78563412u) == 0x12345678);
static_assert(SHORT(0xffffu) == -1);
static_assert(LONG(0xffffffffu) == -1);
static_assert(SHORT(0x0080u) == -32768);
static_assert(LONG(0x00000080u) == (-2147483647 - 1));
#else
static_assert(SHORT(0x1234) == 0x1234);
static_assert(LONG(0x12345678) == 0x12345678);
static_assert(SHORT(-1) == -1);
static_assert(LONG(-1) == -1);
#endif
