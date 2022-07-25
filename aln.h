/*
 * Spacewar - include file that defines an alien
 *	      (requires prior inclusion of sys.h and universe.h)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

struct aln {
	double	al_pstn[3];	/* alien position */
	double	al_vel[3];	/* alien velocity */
	double	al_thr[3];	/* alien thrust */
	short	al_dly;		/* alien delay counter */
	char	al_htyp;	/* alien hull type */
	idxptr	al_lhit;	/* alien last hit by */
	idxptr	al_atck;	/* alien currently attacking */
	short	al_aeval;	/* alien attack evaluation */
	idxptr	al_univ;	/* alien universe location */
	struct sys al_sys[MSYS];/* alien systems */
};

#define MAXALN	30

extern struct aln alnlst[];
