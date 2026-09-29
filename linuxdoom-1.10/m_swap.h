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


#include <bit>
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

// The host byte order has to be known to the preprocessor, because SHORT and
// LONG must stay macros: on a little-endian host they expand to their argument
// unchanged, which preserves its type. A function would impose a return type on
// every one of the hundreds of call sites.
//
// __BYTE_ORDER__ is a GCC and Clang extension; MSVC does not define it, and
// every target it supports runs little-endian.
//
// The std::endian assertion below is deliberately kept even though libstdc++
// defines endian::native as __BYTE_ORDER__ itself, which makes it vacuous
// there. MSVC's standard library derives endian::native independently, so on
// the one compiler whose branch is a hand-written assumption, the assertion is
// the thing that checks it -- a wrong guess becomes a build error rather than
// a silently byte-swapped WAD.
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define DOOM_BIG_ENDIAN 1
#  elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define DOOM_BIG_ENDIAN 0
#  else
#    error "Unsupported host byte order"
#  endif
#elif defined(_MSC_VER)
#  define DOOM_BIG_ENDIAN 0
#else
#  error "Compiler must define host byte order"
#endif

static_assert((std::endian::native == std::endian::big) == (DOOM_BIG_ENDIAN == 1),
              "DOOM_BIG_ENDIAN disagrees with std::endian::native");
static_assert(std::endian::native == std::endian::big
              || std::endian::native == std::endian::little,
              "mixed-endian hosts are not supported");

// WAD files are little endian. Preserve signed coordinates and -1 sentinels.
#if DOOM_BIG_ENDIAN
#define SHORT(x) (static_cast<std::int16_t>(SwapSHORT(static_cast<std::uint16_t>(x))))
#define LONG(x) (static_cast<std::int32_t>(SwapLONG(static_cast<std::uint32_t>(x))))
#else
#define SHORT(x) (x)
#define LONG(x) (x)
#endif




#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
