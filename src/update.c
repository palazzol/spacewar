/*
 * Spacewar - update the universe
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#include <sys/types.h>
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "aln.h"
#include "build.h"
#include "flds.h"
#include "torp.h"
#include "crft.h"

long gametime;

void update()
{
	struct universe *puniv;
	struct login *plogin;
	struct crft *pcrft;
	char *s,*lckmsg();

	DBG("update()\n");

	/* periodic sync and lockout check */
	if (++gametime%60L == 0) {
	    objupdate();
	    crftupdate((struct login *)0);
	    if (s=lckmsg()) {
		numpling = 0;
		for (plogin=loginlst+MAXLOGIN;plogin-- > loginlst;) {
		    if (plogin->ln_tty && plogin->ln_play.ip_ptr) {
			removeu(plogin->ln_play);
			plogin->ln_iomode = 0;
			plogin->ln_crft[0] = 0;
			plogin->ln_play.ip_ptr = NULL;
			plogin->ln_stat = 0;
			plogin->ln_substat = NULL;
			output(plogin,'E',0,0);
			output(plogin,'B',0,0);
			output(plogin,'C',0,"\n\n");
			output(plogin,'C',0,s);
			output(plogin,'C',0,"\n");
			output(plogin,'C',0,PROMPT);
			output(plogin,0,0,0);
		    }
		}
	    }
	}

	/* movement of objects */
	updobjs();
	updmov();

	/* movement of intelligent nasties */
	updtorp();
	updaln();

	/* damage/destruction from hull damage, damage control */
	upddmg();

	/* energy subsystems */
	updsys();

	/* direction, sensors, homing */
	upddsh();

	/* player's screens */
	for (plogin=loginlst;plogin < loginlst+MAXLOGIN;++plogin) {
	    if (!plogin->ln_tty || !(puniv=plogin->ln_play.ip_ptr)) continue;
	    nums(pcrft=puniv->uv_ptr.uv_crft);
	    view(pcrft);
	    output(plogin,0,0,0);
	    pcrft->cr_time += 1;
	}

	VDBG("update return\n");
}
