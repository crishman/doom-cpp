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
//  MapObj data. Map Objects or mobjs are actors, entities,
//  thinker, take-your-pick... anything that moves, acts, or
//  suffers state changes of more or less violent nature.
//
//-----------------------------------------------------------------------------


#ifndef __D_THINK__
#define __D_THINK__





//
// Thinkers are all invoked through one signature, so the indirect call in
// P_RunThinkers is type-correct. Each concrete thinker keeps its own parameter
// type and gets a generated thunk that performs the downcast; thinker_s is the
// first member of every thinker struct, so the addresses coincide.
//
// The previous arrangement stored them in a union of void(*)(void*) and called
// them through that, which is undefined behaviour however identically the ABI
// happens to pass the arguments. clang's -fsanitize=function reports it; gcc
// has no such check.
//
struct thinker_s;

using think_t = void (*)(struct thinker_s*);

template<auto Fn>
struct thinker_thunk;

template<class T, void (*Fn)(T*)>
struct thinker_thunk<Fn>
{
    static void call(struct thinker_s* thinker)
    {
	Fn(reinterpret_cast<T*>(thinker));
    }
};

// P_Thinker<T_MoveFloor> is what gets stored, and the same expression compares
// equal afterwards, so identifying a thinker by its function still works --
// which p_saveg.cpp depends on to tell the thinker types apart.
template<auto Fn>
inline constexpr think_t P_Thinker = &thinker_thunk<Fn>::call;

// Marks a thinker for removal at the end of the tic. A real function rather
// than a cast -1 so the type system stays honest; it is never called.
void P_ThinkerRemoved(struct thinker_s* thinker);
inline constexpr think_t THINKER_REMOVED = &P_ThinkerRemoved;


// Doubly linked list of actors.
typedef struct thinker_s
{
    struct thinker_s*	prev;
    struct thinker_s*	next;
    think_t		function;
    
} thinker_t;



#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
