" Vim Compiler File
" Compiler:	Jikes
" Maintainer:	Dan Sharp <vimuser@crosswinds.net>
" Last Change:	Thu, 26 Jul 2001 11:17:16 Eastern Daylight Time

if exists("current_compiler")
  finish
endif
let current_compiler = "jikes"

" Jikes defaults to printing output on stderr
setlocal makeprg=jikes\ -Xstdout\ +E
setlocal errorformat=%f:%l:%v:%*\\d:%*\\d:%*\\s%m
