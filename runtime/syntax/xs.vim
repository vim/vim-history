" Vim syntax file
" Language:	XS (Perl extension interface language)
" Maintainer:	Michael W. Dodge <sarge@pobox.com>
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Read the C syntax to start with
runtime! syntax/c.vim
unlet b:current_syntax

" XS extentions
" TODO: Figure out how to look for trailing '='.
syn keyword xsKeyword	MODULE PACKAGE PREFIX
syn keyword xsKeyword	OUTPUT: CODE: INIT: PREINIT: INPUT:
syn keyword xsKeyword	PPCODE: REQUIRE: CLEANUP: BOOT:
syn keyword xsKeyword	VERSIONCHECK: PROTOTYPES: PROTOTYPE:
syn keyword xsKeyword	ALIAS: INCLUDE: CASE:
" TODO: Figure out how to look for trailing '('.
syn keyword xsMacro	SV EXTEND PUSHs
syn keyword xsVariable	RETVAL NO_INIT
"syn match xsCast	"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*<"me=e-1
"syn match xsCast	"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*$"

" The default highlighting.
hi def link xsKeyword	Keyword
hi def link xsMacro	Macro
hi def link xsVariable	Identifier

let b:current_syntax = "xs"

" vim: ts=8
