" Vim syntax file
" Language:	Makefile
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/make.vim
" Last change:	1998 Jan 12

" Remove any old syntax stuff hanging around
syn clear

" This file makes use of the highlighting "Function", which is not defined
" in the normal syntax.vim file yet.


" some directives
syn match makePreCondit	"^\s*\(ifeq\>\|else\>\|endif\>\|define\>\|ifneq\>\|ifdef\>\|ifndef\)"
syn match makeInclude	"^\s*include"
syn match makeStatement	"^\s*vpath"
syn match makeOverride	"^\s*override"
hi link makeOverride makeStatement


" make targets
syn match makeSpecTarget	"^\.SUFFIXES"
syn match makeSpecTarget	"^\.PHONY"
syn match makeSpecTarget	"^\.DEFAULT"
syn match makeSpecTarget	"^\.PRECIOUS"
syn match makeSpecTarget	"^\.IGNORE"
syn match makeSpecTarget	"^\.SILENT"
syn match makeSpecTarget	"^\.EXPORT_ALL_VARIABLES"
syn match makeImplicit	"^\.[A-Za-z0-9_]*\.[A-Za-z0-9_]*\s*:[^=]"me=e-2
syn match makeImplicit	"^\.[A-Za-z0-9_]*\.[A-Za-z0-9_]*\s*:$"me=e-1
syn match makeTarget		"^[A-Za-z0-9_][A-Za-z0-9_./\t ]*:[^=]"me=e-2
syn match makeTarget		"^[A-Za-z0-9_][A-Za-z0-9_./\t ]*:$"me=e-1

" Statements / Functions (GNU make)
syn match makeStatement contained "(subst"ms=s+1
syn match makeStatement contained "(addprefix"ms=s+1
syn match makeStatement contained "(addsuffix"ms=s+1
syn match makeStatement contained "(basename"ms=s+1
syn match makeStatement contained "(dir"ms=s+1
syn match makeStatement contained "(filter"ms=s+1
syn match makeStatement contained "(filter-out"ms=s+1
syn match makeStatement contained "(findstring"ms=s+1
syn match makeStatement contained "(firstword"ms=s+1
syn match makeStatement contained "(foreach"ms=s+1
syn match makeStatement contained "(join"ms=s+1
syn match makeStatement contained "(notdir"ms=s+1
syn match makeStatement contained "(origin"ms=s+1
syn match makeStatement contained "(patsubst"ms=s+1
syn match makeStatement contained "(shell"ms=s+1
syn match makeStatement contained "(sort"ms=s+1
syn match makeStatement contained "(strip"ms=s+1
syn match makeStatement contained "(suffix"ms=s+1
syn match makeStatement contained "(wildcard"ms=s+1
syn match makeStatement contained "(word"ms=s+1
syn match makeStatement contained "(words"ms=s+1

" some special characters
syn match makeSpecial	"^\s*[@-][@-]*"
syn match makeNextLine	"\\$"

" identifiers
syn match makeIdent		"\$([^)]*)" contains=makeStatement
syn match makeIdent		"\$\$[A-Za-z0-9_]*"
syn match makeIdent		"\$[^({]"
syn match makeIdent		"\${[^}]*}"
syn match makeIdent		"[A-Za-z][A-Za-z0-9_]*[ \t]*[:+]="me=e-2
syn match makeIdent		"[A-Za-z][A-Za-z0-9_]*[ \t]*="me=e-1
syn match makeIdent		"%"

" Comment
syn match  makeComment	"#.*$"

" match escaped quotes, $ and any other escaped character
" The escaped char is not highlightet currently
syn match makeEscapedChar 	"\\."

syn region  makeDString      start=+"+  skip=+\\"+  end=+"+  contains=makeIdent
syn region  makeSString      start=+'+  skip=+\\'+  end=+'+  contains=makeIdent
syn region  makeBString      start=+`+  skip=+\\`+  end=+`+  contains=makeIdent,makeSString,makeDString,makeNextLine

if !exists("did_makefile_syntax_inits")
  let did_makefile_syntax_inits = 1
  hi link makeNextLine	makeSpecial
  hi link makeSpecTarget	Statement
  hi link makeImplicit	Function
  hi link makeTarget	Function
  hi link makeInclude	Include
  hi link makePreCondit	PreCondit
  hi link makeStatement	Statement
  hi link makeIdent	Identifier
  hi link makeSpecial	Special
  hi link makeComment	Comment
  hi link makeDString	String
  hi link makeSString	String
  hi link makeBString	Function
endif

let b:current_syntax = "make"

" vim: ts=8
