" local syntax file - set colors on a per-machine basis:
" vim: tw=0 ts=4 sw=4
" Vim color file
" Maintainer:	Steven Vertigan <steven@vertigan.wattle.id.au>
" Last Change:	2001 Sep 9

set background=dark
hi clear
if exists("syntax_on")
  syntax reset
endif
let g:colors_name = "blue"
hi Normal		guifg=white	guibg=darkblue
hi NonText		guifg=#cc33cc
hi comment		guifg=gray		gui=bold
hi constant		guifg=cyan
hi identifier	guifg=gray
hi statement	guifg=yellow	gui=none
hi preproc		guifg=green
hi type			guifg=orange
hi special		guifg=#cc33cc
hi ErrorMsg		guifg=orange	guibg=darkblue
hi WarningMsg	guifg=cyan		guibg=darkblue	gui=bold
hi ModeMsg		guifg=yellow	gui=NONE
hi MoreMsg		guifg=yellow	gui=NONE
hi Error		guifg=Red		guibg=darkblue
hi Todo			guifg=Black		guibg=orange
hi Cursor		guifg=Black		guibg=white
hi Search		guifg=Black		guibg=orange
hi IncSearch	guifg=Black		guibg=steelblue	gui=NONE
hi LineNr		guifg=pink
hi title		guifg=white	gui=bold
hi StatusLineNC	gui=NONE	guifg=black guibg=blue
hi StatusLine	gui=bold	guifg=cyan	guibg=blue
hi label		guifg=gold2
hi operator		guifg=orange	gui=bold
hi clear Visual
hi Visual		term=reverse cterm=reverse gui=reverse
hi DiffChange   guibg=darkgreen	guifg=black
hi DiffText		guibg=olivedrab	guifg=black
hi DiffAdd		guibg=slateblue	guifg=black
hi DiffDelete   guibg=coral		guifg=black
hi Folded		guibg=orange guifg=black
hi FoldColumn	guibg=gray30 guifg=black
hi cIf0			guifg=gray
