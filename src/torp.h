/*
 * Spacewar - include file that defines a torpedo
 *	      (requires prior inclusion of universe.h)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

struct torp {
	double	tp_pstn[3];	/* torpedo position */
	double	tp_vel[3];	/* torpedo velocity */
	double	tp_thr[3];	/* torpedo thrust */
	idxptr	tp_aim;		/* torpedo aimed at */
	idxptr	tp_fby;		/* torpedo fired by */
	long	tp_dist;	/* torpedo quarry distance */
	char	tp_dmg;		/* torpedo hull damage */
	idxptr	tp_lhit;	/* torpedo last hit by */
	idxptr	tp_univ;	/* torpedo universe pointer */
};

#define MAXTORP	90	/* 3*MAXCRFT */

extern struct torp torplst[];

#define MAXTTHR	50	/* maximum torp thrust from rockets */
