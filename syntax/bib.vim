" Vim syntax file
" Language   : BibTeX (bibtex)
" Maintainer : Bernd Feige <feige@ukl.uni-freiburg.de>
" Last change: Aug 13, 1998

" Remove any old syntax stuff hanging around
syn clear

" Read the TeX syntax to start with
"source <sfile>:p:h/tex.vim

" Ignore case
syn case ignore

syn keyword bibEntryKw contained	crossref author title pages journal
syn keyword bibEntryKw contained	year month volume number month publisher 
syn keyword bibEntryKw contained	series edition editor key note
syn keyword bibEntryKw contained	booktitle chapter type howpublished
syn keyword bibEntryKw contained	organization institution school address
" Non-standard:
syn keyword bibEntryKw contained	abstract keywords annote isbn issn
syn match bibVariable contained	/[^= \t"{},]\+/
syn match bibVarSide oneline transparent contained	/=\s*[^= \t"{},]\+,\=\s*$/ contains=bibVariable
syn match bibVarLine oneline transparent "^\s*\S\+\s*=" contains=bibEntryKw,bibVarSide
syn match bibEntryLine oneline transparent /^\s*\S\+\s*=\s*[{"]/ contains=bibEntryKw

syn match bibCite	oneline	"\k\+:\d\{2,}\k*" 

syn match bibKey contained	"[{(]\s*[^ \t}]\+,"hs=s+1,he=e-1
syn keyword bibType contained	article book inbook booklet collection incollection
syn keyword bibType contained	proceedings inproceedings conference 
syn keyword bibType contained	manual mastersthesis phdthesis techreport 
syn keyword bibType contained	misc unpublished
syn region bibDataSetStart oneline start="^\s*@" end="$" contains=bibType,bibKey

syn keyword bibDefineKw contained	string
syn match bibVarDef contained	/{\s*[^= \t"{}]\+\s*=/hs=s+1,he=e-1
syn match bibDefineEntry oneline transparent	"^\s*@string\s*{[^=]\+=" contains=bibDefineKw,bibVarDef

syn match	bibUnescapedSpecial	"[^\\][%&]"hs=s+1
" A comment line starts with a % at the start of the line
syn match	bibComment	+^\s*%.*$+

if !exists("did_bibfile_syntax_inits")
  let did_bibfile_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link bibString	String
  hi link bibEntryKw	Statement
  hi link bibType		Function
  hi link bibDefineKw	Function
  hi link bibCite		Number
  hi link bibKey		Number
  hi link bibVariable	Special
  hi link bibVarDef	Special
  hi link bibUnescapedSpecial	Error
  hi link bibComment	Comment
endif

let b:current_syntax = "bibfile"

" vim: ts=18
