# Makefile for the Vim message translations.

# TODO make this configurable
LANGUAGES = cs de fr ko pl
MOFILES = cs.mo de.mo fr.mo ko.mo pl.mo
POFILES = cs.po de.po fr.po ko.po pl.po

PACKAGE = vim
SHELL = /bin/sh

MSGFMT = msgfmt
XGETTEXT = xgettext
MSGMERGE = msgmerge

.SUFFIXES:
.SUFFIXES: .po .mo .pot
.PHONY: all install install-data clean distclean mostlyclean update-po check

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

check:
	@if test "x" = "x$(prefix)"; then \
	  echo "******************************************"; \
	  echo "  please use make from the src directory  "; \
	  echo "******************************************"; \
	  exit 1; \
	fi

clean:
	rm -f core core.* *.old.po *.mo *.pot

distclean: clean

$(PACKAGE).pot: ../*.c ../globals.h
	cd ..; $(XGETTEXT) --default-domain=$(PACKAGE) \
		--add-comments --keyword=_ --keyword=N_ *.c globals.h 
	mv -f ../$(PACKAGE).po $(PACKAGE).pot

# Don't add dependency here, we only want to update the .po files manually
$(POFILES):
	@$(MAKE) $(PACKAGE).pot
	if test ! -f $@.orig; then cp $@ $@.orig; fi
	mv $@ $@.old
	if $(MSGMERGE) $@.old $(PACKAGE).pot -o $@; then \
	    rm -f $@.old; \
	else \
	    echo "msgmerge for $@ failed!"; mv $@.old $@; \
	fi
