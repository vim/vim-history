" Vim syntax file
" Language:	abc music notation language
" Maintainer:	James Allwright <J.R.Allwright@westminster.ac.uk>
" URL:		http://perun.hscs.wmin.ac.uk/vim/syntax/abc.vim
" Last Change:	26th May 1999

" Remove any old syntax stuff hanging around
syn clear


" tags
syn region abcGuitarChord start=+"[A-G]+ end=+"+ contained
syn match abcNote "z[1-9]*[0-9]*" contained
syn match abcNote "z[1-9]*[0-9]*/[248]\=" contained
syn match abcNote "[=_\^]\{,2}[A-G],*[1-9]*[0-9]*" contained
syn match abcNote "[=_\^]\{,2}[A-G],*[1-9]*[0-9]*/[248]\=" contained
syn match abcNote "[=_\^]\{,2}[a-g]'*[1-9]*[0-9]*"  contained
syn match abcNote "[=_\^]\{,2}[a-g]'*[1-9]*[0-9]*/[248]\="  contained
syn match abcBar "|"  contained
syn match abcBar "[:|][:|]"  contained
syn match abcBar ":|2"  contained
syn match abcBar "|1"  contained
syn match abcBar "\[[12]"  contained
syn match abcTuple "([1-9]\+:\=[0-9]*:\=[0-9]*" contained
syn match abcBroken "<\|<<\|<<<\|>\|>>\|>>>" contained
syn match abcTie    "-"
syn match abcHeadField "^[A-EGHIK-TVWXZ]:.*$" contained
syn match abcBodyField "^[KLMPQWVw]:.*$" contained
syn region abcHeader start="^X:" end="^K:.*$" contained contains=abcHeadField,abcComment keepend
syn region abcTune start="^X:" end="^ *$" contains=abcHeader,abcComment,abcBar,abcNote,abcBodyField,abcGuitarChord,abcTuple,abcBroken,abcTie
syn match abcComment "%.*$"

" The default highlighting.
hi def link abcComment		Comment
hi def link abcHeadField	Type
hi def link abcBodyField	Special
hi def link abcBar			Statement
hi def link abcTuple		Statement
hi def link abcBroken		Statement
hi def link abcTie			Statement
hi def link abcGuitarChord	Identifier
hi def link abcNote			Constant

let b:current_syntax = "abc"

" vim: ts=4
