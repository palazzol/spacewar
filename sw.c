/*
 * Spacewar - main for spacewar game
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include <signal.h>
#include "spacewar.h"

int numpling;

#ifdef BSD
#   include <sys/ioctl.h>
    static int sigtrap,swlgnfd;
#else /* VMS SYSIII SYSV */
#ifdef VMS
#    include <descrip.h>
#    include <ssdef.h>
#    include <psldef.h>
    static $DESCRIPTOR(mlbx,SWCOMFILE);
    extern short inmlbx;
#else /* SYSIII SYSV */
#    include <sys/types.h>
#    include <sys/stat.h>
#endif /* VMS SYSIII SYSV */
#endif /* BSD VMS SYSIII SYSV */
extern int doproctrap,doupdate;
static int dbglvl;
static VOID catchtrp(),catchalrm();

main(argc,argv)
int argc;
char *argv[];
{
	extern VOID proctrap(),shutdown(),cmd();
#ifdef VMS
	int i;
#endif /* VMS */
	extern int errno;
#ifdef BSD
	int swpidfd,thispid,pfd[2];
#endif /* BSD */

	if (argc > 1) dbglvl = atoi(argv[1]);

	/* insure running in background */
#ifndef VMS
	/*if (fork() > 0) exit(0);*/
#endif /* VMS BSD SYSIII SYSV */

	/* ignore interrupts, shutdown on terminate */
	/* break connection with controlling tty */
	/* close unneccesary files */
#ifndef VMS
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGTERM,shutdown);
#ifdef SIGTSTP
	signal(SIGTSTP,SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN,SIG_IGN);
#endif
#ifdef SIGTTOU
	signal(SIGTTOU,SIG_IGN);
#endif
#ifdef TIOCNOTTY
	ioctl(0,TIOCNOTTY,0);
	close(open("/dev/console",1));
#endif
	setpgrp(getpid(),getpid());
#endif
	close(0);
	close(1);

	/* set up objects and aliens */
	srand(time(0));
	objinit();
	alninit();

	/* set up readsw pipe/named pipe/mailbox  */
#ifdef BSD
	if (pipe(pfd) || pfd[0] != 0 || pfd[1] != 1) {
		perror("pipe");
		exit(1);
	}

	/* set up communication files */
	thispid = getpid();
	if ((swpidfd=creat(SWPIDFILE,0644)) < 0 ||
	write(swpidfd,&thispid,sizeof(thispid)) != sizeof(thispid) ||
	close(swpidfd)) {
		perror(SWPIDFILE);
		exit(1);
	}
	if ((swlgnfd=creat(SWLGNFILE,0666)) < 0 || close(swlgnfd) ||
	(swlgnfd=open(SWLGNFILE,0)) < 0) {
		perror(SWLGNFILE);
		if (unlink(SWPIDFILE)) perror(SWPIDFILE);
		exit(1);
	}
#else /* VMS SYSIII SYSV */
#ifdef VMS
	if ((i=sys$assign(&mlbx,&inmlbx,PSL$C_USER,0)) != SS$_NORMAL) {
		perror("assign mlbx");
#ifdef DEBUG
		VDBG("sw assign()=%d, errno=%d\n",i,errno);
#endif
		exit(1);
	}
#else /* SYSIII SYSV */
	if (mknod(SWCOMFILE,0666+S_IFIFO,0) ||
	open(SWCOMFILE,0) != 0 ||
	open(SWCOMFILE,1) != 1) {
		perror(SWCOMFILE);
		exit(1);
	}
#endif /* VMS SYSIII SYSV */
#endif /* VMS BSD SYSIII SYSV */

	/* open dbm(3) file */
	if (dbminit(SWDATABASE)) {
		perror(SWDATABASE);
#ifdef BSD
		if (unlink(SWLGNFILE)) perror(SWLGNFILE);
		if (unlink(SWPIDFILE)) perror(SWPIDFILE);
#else /* VMS SYSIII SYSV */
#ifndef VMS
		if (unlink(SWCOMFILE)) perror(SWCOMFILE);
#endif	/* VMS SYSIII SYSV */
#endif /* VMS BSD SYSIII SYSV */
		exit(1);
	}

	/* catch asynchronous event notification from playsw */
#ifdef BSD
	signal(SIGTRAP,catchtrp);
#endif /* BSD */

	/* trap alarm to update universe */
	signal(SIGALRM,catchalrm);

	/*******************/
	/* MAIN PROCESSING */
	/*******************/

	/* get and process commands and interrupts */
	for (;;) {
		cmd();
#ifdef BSD
		if (sigtrap) {
			doproctrap = 0;
			proctrap(swlgnfd,&sigtrap);
			doproctrap = 1;
		}
#endif /* BSD */
		if (doupdate < 0) {
			doproctrap = 0;
			update();
			if (doproctrap == 0) doproctrap = 1;
			doupdate = 1;
		}
	}

}

firstplyr()
{catchalrm();}

static VOID catchalrm()
{

#ifdef DEBUG
	VDBG("catchalrm\n");
#endif
#ifdef VMS
	sys$cancel(inmlbx);
#endif /* VMS */
	signal(SIGALRM,catchalrm);
	if (doproctrap > 0 && doupdate > 0) {
		doproctrap = 0;
		update();
		if (doproctrap == 0) doproctrap = 1;
	} else
		doupdate = -1;
	if (numpling)
		alarm(1);
}

#ifdef BSD
static VOID catchtrp()
{
#ifdef DEBUG
	VDBG("catchtrp [doproctrap=%d]\n",doproctrap);
#endif
	++sigtrap;
	if (doproctrap > 0) {
		doproctrap = 0;
		proctrap(swlgnfd,&sigtrap);
		doproctrap = 1;
	} else
		doproctrap = -1;
}
#endif /* BSD */


#ifdef DEBUG

#undef NULL
#include <stdio.h>

#ifdef VMS
#include <varargs.h>
VOID DBG(va_alist)
va_dcl
{
	va_list ap;
	int nargs,i,a[8];
	char *fmt;

	va_start(ap);
	va_count(nargs);
	fmt = va_arg(ap,char *);
	for (i=0;--nargs > 0;++i)
		a[i] = va_arg(ap,int);
	va_end(ap);
	while (i < sizeof(a)/sizeof(a[0]))
		a[i++] = -1;
	if (dbglvl > 0) fprintf(stderr,fmt,
	a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
}
VOID VDBG(va_alist)
va_dcl
{
	va_list ap;
	int nargs,i,a[8];
	char *fmt;

	va_start(ap);
	va_count(nargs);
	fmt = va_arg(ap,char *);
	for (i=0;--nargs > 0;++i)
		a[i] = va_arg(ap,int);
	va_end(ap);
	while (i < sizeof(a)/sizeof(a[0]))
		a[i++] = -1;
	if (dbglvl > 1) fprintf(stderr,fmt,
	a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
}
#else
/*VARARGS1*/
VOID DBG(fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)
char *fmt;
{if (dbglvl > 0) fprintf(stderr,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);}

/*VARARGS1*/
VOID VDBG(fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)
char *fmt;
{if (dbglvl > 1) fprintf(stderr,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);}
#endif

#endif
