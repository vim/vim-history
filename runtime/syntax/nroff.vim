" VIM syntax file
" Language:	nroff / groff
" Maintainer:	Jérôme Plût <Jerome.Plut@ens.fr>
" URL:		http://www.eleves.ens.fr:8080/home/plut/nroff.vim
" Last Change:	2001 Apr 23
"
" {{{1 Preamble
" groff (GNU troff) behaves slightly differently from groff; it allows
" long names being specified between brackets: for instance, \[hy] is
" equivalent to \(hy.
" This file handle both syntaxes, depending on the value of 'filetype'.

if exists("b:current_syntax")
  finish
endif

" groff.vim sets b:main_syntax to "groff"
if !exists("b:main_syntax")
  let b:main_syntax = "nroff"
endif

if exists("nroff_space_errors")
  syn match nroffError /\s\+$/
endif

" {{{1 Escape sequences
" ------------------------------------------------------------

syn match nroffEscChar /\\[CN]/ nextgroup=nroffEscCharArg
syn match nroffEscape /\\[*gnYV]/ nextgroup=nroffEscRegPar,nroffEscRegArg
syn match nroffEscape /\\s[+-]\=/ nextgroup=nroffSize
syn match nroffEscape /\\[$AbDfhlLRvxXZ]/ nextgroup=nroffEscPar,nroffEscArg

