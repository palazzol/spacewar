/*
 * Spacewar - process an asynchronous trap, usually a player wanting
 *	      to logon to the game or possibly a signal from a user
 *	      noticed and passed on by playsw
 *
 *	      trapmsg structure: pid {signal#|ttyname}
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#ifdef BSD
#	include <sys/ioctl.h>
#else /* VMS SYSIII SYSV */
#ifdef VMS
#	include <descrip.h>
#	include <ssdef.h>
#	include <psldef.h>
#	include "uio.h"
#else /* SYSIII SYSV */
#	include <fcntl.h>
#endif /* VMS SYSIII SYSV */
#endif /* BSD VMS SYSIII SYSV */
#include <signal.h>
#include "universe.h"
#include "login.h"
#ifndef VMS
#	include "uio2.h"
#endif /* VMS */

static int setupread();

#ifdef BSD
VOID proctrap(trapmsgfd,ntrapmsg)
int trapmsgfd,*ntrapmsg;
{
	struct uio2 uio;
#else /* VMS SYSIII SYSV */
VOID proctrap(uio)
#ifdef VMS
struct uio uio;
#else /* SYSIII SYSV */
struct uio2 uio;
#endif /* VMS SYSIII SYSV */
{
#endif /* VMS BSD SYSIII SYSV */
	register struct login *plogin;
	int i;
#ifdef VMS
	int pid;
#endif /* VMS */
	extern VOID logon(),logoff();

#ifdef BSD
#ifdef DEBUG
	DBG("proctrap(%d,%d)\n",trapmsgfd,*ntrapmsg);
#endif

	/* for as many traps as received */
	do {

		/* get the pid */
		if (read(trapmsgfd,&uio,sizeof(uio)) != sizeof(uio)) {
			perror("trapmsg file uio");
			*ntrapmsg = 0;
#ifdef DEBUG
			VDBG("proctrap return\n");
#endif
			return;
		}
#ifdef DEBUG
		VDBG("proctrap: uio %d %d %s\n",uio.uio2sig,uio.uio2pid,
		uio.uio2tty);
#endif
#else /* VMS SYSIII SYSV */
#ifdef VMS
#ifdef DEBUG
		DBG("proctrap(%d,%s)\n",(int)uio.uio_lgn,uio.uio_chrs);
#endif
		sscanf(uio.uio_chrs+2,"%x",&pid);
#else /* SYSIII SYSV */
#ifdef DEBUG
		DBG("proctrap(%d,%d,%s)\n",uio.uio2sig,uio.uio2pid,uio.uio2tty);
#endif
#endif /* VMS SYSIII SYSV */
#endif /* VMS BSD SYSIII SYSV */

		/* try to find player */
		for (plogin=loginlst,i=MAXLOGIN+1;--i > 0;++plogin)
#ifdef VMS
			if (plogin->ln_pid == pid)
#else /* BSD SYSIII SYSV */
			if (plogin->ln_playpid == uio.uio2pid)
#endif /* VMS BSD SYSIII SYSV */
				break;
#ifdef DEBUG
		VDBG("proctrap: login entry #%d\n",plogin-loginlst);
#endif

		/* player is already logged on, therefore its a signal */
		if (i) {

			/* process according to signal# */
#ifdef VMS
			switch((int)uio.uio_lgn) {
#else /* BSD SYSIII SYSV */
			switch(uio.uio2sig) {
#endif /* VMS BSD SYSIII SYSV */

			    case SIGQUIT:	/* wants to go away */
				output(plogin,'E',0,0);
			    case SIGHUP:	/* or just went away */
				logoff(plogin);
				break;

			    case SIGINT:	/* restart if not playing */
				if (plogin->ln_play.ip_ptr == NULL) {
					output(plogin,'C',0,
					"\n\n\nInterrupt - restarting\n");
					logon(plogin);
				}
				break;

			    default:
				perror("proctrap: unknown signal");
				break;
			}

		/* not logged in, therefore its the ttyname */
		} else {

			/* find an available login */
			for (plogin=loginlst,i=MAXLOGIN+1;--i > 0;++plogin)
				if (plogin->ln_tty == NULL)
					break;
#ifdef DEBUG
			VDBG("proctrap: available login entry #%d\n",
			plogin-loginlst);
#endif
#ifdef VMS
			/* psw will timeout if no message from setupread */
			if (i && setupread(plogin,pid))
				logon(plogin);
#else /* BSD SYSIII SYSV */
			if (i && setupread(plogin,uio.uio2pid,uio.uio2tty))
				logon(plogin);
			else
				kill(uio.uio2pid,SIGTERM);
#endif /* VMS BSD SYSIII SYSV */
		}
#ifdef BSD
	} while (--(*ntrapmsg) > 0);
#endif /* BSD */
#ifdef DEBUG
	VDBG("proctrap return\n");
#endif
}

#ifdef VMS
static int setupread(plogin,pid)
struct login *plogin;
int pid;
{
	struct dsc$descriptor_d mlbx;
	char buf[32];
	int i;
	extern int errno;

#ifdef DEBUG
	DBG("setupread(#%d,%x)\n",plogin-loginlst,pid);
#endif

	sprintf(buf,"sw%x",pid);
	mlbx.dsc$w_length = strlen(buf);
	mlbx.dsc$b_dtype = DSC$K_DTYPE_T;
	mlbx.dsc$b_class = DSC$K_CLASS_S;
	mlbx.dsc$a_pointer = buf;
	if ((i=sys$crembx(1,&plogin->ln_tty,0,0,0,PSL$C_USER,&mlbx)) !=
	SS$_NORMAL) {
		perror("crembx");
#ifdef DEBUG
		VDBG("setupread crembx()=%d, errno=%d\n",i,errno);
#endif
		plogin->ln_tty = NULL;
#ifdef DEBUG
		VDBG("setupread return\n");
#endif
		return(0);
	}
	plogin->ln_pid = pid;

	sprintf(buf,"%d",plogin);
	output(plogin,'C',0,buf);
	output(plogin,0,0,0);

#ifdef DEBUG
	VDBG("setupread return\n");
#endif
	return(1);
}
#else /* BSD SYSIII SYSV */
/*
 * setting up a readsw process with:
 *	fd0 attached to the terminal
 *	fd1 as it exists in spacewar
 *	all other fd's closed
 * partially fill in the login structure
 * returns non-zero if successful, 0 otherwise
 */
static int setupread(plogin,playpid,ttynm)
register struct login *plogin;
int playpid;
char *ttynm;
{
	int ttyfd,readpid,i;
	char buf[32];
	extern int doproctrap;

	/* temporarily disable interrupts */
	if (doproctrap == 1) doproctrap = 0;

#ifdef DEBUG
	DBG("setupread(#%d,%d,%s)\n",plogin-loginlst,playpid,ttynm);
#endif

	/* open the player's tty */
	if ((ttyfd = open(ttynm,2)) < 0) {
		perror(ttynm);
		if (doproctrap == 0) doproctrap = 1;
#ifdef DEBUG
		VDBG("setupread return\n");
#endif
		return(0);
	}

	/* fork */
	switch(readpid=fork()) {
		case -1:	/* failed */
			perror("fork");
			if (close(ttyfd)) perror(ttynm);
			if (doproctrap == 0) doproctrap = 1;
#ifdef DEBUG
			VDBG("setupread return\n");
#endif
			return(0);

		case 0:		/* child */
#ifdef BSD
			if (dup2(ttyfd,0) < 0) {
				perror("dup2");
				exit(1);
			}
			if (close(ttyfd)) perror(ttynm);
			for (i=3;i < 20;ioctl(i++,FIOCLEX,NULL));
#ifdef TIOCGPGRP
			ioctl(0,TIOCGPGRP,&i);
			setpgrp(0,i);
#endif
#else /* SYSIII SYSV */
			if (close(0)) {
				perror("close(0)");
				exit(1);
			}
			if (fcntl(ttyfd,F_DUPFD,0) != 0) {
				perror("fcntl(ttyfd,F_DUPFD,0)");
				exit(1);
			}
			if (close(ttyfd)) perror(ttynm);
			for (i=3;i < 20;fcntl(i++,F_SETFD,1));
#endif /* BSD SYSIII SYSV */
			sprintf(buf,"%d",(int)plogin);
			execlp(SWREAD,"rsw",buf,0);
			perror(SWREAD);
			exit(1);
	}

	/* parent; fill in login structure */
	plogin->ln_tty = ttyfd;
	plogin->ln_playpid = playpid;
	plogin->ln_readpid = readpid;

	if (doproctrap == 0) doproctrap = 1;
#ifdef DEBUG
	VDBG("setupread return\n");
#endif
	return(1);
}
#endif /* VMS BSD SYSIII SYSV */
