/*
 * Spacewar - include file for general stuff
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

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
#define DBG(A,...)
#define VDBG(A,...)
#endif

typedef struct {
	double vec[3];
	double dst;
	char valid;
} dsplcmnt;
#define VECVALID 1
#define DSTVALID 2

dsplcmnt vdisp();

// Added declarations
void objinit();
void alninit();
void update();
void firstplyr();

struct login;
void output(register struct login *plogin, 
			char mode, 
			int fld, 
			char *str, 
			...);

void objupdate();
void crftupdate(register struct login *plogin);
void logoff(register struct login *plogin);
void vinit(register double *dst);
void updobjs();
void unity(double mtrx[3][3]);
void xrot(double rotmtrx[3][3], double rotangl);
void yrot(double rotmtrx[3][3], double rotangl);
void zrot(double rotmtrx[3][3], double rotangl);
void binit(register char *dst, register int len);
void unplay(register struct login *plogin);
void play(register struct login *plogin);
void mail(register struct login *plogin);
void build(struct login *plogin);
void see(register struct login *plogin);
void usrcmd(register struct login *plogin);
void who(register struct login *plogin);

#include "uio.h"
#include "uio2.h"

void proctrap(struct uio2 uio);

void shutdown(int e);
void prvlck(char *s);

#include "universe.h"
void removeu(idxptr prmv);

int plinit(register struct login *plogin);
void background(struct crft *pcrft);
void biton(char ary[],int bitno);
void rttosp(double rtvec[3],double spvec[3]);
void vecmul(double avec[3],double bmtrx[3][3],double cvec[3]);
void sptort(double spvec[3],double rtvec[3]);
void vchngd(struct universe *puniv);
void vcopy(register double *dst,register double *src);
void rpt(struct crft *pcrft,char *msg);
void setrpt(struct crft *pcrft);
void fnshrpt(register struct crft *pcrft,int flsh);
void damage(struct universe *patck,
			struct universe *ptrgt,
			double rng,
			double dmg,
			char *msg);
void vdiff(register double *avec,register double *bvec,register double *cvec);
void nums(register struct crft *pcrft);
void view(register struct crft *pcrft);
int tgetent(char *p1,char *termname);
void fixdir(register struct crft *pcrft);
void updmov();
void updtorp();
void updaln();
void upddmg();
void updsys();
void upddsh();
void bytecopy(register char *dst,
		   register char *src,
		   register int len);
int nabit(char ary[],int bitno);
void minit(register double dst[3][3]);
void mcopy(register double dst[3][3],
		   register double src[3][3]);
void vrmv(struct universe *puniv);


void bitoff(char ary[],int bitno);
