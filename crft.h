/*
 * Spacewar - include file that defines a player's craft structure
 *	      (requires prior inclusion of sys.h, login.h, and universe.h)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#define CRAFT	'C'	/* prefix for dbm(3) key */

struct crftkey {
	char	cr_crftkey;	/* prefix for dbm(3) key */
	char	cr_plyr[8+1];	/* player(owner) name */
	char	cr_name[12+1];	/* craft name */
};

#define MHOM	6

struct crft {
	char	cr_htyp;	/* craft hull type */
	long	cr_flsp;	/* craft free floor space */
	long	cr_crew;	/* craft free crew */
	long	cr_pnts;	/* craft points */
	time_t	cr_time;	/* craft playing time */
	short	cr_kill;	/* craft credited kills */
	idxptr	cr_dock;	/* craft docked at (object) */
	double	cr_pstn[3];	/* craft position */
	double	cr_dir[3];	/* craft direction */
	double	cr_vel[3];	/* craft velocity */
	double	cr_thr[3];	/* craft thrust */
	idxptr	cr_hom[MHOM];	/* craft homing channels */
	idxptr	cr_auto;	/* craft autopilot */
	char	cr_ffwd;	/* craft facing forward */
	double	cr_vang;	/* craft viewing angle */
	long	cr_vdst;	/* craft visual distance */
	char	cr_plvl;	/* craft privilege level */

/* in-core only */
	struct login *cr_lgn;	/* craft login */
	idxptr	cr_univ;		/* craft universe pointer */
	long	cr_hdst[MHOM];	/* craft homing channel distances */
	char	cr_sens[2];	/* craft sensors */
	idxptr	cr_lhit;	/* craft last hit by */
	double	cr_lhpstn[3];	/* craft last hit from position */
	char	cr_lrpt;	/* craft last report line */
	char	cr_lcmd;	/* craft last command line */
	char	cr_chng[10];	/* craft field changed flags */
	char	cr_scrn[15][31];/* craft viewscreen */
	double	cr_rmat[3][3];	/* craft rotation matrix */
	struct sys cr_sys[MSYS];/* craft systems */
};

#define MAXCRFT	MAXLOGIN

extern struct crft crftlst[];

#define CRDATSIZ (((char *)(&crftlst[0].cr_lgn))-((char *)(&crftlst[0].cr_htyp)))
