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
#include <fcntl.h>
#include <signal.h>
#include "universe.h"
#include "login.h"
#include "uio2.h"

// add missing headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int setupread();

void proctrap(uio)
struct uio2 uio;
{
	struct login *plogin;
	int i;
	extern void logon(),logoff();

	DBG("proctrap(%d,%d,%.*s)\n",uio.uio2sig,uio.uio2pid,sizeof(uio.uio2tty),uio.uio2tty);

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
			if (plogin->ln_play.ip_ptr == nullptr) {
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
struct login *plogin;
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
			sprintf(buf,"%ld",(long)plogin);
			execlp(SWREAD,"rsw",buf,(char *)nullptr);
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

