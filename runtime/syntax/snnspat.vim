" Vim syntax file
" Language:	SNNS pattern file
" Maintainer:	Davide Alberani <alberanid@bigfoot.com>
" Last Change:	29 Jan 2000
" Version:	0.1
" URL:		http://members.xoom.com/alberanid/vim/syntax/snnspat.vim
"
" SNNS http://www-ra.informatik.uni-tuebingen.de/SNNS/
" is a simulator for neural networks.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" anything that isn't part of the header, a comment or a number
" is wrong
syn match	snnspatError	".*"
" hoping that matches any kind of notation...
syn match	snnspatAccepted	"\([-+]\=\(\d\+\.\|\.\)\=\d\+\([Ee][-+]\=\d\+\)\=\)"
syn match	snnspatAccepted "\s"
syn match	snnspatBrac	"\[\s*\d\+\(\s\|\d\)*\]" contains=snnspatNumbers

" the accepted fields in the header
syn match	snnspatNoHeader	"No\. of patterns\s*:\s*" contained
syn match	snnspatNoHeader	"No\. of input units\s*:\s*" contained
syn match	snnspatNoHeader	"No\. of output units\s*:\s*" contained
syn match	snnspatNoHeader	"No\. of variable input dimensions\s*:\s*" contained
syn match	snnspatNoHeader	"No\. of variable output dimensions\s*:\s*" contained
syn match	snnspatNoHeader	"Maximum input dimensions\s*:\s*" contained
syn match	snnspatNoHeader	"Maximum output dimensions\s*:\s*" contained
syn match	snnspatGen	"generated at.*" contained contains=snnspatNumbers
syn match	snnspatGen	"SNNS pattern definition file [Vv]\d\.\d" contained contains=snnspatNumbers

" the header, what is not an accepted field, is an error
syn region	snnspatHeader	start="^SNNS" end="^\s*[-+\.]\=[0-9#]"me=e-2 contains=snnspatNoHeader,snnspatNumbers,snnspatGen,snnspatBrac

" numbers inside the header
syn match	snnspatNumbers	"\d" contained
syn match	snnspatComment	"#.*$" contains=snnspatTodo
syn keyword	snnspatTodo	TODO XXX FIXME contained

" The default highlighting.
hi def link snnspatGen		Statement
hi def link snnspatHeader	Error
hi def link snnspatNoHeader	Define
hi def link snnspatNumbers	Number
hi def link snnspatComment	Comment
hi def link snnspatError	Error
hi def link snnspatTodo		Todo
hi def link snnspatAccepted	NONE
hi def link snnspatBrac		NONE

let b:current_syntax = "snnspat"

" vim: ts=8
