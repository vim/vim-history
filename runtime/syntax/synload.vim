" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Aug 12

" This file sets up for syntax highlighting.
" It is loaded from "syntax.vim" and "manual.vim".
" 1. Set the default highlight groups.
" 2. Install Syntax autocommands for all the available syntax files.

if !has("syntax")
  finish
endif

" let others know that syntax has been switched on
let syntax_on = 1

" The default methods for highlighting.  Can be overridden later.
" Many terminals can only use six different colors (plus black and white).
" Therefore the number of colors used is kept low. It doesn't look nice with
" too many colors anyway.
" Careful with "cterm=bold", it may change the color to bright.

" There are two sets of defaults: for a dark and a light background.
if &background == "dark"
  hi Comment	term=bold ctermfg=Cyan guifg=#80a0ff
  hi Constant	term=underline ctermfg=Magenta guifg=#ffa0a0
  hi Special	term=bold ctermfg=LightRed guifg=Orange
  hi Identifier term=underline cterm=bold ctermfg=Cyan guifg=#40ffff
  hi Statement	term=bold ctermfg=Yellow guifg=#ffff60 gui=bold
  hi PreProc	term=underline ctermfg=LightBlue guifg=#ff80ff
  hi Type	term=underline ctermfg=LightGreen guifg=#60ff60 gui=bold
  hi Ignore	ctermfg=black guifg=bg
else
  hi Comment	term=bold ctermfg=DarkBlue guifg=Blue
  hi Constant	term=underline ctermfg=DarkRed guifg=Magenta
  hi Special	term=bold ctermfg=DarkMagenta guifg=SlateBlue
  hi Identifier term=underline ctermfg=DarkCyan guifg=DarkCyan
  hi Statement	term=bold ctermfg=Brown gui=bold guifg=Brown
  hi PreProc	term=underline ctermfg=DarkMagenta guifg=Purple
  hi Type	term=underline ctermfg=DarkGreen guifg=SeaGreen gui=bold
  hi Ignore	ctermfg=white guifg=bg
endif
hi Error term=reverse ctermbg=Red ctermfg=White guibg=Red guifg=White
hi Todo	 term=standout ctermbg=Yellow ctermfg=Black guifg=Blue guibg=Yellow

" Common groups that link to default highlighting.
" You can specify other highlighting easily.
hi link String		Constant
hi link Character	Constant
hi link Number		Constant
hi link Boolean		Constant
hi link Float		Number
hi link Function	Identifier
hi link Conditional	Statement
hi link Repeat		Statement
hi link Label		Statement
hi link Operator	Statement
hi link Keyword		Statement
hi link Exception	Statement
hi link Include		PreProc
hi link Define		PreProc
hi link Macro		PreProc
hi link PreCondit	PreProc
hi link StorageClass	Type
hi link Structure	Type
hi link Typedef		Type
hi link Tag		Special
hi link SpecialChar	Special
hi link Delimiter	Special
hi link SpecialComment	Special
hi link Debug		Special


" First remove all old syntax autocommands.
au! Syntax

" OFF
au Syntax OFF		syn clear

" ON
au Syntax ON		if &filetype != "" | exe "set syntax=" . &filetype | else | echohl ErrorMsg | echo "filetype unknown" | echohl None | endif

" Load the main syntax file when the Syntax option is set.  Only load the
" first one found, because it will do a "syn clear".
" Also load the modifier files.  Load all that can be found.
au Syntax * if expand("<amatch>") != "" |
	\ runtime syntax/<amatch>.vim |
	\ runtime! syntax2/<amatch>.vim |
	\ endif


" Source the user-specified syntax highlighting file
if exists("mysyntaxfile") && filereadable(expand(mysyntaxfile))
  execute "source " . mysyntaxfile
endif
