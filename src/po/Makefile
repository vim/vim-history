# Makefile for the Vim message translations.

# TODO make this configurable
# Note: ja.sjis is only for MS-Windows, it's not installed on Unix
LANGUAGES = af cs de es fr it ja ko pl sk tr uk zh_TW
MOFILES = af.mo cs.mo de.mo es.mo fr.mo it.mo ja.mo \
		ko.mo pl.mo sk.mo tr.mo uk.mo zh_TW.mo

PACKAGE = vim
SHELL = /bin/sh

# The OLD_PO_FILE_INPUT and OLD_PO_FILE_OUTPUT are for the new GNU gettext
# tools 0.10.37, which use a slightly different .po file format that is not
# compatible with Solaris (and old gettext implementations) unless these are
# set.  gettext 0.10.36 will not work!
MSGFMT = OLD_PO_FILE_INPUT=yes msgfmt
XGETTEXT = OLD_PO_FILE_INPUT=yes OLD_PO_FILE_OUTPUT=yes xgettext
MSGMERGE = OLD_PO_FILE_INPUT=yes OLD_PO_FILE_OUTPUT=yes msgmerge

.SUFFIXES:
.SUFFIXES: .po .mo .pot
.PHONY: all install uninstall check clean distclean $(LANGUAGES)

.po.mo:
	$(MSGFMT) -o $@ $<

all: $(MOFILES)

install: $(MOFILES)
	@$(MAKE) check
	for lang in $(LANGUAGES); do \
	  dir=$(LOCALEDIR)/$$lang/; \
	  if test ! -x "$$dir"; then \
	    mkdir $$dir; chmod 755 $$dir; \
	  fi; \
	  dir=$(LOCALEDIR)/$$lang/LC_MESSAGES; \
	  if test ! -x "$$dir"; then \
	    mkdir $$dir; chmod 755 $$dir; \
	  fi; \
	  if test -r $$lang.mo; then \
	    $(INSTALL_DATA) $$lang.mo $$dir/$(PACKAGE).mo; \
	    chmod $(FILEMOD) $$dir/$(PACKAGE).mo; \
	  fi; \
	done

uninstall:
	@$(MAKE) check
	for cat in $(MOFILES); do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(LOCALEDIR)/$$lang/LC_MESSAGES/$(PACKAGE).mo; \
	done

# Convert ja.po to create ja.sjis.po.  Requires doubling backslashes in the
# second byte.
ja.sjis.po: ja.po sjiscorr
	iconv -f euc-jp -t cp932 ja.po | ./sjiscorr > ja.sjis.po

sjiscorr: sjiscorr.c
	$(CC) -o sjiscorr sjiscorr.c

check:
	@if test "x" = "x$(prefix)"; then \
	  echo "******************************************"; \
	  echo "  please use make from the src directory  "; \
	  echo "******************************************"; \
	  exit 1; \
	fi

clean:
	rm -f core core.* *.old.po *.mo *.pot sjiscorr ja.sjis.po

distclean: clean

#
# NOTE: If you get an error for gvimext.cpp not found, you need to unpack the
# extra archive.
#
$(PACKAGE).pot: ../*.c ../../GvimExt/gvimext.cpp ../globals.h
	cd ..; $(XGETTEXT) --default-domain=$(PACKAGE) \
		--add-comments --keyword=_ --keyword=N_ \
		*.c ../GvimExt/gvimext.cpp globals.h
	mv -f ../$(PACKAGE).po $(PACKAGE).pot

# Don't add a dependency here, we only want to update the .po files manually
$(LANGUAGES):
	@$(MAKE) $(PACKAGE).pot
	if test ! -f $@.po.orig; then cp $@.po $@.po.orig; fi
	mv $@.po $@.po.old
	if $(MSGMERGE) $@.po.old $(PACKAGE).pot -o $@.po; then \
	    rm -f $@.po.old; \
	else \
	    echo "msgmerge for $@.po failed!"; mv $@.po.old $@.po; \
	fi
