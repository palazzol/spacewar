/*
 * Spacewar - system configuration tables for player and alien ships
 *	      includes system name and offset
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#ifndef __BUILD_H__
#define __BUILD_H__

#define MAXSYS	11
#define MAXHTYP	9

#define HULL		0
#define SOLARE		1
#define ANTME		2
#define WARP		3
#define ROCKETS		4
#define DILITH		5
#define PHASERS		6
#define SHIELDS		7
#define TORPS		8
#define DMGCON		9
#define SECURITY	10

extern char *subsysnam[MAXSYS];

extern struct sysc {
	long	sc_bsp;		/* system configuration base space */
	long	sc_bcr;		/* system configuration base crew */
	long	sc_fsp;		/* system configuration 100% space */
	long	sc_fcr;		/* system configuration 100% crew */
	char	sc_edmg;	/* system configuration ease of damage */
	short	sc_ilvl;	/* system configuration initial level */
	short	sc_cap;		/* system configuration capacity */
	char	sc_rpct;	/* system configuration recommended % */

} config[MAXSYS][MAXHTYP+1];

#endif
