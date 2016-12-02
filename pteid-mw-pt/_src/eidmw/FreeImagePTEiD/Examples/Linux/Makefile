CC = gcc
CPP = g++
COMPILERFLAGS = -O0 -g
INCLUDE = -I../../Dist
VGALIBRARIES = -lfreeimagePTEiD
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)

all: default

default: linux-gtk

linux-gtk: linux-gtk.c
	$(CC) $(CFLAGS) $< -o $@ $(VGALIBRARIES)

clean:
	rm -f core linux-gtk
