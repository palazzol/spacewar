/*
 * Spacewar - definitions for fields
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#ifndef __FLDS_H__
#define __FLDS_H__

struct flddesc {
	char	f_row;		/* field row (uppermost is 0) */
	char	f_col;		/* field column (leftmost is 0) */
	char	f_len;		/* field length */
	char	*f_fmt;		/* printf format */
	char	f_grpw;		/* repeating group width (# of fields) */
	char	f_maxg;		/* maximum group repetitions */
};

#define FLD_VIEWSCREEN		0
#define FLD_AUTOFFWD		1
#define FLD_DIR1		2
#define FLD_DIR2		3
#define FLD_PN1			4
#define FLD_PN2			5
#define FLD_PN3			6
#define FLD_VEL1		7
#define FLD_VEL2		8
#define FLD_VEL3		9
#define FLD_VANGL		10
#define FLD_VDIST		11
#define FLD_SSEE		12
#define FLD_SBAD		13
#define FLD_HOMCHAN		14
#define FLD_RADIO		20
#define FLD_TIME		21
#define FLD_SNAME		22
#define FLD_SLEVEL		23
#define FLD_SDMG		24
#define FLD_REPORT		25
#define FLD_LSTCMD		26
#define FLD_COMMAND		27
#define FLD_BACKGROUND		28

extern struct flddesc flds[];

#define BIT_AUTOFFWD		FLD_AUTOFFWD
#define BIT_DIR1		FLD_DIR1
#define BIT_DIR2		FLD_DIR2
#define BIT_PN1			FLD_PN1
#define BIT_PN2			FLD_PN2
#define BIT_PN3			FLD_PN3
#define BIT_VEL1		FLD_VEL1
#define BIT_VEL2		FLD_VEL2
#define BIT_VEL3		FLD_VEL3
#define BIT_VANGL		FLD_VANGL
#define BIT_VDIST		FLD_VDIST
#define BIT_SSEE		FLD_SSEE
#define BIT_SBAD		FLD_SBAD
#define BIT_HOMCHAN		FLD_HOMCHAN
#define BIT_RADIO		FLD_RADIO
#define BIT_TIME		FLD_TIME
#define BIT_SNAME		FLD_SNAME
#define BIT_SLEVEL		FLD_SLEVEL
#define BIT_SDMG		FLD_SDMG
#define BIT_REPORT		FLD_SNAME+(3*MSYS)

#endif
