" Vim syntax file
" Language:	BibTeX (bibtex)
" Maintainer:	Bernd Feige <Bernd.Feige@gmx.net>
" Last Change:	Aug 27, 1999

" Thanks go to David Squire <squire@cui.unige.ch> for the change to
" discriminate between inside and outside of bib entries :-)

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
syn match bibVariable contained /[^{}," \t=\k]/
syn region bibQuote contained start=/"/ end=/"/ skip=/\(\\"\)/ contains=bibUnescapedSpecial,bibBrace
syn region bibBrace contained start=/{/ end=/}/ contains=bibUnescapedSpecial,bibBrace
syn region bibField contained start=/\k\+\s*=\s*/ end=/[},]/me=e-1 contains=bibEntryKw,bibNSEntryKw,bibBrace,bibQuote,bibVariable
syn region bibEntryData contained start=/[{(]/ms=e+1 end=/[})]/me=e-1 contains=bibKey,bibField
syn region bibEntry start=/\@\k\+[{(]/ end=/[})]/ contains=bibType,bibEntryData nextgroup=bibEntry,bibComment
syn region bibComment start=/^[^\@]/ end=/^\s*\@/me=e-1 nextgroup=bibEntry

syn sync match All grouphere bibEntry /^\s*\@/
syn sync maxlines=200
syn sync minlines=50

if !exists("did_bib_syntax_inits")
  let did_bib_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link bibType		Identifier
  hi link bibEntryKw		Statement
  hi link bibNSEntryKw		PreProc
  hi link bibKey		Special
  hi link bibVariable		Constant
  hi link bibUnescapedSpecial	Error
  hi link bibComment		Comment
endif

let b:current_syntax = "bib"
