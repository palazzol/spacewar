/*
 * Spacewar - include file that defines structure containing user input
 *	      (requires prior inclusion of login.h)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#ifndef __UIO_H__
#define __UIO_H__

struct uio {
	struct login *uio_lgn;	/* user i/o pointer to login struct */
	char	uio_chrs[16];	/* user i/o characters */
};

/*
 * bastardization for named pipe communication from psw:
 *	uio_lgn			is signal# or 0 for new login
 *	uio_chrs[0:1]		is process id
 *	uio_chrs[2:n]		is ttyname for new login
 */

#endif /* __UIO_H__ */
