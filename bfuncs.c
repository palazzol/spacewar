/*
 * Spacewar - byte functions
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"

VOID bytecopy(dst,src,len)
register char *dst,*src;
register int len;
{
	while (len-- > 0)
		*dst++ = *src++;
}

VOID binit(dst,len)
register char *dst;
register int len;
{
	while (len-- > 0)
		*dst++ = 0;
}

VOID vcopy(dst,src)
register double *dst,*src;
{
	register int i;

	for (i=0;i++<3;)
		*dst++ = *src++;
}

VOID mcopy(dst,src)
register double dst[3][3],src[3][3];
{
	register int i;
	register double *dst2 = (double *)dst;
	register double *src2 = (double *)src;	
	for (i=0;i++<9;)
		*dst2++ = *src2++;
}

VOID vinit(dst)
register double *dst;
{
	register int i;

	for (i=0;i++<3;)
		*dst++ = 0.;
}

VOID minit(dst)
register double dst[3][3];
{
	register int i;
	register double *dst2 = (double *)dst;
	for (i=0;i++<9;)
		*dst2++ = 0.;
}
