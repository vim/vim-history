" Vim syntax file
" Language: CVS commit file
" Maintainer:  Matt Dunford (zoot@zotikos.com)
" Last Change: Tue May 30 14:45:43 PDT 2000

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" syn region cvsText  start="^[^C][^V][^S][^:]" end="$"
syn region cvsLine  start="^CVS:" end="$" contains=cvsFile,cvsDir,cvsFiles
syn match cvsFile contained "\s\t.*"
syn match cvsDir  contained "Committing in.*$"
syn match cvsFiles contained "\S\+ Files:"

" The default highlighting.
" hi def link cvsText String
hi def link cvsLine Comment
hi def link cvsFile Identifier
hi def link cvsFiles cvsDir
hi def link cvsDir Statement

let b:current_syntax = "cvs"
