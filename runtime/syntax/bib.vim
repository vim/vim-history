" Vim syntax file
" Language:	BibTeX (bibliographic database format for (La)TeX)
" Maintainer:	Bernd Feige <Bernd.Feige@gmx.net>
" Filenames:	*.bib
" Last Change:	Aug 23, 2000
" URL:          http://home.t-online.de/home/Bernd.Feige/bib.vim

" Thanks to those who pointed out problems with this file or supplied fixes!

" Remove any old syntax stuff hanging around
syn clear

" Ignore case
syn case ignore

syn keyword bibType contained	article book booklet conference inbook
syn keyword bibType contained	incollection inproceedings manual
syn keyword bibType contained	mastersthesis misc phdthesis
syn keyword bibType contained	proceedings techreport unpublished
syn keyword bibType contained	string

syn keyword bibEntryKw contained	address annote author booktitle chapter
syn keyword bibEntryKw contained	crossref edition editor howpublished
syn keyword bibEntryKw contained	institution journal key month note
syn keyword bibEntryKw contained	number organization pages publisher
syn keyword bibEntryKw contained	school series title type volume year
" Non-standard:
syn keyword bibNSEntryKw contained	abstract isbn issn keywords url

syn match bibUnescapedSpecial contained /[^\\][%&]/hs=s+1
syn match bibKey contained /\s*[^ \t}="]\+,/hs=s,he=e-1 nextgroup=bibField
syn match bibVariable contained /[^{}," \t=]/
syn region bibComment start=/^/ end=/^\s*@/me=e-1 nextgroup=bibEntry
syn region bibComment start=/[})]/hs=s+1 end=/^\s*@/me=e-1 nextgroup=bibEntry
syn region bibQuote contained start=/"/ end=/"/ skip=/\(\\"\)/ contains=bibUnescapedSpecial,bibBrace,bibParen
syn region bibBrace contained start=/{/ end=/}/ skip=/\(\\[{}]\)/ contains=bibUnescapedSpecial,bibBrace,bibParen
syn region bibParen contained start=/(/ end=/)/ skip=/\(\\[()]\)/ contains=bibUnescapedSpecial,bibBrace,bibParen
syn region bibField contained start="\S\+\s*=\s*" end=/[}),]/me=e-1 contains=bibEntryKw,bibNSEntryKw,bibBrace,bibParen,bibQuote,bibVariable
syn region bibEntryData contained start=/[{(]/ms=e+1 end=/[})]/me=e-1 contains=bibKey,bibField
syn region bibEntry start=/@\S\+[{(]/ end=/[})]/me=e-1 contains=bibType,bibEntryData nextgroup=bibComment

syn sync match All grouphere bibEntry /^\s*@/
syn sync maxlines=200
syn sync minlines=50

" The default highlighting.
hi def link bibType		Identifier
hi def link bibEntryKw		Statement
hi def link bibNSEntryKw	PreProc
hi def link bibKey		Special
hi def link bibVariable		Constant
hi def link bibUnescapedSpecial	Error
hi def link bibComment		Comment

let b:current_syntax = "bib"