syn match nroffEscRegArg /./ contained
syn match nroffEscRegArg2 /../ contained
syn match nroffEscRegPar /(/ contained nextgroup=nroffEscRegArg2
syn match nroffEscArg /./ contained
syn match nroffEscArg2 /../ contained
syn match nroffEscPar /(/ contained nextgroup=nroffEscArg2
syn match nroffSize /\((\d\)\=\d/ contained

syn region nroffEscCharArg start=/'/ end=/'/ contained
syn region nroffEscArg start=/'/ end=/'/ contained contains=nroffEscape,@nroffSpecial
syn sync fromstart

if b:main_syntax == "groff"
  syn region nroffEscRegArg matchgroup=nroffEscape start=/\[/ end=/\]/ contained oneline 
  syn region nroffSize matchgroup=nroffEscape start=/\[/ end=/\]/ contained
endif

syn match nroffEscape /\\[adprtu{}]/
syn match nroffEscape /\\$/
syn match nroffEscape /\\\$[@*]/

" {{{1 Strings and special characters
" ------------------------------------------------------------

syn match nroffSpecialChar /\\[\\eE?!-]/
syn match nroffSpace  "\\[&%~|^0)/,]"
syn match nroffSpecialChar /\\(../
if b:main_syntax == "groff"
  syn match nroffSpecialChar /\\\[[^]]*]/
  syn region nroffPreserve matchgroup=nroffSpecialChar start=/\\?/ end=/\\?/ oneline
endif
syn region nroffPreserve matchgroup=nroffSpecialChar start=/\\!/ end=/$/ oneline

syn cluster nroffSpecial contains=nroffSpecialChar,nroffSpace


syn region nroffString start=/"/ end=/"/ contains=nroffEscape,@nroffSpecial contained oneline
syn region nroffString start=/'/ end=/'/ contains=nroffEscape,@nroffSpecial contained oneline


" {{{1 Numbers and units
" ------------------------------------------------------------ 
syn match nroffNumBlock /[0-9.]\a\=/ contained contains=nroffNumber
syn match nroffNumber /\d\+\(\.\d*\)\=/ contained nextgroup=nroffUnit,nroffBadChar
syn match nroffNumber /\.\d\+)/ contained nextgroup=nroffUnit,nroffBadChar
syn match nroffBadChar /./ contained
syn match nroffUnit /[icpPszmnvMu]/ contained


" {{{1 Requests
" ------------------------------------------------------------ 

" Requests begin with . or ' at the beginning of a line, or after .if or
" .ie.

syn match nroffReqLeader /^[.']/ nextgroup=nroffReqName skipwhite
syn match nroffReqLeader /[.']/ contained nextgroup=nroffReqName skipwhite
if b:main_syntax == "groff"
" GNU troff allows long request names
  syn match nroffReqName /[^\t \\\[?]\+/ contained nextgroup=nroffReqArg
else
  syn match nroffReqName /[^\t \\\[?]\{1,2}/ contained nextgroup=nroffReqArg
endif
syn region roffReqArg start=/\S/ skip=/\\$/ end=/$/ contained contains=nroffEscape,@nroffSpecial,nroffString,nroffError,nroffNumBlock,nroffComment

" {{{2 Conditional: .if .ie .el
syn match nroffReqName /\(if\|ie\)/ contained nextgroup=nroffCond skipwhite
syn match nroffReqName /el/ contained nextgroup=nroffReqLeader skipwhite
syn match nroffCond /\S\+/ contained nextgroup=nroffReqLeader skipwhite

" {{{2 String definition: .ds .as
syn match nroffReqname /[da]s/ contained nextgroup=nroffDefIdent skipwhite
syn match nroffDefIdent /\S\+/ contained nextgroup=nroffDefinition skipwhite
syn region nroffDefinition matchgroup=nroffSpecialChar start=/"/ matchgroup=NONE end=/\\"/me=e-2 skip=/\\$/ start=/\S/ end=/$/ contained contains=nroffDefSpecial
syn match nroffDefSpecial /\\$/ contained
syn match nroffDefSpecial /\\\((.\)\=./ contained

if b:main_syntax == "groff"
  syn match nroffDefSpecial /\\\[[^]]*]/ contained
endif

" {{{2 Macro definition: .de .am, also diversion: .di
syn match nroffReqName /\(d[ei]\|am\)/ contained nextgroup=nroffIdent skipwhite
syn match nroffIdent /[^[?( \t]\+/ contained
if b:main_syntax == "groff"
  syn match nroffReqName /als/ contained nextgroup=nroffIdent skipwhite
endif

" {{{2 Register definition: .rn .rr
syn match nroffReqName /[rn]r/ contained nextgroup=nroffIdent skipwhite
if b:main_syntax == "groff"
  syn match nroffReqName /\(rnn\|aln\)/ contained nextgroup=nroffIdent skipwhite
endif


" {{{1 eqn/tbl/pic
" ------------------------------------------------------------ 
" XXX: write proper syntax highlight for eqn / tbl / pic ?

syn region nroffEquation start=/^\.\s*EQ/ end=/^\.\s*EN/
syn region nroffTable start=/^\.\s*TB/ end=/^\.\s*TE/
syn region nroffPicture start=/^\.\s*PB/ end=/^\.\s*PE/

" {{{1 Comments
" ------------------------------------------------------------ 

syn region nroffIgnore start=/^[.']\s*ig/ end=/^['.]\s*\./
syn match nroffComment /\(^[.']\s*\)\=\\".*/ contains=nroffTodo
syn match nroffComment /^'''.*/ contains=nroffTodo
if b:main_syntax == "groff"
  syn match nroffComment "\\#.*$" contains=nroffTodo
endif
syn keyword nroffTodo TODO XXX FIXME contained

" {{{1 Hilighting
" ------------------------------------------------------------ 

hi def link nroffEscChar	nroffSpecialChar
hi def link nroffEscCharAr	nroffSpecialChar
hi def link nroffSpecialChar	SpecialChar
hi def link nroffSpace		Delimiter

hi def link nroffEscRegArg2	nroffEscRegArg
hi def link nroffEscRegArg	nroffIdent

hi def link nroffEscArg2	nroffEscArg
hi def link nroffEscPar		nroffEscape

hi def link nroffEscRegPar	nroffEscape
hi def link nroffEscArg		nroffEscape
hi def link nroffSize		nroffEscape
hi def link nroffEscape		Preproc

hi def link nroffIgnore		Comment
hi def link nroffComment	Comment
hi def link nroffTodo		Todo

hi def link nroffReqLeader	nroffRequest
hi def link nroffReqName	nroffRequest
hi def link nroffRequest	Statement
hi def link nroffCond		PreCondit
hi def link nroffDefIdent	nroffIdent
hi def link nroffIdent		Identifier

hi def link nroffEquation	PreProc
hi def link nroffTable		PreProc
hi def link nroffPicture	PreProc

hi def link nroffNumber		Number
hi def link nroffBadChar	nroffError
hi def link nroffError		Error

hi def link nroffPreserve	String
hi def link nroffString		String
hi def link nroffDefinition	String
hi def link nroffDefSpecial	Special

" I recommend using for nroffDefinition an highlight that shows spaces,
" since nroff includes them in the string, for instance:
" hi def nroffDefinition term=italic cterm=italic gui=reverse
" hi def nroffDefSpecial term=italic,bold cterm=italic,bold gui=reverse,bold

let b:current_syntax = b:main_syntax
 
" }}}1
" vim: set ts=8 sw=2:
" vim600: set fdm=marker fdl=2:
