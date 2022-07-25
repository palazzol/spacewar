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

#ifdef BSD
#	include <sgtty.h>
#else /* VMS SYSIII SYSV */
#ifndef VMS
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <termio.h>
#endif /* VMS SYSIII SYSV */
#endif /* BSD VMS SYSIII SYSV */

VOID logon(plogin)
register struct login *plogin;
{

#ifdef DEBUG
	DBG("logon(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/* clear out most of login structure */
	plogin->ln_name[0] = NULL;
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
	plogin->ln_iomode = NULL;
	plogin->ln_crft[0] = NULL;
	plogin->ln_play.ip_ptr = NULL;
	plogin->ln_stat = NULL;
	plogin->ln_substat = NULL;
	plogin->ln_input[0] = NULL;

	/*****************/
	/* set tty modes */
	/*****************/
#ifdef BSD
	{
	struct sgttyb tmode;

	if (gtty(plogin->ln_tty,&tmode)) {
		perror("gtty");
		logoff(plogin);
#ifdef DEBUG
		VDBG("logon return\n");
#endif
		return;
	}

	/* insure no echo and cbreak mode */
	/* (too bad the previous states aren't saved)  */
	tmode.sg_flags |= CBREAK;
	tmode.sg_flags &= ~(RAW+ECHO);

	if (stty(plogin->ln_tty,&tmode)) {
		perror("stty");
		logoff(plogin);
#ifdef DEBUG
		VDBG("logon return\n");
#endif
		return;
	}
	}
#else /* VMS SYSIII SYSV */
#ifndef VMS
	{
	struct termio tmode;

	if (ioctl(plogin->ln_tty,TCGETA,&tmode)) {
		perror("ioctl TCGETA");
		logoff(plogin);
#ifdef DEBUG
		VDBG("logon return\n");
#endif
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
#ifdef DEBUG
		VDBG("logon return\n");
#endif
		return;
	}
	}
#endif /* VMS */
#endif /* VMS BSD SYSIII SYSV */


	/* prompt player for name */
	output(plogin,'C',0,"\nWhat is your name?");
	output(plogin,0,0,0);
#ifdef DEBUG
	VDBG("logon return\n");
#endif
}
