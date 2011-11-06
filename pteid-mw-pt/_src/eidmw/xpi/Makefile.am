ACLOCAL_AMFLAGS         = -I m4

SUBDIRS = src

export XPI_ID = portugaleid@eid.portugal.be
export mozext
export TMPDIR ?= /tmp
export LINUX
export WIN32
export OSX

xpipackage:
	$(MAKE) -C src xpipackage
