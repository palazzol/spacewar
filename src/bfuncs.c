/*
 * Spacewar - byte functions
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"

#include <string.h>

void bytecopy(dst,src,len)
register char *dst,*src;
register int len;
{
	memcpy(dst,src,len);
}

void binit(dst,len)
register char *dst;
register int len;
{
	memset(dst, 0, len);
}

void vcopy(dst,src)
register double *dst,*src;
{
	memcpy(dst,src,3*sizeof(double));
}

void mcopy(dst,src)
register double dst[3][3],src[3][3];
{
	memcpy(dst,src,9*sizeof(double));
}

void vinit(dst)
register double *dst;
{
	memset(dst, 0, 3*sizeof(double));
}

void minit(dst)
register double dst[3][3];
{
	memset(dst, 0, 9*sizeof(double));
}
