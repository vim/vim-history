" Vim syntax file
" Language:	automake Makefile.am
" Maintainer:	John Williams <jrw@pobox.com>
" Last change:	2001 Jan 15


" This script adds support for automake's Makefile.am format. It highlights
" Makefile variables significant to automake as well as highlighting
" autoconf-style @variable@ substitutions . Subsitutions are marked as errors
" when they are used in an inappropriate place, such as in defining
" EXTRA_SOURCES.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Read the Makefile syntax to start with
runtime! syntax/make.vim
unlet b:current_syntax

syn match automakePrimary "^[A-Za-z0-9_]\+\(_PROGRAMS\|LIBRARIES\|_LIST\|_SCRIPTS\|_DATA\|_HEADERS\|_MANS\|_TEXINFOS\|_JAVA\|_LTLIBRARIES\)\s*="me=e-1
syn match automakePrimary "^TESTS\s*="me=e-1
syn match automakeSecondary "^[A-Za-z0-9_]\+\(_SOURCES\|_LDADD\|_LIBADD\|_LDFLAGS\|_DEPENDENCIES\)\s*="me=e-1
syn match automakeSecondary "^OMIT_DEPENDENCIES\s*="me=e-1
syn match automakeExtra "^EXTRA_[A-Za-z0-9_]\+\s*="me=e-1
syn match automakeOptions "^\(AUTOMAKE_OPTIONS\|ETAGS_ARGS\|TAGS_DEPENDENCIES\)\s*="me=e-1
syn match automakeClean "^\(MOSTLY\|DIST\|MAINTAINER\)\=CLEANFILES\s*="me=e-1
syn match automakeSubdirs "^\(DIST_\)\=SUBDIRS\s*="me=e-1
syn match automakeConditional "^\(if\s*[a-zA-Z0-9_]\+\|else\|endif\)\s*$"

syn match automakeSubst     "@[a-zA-Z0-9_]\+@"
syn match automakeSubst     "^\s*@[a-zA-Z0-9_]\+@"
syn match automakeComment1 "#.*$" contains=automakeSubst
syn match automakeComment2 "##.*$"

syn match automakeMakeError "$[{(][^})]*[^a-zA-Z0-9_})][^})]*[})]" " GNU make function call

syn region automakeNoSubst start="^EXTRA_[a-zA-Z0-9_]*\s*=" end="$" contains=ALLBUT,automakeNoSubst transparent
syn region automakeNoSubst start="^DIST_SUBDIRS\s*=" end="$" contains=ALLBUT,automakeNoSubst transparent
syn region automakeNoSubst start="^[a-zA-Z0-9_]*_SOURCES\s*=" end="$" contains=ALLBUT,automakeNoSubst transparent
syn match automakeBadSubst  "@\([a-zA-Z0-9_]*@\=\)\=" contained

syn region  automakeMakeDString start=+"+  skip=+\\"+  end=+"+  contains=makeIdent,automakeSubstitution
syn region  automakeMakeSString start=+'+  skip=+\\'+  end=+'+  contains=makeIdent,automakeSubstitution
syn region  automakeMakeBString start=+`+  skip=+\\`+  end=+`+  contains=makeIdent,makeSString,makeDString,makeNextLine,automakeSubstitution

" The default highlighting.
hi def link automakePrimary     Statement
hi def link automakeSecondary   Type
hi def link automakeExtra       Special
hi def link automakeOptions     Special
hi def link automakeClean       Special
hi def link automakeSubdirs     Statement
hi def link automakeConditional PreProc
hi def link automakeSubst       PreProc
hi def link automakeComment1    makeComment
hi def link automakeComment2    makeComment
hi def link automakeMakeError   makeError
hi def link automakeBadSubst    makeError
hi def link automakeMakeDString makeDString
hi def link automakeMakeSString makeSString
hi def link automakeMakeBString makeBString

let b:current_syntax = "automake"

" vi: ts=8 sw=4 sts=4
