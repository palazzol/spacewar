/*
 * Spacewar - byte functions
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"

void bytecopy(dst,src,len)
char *dst,*src;
int len;
{
	while (len-- > 0)
		*dst++ = *src++;
}

void binit(dst,len)
char *dst;
int len;
{
	while (len-- > 0)
		*dst++ = 0;
}

void vcopy(dst,src)
double *dst,*src;
{
	int i;

	for (i=0;i++<3;)
		*dst++ = *src++;
}

void mcopy(dst,src)
double *dst,*src;
{
	int i;

	for (i=0;i++<9;)
		*dst++ = *src++;
}

void vinit(dst)
double *dst;
{
	int i;

	for (i=0;i++<3;)
		*dst++ = 0.;
}

void minit(dst)
double *dst;
{
	int i;

	for (i=0;i++<9;)
		*dst++ = 0.;
}
