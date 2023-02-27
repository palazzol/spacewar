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

// add missing headers
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <termios.h>
#include <errno.h>

static void setTtyModes(plogin)
struct login *plogin;
{
	struct termios tmode;

	if (tcgetattr(plogin->ln_tty,&tmode)) {
		perror("tcgetattr");
	}

	/* reset echo and erase/kill edit processing */
	/* (too bad the previous states weren't saved)  */
	tmode.c_lflag |= ICANON+ECHO+ECHOE+ECHOK+ECHONL;
	tmode.c_cc[VEOF] = CEOF;
	tmode.c_cc[VEOL] = CEOL;

	if (tcsetattr(plogin->ln_tty,TCSANOW,&tmode)) {
		perror("tcsetattr");
	}
}

void logoff(plogin)
struct login *plogin;
{
	DBG("logoff(#%d/%s)\n",plogin-loginlst,plogin->ln_name);

	/* remove from universe if playing */
	if (plogin->ln_play.ip_ptr) unplay(plogin);

	/*****************/
	/* set tty modes */
	/*****************/
	setTtyModes(plogin);

	/* close the player's terminal and kill the read and play processes */
	
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
	VDBG("logoff return\n");
}
