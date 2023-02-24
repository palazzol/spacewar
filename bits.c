/*
 * Spacewar - turn bits on, off, and return value
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"

#define BPB	8	/* bits per byte */

void biton(ary,bitno)
char ary[];
int bitno;
{
	ary[bitno/BPB] |= 1<<(bitno%BPB);
}

void bitoff(ary,bitno)
char ary[];
int bitno;
{
	ary[bitno/BPB] &= ~(1<<(bitno%BPB));
}

int nabit(ary,bitno)
char ary[];
int bitno;
{
	return((ary[bitno/BPB]>>(bitno%BPB))&1);
}
