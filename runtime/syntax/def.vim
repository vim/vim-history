" Vim syntax file
" Language:	Microsoft Module-Definition (.def) File
" Maintainer:	Rob Brady <robb@datatone.com>
" Last Change:	$Date$
" URL: http://www.datatone.com/~robb/vim/syntax/def.vim
" $Revision$

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore

syn match defComment	";.*"

syn keyword defKeyword	LIBRARY STUB EXETYPE DESCRIPTION CODE WINDOWS DOS
syn keyword defKeyword	RESIDENTNAME PRIVATE EXPORTS IMPORTS SEGMENTS
syn keyword defKeyword	HEAPSIZE DATA
syn keyword defStorage	LOADONCALL MOVEABLE DISCARDABLE SINGLE
syn keyword defStorage	FIXED PRELOAD

syn match   defOrdinal	"@\d\+"

syn region  defString	start=+'+ end=+'+

syn match   defNumber	"\d+"
syn match   defNumber	"0x\x\+"

" The default highlighting.
hi def link defComment	Comment
hi def link defKeyword	Keyword
hi def link defStorage	StorageClass
hi def link defString	String
hi def link defNumber	Number
hi def link defOrdinal	Operator

let b:current_syntax = "def"

" vim: ts=8
