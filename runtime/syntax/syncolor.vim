" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Apr 28

" This file sets up the default methods for highlighting.
" It is loaded from "synload.vim" and from Vim for ":syntax reset".
" Also used from init_highlight().

if !exists("syntax_cmd") || syntax_cmd == "on"
  " ":syntax on" works like in Vim 5.7: set colors but keep links
  command -nargs=* SynColor hi <args>
  command -nargs=* SynLink hi link <args>
else
  if syntax_cmd == "enable"
    " ":syntax enable" keeps any existing colors
    command -nargs=* SynColor hi def <args>
    command -nargs=* SynLink hi def link <args>
  else
    " ":syntax reset" resets all colors to the default
    command -nargs=* SynColor hi <args>
    command -nargs=* SynLink hi! link <args>
  endif
endif

" Many terminals can only use six different colors (plus black and white).
" Therefore the number of colors used is kept low. It doesn't look nice with
" too many colors anyway.
" Careful with "cterm=bold", it changes the color to bright for some terminals.
" There are two sets of defaults: for a dark and a light background.
if &background == "dark"
  SynColor Comment	term=bold ctermfg=Cyan guifg=#80a0ff
  SynColor Constant	term=underline ctermfg=Magenta guifg=#ffa0a0
  SynColor Special	term=bold ctermfg=LightRed guifg=Orange
  SynColor Identifier	term=underline cterm=bold ctermfg=Cyan guifg=#40ffff
  SynColor Statement	term=bold ctermfg=Yellow guifg=#ffff60 gui=bold
  SynColor PreProc	term=underline ctermfg=LightBlue guifg=#ff80ff
  SynColor Type		term=underline ctermfg=LightGreen guifg=#60ff60 gui=bold
  SynColor Ignore	ctermfg=black guifg=bg
else
  SynColor Comment	term=bold ctermfg=DarkBlue guifg=Blue
  SynColor Constant	term=underline ctermfg=DarkRed guifg=Magenta
  SynColor Special	term=bold ctermfg=DarkMagenta guifg=SlateBlue
  SynColor Identifier	term=underline ctermfg=DarkCyan guifg=DarkCyan
  SynColor Statement	term=bold ctermfg=Brown gui=bold guifg=Brown
  SynColor PreProc	term=underline ctermfg=DarkMagenta guifg=Purple
  SynColor Type		term=underline ctermfg=DarkGreen guifg=SeaGreen gui=bold
  SynColor Ignore	ctermfg=white guifg=bg
endif
SynColor Error		term=reverse ctermbg=Red ctermfg=White guibg=Red guifg=White
SynColor Todo		term=standout ctermbg=Yellow ctermfg=Black guifg=Blue guibg=Yellow

" Common groups that link to default highlighting.
" You can specify other highlighting easily.
SynLink String		Constant
SynLink Character	Constant
SynLink Number		Constant
SynLink Boolean		Constant
SynLink Float		Number
SynLink Function	Identifier
SynLink Conditional	Statement
SynLink Repeat		Statement
SynLink Label		Statement
SynLink Operator	Statement
SynLink Keyword		Statement
SynLink Exception	Statement
SynLink Include		PreProc
SynLink Define		PreProc
SynLink Macro		PreProc
SynLink PreCondit	PreProc
SynLink StorageClass	Type
SynLink Structure	Type
SynLink Typedef		Type
SynLink Tag		Special
SynLink SpecialChar	Special
SynLink Delimiter	Special
SynLink SpecialComment	Special
SynLink Debug		Special

delcommand SynColor
delcommand SynLink
