#!/bin/sh
# FILE: "/disk01/home/joze/FIND.BINARY.sh"
# LAST MODIFICATION: "Tue Dec 21 14:19:37 1999 (joze)"
# (C) 1999 by Johannes Zellner, <johannes@zellner.org>
# vim:set sts=4 ts=4 sw=4:
# $Id$

find . -type f -print \
	| egrep '\.(bmp|dll|exe|gif|ico|info|rsrc|tlb)$' > BINARY.files

find . -name 'far-a01.*' -print >> BINARY.files
