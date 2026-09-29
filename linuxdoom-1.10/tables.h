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
//	Lookup tables.
//	Do not try to look them up :-).
//	In the order of appearance: 
//
//	int finetangent[4096]	- Tangens LUT.
//	 Should work with BAM fairly well (12 of 16bit,
//      effectively, by shifting).
//
//	int finesine[10240]		- Sine lookup.
//	 Guess what, serves as cosine, too.
//	 Remarkable thing is, how to use BAMs with this? 
//
//	int tantoangle[2049]	- ArcTan LUT,
//	  maps tan(angle) to angle fast. Gotta search.	
//    
//-----------------------------------------------------------------------------


#ifndef __TABLES__
#define __TABLES__



#ifdef LINUX
#include <math.h>
#else
#define PI				3.141592657
#endif


#include "m_fixed.h"
#include <limits>
#include <cstdint>
	
inline constexpr int FINEANGLES = 8192;
inline constexpr int FINEMASK = (FINEANGLES-1);


// 0x100000000 to 0x2000
inline constexpr int ANGLETOFINESHIFT = 19;

// Effective size is 10240.
extern  fixed_t		finesine[5*FINEANGLES/4];

// Re-use data, is just PI/2 pahse shift.
extern  fixed_t*	finecosine;


// Effective size is 4096.
extern fixed_t		finetangent[FINEANGLES/2];

// Binary Angle Measurement, BAM. Preserve the original literal signedness.
inline constexpr int ANG45 = 0x20000000;
inline constexpr int ANG90 = 0x40000000;
inline constexpr unsigned ANG180 = 0x80000000;
inline constexpr unsigned ANG270 = 0xc0000000;


inline constexpr int SLOPERANGE = 2048;
inline constexpr int SLOPEBITS = 11;
inline constexpr int DBITS = (FRACBITS-SLOPEBITS);

typedef unsigned angle_t;

// Binary angles wrap modulo one turn; shift them as unsigned values.
constexpr unsigned FineAngleIndex(angle_t angle)
{
    return angle >> ANGLETOFINESHIFT;
}
static_assert(FineAngleIndex(0) == 0);
static_assert(FineAngleIndex(ANG180) == FINEANGLES / 2);
static_assert(FineAngleIndex(0x80700000u) == 4110); // Previously indexed -4082.
static_assert(FineAngleIndex(0xffffffffu) == FINEMASK);



// Tangent repeats every half turn, unlike the sine table.
constexpr unsigned FineTangentIndex(angle_t angle)
{
    return FineAngleIndex(angle) & (FINEANGLES / 2 - 1);
}
static_assert(FineTangentIndex(0) == 0);
static_assert(FineTangentIndex(ANG180) == 0);
static_assert(FineTangentIndex(4733u << ANGLETOFINESHIFT) == 637);
static_assert(FineTangentIndex(0xffffffffu) == FINEANGLES / 2 - 1);

// Effective size is 2049;
// The +1 size is to handle the case when x==y
//  without additional checking.
extern angle_t		tantoangle[SLOPERANGE+1];


// Utility function,
//  called by R_PointToAngle.
constexpr int SlopeDiv(unsigned num, unsigned den)
{
    if (den < 512)
        return SLOPERANGE;

    // Preserve the original unsigned shift, including wraparound.
    const unsigned ans = (num << 3) / (den >> 8);
    return ans <= SLOPERANGE ? static_cast<int>(ans) : SLOPERANGE;
}

static_assert(std::numeric_limits<angle_t>::digits == 32);
static_assert(FINEANGLES > 0 && (FINEANGLES & (FINEANGLES - 1)) == 0);
static_assert(FINEMASK == FINEANGLES - 1);
static_assert((std::uint64_t{FINEANGLES} << ANGLETOFINESHIFT) == (std::uint64_t{1} << 32));
static_assert(ANG90 == 2u * ANG45 && ANG180 == 2u * ANG90);
static_assert(ANG270 == ANG180 + ANG90);
static_assert(SLOPERANGE == (1 << SLOPEBITS));
static_assert(DBITS >= 0);

static_assert(SlopeDiv(0, 0) == SLOPERANGE);
static_assert(SlopeDiv(1, 511) == SLOPERANGE);
static_assert(SlopeDiv(0, 512) == 0);
static_assert(SlopeDiv(256, 512) == 1024);
static_assert(SlopeDiv(511, 512) == 2044);
static_assert(SlopeDiv(512, 512) == SLOPERANGE);
static_assert(SlopeDiv(513, 512) == SLOPERANGE);
static_assert(SlopeDiv(256, 767) == 1024);
static_assert(SlopeDiv(256, 768) == 682);
static_assert(SlopeDiv(0x20000000u, 512) == 0);



#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
