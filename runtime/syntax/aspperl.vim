" Vim syntax file
" Language:	Active State's PerlScript (ASP)
" Maintainer:	Aaron Hope <edh@brioforge.com>
" URL:		http://nim.dhs.org/~edh/aspperl.vim
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

if !exists("main_syntax")
  let main_syntax = 'perlscript'
endif

runtime! syntax/html.vim
unlet b:current_syntax
syn include @AspPerlScript <sfile>:p:h/perl.vim
unlet b:current_syntax

syn cluster htmlPreproc add=AspPerlScriptInsideHtmlTags

syn region  AspPerlScriptInsideHtmlTags keepend matchgroup=Delimiter start=+<%=\=+ skip=+".*%>.*"+ end=+%>+ contains=@AspPerlScript
syn region  AspPerlScriptInsideHtmlTags keepend matchgroup=Delimiter start=+<script\s\+language="\=perlscript"\=[^>]*>+ end=+</script>+ contains=@AspPerlScript
