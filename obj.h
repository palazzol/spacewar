/*
 * Spacewar - include file that defines a universe object
 *	      (requires prior inclusion of universe.h)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

struct obj {
	long	oj_mass;	/* object mass (for gravity) */
	short	oj_rad;		/* object radius */
	char	oj_rep;		/* object representation (screen character) */
	idxptr	oj_octr;	/* object orbital center (a previous obj) */
	short	oj_oprd;	/* object orbital period */
	long	oj_orad;	/* object orbital center radius */
	double	oj_ocrpt;	/* object orbital center relative plane theta */
	double	oj_optx;	/* object orbital plane tilt x (in degrees) */
	double	oj_opty;	/* object orbital plane tilt y (in degrees) */
	double	oj_optz;	/* object orbital plane tilt z (in degrees) */

/* in-core only */
	idxptr	oj_univ;	/* object universe pointer */
	double	oj_pstn[3];	/* object position */
	double	oj_vel[3];	/* object velocity */
	double	oj_rmat[3][3];	/* object rotation matrix */
};

#define MAXOBJ	42

extern struct obj objlst[];
