/*
 * Spacewar - include file for general stuff
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#ifndef __SPACEWAR_H__
#define __SPACEWAR_H__

#define SWCOMFILE	"/home/palazzol/sw/swcomm"
#define SWDATABASE	"/home/palazzol/sw/swdb"
#define SWGAME		"/home/palazzol/sw/sw"
#define SWREAD		"/home/palazzol/sw/rsw"
#define SWNEWS		"/home/palazzol/sw/swnews"
#define SWERR		"/home/palazzol/sw/swerr"
#define SWOBJ		"/home/palazzol/sw/swobj" /* see objupdate.c */

#define SWMASTER	"Dan R"

#define PI		(3.1415926536)
#define TWOPI		(6.2831853072)
#define DEGTORAD	DIV(TWOPI,360.)
#define PROMPT		"\nPlay/See/Info/Complain/Mail/reBuild/Usercmd/Logoff/Who>"

/* uninteresting fudge factors */
#define VANGVDST	(3084.2514)		/* makes 45deg match 5000 */

/*
 * all floating-point operations through these macros
 * allowing co-processor routines to be substituted for in-line code
 */
#define ADD(x,y)	((x)+(y))
#define SUB(x,y)	((x)-(y))
#define	MUL(x,y)	((x)*(y))
#define DIV(x,y)	((x)/(y))
#define SQUARE(x)	((x)*(x))
#define CUBE(x)		((x)*(x)*(x))

#include <stdlib.h>
#include <math.h>

#define SQRT(x)		(sqrt(x))
#define RANDOM(x)	(rand()%(x))

#define ATAN2(x,y)	(atan2((x),(y)))
#define SIN(x)		(sin(x))
#define COS(x)		(cos(x))

#define FMOD(x,y)	(fmod((x),(y)))
#define NEG(x)		(-(x))
#define INT(x)		((long)(x))
#define FLOAT(i)	((double)(i))
#define MAX(x,y)	(((x) > (y)) ? (x) : (y))
#define MIN(x,y)	(((x) < (y)) ? (x) : (y))

#ifdef DEBUG
void DBG(char *fmt, ...);
void VDBG(char *fmt, ...);
#else
#define DBG(A,...)cmd
#define VDBG(A,...)
#endif

typedef struct {
	double vec[3];
	double dst;
	char valid;
} dsplcmnt;
#define VECVALID 1
#define DSTVALID 2

#include "universe.h"

dsplcmnt vdisp(struct universe *p1,  // vdisp.c
               struct universe *p2,
			   char which);

// Added declarations
void vrmv(struct universe *puniv); // vdisp.c
void vchngd(struct universe *puniv); // vdisp.c

void alninit();	// alninit.c

void update(); // update.c

void firstplyr(); // sw.c

struct login;	// login.h

void output(struct login *plogin, 	// output.c
			char mode, 
			int fld, 
			char *str, 
			...);

void objupdate(); // objupdate.c

void crftupdate(struct login *plogin); // crftupdate.c

void logoff(struct login *plogin); // logoff.c

void vinit(double *dst); // bfuncs.c
void binit(char *dst, int len);	// bfuncs.c
void bytecopy(char *dst, // bfuncs.c
		   char *src,
		   int len);
void vcopy(double *dst,double *src); // bfuncs.c
void minit(double dst[3][3]); // bfuncs.c
void mcopy(double dst[3][3], // bfuncs.c
		   double src[3][3]);

void updobjs(); // updobjs.c

void unity(double mtrx[3][3]);	// mutils.c
void xrot(double rotmtrx[3][3], double rotangl);	// mutils.c
void yrot(double rotmtrx[3][3], double rotangl);	// mutils.c
void zrot(double rotmtrx[3][3], double rotangl);	// mutils.c
void rttosp(double rtvec[3],double spvec[3]); // mutils.c
void vecmul(double avec[3],double bmtrx[3][3],double cvec[3]); // mutils.c
void sptort(double spvec[3],double rtvec[3]); // mutils.c
void vdiff(double *avec,double *bvec,double *cvec); // mutils.c
double vdist(double avec[3],double bvec[3]); // mutils.c
double vlen(double avec[3]); // mutils.c

void unplay(struct login *plogin); // unplay.c

void play(struct login *plogin); // play.c

void mail(struct login *plogin); // mail.c

void build(struct login *plogin); // build.c

void see(struct login *plogin); // see.c

void usrcmd(struct login *plogin); // usrcmd.c

void who(struct login *plogin); // who.c

#include "uio.h"
#include "uio2.h"

void proctrap(struct uio2 uio); // proctrap.c

void shutdown(int e);	// shutdown.c

void prvlck(char *s);	// lckmsg.c
char *lckmsg(); // lckmsg.c

void removeu(idxptr prmv);	// remove.c

int plinit(struct login *plogin); // plinit.c

void background(struct crft *pcrft); // scrn.c
void nums(struct crft *pcrft); // scrn.c
void view(struct crft *pcrft); // scrn.c

void biton(char ary[],int bitno); // bits.c
void bitoff(char ary[],int bitno); // bits.c
int nabit(char ary[],int bitno); // bits.c

void rpt(struct crft *pcrft,char *msg); // rpt.c
void setrpt(struct crft *pcrft); // rpt.c
void fnshrpt(struct crft *pcrft,int flsh); // rpt.c

void damage(struct universe *patck,	// damage.c
			struct universe *ptrgt,
			double rng,
			double dmg,
			char *msg);

void fixdir(struct crft *pcrft); // fixdir.c

void updmov(); // updmov.c

void updtorp(); // updtorp.c

void updaln(); // updaln.c

void upddmg(); // upddmg.c

void updsys(); // updsys.c

void upddsh(); // upddsh.c

void prvcmd(struct login *plogin); // prvcmd.c

void logon(struct login *plogin); // logon.c

void cmd(); // cmd.c
extern int doproctrap;	// cmd.c
extern int doupdate;	// cmd.c

extern int numpling;	// sw.c

extern long gametime;	// update.c

extern char **environ; 	// Linux main???

#endif
