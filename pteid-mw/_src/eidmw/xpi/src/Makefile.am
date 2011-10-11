XPI_NAME=portugaleid.xpi

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

dist_content_DATA = content/portugaleidpkcs11.js \
			content/portugaleidpkcs11.xul \
			content/portugaleidprompt.js \
			content/portugaleidprompt.xul \
			content/portugaleidpromptsecure.js \
			content/portugaleidpromptsecure.xul

dist_defaults_preferences_DATA = defaults/preferences/portugaleid.js

dist_locale_enus_DATA = locale/en-US/portugaleid.properties

dist_locale_frfr_DATA = locale/fr-FR/portugaleid.properties

dist_locale_nlnl_DATA = locale/nl-NL/portugaleid.properties

dist_locale_dede_DATA = locale/de-DE/portugaleid.properties

dist_skin_DATA = skin/icon.png \
			skin/icon40x40.png \
			skin/overlay.css
			
dist_components_DATA = components/portugaleidpkcs11.js
			
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

