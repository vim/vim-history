" Vim syntax file
" Language:	X resources files like ~/.Xdefaults
" Maintainer:	Gautam H. Mudunuri <gmudunur@informatica.com>
" Last Change:	1999 Jun 14

" clear any unwanted syntax defs
syn clear

" shut case off
syn case ignore

syn match  xdefaultsLabel           /^.\{-}:/he=e-1
syn match  xdefaultsValue           /:.*$/lc=1
syn match  xdefaultsCommentedLine   "!.*$"

if !exists("did_xdefaults_syntax_inits")
	let did_xdefaults_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi link xdefaultsLabel		Type
	hi link xdefaultsValue		Constant
	hi link xdefaultsCommentedLine	Comment
endif

let b:current_syntax = "xdefaults"

" vim:ts=8
