" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1997 Dec 16

" This is the cleanup file for syntax highlighting.
" It removes the autocommands and stops highlighting for all buffers.

if has("syntax")
  augroup syntax
    au!
    au BufEnter * syn clear
    au BufEnter * if exists("b:current_syntax") | unlet b:current_syntax | endif
    doautoall syntax BufEnter *
    au!
  augroup END
  if exists("syntax_on")
    unlet syntax_on
  endif
endif

" vim: ts=8
