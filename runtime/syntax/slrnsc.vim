" Vim syntax file
" Language:	Slrn score file
" Maintainer:	Preben "Peppe" Guldberg (c928400@student.dtu.dk)
" Last Change:	Thu Apr  2 14:02:43 1998

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match slrnscComment		"%.*$"
syn match slrnscSectionCom	".].*"lc=2

" characters in newsgroup names
setlocal isk=@,48-57,.,-,_,+

syn match slrnscGroup		contained "\(\k\|\*\)\+"
syn match slrnscNumber		contained "\d\+"
syn match slrnscDate		contained "\(\d\{1,2}[-/]\)\{2}\d\{4}"
syn match slrnscDelim		contained ":"
syn match slrnscComma		contained ","
syn match slrnscOper		contained "\~"
syn match slrnscEsc		contained "\\[ecC<>.]"
syn match slrnscEsc		contained "[?^]"
syn match slrnscEsc		contained "[^\\]$\s*$"lc=1

syn region slrnscSection	matchgroup=slrnscSectionStd start="^\s*\[" end='\]' contains=slrnscGroup,slrnscComma,slrnscSectionCom
syn region slrnscSection	matchgroup=slrnscSectionNot start="^\s*\[\~" end='\]' contains=slrnscGroup,slrnscCommas,slrnscSectionCom

syn keyword slrnscItem		contained Expires From Lines References Subject Xref

syn match slrnscItemFill	contained ".*$" skipempty nextgroup=slrnscScoreItem contains=slrnscEsc

syn match slrnscScoreItem	contained "^\s*Expires:\s*\(\d\{1,2}[-/]\)\{2}\d\{4}\s*$" skipempty nextgroup=slrnscScoreItem contains=slrnscItem,slrnscDelim,slrnscDate
syn match slrnscScoreItem	contained "^\s*\~\=Lines:\s*\d\+\s*$" skipempty nextgroup=slrnscScoreItem contains=slrnscOper,slrnscItem,slrnscDelim,slrnscNumber
syn match slrnscScoreItem	contained "^\s*\~\=\(From\|References\|Subject\|Xref\):" nextgroup=slrnscItemFill contains=slrnscOper,slrnscItem,slrnscDelim
syn match slrnscScoreItem	contained "^\s*%.*$" skipempty nextgroup=slrnscScoreItem contains=slrnscComment

syn keyword slrnscScore		contained Score
syn match slrnScoreLine		"^\s*Score::\=\s\+=\=-\=\d\+\s*$" skipempty nextgroup=slrnscScoreItem contains=slrnscScore,slrnscDelim,slrnscOper,slrnscNumber

" The default highlighting.
hi def link slrnscComment	Comment
hi def link slrnscSectionCom	slrnscComment
hi def link slrnscGroup		String
hi def link slrnscNumber	Number
hi def link slrnscDate		Special
hi def link slrnscDelim		Delimiter
hi def link slrnscComma		SpecialChar
hi def link slrnscOper		SpecialChar
hi def link slrnscEsc		String
hi def link slrnscSectionStd	Type
hi def link slrnscSectionNot	Delimiter
hi def link slrnscItem		Statement
hi def link slrnscScore		Keyword

let b:current_syntax = "slrnsc"

"EOF	vim: ts=8 noet tw=200 sw=8 sts=0
