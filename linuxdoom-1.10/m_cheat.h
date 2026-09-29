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
//	Cheat code checking.
//
//-----------------------------------------------------------------------------


#ifndef __M_CHEAT__
#define __M_CHEAT__

//
// CHEAT SEQUENCE PACKAGE
//

constexpr unsigned char ScrambleCheatByte(unsigned char value)
{
    return static_cast<unsigned char>(
        ((value & 1u) << 7) | ((value & 2u) << 5) | (value & 4u)
        | ((value & 8u) << 1) | ((value & 16u) >> 1) | (value & 32u)
        | ((value & 64u) >> 5) | ((value & 128u) >> 7));
}

typedef struct
{
    unsigned char*	sequence;
    unsigned char*	p;
    
} cheatseq_t;

int
cht_CheckCheat
( cheatseq_t*		cht,
  char			key );


void
cht_GetParam
( cheatseq_t*		cht,
  char*			buffer );


#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
