/*
 * Spacewar - test size of uio and alternate
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include "uio.h"
#include "uio2.h"

main()
{
#ifdef BSD
	printf("only one uio, nothing to worry about\n");
#else /* VMS SYSIII SYSV */
#ifdef VMS
	printf("only one uio, nothing to worry about\n");
#else /* SYSIII SYSV */
	printf("since there are two uios, they must match in size\n");
	printf("sizeof(uio)(%d) %c= sizeof(uio2)(%d)\n", sizeof(struct uio),
	(sizeof(struct uio) == sizeof(struct uio2)) ? '=' : '!',
	sizeof(struct uio2));
#endif /* VMS SYSIII SYSV */
#endif /* BSD VMS SYSIII SYSV */
	return(0);
}
