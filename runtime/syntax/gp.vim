" Vim syntax file
" Language:	gp (version 2.0)
" Maintainer:	Karim Belabas <Karim.Belabas@math.u-psud.fr>
" Last change:	2000 Nov 04

syntax clear
" some control statements
syntax keyword gpStatement	break return next
syntax keyword gpConditional	if
syntax keyword gpRepeat		until while for fordiv forprime forstep forvec
syntax keyword gpLocal          local

syntax keyword gpInterfaceKey	buffersize colors compatible debug debugmem
syntax keyword gpInterfaceKey	echo format help histsize log logfile output
syntax keyword gpInterfaceKey	parisize path primelimit prompt psfile
syntax keyword gpInterfaceKey	realprecision seriesprecision simplify
syntax keyword gpInterfaceKey	strictmatch timer

syntax match   gpInterface	"^\s*\\[a-z].*"
syntax keyword gpInterface	default
syntax keyword gpInput		read input

" functions
syntax match gpFunRegion "^\s*[a-zA-Z][_a-zA-Z0-9]*(.*)\s*=\s*[^ \t=]"me=e-1 contains=gpFunction,gpArgs
syntax match gpFunRegion "^\s*[a-zA-Z][_a-zA-Z0-9]*(.*)\s*=\s*$" contains=gpFunction,gpArgs
syntax match gpArgs contained "[a-zA-Z][_a-zA-Z0-9]*"
syntax match gpFunction contained "^\s*[a-zA-Z][_a-zA-Z0-9]*("me=e-1

" String and Character constants
" Highlight special (backslash'ed) characters differently
syntax match  gpSpecial contained "\\[ent\\]"
syntax region gpString  start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=gpSpecial

"comments
syntax region gpComment	start="/\*"  end="\*/" contains=gpTodo
syntax match  gpComment "\\\\.*" contains=gpTodo
syntax keyword gpTodo contained	TODO
syntax sync ccomment gpComment minlines=10

"catch errors caused by wrong parenthesis
syntax region gpParen		transparent start='(' end=')' contains=ALLBUT,gpParenError,gpTodo,gpFunction,gpArgs,gpSpecial
syntax match gpParenError	")"
syntax match gpInParen contained "[{}]"

" The default highlighting.
hi def link gpConditional	Conditional
hi def link gpRepeat		Repeat
hi def link gpError		Error
hi def link gpParenError	gpError
hi def link gpInParen		gpError
hi def link gpStatement		Statement
hi def link gpString		String
hi def link gpComment		Comment
hi def link gpInterface		Type
hi def link gpInput		Type
hi def link gpInterfaceKey	Statement
hi def link gpFunction		Function
hi def link gpLocal		Type

  " contained ones
hi def link gpSpecial		Special
hi def link gpTodo		Todo
hi def link gpArgs		Type

let b:current_syntax = "gp"
" vim: ts=8
