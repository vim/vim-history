# This is just a stub for the Unix Makefile, to provide support for doing
# "make install" in the top Vim directory.
#
# NOTE: If this doesn't work properly, do "cd src" first, and then type
# "make".  Noticed on AIX systems: Trying to run "cproto" or something else
# after Vim has been compiled.  Don't know why...

all install uninstall tools config configure proto depend lint tags types test testclean clean distclean:
	cd src && $(MAKE) $@
