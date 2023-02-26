/*
 * Spacewar - logon a player
 *	      clear out most of the login structure
 *	      set tty modes
 *	      prompt player for his/her name
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "login.h"

// add missing headers
#include <stdlib.h>
#include <stdio.h>

#if 0
#ifdef BSD
#	include <sgtty.h>
#else /* SYSIII SYSV */
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <termio.h>
#endif /* BSD SYSIII SYSV */
#endif

#include <termios.h>

void logon(plogin)
register struct login *plogin;
{

	DBG("logon(#%d/%s)\n",plogin-loginlst,plogin->ln_name);

	/* clear out most of login structure */
	plogin->ln_name[0] = 0;
	if (plogin->ln_term)
		free(plogin->ln_term);
	plogin->ln_term = NULL;
	if (plogin->ln_tcm)
		free(plogin->ln_tcm);
	plogin->ln_tcm = NULL;
	if (plogin->ln_tcl)
		free(plogin->ln_tcl);
	plogin->ln_tcl = NULL;
	if (plogin->ln_tce)
		free(plogin->ln_tce);
	plogin->ln_tce = NULL;
	if (plogin->ln_tso)
		free(plogin->ln_tso);
	plogin->ln_tso = NULL;
	if (plogin->ln_tse)
		free(plogin->ln_tse);
	plogin->ln_tse = NULL;
	plogin->ln_rvslh = 0;
	plogin->ln_iomode = 0;
	plogin->ln_crft[0] = 0;
	plogin->ln_play.ip_ptr = NULL;
	plogin->ln_stat = 0;
	plogin->ln_substat = NULL;
	plogin->ln_input[0] = 0;

	/*****************/
	/* set tty modes */
	/*****************/
#if 0
#ifdef BSD
	{
	struct sgttyb tmode;

	if (gtty(plogin->ln_tty,&tmode)) {
		perror("gtty");
		logoff(plogin);
		VDBG("logon return\n");
		return;
	}

	/* insure no echo and cbreak mode */
	/* (too bad the previous states aren't saved)  */
	tmode.sg_flags |= CBREAK;
	tmode.sg_flags &= ~(RAW+ECHO);

	if (stty(plogin->ln_tty,&tmode)) {
		perror("stty");
		logoff(plogin);
		VDBG("logon return\n");
		return;
	}
	}
#else /* SYSIII SYSV */
	{
	struct termio tmode;

	if (ioctl(plogin->ln_tty,TCGETA,&tmode)) {
		perror("ioctl TCGETA");
		logoff(plogin);
		VDBG("logon return\n");
		return;
	}

	/* insure no echo and no erase/kill edit processing */
	/* (too bad the previous states aren't saved)  */
	tmode.c_lflag &= ~(ICANON+ECHO+ECHOE+ECHOK+ECHONL);
	tmode.c_cc[VMIN] = 1;
	tmode.c_cc[VTIME] = 0;

	if (ioctl(plogin->ln_tty,TCSETA,&tmode)) {
		perror("ioctl TCSETA");
		logoff(plogin);
		VDBG("logon return\n");
		return;
	}
	}
#endif /* BSD SYSIII SYSV */
#endif
	{
	struct termios tmode;
	// Get the current tty settings
	if (tcgetattr(plogin->ln_tty, &tmode))
	{
		perror("tcgetattr");
		logoff(plogin);
		VDBG("logon return\n");
		return;
	}

	// noncanonical mode
	// dont echo input chars
	// dont echo erase char
	// dont echo kill
	// dont echo newline
	// set minimum number of character for noncanon read to 1
	// set timeout for noncanonical read to 0

	tmode.c_lflag &= ~(ICANON+ECHO+ECHOE+ECHOK+ECHONL);
	tmode.c_cc[VMIN] = 1;
	tmode.c_cc[VTIME] = 0;

	if (tcsetattr(plogin->ln_tty,TCSANOW,&tmode)) {
		perror("tcsetattr");
		logoff(plogin);
		VDBG("logon return\n");
		return;
	}

	}
	/* prompt player for name */
	output(plogin,'C',0,"\nWhat is your name?");
	output(plogin,0,0,0);
	VDBG("logon return\n");
}
