# This is just a stub for the Unix Makefile, to provide support for doing
# "make install" in the top Vim directory.

all install uninstall tools config configure proto depend lint tags types test testclean clean distclean:
	cd src && $(MAKE) $@
