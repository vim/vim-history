" Vim syntax file
" Language:	Focus Master File
" Maintainer:	Rob Brady <robb@datatone.com>
" Last Change:	$Date$
" URL: http://www.datatone.com/~robb/vim/syntax/master.vim
" $Revision$

" this is a very simple syntax file - I will be improving it
" add entire DEFINE syntax

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case match

" A bunch of useful keywords
syn keyword masterKeyword	FILENAME SUFFIX SEGNAME SEGTYPE PARENT FIELDNAME
syn keyword masterKeyword	FIELD ALIAS USAGE INDEX MISSING ON
syn keyword masterKeyword	FORMAT CRFILE CRKEY
syn keyword masterDefine	DEFINE DECODE EDIT
syn region  masterString	start=+"+  end=+"+
syn region  masterString	start=+'+  end=+'+
syn match   masterComment	"\$.*"

" The default highlighting.
hi def link masterKeyword Keyword
hi def link masterComment Comment
hi def link masterString  String

let b:current_syntax = "master"

" vim: ts=8
