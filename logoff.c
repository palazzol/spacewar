/*
 * Spacewar - logoff a player:
 *		reset tty modes
 *		close the tty I/O channel
 *	 	terminate (signal) the play and read processes
 *		clear out the login structure
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include <signal.h>
#include "spacewar.h"
#include "universe.h"
#include "login.h"

#ifdef BSD
#	include <sgtty.h>
#else /* SYSIII SYSV */
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <termio.h>
#endif /* BSD SYSIII SYSV */

// add missing headers
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

VOID logoff(plogin)
register struct login *plogin;
{
	extern int errno;

#ifdef DEBUG
	DBG("logoff(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/* remove from universe if playing */
	if (plogin->ln_play.ip_ptr) unplay(plogin);

	/*****************/
	/* set tty modes */
	/*****************/
#ifdef BSD
	{
	struct sgttyb tmode;

	if (gtty(plogin->ln_tty,&tmode)) {
		perror("gtty");
		goto sigh;	/* horrendous */
	}

	/* reset echo and no cbreak mode */
	/* (too bad the previous states weren't saved)  */
	tmode.sg_flags &= ~CBREAK;
	tmode.sg_flags |= ECHO;

	if (stty(plogin->ln_tty,&tmode)) {
		perror("stty");
		goto sigh;	/* horrendous */
	}
	}
#else /* SYSIII SYSV */
	{
	struct termio tmode;

	if (ioctl(plogin->ln_tty,TCGETA,&tmode)) {
		perror("ioctl TCGETA");
		goto sigh;	/* horrendous */
	}

	/* reset echo and erase/kill edit processing */
	/* (too bad the previous states weren't saved)  */
	tmode.c_lflag |= ICANON+ECHO+ECHOE+ECHOK+ECHONL;
	tmode.c_cc[VEOF] = CEOF;
	//tmode.c_cc[VEOL] = CNUL; // CNUL seems to be gone?

	if (ioctl(plogin->ln_tty,TCSETA,&tmode)) {
		perror("ioctl TCSETA");
		goto sigh;	/* horrendous */
	}
	}
#endif /* BSD SYSIII SYSV */

	/* close the player's terminal and kill the read and play processes */
sigh:
	if (close(plogin->ln_tty))
		perror("close");
	if (kill(plogin->ln_readpid,SIGTERM))
		perror("kill readsw");
	else
		wait(0);
	if (kill(plogin->ln_playpid,SIGTERM))
		perror("kill playsw");

	/* reset the login entry */
	binit((char *)plogin,sizeof(*plogin));
#ifdef DEBUG
	VDBG("logoff return\n");
#endif
}
