" Vim syntax file
" Language:	DSSSL
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Fre, 13 Apr 2001 23:18:44 +0200
" Filenames:	*.dsl
" URL:		http://www.zellner.org/vim/syntax/dsl.vim
" $Id$

if exists("b:current_syntax") | finish | endif

runtime syntax/xml.vim
syn cluster xmlRegionHook add=dslRegion,dslComment
syn cluster xmlCommentHook add=dslCond

" EXAMPLE:
"   <![ %output.html; [
"     <!-- some comment -->
"     (define html-manifest #f)
"   ]]>
"
" NOTE: 'contains' the same as xmlRegion, except xmlTag / xmlEndTag
syn region  dslCond matchgroup=dslCondDelim start="\[\_[^[]\+\[" end="]]" contains=xmlCdata,@xmlRegionCluster,xmlComment,xmlEntity,xmlProcessing,@xmlRegionHook

" NOTE, that dslRegion and dslComment do both NOT have a 'contained'
" argument, so this will also work in plain dsssl documents.

syn region dslRegion matchgroup=Delimiter start=+(+ end=+)+ contains=dslRegion,dslString,dslComment
syn match dslString +"\_[^"]*"+ contained
syn match dslComment +;;.*$+

" The default highlighting.
hi def link dslString		String
hi def link dslComment		Comment
" compare the following with xmlCdataStart / xmlCdataEnd
hi def link dslCondDelim	Type

let b:current_syntax = "dsl"
