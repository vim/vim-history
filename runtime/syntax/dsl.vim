" Vim syntax file
" Language:	DSSSL
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Mit, 21 Mär 2001 11:38:25 +0100
" Filenames:	*.dsl
" URL:		http://www.zellner.org/vim/syntax/dsl.vim
" $Id$

if exists("b:current_syntax") | finish | endif

runtime syntax/xml.vim
syn cluster xmlRegionHook add=dslRegion,dslComment

" NOTE, that dslRegion and dslComment do both NOT have a 'contained'
" argument, so this will also work in plain dsssl documents.

syn region dslRegion matchgroup=Delimiter start=+(+ end=+)+ contains=dslRegion,dslString,dslComment
syn match dslString +"\_[^"]*"+ contained
syn match dslComment +;;.*$+

" The default highlighting.
hi def link dslString		String
hi def link dslComment		Comment

let b:current_syntax = "dsl"
