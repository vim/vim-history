" Vim syntax file
" Language:	JSP (Java Server Pages)
" Maintainer:	Rafael Garcia-Suarez <rgarciasuarez@free.fr>
" URL:		http://rgarciasuarez.free.fr/vim/syntax/jsp.vim
" Last change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

if !exists("main_syntax")
  let main_syntax = 'jsp'
endif

" Source HTML syntax
runtime! syntax/html.vim
unlet b:current_syntax

" Next syntax items are case-sensitive
syn case match

" Include Java syntax
syn include @jspJava <sfile>:p:h/java.vim
unlet b:current_syntax

syn region jspScriptlet matchgroup=jspTag start=/<%/  keepend end=/%>/ contains=@jspJava
syn region jspComment                     start=/<%--/        end=/--%>/
syn region jspDecl      matchgroup=jspTag start=/<%!/ keepend end=/%>/ contains=@jspJava
syn region jspExpr      matchgroup=jspTag start=/<%=/ keepend end=/%>/ contains=@jspJava
syn region jspDirective                   start=/<%@/         end=/%>/ contains=htmlString,jspDirName,jspDirArg

syn keyword jspDirName contained include page taglib
syn keyword jspDirArg contained file uri prefix language extends import session buffer autoFlush
syn keyword jspDirArg contained isThreadSafe info errorPage contentType isErrorPage

" The default highlighting.
  " java.vim has redefined htmlComment highlighting
hi def link htmlComment     Comment
hi def link htmlCommentPart Comment
  " Be consistent with html highlight settings
hi def link jspComment      htmlComment
hi def link jspTag          htmlTag
hi def link jspDirective    jspTag
hi def link jspDirName      htmlTagName
hi def link jspDirArg       htmlArg

if main_syntax == 'jsp'
  unlet main_syntax
endif

let b:current_syntax = "jsp"

" vim: ts=8
