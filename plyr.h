/*
 * Spacewar - include file that defines a player structure
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#define PLYR	'P'	/* prefix for dbm(3) key */

struct plyrkey {
	char	pl_plyrkey;	/* prefix for dbm(3) key */
	char	pl_name[8+1];	/* player name */
};

struct plyr {
	char	pl_passwd[8+1];	/* player password */
	short	pl_numlgn;	/* player number of logins */
	time_t	pl_lstlgn;	/* player last login */
	short	pl_frstml;	/* player first mail sequence# */
	short	pl_seenml;	/* player seen mail sequence# (old/new) */
	short	pl_lstml;	/* player last mail sequence# */
	short	pl_slst;	/* player number of ships lost */
	short	pl_klst;	/* player number of kills lost */
	long 	pl_plst;	/* player number of points lost */
	time_t	pl_tlst;	/* player playing time lost */
};
