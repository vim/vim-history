" Vim syntax file
" Language:	Ada (95)
" Maintainer:	David A. Wheeler <wheeler@ida.org>
" Last Change:	2001 Jan 15

" Former Maintainer:	Simon Bradley <simon.bradley@pitechnology.com>
"			(was <sib93@aber.ac.uk>)
" This version fixes an error in String/Character handling from 1997-May-5.
" The formal spec of Ada95 is available at "http://www.adahome.com/rm95/".

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Ada is entirely case-insensitive.
syn case ignore

" The Ada 95 reserved words. There are 69 reserved words in total.
syn keyword adaConditional	abort case else elsif if select when

syn keyword adaKeyword		all do end exception in is new null out pragma
syn keyword adaKeyword		separate then until

syn keyword adaLabel		others

syn keyword adaOperator		abs and mod not or rem xor

syn keyword adaPreCondit	begin body declare entry function generic
syn keyword adaPreCondit	package procedure protected renames task use
syn keyword adaPreCondit	with

syn keyword adaRepeat		exit for loop reverse while

syn keyword adaStatement	accept delay goto raise requeue return
syn keyword adaStatement	terminate

syn keyword adaStructure	record

syn keyword adaType		abstract access aliased array at constant delta
syn keyword adaType		digits limited of private range tagged subtype
syn keyword adaType		type

" Todo.
syn keyword adaTodo contained	TODO FIXME XXX

" Strings and characters.
syn region  adaString		start=+"+  skip=+""+  end=+"+
syn match   adaCharacter	"'.'"

" Numbers.
syn match   adaNumber		"[+-]\=\<[0-9_]*\.\=[0-9_]*\>"

" Labels for the goto statement.
syn region  adaLabel		start="<<"  end=">>"

" Comments.
syn region  adaComment	oneline contains=adaTodo start="--"  end="$"

" The default highlighting.
hi def link adaCharacter	Character
hi def link adaComment		Comment
hi def link adaConditional	Conditional
hi def link adaKeyword		Keyword
hi def link adaLabel		Label
hi def link adaNumber		Number
hi def link adaOperator		Operator
hi def link adaPreCondit	PreCondit
hi def link adaRepeat		Repeat
hi def link adaSpecial		Special
hi def link adaStatement	Statement
hi def link adaString		String
hi def link adaStructure	Structure
hi def link adaTodo		Todo
hi def link adaType		Type

let b:current_syntax = "ada"

" vim: ts=8
