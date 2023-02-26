/*
 * Spacewar - include file that defines a craft's systems
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#define SUBSYS	'S'	/* prefix for dbm(3) key */

struct syskey {
	char	s_syskey;	/* prefix for dbm(3) key */
	char	s_plyr[8+1];	/* player(owner) name */
	char	s_crft[12+1];	/* craft name */
	char	s_type;		/* system type */
};

struct sys {
	char	s_pct;		/* system percent */
	char	s_edmg;		/* system ease of damage */
	char	s_dmg;		/* system current damage */
	short	s_lvl;		/* system current level */
	short	s_cap;		/* system capacity */
};

#define MSYS	16	/* maximum possible number of systems */
