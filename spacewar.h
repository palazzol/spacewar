/*
 * Spacewar - include file for general stuff
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#ifdef BSD
#	define SWPIDFILE	"/home/palazzol/sw/swpid"
#	define SWLGNFILE	"/home/palazzol/sw/swlgn"
#else /* SYSIII SYSV */
#	define SWCOMFILE	"/home/palazzol/sw/swcomm"
#endif /* BSD SYSIII SYSV */

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

VOID DBG(char *fmt, ...);
VOID VDBG(char *fmt, ...);

typedef struct {
	double vec[3];
	double dst;
	char valid;
} dsplcmnt;
#define VECVALID 1
#define DSTVALID 2

dsplcmnt vdisp();

// Added declarations
VOID objinit();
VOID alninit();
VOID update();
VOID firstplyr();

struct login;
VOID output(register struct login *plogin, 
			char mode, 
			int fld, 
			char *str, 
			...);

VOID objupdate();
VOID crftupdate(register struct login *plogin);
VOID logoff(register struct login *plogin);
VOID vinit(register double *dst);
VOID updobjs();
VOID unity(double mtrx[3][3]);
VOID xrot(double rotmtrx[3][3], double rotangl);
VOID yrot(double rotmtrx[3][3], double rotangl);
VOID zrot(double rotmtrx[3][3], double rotangl);
VOID binit(register char *dst, register int len);
VOID unplay(register struct login *plogin);
VOID play(register struct login *plogin);
VOID mail(register struct login *plogin);
VOID build(struct login *plogin);
VOID see(register struct login *plogin);
VOID usrcmd(register struct login *plogin);
VOID who(register struct login *plogin);

#include "uio.h"
#include "uio2.h"

#ifdef BSD
VOID proctrap(int trapmsgfd,int *ntrapmsg);
#else
VOID proctrap(struct uio2 uio);
#endif

VOID shutdown(int e);
VOID prvlck(char *s);

#include "universe.h"
VOID removeu(idxptr prmv);

int plinit(register struct login *plogin);
VOID background(struct crft *pcrft);
VOID biton(char ary[],int bitno);
VOID rttosp(double rtvec[3],double spvec[3]);
VOID vecmul(double avec[3],double bmtrx[3][3],double cvec[3]);
VOID sptort(double spvec[3],double rtvec[3]);
VOID vchngd(struct universe *puniv);
VOID vcopy(register double *dst,register double *src);
VOID rpt(struct crft *pcrft,char *msg);
VOID setrpt(struct crft *pcrft);
VOID fnshrpt(register struct crft *pcrft,int flsh);
VOID damage(struct universe *patck,
			struct universe *ptrgt,
			double rng,
			double dmg,
			char *msg);
VOID vdiff(register double *avec,register double *bvec,register double *cvec);
VOID nums(register struct crft *pcrft);
VOID view(register struct crft *pcrft);
int tgetent(char *p1,char *termname);
VOID fixdir(register struct crft *pcrft);
VOID updmov();
VOID updtorp();
VOID updaln();
VOID upddmg();
VOID updsys();
VOID upddsh();
VOID bytecopy(register char *dst,
		   register char *src,
		   register int len);
int nabit(char ary[],int bitno);
VOID minit(register double dst[3][3]);
VOID mcopy(register double dst[3][3],
		   register double src[3][3]);
VOID vrmv(struct universe *puniv);


VOID bitoff(char ary[],int bitno);
