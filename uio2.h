/*
 * Spacewar - login and signal communication structure
 *	      (alternate uio for SYSIII SYSV)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

struct uio2 {
	int	uio2sig;	/* signal, 0 for logon */
	short	uio2pid;	/* process-id of playsw */
	char	uio2tty[14];	/* ttyname for logon (signal 0 only) */
};
