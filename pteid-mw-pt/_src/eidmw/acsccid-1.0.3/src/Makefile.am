CCID_BUNDLE = $(bundle)
CCID_LIB = libacsccid.$(DYN_LIB_EXT)

CCID_VERSION=CCID_VERSION=`$(srcdir)/convert_version.pl $(PACKAGE_VERSION)`

COMMON = ccid.c \
	ccid.h \
	ccid_ifdhandler.h \
	commands.c \
	commands.h \
	acr38cmd.c \
	acr38cmd.h \
	debug.h \
	defs.h \
	ifdhandler.c \
	utils.c \
	utils.h
USB = ccid_usb.c ccid_usb.h
T1 = towitoko/atr.c \
	towitoko/atr.h \
	towitoko/defines.h \
	towitoko/pps.c \
	towitoko/pps.h \
	openct/buffer.c \
	openct/buffer.h \
	openct/checksum.c \
	openct/checksum.h \
	openct/proto-t1.c \
	openct/proto-t1.h
TOKEN_PARSER = tokenparser.l parser.h \
	strlcpy.c \
	misc.h \
	strlcpycat.h

if WITHOUT_PCSC
PROVIDED_BY_PCSC = debug.c
endif

lib_LTLIBRARIES = libacsccid.la
libacsccid_la_SOURCES = $(COMMON) $(USB) $(TOKEN_PARSER) $(PROVIDED_BY_PCSC) $(T1)
libacsccid_la_LIBADD = $(LEXLIB) $(LIBUSB_LIBS) $(PTHREAD_LIBS)
libacsccid_la_CFLAGS = $(PCSC_CFLAGS) $(LIBUSB_CFLAGS) $(PTHREAD_CFLAGS) \
	$(SYMBOL_VISIBILITY) -D$(CCID_VERSION)

EXTRA_DIST = Info.plist.src create_Info_plist.pl supported_readers.txt \
	towitoko/COPYING towitoko/README openct/LICENSE \
	convert_version.pl pcscd_acsccid.rules
	
if UDEV
ifdCapabilities=0x00000001
INSTALL_UDEV_RULE_FILE=@echo -e "\n\33[01;31m***************\n" ; echo "copy the src/pcscd_acsccid.rules file in udev directory (/etc/udev/rules.d/)" ; echo -e "\n***************\n\33[0m"
else
ifdCapabilities=0x00000000
endif

Info.plist: Info.plist.src $(srcdir)/supported_readers.txt
	$(srcdir)/create_Info_plist.pl $(srcdir)/supported_readers.txt $(srcdir)/Info.plist.src --ifdCapabilities=$(ifdCapabilities) --target=$(CCID_LIB) --version=$(VERSION) --bundle=$(CCID_BUNDLE) $(NOCLASS) > Info.plist

DISTCLEANFILES = tokenparser.c Info.plist

install: libacsccid.la Info.plist
	$(mkinstalldirs) $(DESTDIR)$(usbdropdir)/$(CCID_BUNDLE)/Contents/$(BUNDLE_HOST)/
	cp Info.plist $(DESTDIR)$(usbdropdir)/$(CCID_BUNDLE)/Contents/
	cp .libs/$(CCID_LIB) $(DESTDIR)$(usbdropdir)/$(CCID_BUNDLE)/Contents/$(BUNDLE_HOST)/$(CCID_LIB).$(VERSION)
	ln -fs $(CCID_LIB).$(VERSION) $(DESTDIR)$(usbdropdir)/$(CCID_BUNDLE)/Contents/$(BUNDLE_HOST)/$(CCID_LIB)
	$(INSTALL_UDEV_RULE_FILE)

uninstall:
	rm -rf $(DESTDIR)$(usbdropdir)/$(CCID_BUNDLE)
