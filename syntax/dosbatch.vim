" Vim syntax file
" Language:	MSDOS batch file
" Maintainer:	Mario Eusebio <bio@dq.fct.unl.pt>
" Last change:	1997 April 25

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

syn keyword dosbatchStatement	call off
syn keyword dosbatchLabel		goto in
syn keyword dosbatchConditional	if not errorlevel exist
syn keyword dosbatchRepeat		for do
syn keyword dosbatchCommands		command set cls cd copy xcopy del
syn keyword dosbatchCommands		move ren md rd path pause shift

syn keyword dosbatchTodo contained	TODO

" String
syn region  dosbatchString	start=+"+  end=+"+

"syn match  dosbatchIdentifier	"\<[a-zA-Z_][a-zA-Z0-9_]*\>"

" If you don't like tabs
"syn match dosbatchShowTab "\t"
"syn match dosbatchShowTabc "\t"
"syn match  dosbatchComment		"^\ *rem.*$" contains=dosbatchTodo,dosbatchShowTabc

" Some people use this as a comment line
" In fact this is a Label
"syn match  dosbatchComment		"^\ *:\ \+.*$" contains=dosbatchTodo

syn match  dosbatchComment		"^\ *rem.*$" contains=dosbatchTodo
syn match  dosbatchComment		"^\ *::.*$" contains=dosbatchTodo

syn match dosbatchLabelMark		"^\ *:[0-9a-zA-Z_\-]\+\>"

syn match dosbatchEchoLine		"^\ *echo.*$"

syn match dosbatchEchoStatement	"\<echo\>"
syn match dosbatchEchoStatement	"@[a-zA-Z]\+\>"

" this is also a valid Label. I don't use it.
"syn match dosbatchLabelMark		"^\ *:\ \+[0-9a-zA-Z_\-]\+\>"

syn match dosbatchVariable		"%[0-9a-z_\-]%"
syn match dosbatchArgument		"%[0-9]\>"
syn match dosbatchArgument		"%%[a-zA-Z]\>"

syn keyword dosbatchImplicit		append assign attrib backup break
syn keyword dosbatchImplicit		chcp chdir cd chkdsk scandisk defrag
syn keyword dosbatchImplicit		cls comp ctty date debug del erase
syn keyword dosbatchImplicit		dir diskcomp diskcopy doskey dosshell
syn keyword dosbatchImplicit		edit edlin emm386 exe2bin exit expand
syn keyword dosbatchImplicit		fastopen fc fdisk find format graftabl
syn keyword dosbatchImplicit		graphics help join keyb label lh
syn keyword dosbatchImplicit		loadhigh mem mirror mkdir md mode
syn keyword dosbatchImplicit		more nlsfunc print prompt qbasic
syn keyword dosbatchImplicit		recover rename ren replace restore
syn keyword dosbatchImplicit		rmdir rm setver share sort subst
syn keyword dosbatchImplicit		sys time tree type undelete unformat
syn keyword dosbatchImplicit		ver verify vol

if !exists("did_dosbatch_syntax_inits")
  let did_dosbatch_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link dosbatchStatement		dosbatchStatement
  hi link dosbatchLabel			Special
  hi link dosbatchLabelMark		Special
  hi link dosbatchConditional		dosbatchStatement
  hi link dosbatchRepeat			dosbatchStatement
  hi link dosbatchCommands		dosbatchStatement
  hi link dosbatchEchoStatement		dosbatchStatement
  hi link dosbatchStatement		Statement
  hi link dosbatchTodo			Todo
  hi link dosbatchString			String
  hi link dosbatchComment		Comment
  hi link dosbatchArgument		Identifier
  hi link dosbatchVariable		Identifier
  hi link dosbatchEchoLine		String
  hi link dosbatchImplicit		precondit

"optional highlighting
  "hi link dosbatchShowTab		Error
  "hi link dosbatchShowTabc		Error

  "hi link dosbatchIdentifier		Identifier
endif

let b:current_syntax = "dosbatch"

" vim: ts=8
