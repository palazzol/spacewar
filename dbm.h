/*
 * NOTE: VMS only
 */

typedef	struct
{
	char	*dptr;
	int	dsize;
} datum;

datum	fetch();
datum	firstkey();
datum	nextkey();
