" Vim syntax file
" Language:	Rexx
" Maintainer:	Thomas Geulig <geulig@nentec.de>
" Last Change:	1999 Jul 27

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

" A bunch of useful Rexx keywords
syn keyword rexxKeyword address arg begin break by call do drop echo else end
syn keyword rexxKeyword exit forever function if interpret iterate leave nop
syn keyword rexxKeyword module numeric off on options otherwise parse procedure
syn keyword rexxKeyword pull push queue return say select shell signal then to
syn keyword rexxKeyword trace until upper value var when while with

syn keyword rexxTodo contained	TODO FIXME XXX

" String constants
syn region rexxString		start=+"+ skip=+\\\\\|\\"+ end=+"+
syn region rexxString		start=+'+ skip=+\\\\\|\\"+ end=+'+
syn match rexxCharacter		"'[^\\]'"

"catch errors caused by wrong parenthesis
syn region rexxParen		transparent start='(' end=')' contains=ALLBUT,rexxParenError,rexxTodo,rexxUserLabel
syn match rexxParenError	")"
syn match rexxInParen contained	"[{}]"

" Comments
syn region rexxComment		start="/\*" end="\*/" contains=rexxTodo
syn match rexxCommentError	"\*/"

" Highlight User Labels
syn region rexxMulti	transparent start='?' end=':' contains=ALLBUT,rexxTodo,rexxUserLabel
syn match rexxUserLabel	"\s*\I\i*\s*:$"
syn match rexxUserLabel	";\s*\I\i*\s*:$"ms=s+1
syn match rexxUserLabel	"\s*\I\i*\s*:"me=e-1
syn match rexxUserLabel	";\s*\I\i*\s*:"ms=s+1,me=e-1

if !exists("rexx_minlines")
  let rexx_minlines = 10
endif
exec "syn sync ccomment rexxComment minlines=" . rexx_minlines

if !exists("did_rexx_syntax_inits")
  let did_rexx_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link rexxUserLabel		Label
  hi link rexxCharacter		Character
  hi link rexxParenError	rexxError
  hi link rexxInParen		rexxError
  hi link rexxCommentError	rexxError
  hi link rexxError		Error
  hi link rexxKeyword		Statement
  hi link rexxString		String
  hi link rexxComment		Comment
  hi link rexxTodo		Todo
endif

let b:current_syntax = "rexx"

" vim: ts=8
