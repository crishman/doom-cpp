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
//	Endianess handling, swapping 16bit and 32bit.
//
//-----------------------------------------------------------------------------


#ifndef __M_SWAP__
#define __M_SWAP__


#include <cstdint>

// Fixed-width swaps are available on either host byte order.
constexpr std::uint16_t SwapSHORT(std::uint16_t value)
{
    return static_cast<std::uint16_t>((value >> 8) | (value << 8));
}

constexpr std::uint32_t SwapLONG(std::uint32_t value)
{
    return (value >> 24)
        | ((value >> 8) & 0x0000ff00u)
        | ((value << 8) & 0x00ff0000u)
        | (value << 24);
}

// WAD files are little endian. Preserve signed coordinates and -1 sentinels.
#if !defined(__BYTE_ORDER__) || !defined(__ORDER_BIG_ENDIAN__) || !defined(__ORDER_LITTLE_ENDIAN__)
#error "Compiler must define host byte order"
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SHORT(x) (static_cast<std::int16_t>(SwapSHORT(static_cast<std::uint16_t>(x))))
#define LONG(x) (static_cast<std::int32_t>(SwapLONG(static_cast<std::uint32_t>(x))))
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SHORT(x) (x)
#define LONG(x) (x)
#else
#error "Unsupported host byte order"
#endif




#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
