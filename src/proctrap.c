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
#else /* SYSIII SYSV */
#	include <fcntl.h>
#endif /* BSD SYSIII SYSV */
#include <signal.h>
#include "universe.h"
#include "login.h"
#include "uio2.h"

// add missing headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int setupread();

#ifdef BSD
void proctrap(trapmsgfd,ntrapmsg)
int trapmsgfd,*ntrapmsg;
{
	struct uio2 uio;
#else /* SYSIII SYSV */
void proctrap(uio)
struct uio2 uio;
{
#endif /* BSD SYSIII SYSV */
	register struct login *plogin;
	int i;
	extern void logon(),logoff();

#ifdef BSD
	DBG("proctrap(%d,%d)\n",trapmsgfd,*ntrapmsg);

	/* for as many traps as received */
	do {

		/* get the pid */
		if (read(trapmsgfd,&uio,sizeof(uio)) != sizeof(uio)) {
			perror("trapmsg file uio");
			*ntrapmsg = 0;
			VDBG("proctrap return\n");
			return;
		}
		VDBG("proctrap: uio %d %d %.*s\n",uio.uio2sig,uio.uio2pid,
		sizeof(uio.uio2tty),uio.uio2tty);
#else /* SYSIII SYSV */
		DBG("proctrap(%d,%d,%.*s)\n",uio.uio2sig,uio.uio2pid,sizeof(uio.uio2tty),uio.uio2tty);
#endif /* BSD SYSIII SYSV */

		/* try to find player */
		for (plogin=loginlst,i=MAXLOGIN+1;--i > 0;++plogin)
			if (plogin->ln_playpid == uio.uio2pid)
				break;
		VDBG("proctrap: login entry #%d\n",plogin-loginlst);
		/* player is already logged on, therefore its a signal */
		if (i) {

			/* process according to signal# */
			switch(uio.uio2sig) {

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
				if (plogin->ln_tty == 0)
					break;
			VDBG("proctrap: available login entry #%d\n",
			plogin-loginlst);
			if (i && setupread(plogin,uio.uio2pid,uio.uio2tty))
				logon(plogin);
			else
				kill(uio.uio2pid,SIGTERM);
		}
#ifdef BSD
	} while (--(*ntrapmsg) > 0);
#endif /* BSD */
	VDBG("proctrap return\n");
}

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

	DBG("setupread(#%d,%d,%s)\n",plogin-loginlst,playpid,ttynm);

	/* open the player's tty */
	if ((ttyfd = open(ttynm,2)) < 0) {
		perror(ttynm);
		if (doproctrap == 0) doproctrap = 1;
		VDBG("setupread return\n");
		return(0);
	}

	/* fork */
	switch(readpid=fork()) {
		case -1:	/* failed */
			perror("fork");
			if (close(ttyfd)) perror(ttynm);
			if (doproctrap == 0) doproctrap = 1;
			VDBG("setupread return\n");
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
			sprintf(buf,"%ld",(long)plogin);
			execlp(SWREAD,"rsw",buf,NULL);
			perror(SWREAD);
			exit(1);
	}

	/* parent; fill in login structure */
	plogin->ln_tty = ttyfd;
	plogin->ln_playpid = playpid;
	plogin->ln_readpid = readpid;

	if (doproctrap == 0) doproctrap = 1;
	VDBG("setupread return\n");
	return(1);
}

