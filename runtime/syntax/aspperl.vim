" Vim syntax file
" Language:	Active State's PerlScript (ASP)
" Maintainer:	Aaron Hope <edh@brioforge.com>
" URL:		http://nim.dhs.org/~edh/aspperl.vim
" Last Change:	2000 Dec 17

" Remove any old syntax stuff hanging around
syn clear

if !exists("main_syntax")
  let main_syntax = 'perlscript'
endif

runtime syntax/html.vim
syn include @AspPerlScript <sfile>:p:h/perl.vim

syn cluster htmlPreproc add=AspPerlScriptInsideHtmlTags

syn region  AspPerlScriptInsideHtmlTags keepend matchgroup=Delimiter start=+<%=\=+ skip=+".*%>.*"+ end=+%>+ contains=@AspPerlScript
syn region  AspPerlScriptInsideHtmlTags keepend matchgroup=Delimiter start=+<script\s\+language="\=perlscript"\=[^>]*>+ end=+</script>+ contains=@AspPerlScript
