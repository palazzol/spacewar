/*
 * Spacewar - turn bits on, off, and return value
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"

#define BPB	8	/* bits per byte */

VOID biton(ary,bitno)
char ary[];
int bitno;
{
	ary[bitno/BPB] |= 1<<(bitno%BPB);
}

VOID bitoff(ary,bitno)
char ary[];
int bitno;
{
	ary[bitno/BPB] &= ~(1<<(bitno%BPB));
}

nabit(ary,bitno)
char ary[];
int bitno;
{
	return((ary[bitno/BPB]>>(bitno%BPB))&1);
}
