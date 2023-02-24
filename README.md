
# SPACEWAR

## A multi-user UNIX game, from 1988

### Original Documentation is here:
[README](README) - some installation notes

[doc.pdf](doc.pdf) - nice user manual

### Linux Porting notes - WIP 2/2023

#### Current Status
- "I just got this to run, but I don't know if it works yet - Frank"
- Built as 32-bit executable, because there are places where pointer/integer sizes matter
- Compiles/links with gcc-11 on Ubuntu
- Fixed all warnings by adding #includes, prototypes (in spacewar.h)
- Added Makefile target to build original documentation
- Added temporary Makefile target to do some install stuff for debugging - WIP
- fix varargs stuff
- remove VMS code for clarity
- remove -DVOID stuff
- cleanup almost all the #ifdef DEBUG statements

#### TBD 
- playtesting
- fixup hardcoded user stuff for install
- review termcap stuff
- fix time_t stuff to be 2038 compliant
- do proper make install target





