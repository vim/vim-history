" Vim compiler file
" Compiler:	Miscrosoft Visual C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Jun 10

if exists("did_load_compiler")
  finish
endif
let did_load_compiler = 1

" The errorformat for MSVC is the default.
set errorformat&
set makeprg=nmake
