/*
 * Spacewar - definitions for fields
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#ifndef FLDS
extern
#endif
struct flddesc {
	char	f_row;		/* field row (uppermost is 0) */
	char	f_col;		/* field column (leftmost is 0) */
	char	f_len;		/* field length */
	char	*f_fmt;		/* printf format */
	char	f_grpw;		/* repeating group width (# of fields) */
	char	f_maxg;		/* maximum group repetitions */
} flds[]
#ifdef FLDS
    = {
	/* upper left of viewscreen */
	{0,	24,	0,	"%s"},

	/* direction */
	{1,	19,	4,	"%-4.4s"},
	{1,	6,	5,	"%5.1f"},
	{1,	12,	5,	"%5.1f"},

	/* position */
	{3,	0,	7,	"%7.0f"},
	{3,	8,	7,	"%7.0f"},
	{3,	16,	7,	"%7.0f"},

	/* velocity */
	{5,	0,	7,	"%7.1f"},
	{5,	8,	7,	"%7.1f"},
	{5,	16,	7,	"%7.1f"},

	/* viewscreen attributes */
	{8,	0,	5,	"%5.1f"},	/* angle */
	{8,	6,	9,	"%9ld"},	/* distance */

	/* sensors */
	{8,	16,	3,	"%3d"},		/* see */
	{8,	20,	3,	"%3d"},		/* bad */

	/* homing */
	{11,	4,	7,	"%7ld"},
	{12,	4,	7,	"%7ld"},
	{13,	4,	7,	"%7ld"},
	{11,	16,	7,	"%7ld"},
	{12,	16,	7,	"%7ld"},
	{13,	16,	7,	"%7ld"},

	/* radio */
	{15,	7,	40,	"%-40.40s"},

	/* time */
	{15,	48,	7,	"%7ld"},

	/* subsystems */
	{1,	56,	11,	"%2d:%-8.8s",	3,	MSYS},	/* name */
	{1,	68,	6,	"%6d",	3,	MSYS},	/* level */
	{1,	76,	3,	"%3d",	3,	MSYS},	/* %damage */

	/* report */
	{17,	41,	38,	"%-38.38s",	1,	6},

	/* commands */
	{16,	0,	1,	"%1.1s",	2,	7}, /* last indicator */
	{16,	1,	40,	"%-40.40s",	2,	7}, /* command */

	/* background */
	{0,	0,	0,	"%s"}
}
#endif
;

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
