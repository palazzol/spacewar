/*
 * Spacewar - shutdown (gracefully) the spacewar game
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "login.h"

#ifdef VMS
#	include <ssdef.h>
	extern short inmlbx;
#endif /* VMS */

VOID shutdown(e)
int e;
{
	register struct login *plogin;
	int i;
	extern int errno;

#ifdef DEBUG
	DBG("shutdown(%d)\n",e);
#endif

	/* don't update the universe any more */
	alarm(0);

	/* update objects, crafts */
	objupdate();
	crftupdate((struct login *)0);

	/* notify and log all players off */
	for (plogin=loginlst,i=MAXLOGIN;i-- > 0;++plogin)
		if (plogin->ln_tty) {
			if (plogin->ln_play.ip_ptr) {
				plogin->ln_play.ip_ptr = NULL;
				output(plogin,'E',0,0);
			}
			output(plogin,'B',0,0);
			output(plogin,'C',0,"\n\nSpacewar has been shut down\n");
			output(plogin,0,0,0);
			logoff(plogin);
		}

	/* remove communication files */
#ifdef BSD
	if (unlink(SWLGNFILE)) perror(SWLGNFILE);
	if (unlink(SWPIDFILE)) perror(SWPIDFILE);
#else /* VMS SYSIII SYSV */
#ifdef VMS
	if ((i=sys$delmbx(inmlbx)) != SS$_NORMAL) {
		perror("delete mlbx 2");
#ifdef DEBUG
		VDBG("shutdown delmbx()=%d, errno=%d\n",i,errno);
#endif
	}
	if ((i=sys$dassgn(inmlbx)) != SS$_NORMAL) {
		perror("dassgn mlbx 2");
#ifdef DEBUG
		VDBG("shutdown dassgn()=%d, errno=%d\n",i,errno);
#endif
	}
#else /* SYSIII SYSV */
	if (unlink(SWCOMFILE)) perror(SWCOMFILE);
#endif /* VMS SYSIII SYSV */
#endif /* VMS BSD SYSIII SYSV */

#ifdef DEBUG
	VDBG("shutdown exiting\n");
#endif
	exit(e);
}
