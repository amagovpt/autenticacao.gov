MAKEFILE = Makefile
 
ATPROJ_SRCDIR = ../acsccid-1.0.3
 
autoreconf.target = $${ATPROJ_SRCDIR}/configure
autoreconf.commands = cd $${ATPROJ_SRCDIR} && autoreconf
 
aclocal.target = $${ATPROJ_SRCDIR}/aclocal.m4
aclocal.depends = autoreconf
 
automake.target = $${ATPROJ_SRCDIR}/Makefile.in
automake.depends = autoreconf
 
autoheader.target = $${ATPROJ_SRCDIR}/Makefile.in
autoheader.depends = autoreconf
 
libtoolize.target = $${ATPROJ_SRCDIR}/ltmain.sh
libtoolize.depends = autoreconf
 
Makefile.target = Makefile
Makefile.commands = $${ATPROJ_SRCDIR}/configure
Makefile.depends = autoreconf aclocal automake autoheader libtoolize
 
all.commands = make
all.depends = Makefile
all.CONFIG = phony
 
TARGET = \\\
 
QMAKE_DISTCLEAN += Makefile
QMAKE_EXTRA_TARGETS += autoreconf aclocal automake autoheader libtoolize Makefile all
