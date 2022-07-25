/*
 * Spacewar - set up aliens into the universe
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "sys.h"
#include "aln.h"
#include "obj.h"
#include "build.h"

VOID alninit()
{
	struct aln *paln=alnlst+MAXALN;
	int i;
	struct sysc *psysc;
	struct sys *psys;
	struct universe *puniv;

#ifdef DEBUG
	DBG("alninit()\n");
#endif

	/* build all the aliens, one at a time */
	while (paln-- > alnlst) {

		/* random position, but not too far */
		/* from the plane of the ecliptic   */
		paln->al_pstn[0] = SUB(MUL(FLOAT(RANDOM(100)),4000.),200000.);
		paln->al_pstn[1] = SUB(MUL(FLOAT(RANDOM(100)),4000.),200000.);
		paln->al_pstn[2] = SUB(MUL(FLOAT(RANDOM(100)),100.),5000.);

		/* no velocity or thrust */
		vinit(paln->al_vel);
		vinit(paln->al_thr);

		/* not doing(attacking) anything */
		paln->al_dly = 0;
		paln->al_lhit.ip_ptr = NULL;
		paln->al_atck.ip_ptr = NULL;
		paln->al_aeval = 0;

		/* choose a (random) hull type and give */
		/* random percentages of each subsystem */
		paln->al_htyp = RANDOM(6) + 4; /* see build.h */
		for (i=0;i < MSYS && i < MAXSYS;++i) {

		    /* point to alien subsystem and configuration */
		    psys = paln->al_sys + i;
		    psysc = &config[i][paln->al_htyp];
		    if (!psysc->sc_cap) continue;  /* not in this craft */

		    /* actual% = recommended% +/- 20 and within limits */
			if ((psys->s_pct = psysc->sc_rpct + RANDOM(40) - 20) > 100)
				psys->s_pct = 100;
			else if (psys->s_pct < 0)
				psys->s_pct = 0;

		    /* rest of subsystem */
		    psys->s_edmg = psysc->sc_edmg;
		    psys->s_dmg = 0;
		    psys->s_lvl = psysc->sc_ilvl;
		    psys->s_cap = psysc->sc_cap;
		    if (i == ROCKETS)
			psys->s_lvl = ((long)psys->s_lvl * (long)psys->s_pct)
			/ 100L;
		    else if (i == TORPS)
			psys->s_lvl = (psys->s_cap * psys->s_lvl) / 100L;
		}

		/* place into universe */
		puniv = univlst + MAXOBJ + (paln-alnlst);
		puniv->uv_type = 'A';
		puniv->uv_pctr = paln->al_htyp + '0';
		puniv->uv_pstn = paln->al_pstn;
		puniv->uv_mass = paln->al_sys[HULL].s_dmg; /* kludge */
		puniv->uv_rad = 1;
		puniv->uv_ptr.uv_aln = paln;
		paln->al_univ.ip_ptr = puniv;
	}
#ifdef DEBUG
	VDBG("alninit return\n");
#endif
}
