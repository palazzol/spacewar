/*
 * Spacewar - test size of uio and alternate
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include "uio.h"
#include "uio2.h"

// add missing headers
#include <stdio.h>

int main()
{
	printf("since there are two uios, they must match in size\n");
	printf("sizeof(uio)(%d) %c= sizeof(uio2)(%d)\n", sizeof(struct uio),
	(sizeof(struct uio) == sizeof(struct uio2)) ? '=' : '!',
	sizeof(struct uio2));
	return(0);
}
