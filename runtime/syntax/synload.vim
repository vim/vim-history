" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Mar 01

" This file sets up for syntax highlighting.
" It is loaded from "syntax.vim" and "manual.vim".
" 1. Set the default highlight groups.
" 2. Install Syntax autocommands for all the available syntax files.

if !has("syntax")
  finish
endif

" let others know that syntax has been switched on
let syntax_on = 1

" The default methods for highlighting.  Using "def" to avoid overruling
" highlighting that the user has already set.  Can also be overridden later.
" Many terminals can only use six different colors (plus black and white).
" Therefore the number of colors used is kept low. It doesn't look nice with
" too many colors anyway.
" Careful with "cterm=bold", it changes the color to bright for some terminals.

" There are two sets of defaults: for a dark and a light background.
if &background == "dark"
  hi def Comment	term=bold ctermfg=Cyan guifg=#80a0ff
  hi def Constant	term=underline ctermfg=Magenta guifg=#ffa0a0
  hi def Special	term=bold ctermfg=LightRed guifg=Orange
  hi def Identifier 	term=underline cterm=bold ctermfg=Cyan guifg=#40ffff
  hi def Statement	term=bold ctermfg=Yellow guifg=#ffff60 gui=bold
  hi def PreProc	term=underline ctermfg=LightBlue guifg=#ff80ff
  hi def Type		term=underline ctermfg=LightGreen guifg=#60ff60 gui=bold
  hi def Ignore		ctermfg=black guifg=bg
else
  hi def Comment	term=bold ctermfg=DarkBlue guifg=Blue
  hi def Constant	term=underline ctermfg=DarkRed guifg=Magenta
  hi def Special	term=bold ctermfg=DarkMagenta guifg=SlateBlue
  hi def Identifier	term=underline ctermfg=DarkCyan guifg=DarkCyan
  hi def Statement	term=bold ctermfg=Brown gui=bold guifg=Brown
  hi def PreProc	term=underline ctermfg=DarkMagenta guifg=Purple
  hi def Type		term=underline ctermfg=DarkGreen guifg=SeaGreen gui=bold
  hi def Ignore		ctermfg=white guifg=bg
endif
hi def Error 		term=reverse ctermbg=Red ctermfg=White guibg=Red guifg=White
hi def Todo	 	term=standout ctermbg=Yellow ctermfg=Black guifg=Blue guibg=Yellow

" Common groups that link to default highlighting.
" You can specify other highlighting easily.
hi def link String		Constant
hi def link Character		Constant
hi def link Number		Constant
hi def link Boolean		Constant
hi def link Float		Number
hi def link Function		Identifier
hi def link Conditional		Statement
hi def link Repeat		Statement
hi def link Label		Statement
hi def link Operator		Statement
hi def link Keyword		Statement
hi def link Exception		Statement
hi def link Include		PreProc
hi def link Define		PreProc
hi def link Macro		PreProc
hi def link PreCondit		PreProc
hi def link StorageClass	Type
hi def link Structure		Type
hi def link Typedef		Type
hi def link Tag			Special
hi def link SpecialChar		Special
hi def link Delimiter		Special
hi def link SpecialComment	Special
hi def link Debug		Special

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
