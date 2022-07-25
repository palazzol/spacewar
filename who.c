/*
 * Spacewar - display who is logged in and what craft they are playing
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "login.h"

VOID who(plogin)
register struct login *plogin;
{
	register struct login *plgn=loginlst+MAXLOGIN;
	char buf[40+1];

#ifdef DEBUG
	DBG("who(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	strcpy(buf,"\n");
	while (--plgn >= loginlst) {
		if (!plgn->ln_tty) continue;	/* no one there */
		if (plgn == plogin) continue;	/* not the current player */

		if (strlen(buf) + strlen(plgn->ln_name) + 1 +
		((plgn->ln_stat == 'P') ? strlen(plgn->ln_crft)+1 : 0)) {
			output(plogin,'C',0,buf);
			strcpy(buf,"\n");
		}
		strcat(buf,plgn->ln_name);
		if (plgn->ln_stat == 'P') {
			strcat(buf,"/");
			strcat(buf,plgn->ln_crft);
		}
		strcat(buf," ");
	}
	if (strlen(buf) > 1) {
		output(plogin,'C',0,buf);
		output(plogin,'C',0,"\n");
	} else
		output(plogin,'C',0,"\nNo one else\n");
	
	plogin->ln_stat = NULL;
	output(plogin,'C',0,PROMPT);
	output(plogin,0,0,0);
#ifdef DEBUG
	VDBG("who return\n");
#endif
}
