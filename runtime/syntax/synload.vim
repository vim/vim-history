" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Mar 14

" This file sets up for syntax highlighting.
" It is loaded from "syntax.vim" and "manual.vim".
" 1. Set the default highlight groups.
" 2. Install Syntax autocommands for all the available syntax files.

if !has("syntax")
  finish
endif

" let others know that syntax has been switched on
let syntax_on = 1

" Set the default highlighting colors
source <sfile>:p:h/syncolor.vim

" Line continuation is used here, remove 'C' from 'cpoptions'
let s:cpo_save = &cpo
set cpo&vim

" First remove all old syntax autocommands.
au! Syntax

" :set syntax=OFF  and any syntax name that doesn't exist
au Syntax *		syn clear

" :set syntax=ON
au Syntax ON		if &filetype != "" | exe "set syntax=" . &filetype | else | echohl ErrorMsg | echo "filetype unknown" | echohl None | endif

" Load the syntax file(s) when the Syntax option is set.
if has("mac")
  au Syntax * if expand("<amatch>") != "" |
	\ syntax clear |
	\ if exists("b:current_syntax") | unlet b:current_syntax | endif |
	\ runtime! syntax:<amatch>.vim |
	\ endif
else
  au Syntax * if expand("<amatch>") != "" |
	\ syntax clear |
	\ if exists("b:current_syntax") | unlet b:current_syntax | endif |
	\ runtime! syntax/<amatch>.vim |
	\ endif
endif


" Source the user-specified syntax highlighting file
if exists("mysyntaxfile") && filereadable(expand(mysyntaxfile))
  execute "source " . mysyntaxfile
endif

" Restore 'cpoptions'
let &cpo = s:cpo_save
unlet s:cpo_save
