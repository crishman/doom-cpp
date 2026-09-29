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
//	Simple basic typedefs, isolated here to make it easier
//	 separating modules.
//    
//-----------------------------------------------------------------------------


#ifndef __DOOMTYPE__
#define __DOOMTYPE__


#ifndef __BYTEBOOL__
#define __BYTEBOOL__
// Fixed to use builtin bool type with C++.
#ifdef __cplusplus
typedef bool boolean;
#else
typedef enum {false, true} boolean;
#endif
typedef unsigned char byte;
#endif


// These came from <values.h> on Linux and from hand-written fallbacks
// everywhere else. <values.h> is a deprecated SVID header that does not exist
// on macOS or Windows, and the fallbacks resolved to the same values anyway,
// so both branches are gone in favour of <climits>.
//
// That also corrects MAXLONG/MINLONG, which the fallback defined as the 32-bit
// limits; long is 64-bit on LP64. Neither is used anywhere in the tree, but a
// knowingly wrong macro is not worth keeping.
#include <climits>

#define MAXCHAR		SCHAR_MAX
#define MAXSHORT	SHRT_MAX
#define MAXINT		INT_MAX
#define MAXLONG		LONG_MAX
#define MINCHAR		SCHAR_MIN
#define MINSHORT	SHRT_MIN
#define MININT		INT_MIN
#define MINLONG		LONG_MIN




#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
