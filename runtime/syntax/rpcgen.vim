" Vim syntax file
" Language:	rpcgen
" Version:	5.4-1
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	April 23, 1999

" Removes any old syntax stuff hanging around
syn clear

" Read the C syntax to start with
runtime syntax/c.vim

syn keyword rpcProgram	program				skipnl skipwhite nextgroup=rpcProgName
syn match   rpcProgName	contained	"\<\i\I*\>"	skipnl skipwhite nextgroup=rpcProgZone
syn region  rpcProgZone	contained	matchgroup=Delimiter start="{" matchgroup=Delimiter end="}\s*=\s*\(\d\+\|0x[23]\x\{7}\)\s*;"me=e-1 contains=rpcVersion,cComment,rpcProgNmbrErr
syn keyword rpcVersion	contained	version		skipnl skipwhite nextgroup=rpcVersName
syn match   rpcVersName	contained	"\<\i\I*\>"	skipnl skipwhite nextgroup=rpcVersZone
syn region  rpcVersZone	contained	matchgroup=Delimiter start="{" matchgroup=Delimiter end="}\s*=\s*\d\+\s*;"me=e-1 contains=cType,cStructure,cStorageClass,rpcDecl,rpcProcNmbr,cComment
syn keyword rpcDecl	contained	string
syn match   rpcProcNmbr	contained	"=\s*\d\+;"me=e-1
syn match   rpcProgNmbrErr contained	"=\s*0x[^23]\x*"ms=s+1
syn match   rpcPassThru			"^\s*%.*$"

" The default highlighting.
hi def link rpcProgName		rpcName
hi def link rpcProgram		rpcStatement
hi def link rpcVersName		rpcName
hi def link rpcVersion		rpcStatement

hi def link rpcDecl		cType
hi def link rpcPassThru		cComment

hi def link rpcName		Special
hi def link rpcProcNmbr		Delimiter
hi def link rpcProgNmbrErr	Error
hi def link rpcStatement	Statement

let b:current_syntax = "rpcgen"

" vim: ts=8
