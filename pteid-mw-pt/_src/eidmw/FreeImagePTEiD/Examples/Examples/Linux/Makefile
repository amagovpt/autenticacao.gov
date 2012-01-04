CC = gcc
CPP = g++
COMPILERFLAGS = -O3
INCLUDE = -I../../Dist
VGALIBRARIES = -lfreeimagePTEiD
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)

all: default

default: linux-gtk

linux-gtk: linux-gtk.c
	$(CC) $(CFLAGS) $< -o $@ $(VGALIBRARIES)
	strip $@

clean:
	rm -f core linux-gtk
