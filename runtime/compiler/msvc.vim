" Vim compiler file
" Compiler:	Miscrosoft Visual C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Jun 16

if exists("current_compiler")
  finish
endif
let current_compiler = "msvc"

" The errorformat for MSVC is the default.
set errorformat&
set makeprg=nmake
