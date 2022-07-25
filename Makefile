#
#  Spacewar - makefile
# 
#  Copyright 1984 obo Systems, Inc.
#  Copyright 1984 Dan Rosenblatt
#
LDFLAGS= -n
CFLAGS= -O -DBSD -DVOID=void -m68020 -f68881 -DNEEDFMOD
LPR= lpr

INCLUDES= aln.h build.h crft.h flds.h login.h mlbx.h obj.h plyr.h \
	spacewar.h sys.h torp.h ucmd.h uio.h uio2.h universe.h
SOURCES= sw.c globals.c lckmsg.c proctrap.c shutdown.c \
	alninit.c objinit.c crftupdate.c objupdate.c \
	logon.c logoff.c \
	cmd.c prvcmd.c \
	build.c mail.c see.c usrcmd.c who.c \
	play.c plinit.c unplay.c \
	update.c \
	updaln.c upddmg.c upddsh.c updmov.c updobjs.c updsys.c updtorp.c \
	bfuncs.c bits.c damage.c fixdir.c mutils.c vdisp.c remove.c rpt.c \
	output.c scrn.c
OBJECTS= sw.o globals.o lckmsg.o proctrap.o shutdown.o \
	alninit.o objinit.o crftupdate.o objupdate.o \
	logon.o logoff.o \
	cmd.o prvcmd.o \
	build.o mail.o see.o usrcmd.o who.o \
	play.o plinit.o unplay.o \
	update.o \
	updaln.o upddmg.o upddsh.o updmov.o updobjs.o updsys.o updtorp.o \
	bfuncs.o bits.o damage.o fixdir.o mutils.o vdisp.o remove.o rpt.o \
	output.o scrn.o

all : sw psw rsw dmpdbm upddbm tstsz

.PRECIOUS: print send

print : Makefile ${INCLUDES} ${SOURCES} psw.c psw.sh vmspsw.c rsw.c vmsrsw.c \
	tget.c dmpdbm.c upddbm.c tstsz.c swobj.init dbm.h dbm.c \
	vmsdelmbx.c
	pr $? | ${LPR}
	touch print

send : Makefile ${INCLUDES} ${SOURCES} psw.c psw.sh vmspsw.c rsw.c vmsrsw.c \
	tget.c dmpdbm.c upddbm.c tstsz.c swobj.init doc dbm.h dbm.c \
	vmsdelmbx.c swnews
	uucp $? 'obo586!~/sw/'
	touch send

clean :
	rm *.o sw psw rsw dmpdbm upddbm tstsz

sw : ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} -o sw ${OBJECTS} -ltermlib -ldbm -lm
	chmod u+s sw

psw : psw.c spacewar.h uio2.h
	${CC} ${CFLAGS} ${LDFLAGS} psw.c -o psw
	chmod u+s psw

rsw : rsw.c login.h uio.h universe.h spacewar.h
	${CC} ${CFLAGS} ${LDFLAGS} rsw.c -o rsw

dmpdbm : dmpdbm.c spacewar.h universe.h login.h sys.h crft.h mlbx.h \
	plyr.h ucmd.h bfuncs.o
	${CC} ${CFLAGS} ${LDFLAGS} dmpdbm.c bfuncs.o -ldbm -o dmpdbm

upddbm : upddbm.c spacewar.h universe.h login.h sys.h crft.h mlbx.h \
	plyr.h ucmd.h bfuncs.o
	${CC} ${CFLAGS} ${LDFLAGS} upddbm.c bfuncs.o -ldbm -o upddbm

tstsz : tstsz.c uio.h uio2.h spacewar.h
	${CC} ${CFLAGS} ${LDFLAGS} tstsz.c -o tstsz
	tstsz

lint : ${SOURCES} psw.c rsw.c dmpdbm.c upddbm.c tstsz.c
	lint ${CFLAGS} ${SOURCES}
	lint ${CFLAGS} psw.c
	lint ${CFLAGS} rsw.c
	lint ${CFLAGS} dmpdbm.c bfuncs.c
	lint ${CFLAGS} upddbm.c bfuncs.c
	lint ${CFLAGS} tstsz.c

bfuncs.o : spacewar.h

bits.o : spacewar.h

sw.o : spacewar.h

proctrap.o : spacewar.h universe.h login.h uio2.h

globals.o : universe.h torp.h sys.h obj.h login.h crft.h aln.h spacewar.h

logon.o : spacewar.h universe.h login.h

logoff.o : universe.h login.h spacewar.h

output.o : spacewar.h universe.h login.h flds.h sys.h

cmd.o : spacewar.h universe.h login.h uio.h plyr.h mlbx.h uio2.h

shutdown.o : spacewar.h universe.h login.h

mail.o : spacewar.h universe.h login.h mlbx.h plyr.h

build.o : universe.h login.h build.h sys.h crft.h obj.h spacewar.h

see.o : universe.h login.h sys.h crft.h plyr.h spacewar.h

usrcmd.o : spacewar.h universe.h login.h ucmd.h

objinit.o : spacewar.h universe.h obj.h

objupdate.o : spacewar.h universe.h obj.h

alninit.o : spacewar.h universe.h sys.h aln.h obj.h build.h

prvcmd.o : spacewar.h universe.h login.h sys.h aln.h build.h obj.h crft.h torp.h

who.o : spacewar.h universe.h login.h

play.o : spacewar.h universe.h login.h sys.h crft.h flds.h build.h aln.h obj.h \
	torp.h ucmd.h

plinit.o : spacewar.h universe.h login.h sys.h crft.h obj.h aln.h torp.h build.h

updobjs.o : spacewar.h universe.h obj.h

damage.o : spacewar.h universe.h sys.h login.h crft.h aln.h flds.h build.h \
	obj.h torp.h

lckmsg.o : spacewar.h

fixdir.o : spacewar.h universe.h sys.h crft.h flds.h

crftupdate.o : universe.h login.h sys.h crft.h spacewar.h

unplay.o : spacewar.h universe.h plyr.h login.h crft.h sys.h

rpt.o : universe.h login.h sys.h crft.h flds.h spacewar.h

remove.o : spacewar.h sys.h login.h crft.h obj.h torp.h universe.h flds.h aln.h

scrn.o : universe.h login.h flds.h spacewar.h sys.h crft.h build.h aln.h obj.h \
	torp.h

update.o : spacewar.h universe.h login.h sys.h aln.h build.h flds.h torp.h \
	crft.h

upddmg.o : spacewar.h universe.h sys.h login.h crft.h build.h aln.h flds.h \
	obj.h torp.h

updsys.o : spacewar.h universe.h login.h sys.h obj.h aln.h crft.h build.h \
	flds.h torp.h

upddsh.o : spacewar.h universe.h login.h sys.h aln.h crft.h flds.h obj.h torp.h

updmov.o : spacewar.h universe.h sys.h login.h obj.h crft.h aln.h torp.h \
	build.h flds.h

updtorp.o : spacewar.h universe.h sys.h login.h crft.h aln.h torp.h obj.h

updaln.o : spacewar.h universe.h sys.h aln.h build.h login.h crft.h torp.h \
	obj.h flds.h

vdisp.o : spacewar.h universe.h login.h sys.h aln.h crft.h obj.h torp.h
