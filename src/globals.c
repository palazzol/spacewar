/*
 * Spacewar - file that declares all global lists
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include <sys/types.h>
#include "universe.h"
#include "torp.h"
#include "sys.h"
#include "obj.h"
#include "login.h"
#include "crft.h"
#include "aln.h"

struct aln alnlst[MAXALN];
struct crft crftlst[MAXCRFT];
struct login loginlst[MAXLOGIN];
struct obj objlst[MAXOBJ];
struct torp torplst[MAXTORP];
struct universe univlst[MAXUNIVERSE];
