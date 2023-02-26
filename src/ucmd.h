/*
 * Spacewar - include file that defines a player's-defined commands
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#define UCMD	'U'	/* prefix for dbm(3) key */

struct ucmdkey {
	char	uc_ucmdkey;	/* prefix for dbm(3) key */
	char	uc_plyr[8+1];	/* player(definer/user) name */
	char	uc_name[8+1];	/* user-command name */
	short	uc_ucmd;	/* user-command sequence */
};
