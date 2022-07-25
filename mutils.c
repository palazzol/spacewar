/*
 * Spacewar - matrix, vector, and trig routines
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"

VOID unity(mtrx)
double mtrx[3][3];
{
	minit(mtrx);
	mtrx[0][0] = 1.0;
	mtrx[1][1] = 1.0;
	mtrx[2][2] = 1.0;
}

VOID matmul(amtrx,bmtrx,cmtrx)
double amtrx[3][3],bmtrx[3][3],cmtrx[3][3];
{
	double tmp,tmpmtrx[3][3];
	int i,j,k;

	for (i=0;i<3;++i)
	    for (j=0;j<3;++j) {
		tmp = 0.0;
		for (k=0;k<3;++k)
		    tmp = ADD(tmp,MUL(amtrx[k][j],bmtrx[i][k]));
		tmpmtrx[i][j] = tmp;
	    }
	mcopy(cmtrx,tmpmtrx);
}

VOID vecmul(avec,bmtrx,cvec)
double avec[3],bmtrx[3][3],cvec[3];
{
	double tmp,tmpvec[3];
	int i,j;

	for (i=0;i<3;++i) {
	    tmp = 0.0;
	    for (j=0;j<3;++j)
		tmp = ADD(tmp,MUL(avec[j],bmtrx[i][j]));
	    tmpvec[i] = tmp;
	}
	vcopy(cvec,tmpvec);
}

VOID xrot(rotmtrx,rotangl)
double rotmtrx[3][3],rotangl;
{
	double tmpmtrx[3][3];

	minit(tmpmtrx);
	tmpmtrx[1][1] = tmpmtrx[2][2] = COS(rotangl);
	tmpmtrx[1][2] = NEG(tmpmtrx[2][1] = SIN(rotangl));
	tmpmtrx[0][0] = 1.0;
	matmul(rotmtrx,tmpmtrx,rotmtrx);
}

VOID yrot(rotmtrx,rotangl)
double rotmtrx[3][3],rotangl;
{
	double tmpmtrx[3][3];

	minit(tmpmtrx);
	tmpmtrx[0][0] = tmpmtrx[2][2] = COS(rotangl);
	tmpmtrx[2][0] = NEG(tmpmtrx[0][2] = SIN(rotangl));
	tmpmtrx[1][1] = 1.0;
	matmul(rotmtrx,tmpmtrx,rotmtrx);
}

VOID zrot(rotmtrx,rotangl)
double rotmtrx[3][3],rotangl;
{
	double tmpmtrx[3][3];

	minit(tmpmtrx);
	tmpmtrx[0][0] = tmpmtrx[1][1] = COS(rotangl);
	tmpmtrx[0][1] = NEG(tmpmtrx[1][0] = SIN(rotangl));
	tmpmtrx[2][2] = 1.0;
	matmul(rotmtrx,tmpmtrx,rotmtrx);
}

VOID sptort(spvec,rtvec)
double spvec[3],rtvec[3];
{
	double sin2,tmpvec[3];

	sin2 = SIN(spvec[2]);
	tmpvec[0] = MUL(spvec[0],MUL(COS(spvec[1]),sin2));
	tmpvec[1] = MUL(spvec[0],MUL(SIN(spvec[1]),sin2));
	tmpvec[2] = MUL(spvec[0],COS(spvec[2]));
	vcopy(rtvec,tmpvec);
}

double xatan2(x,y)
double x,y;
{
	if (x == 0. && y == 0.)
	    return(0.);
	else
	    return(ATAN2(x,y));
}

VOID rttosp(rtvec,spvec)
double rtvec[3],spvec[3];
{
	double tmp,tmpvec[3];

	tmp = ADD(SQUARE(rtvec[0]),SQUARE(rtvec[1]));
	tmpvec[0] = SQRT(ADD(tmp,SQUARE(rtvec[2])));
	tmpvec[1] = FMOD(ADD(xatan2(rtvec[1],rtvec[0]),TWOPI),TWOPI);
	tmpvec[2] = FMOD(ADD(xatan2(SQRT(tmp),rtvec[2]),TWOPI),TWOPI);
	vcopy(spvec,tmpvec);
}

VOID vdiff(avec,bvec,cvec)
register double *avec,*bvec,*cvec;
{
	register int i;

	for (i=0;i++<3;)
		*cvec++ = *avec++ - *bvec++;
}

double vdist(avec,bvec)
double avec[3],bvec[3];
{
	double tmp,sumsqr=0.;
	int i;

	for (i=0;i<3;++i) {
	    tmp = SUB(avec[i],bvec[i]);
	    sumsqr = ADD(sumsqr,SQUARE(tmp));
	}
	return(SQRT(sumsqr));
}

double vlen(avec)
double avec[3];
{
	double sumsqr;

	sumsqr = SQUARE(avec[0]);
	sumsqr = ADD(sumsqr,SQUARE(avec[1]));
	sumsqr = ADD(sumsqr,SQUARE(avec[2]));
	return(SQRT(sumsqr));
}

#if defined(NEEDFMOD)
double fmod(arg1,arg2)
double arg1,arg2;
{
	double tmp;

	while ((tmp = SUB(arg1,arg2)) >= 0.)
	    arg1 = tmp;
	return(arg1);
}
#endif
