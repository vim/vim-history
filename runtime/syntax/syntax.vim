" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Jul 21

" This file is used for ":syntax on".
" It installs the autocommands and starts highlighting for all buffers.

if !has("syntax")
  finish
endif

" If Syntax highlighting appears to be on already, turn it off first, so that
" any leftovers are cleared.
if exists("syntax_on") || exists("syntax_manual")
  so <sfile>:p:h/nosyntax.vim
endif

" Load the Syntax autocommands and set the default methods for highlighting.
so <sfile>:p:h/synload.vim

" Load the FileType autocommands, in case that hasn't been done yet.
filetype on

" Set up the connection between FileType and Syntax autocommands.
" This makes the syntax automatically set when the file type is detected.
augroup syntaxset
au! FileType *		exe "set syntax=" . expand("<amatch>")
augroup END


" Execute the syntax autocommands for the each buffer.
doautoall filetypedetect BufRead
