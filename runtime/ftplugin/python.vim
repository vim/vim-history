" Vim filetype plugin file
" Language:	python
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Don, 25 Jan 2001 20:10:04 +0100

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

setlocal cinkeys-=0#
setlocal indentkeys-=0#
setlocal include=\s*\\(from\\\|import\\)
setlocal suffixesadd=.py
