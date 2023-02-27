/*
 * Spacewar - shutdown (gracefully) the spacewar game
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "login.h"

// add missing headers
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void shutdown(e)
int e;
{
	struct login *plogin;
	int i;

	DBG("shutdown(%d)\n",e);

	/* don't update the universe any more */
	alarm(0);

	/* update objects, crafts */
	objupdate();
	crftupdate((struct login *)0);

	/* notify and log all players off */
	for (plogin=loginlst,i=MAXLOGIN;i-- > 0;++plogin)
		if (plogin->ln_tty) {
			if (plogin->ln_play.ip_ptr) {
				plogin->ln_play.ip_ptr = nullptr;
				output(plogin,'E',0,0);
			}
			output(plogin,'B',0,0);
			output(plogin,'C',0,"\n\nSpacewar has been shut down\n");
			output(plogin,0,0,0);
			logoff(plogin);
		}

	/* remove communication file */
	if (unlink(SWCOMFILE)) perror(SWCOMFILE);

	VDBG("shutdown exiting\n");
	exit(e);
}
