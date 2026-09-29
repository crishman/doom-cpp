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
//	Cheat sequence checking.
//
//-----------------------------------------------------------------------------


static const char
rcsid[] = "$Id: m_cheat.c,v 1.1 1997/02/03 21:24:34 b1 Exp $";

#include "m_cheat.h"
#include <array>

//
// CHEAT SEQUENCE PACKAGE
//

namespace
{
constexpr auto MakeCheatTranslationTable()
{
    std::array<unsigned char, 256> table{};
    for (unsigned i = 0; i < table.size(); ++i)
        table[i] = ScrambleCheatByte(static_cast<unsigned char>(i));
    return table;
}

constexpr auto cheat_xlate_table = MakeCheatTranslationTable();

constexpr bool CheckCheatTranslationTable()
{
    // Independently describe where each input bit goes in the legacy encoding.
    constexpr unsigned destination_bits[] = {7, 6, 2, 4, 3, 5, 1, 0};
    for (unsigned i = 0; i < cheat_xlate_table.size(); ++i)
    {
        unsigned expected = 0;
        for (unsigned bit = 0; bit < 8; ++bit)
            if (i & (1u << bit))
                expected |= 1u << destination_bits[bit];
        if (cheat_xlate_table[i] != expected)
            return false;
    }
    return true;
}
static_assert(CheckCheatTranslationTable());
}


//
// Called in st_stuff module, which handles the input.
// Returns a 1 if the cheat was successful, 0 if failed.
//
int
cht_CheckCheat
( cheatseq_t*	cht,
  char		key )
{
    int rc = 0;

    if (!cht->p)
	cht->p = cht->sequence; // initialize if first time

    if (*cht->p == 0)
	*(cht->p++) = key;
    else if
	(cheat_xlate_table[(unsigned char)key] == *cht->p) cht->p++;
    else
	cht->p = cht->sequence;

    if (*cht->p == 1)
	cht->p++;
    else if (*cht->p == 0xff) // end of sequence character
    {
	cht->p = cht->sequence;
	rc = 1;
    }

    return rc;
}

void
cht_GetParam
( cheatseq_t*	cht,
  char*		buffer )
{

    unsigned char *p, c;

    p = cht->sequence;
    while (*(p++) != 1);
    
    do
    {
	c = *p;
	*(buffer++) = c;
	*(p++) = 0;
    }
    while (c && *p!=0xff );

    if (*p==0xff)
	*buffer = 0;

}


