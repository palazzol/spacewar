/*
 * Spacewar - include file that defines a player's mailbox
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#define MLBX	'M'	/* prefix for dbm(3) key */

struct mlbxkey {
	char	mb_mlbxkey;	/* prefix for dbm(3) key */
	char	mb_plyr[8+1];	/* player(recipient) name */
	short	mb_mlbx;	/* mailbox sequence */
};
