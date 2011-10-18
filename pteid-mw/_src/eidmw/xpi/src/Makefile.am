XPI_NAME=belgiumeid.xpi

CLEANFILES = $(XPI_NAME)

xpi_files = $(dist_content_DATA) \
			$(dist_defaults_preferences_DATA) \
			$(dist_locale_enus_DATA) \
			$(dist_locale_frfr_DATA) \
			$(dist_locale_nlnl_DATA) \
			$(dist_locale_dede_DATA) \
			$(dist_skin_DATA) \
			$(dist_components_DATA) \
			$(dist_extensionroot_DATA)

dist_extensionroot_DATA = chrome.manifest \
			icon.png \
			install.rdf

dist_content_DATA = content/belgiumeidpkcs11.js \
			content/belgiumeidpkcs11.xul \
			content/belgiumeidprompt.js \
			content/belgiumeidprompt.xul \
			content/belgiumeidpromptsecure.js \
			content/belgiumeidpromptsecure.xul

dist_defaults_preferences_DATA = defaults/preferences/belgiumeid.js

dist_locale_enus_DATA = locale/en-US/belgiumeid.properties

dist_locale_frfr_DATA = locale/fr-FR/belgiumeid.properties

dist_locale_nlnl_DATA = locale/nl-NL/belgiumeid.properties

dist_locale_dede_DATA = locale/de-DE/belgiumeid.properties

dist_skin_DATA = skin/icon.png \
			skin/icon40x40.png \
			skin/overlay.css
			
dist_components_DATA = components/belgiumeidpkcs11.js
			
extensionrootdir=$(mozext)/$(XPI_ID)/
contentdir=$(extensionrootdir)content
defaults_preferencesdir=$(extensionrootdir)defaults/preferences
locale_enusdir=$(extensionrootdir)locale/en-US
locale_frfrdir=$(extensionrootdir)locale/fr-FR
locale_nlnldir=$(extensionrootdir)locale/nl-NL
locale_dededir=$(extensionrootdir)locale/de-DE
skindir=$(extensionrootdir)skin
componentsdir=$(extensionrootdir)components

xpipackage: $(xpi_files)
#	 Create a temporary directory $tmp in $TMPDIR (default /tmp).
#	 Use mktemp if possible; otherwise fall back on mkdir,
#	 with $RANDOM to make collisions less likely.
	{ \
		tmp=` \
			(umask 077 && mktemp -t -d "xpiXXXXXX") 2>/dev/null \
		` && \
		test -n "$$tmp" && test -d "$$tmp"; \
	} || { \
		tmp=$$TMPDIR/xpi$$RANDOM; \
		umask 077 && $(mkinstalldirs) "$$tmp"; \
	} || exit $$?; \
	$(MAKE) DESTDIR=$$tmp extensionrootdir=/ install; \
	cd $$tmp; \
        rm -f $(abs_builddir)/$(XPI_NAME); \
	@ZIP@ -rq $(abs_builddir)/$(XPI_NAME) .; \
	cd -; \
	rm -rf $$tmp;

