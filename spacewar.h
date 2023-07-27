/*
 * Spacewar - include file for general stuff
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#ifndef __SPACEWAR_H__
#define __SPACEWAR_H__

#ifdef VMS
#define void
#define delete dbmdelete
#endif /* VMS */

#ifdef BSD
#	define SWPIDFILE	"/home/palazzol/sw/swpid"
#	define SWLGNFILE	"/home/palazzol/sw/swlgn"
#else /* VMS SYSIII SYSV */
#ifdef VMS
#	define SWCOMFILE	"swmlbx"
#else /* SYSIII SYSV */
#	define SWCOMFILE	"/home/palazzol/sw/swcomm"
#endif /* VMS SYSIII SYSV */
#endif /* BSD VMS SYSIII SYSV */

#ifdef VMS
#	define SWDATABASE	"$DISK2:[TSDIR.SW]swdb."
#	define SWGAME		"$DISK2:[TSDIR.SW]sw"
#	define SWREAD		"$DISK2:[TSDIR.SW]vmsrsw"
#	define SWNEWS		"$DISK2:[TSDIR.SW]swnews"
#	define SWERR		"$DISK2:[TSDIR.SW]swerr"
#	define SWOBJ		"$DISK2:[TSDIR.SW]swobj"
#else /* BSD SYSIII SYSV */
#	define SWDATABASE	"/home/palazzol/sw/swdb"
#	define SWGAME		"/home/palazzol/sw/sw"
#	define SWREAD		"/home/palazzol/sw/rsw"
#	define SWNEWS		"/home/palazzol/sw/swnews"
#	define SWERR		"/home/palazzol/sw/swerr"
#	define SWOBJ		"/home/palazzol/sw/swobj" /* see objupdate.c */
#endif /* VMS BSD SYSIII SYSV */

#define SWMASTER	"Dan R"

#define PI		(3.1415926536)
#define TWOPI		(6.2831853072)
#define DEGTORAD	DIV(TWOPI,360.)
#define PROMPT		"\nPlay/See/Info/Complain/Mail/reBuild/Usercmd/Logoff/Who>"
#ifdef NULL
#undef	NULL
#endif
#define	NULL		(0)

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

extern double sqrt();
extern int rand();
#define SQRT(x)		(sqrt(x))
#define RANDOM(x)	(rand()%(x))

extern double atan2(),sin(),cos();
#define ATAN2(x,y)	(atan2((x),(y)))
#define SIN(x)		(sin(x))
#define COS(x)		(cos(x))

extern double fmod();
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

void alninit();	// alninit.c

void update(); // update.c

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

void updobjs(); // updobjs.c

void unity(double mtrx[3][3]);	// mutils.c
void xrot(double rotmtrx[3][3], double rotangl);	// mutils.c
void yrot(double rotmtrx[3][3], double rotangl);	// mutils.c
void zrot(double rotmtrx[3][3], double rotangl);	// mutils.c

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

void cmd(); // cmd.c
extern int doproctrap; // cmd.c
extern int doupdate; // cmd.c

#endif /* __SPACEWAR_H__ */
