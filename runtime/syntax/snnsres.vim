" Vim syntax file
" Language:	SNNS result file
" Maintainer:	Davide Alberani <alberanid@bigfoot.com>
" Last Change:	29 Jan 2000
" Version:	0.1
" URL:		http://members.xoom.com/alberanid/vim/syntax/snnsres.vim
"
" SNNS http://www-ra.informatik.uni-tuebingen.de/SNNS/
" is a simulator for neural networks.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" the accepted fields in the header
syn match	snnsresNoHeader	"No\. of patterns\s*:\s*" contained
syn match	snnsresNoHeader	"No\. of input units\s*:\s*" contained
syn match	snnsresNoHeader	"No\. of output units\s*:\s*" contained
syn match	snnsresNoHeader	"No\. of variable input dimensions\s*:\s*" contained
syn match	snnsresNoHeader	"No\. of variable output dimensions\s*:\s*" contained
syn match	snnsresNoHeader	"Maximum input dimensions\s*:\s*" contained
syn match	snnsresNoHeader	"Maximum output dimensions\s*:\s*" contained
syn match	snnsresNoHeader	"startpattern\s*:\s*" contained
syn match	snnsresNoHeader "endpattern\s*:\s*" contained
syn match	snnsresNoHeader "input patterns included" contained
syn match	snnsresNoHeader "teaching output included" contained
syn match	snnsresGen	"generated at.*" contained contains=snnsresNumbers
syn match	snnsresGen	"SNNS result file [Vv]\d\.\d" contained contains=snnsresNumbers

" the header, what is not an accepted field, is an error
syn region	snnsresHeader	start="^SNNS" end="^\s*[-+\.]\=[0-9#]"me=e-2 contains=snnsresNoHeader,snnsresNumbers,snnsresGen

" numbers inside the header
syn match	snnsresNumbers	"\d" contained
syn match	snnsresComment	"#.*$" contains=snnsresTodo
syn keyword	snnsresTodo	TODO XXX FIXME contained

" The default highlighting.
hi def link snnsresGen		Statement
hi def link snnsresHeader	Statement
hi def link snnsresNoHeader	Define
hi def link snnsresNumbers	Number
hi def link snnsresComment	Comment
hi def link snnsresTodo		Todo

let b:current_syntax = "snnsres"

" vim: ts=8
