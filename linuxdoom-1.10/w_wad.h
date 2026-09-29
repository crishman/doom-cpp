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
//	WAD I/O functions.
//
//-----------------------------------------------------------------------------


#ifndef __W_WAD__
#define __W_WAD__

#include <cstddef>
#include <climits>




//
// TYPES
//
typedef struct
{
    // Should be "IWAD" or "PWAD".
    char		identification[4];		
    int			numlumps;
    int			infotableofs;
    
} wadinfo_t;


typedef struct
{
    int			filepos;
    int			size;
    char		name[8];
    
} filelump_t;

//
// WADFILE I/O related stuff.
//
typedef struct
{
    char	name[8];
    int		handle;
    int		position;
    int		size;
} lumpinfo_t;


extern	void**		lumpcache;
extern	lumpinfo_t*	lumpinfo;
extern	int		numlumps;

void    W_InitMultipleFiles (const char* const* filenames);
void    W_Reload (void);

int	W_CheckNumForName (const char* name);
int	W_GetNumForName (const char* name);

int	W_LumpLength (int lump);
void    W_ReadLump (int lump, void *dest);

void*	W_CacheLumpNum (int lump, int tag);
void*	W_CacheLumpName (const char* name, int tag);




// These structures describe bytes on disk, independent of native pointer size.
static_assert(CHAR_BIT == 8);
static_assert(sizeof(int) == 4);
static_assert(sizeof(wadinfo_t) == 12);
static_assert(offsetof(wadinfo_t, numlumps) == 4);
static_assert(offsetof(wadinfo_t, infotableofs) == 8);
static_assert(sizeof(filelump_t) == 16);
static_assert(offsetof(filelump_t, size) == 4);
static_assert(offsetof(filelump_t, name) == 8);

#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
