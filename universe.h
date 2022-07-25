/*
 * Spacewar - include file that defines objects/parameters of the universe
 *
 * ordering: objects, aliens, crafts&torpedoes (because they can come and go)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

struct universe {
	char	uv_type;	/* universe type of object: NULL 'A|P|O|T' */
	char	uv_pctr;	/* universe screen character of object */
	double	*uv_pstn;	/* universe position of object */
	long	uv_mass;	/* universe mass/hull damage of object */
	short	uv_rad;		/* universe radius of object */
	union {
		struct aln *uv_aln;
		struct crft *uv_crft;
		struct obj *uv_obj;
		struct torp *uv_torp;
	} uv_ptr;
	dsplcmnt *uv_dspl;
};

#define MAXUNIVERSE	MAXALN+MAXCRFT+MAXOBJ+MAXTORP

extern struct universe univlst[];

typedef union {
	struct universe *ip_ptr;
	int		 ip_ofst;
} idxptr;
