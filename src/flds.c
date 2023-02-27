/*
 * Spacewar - relocated from flds.h to here
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "sys.h"
#include "flds.h"

struct flddesc flds[] = {

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
};
