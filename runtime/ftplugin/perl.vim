" Vim filetype plugin file
" Language:	Perl
" Maintainer:	Dan Sharp <vimuser@crosswinds.net>
" Last Change:	Wed, 19 Sep 2001 16:29:01 Eastern Daylight Time
" Current version is at http://sites.netscape.net/sharppeople/vim/ftplugin

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

setlocal com=:#

" Make sure the continuation lines below do not cause problems in
" compatibility mode.
set cpo-=C

" Change the browse dialog on Win32 to show mainly Perl-related files
if has("gui_win32") && !exists("b:browsefilter")
    let b:browsefilter = "Perl Source Files (*.pl)\t*.pl\n" .
		       \ "Perl Modules (*.pm)\t*.pm\n" .
		       \ "Perl Documentation Files (*.pod)\t*.pod\n" .
		       \ "All Files (*.*)\t*.*\n"
endif

" Provided by Ned Konz <ned@bike-nomad.com>
"---------------------------------------------
setlocal include=\\<\\(use\|require\\)\\s*
setlocal includeexpr=substitute(substitute(v:fname,'::','/','g'),'$','.pm','')
setlocal iskeyword=48-57,_,A-Z,a-z,:
setlocal define=[^A-Za-z_]

" Set this once, globally.
if !exists("perlpath")
  if &shellxquote != '"'
    let perlpath = system('perl -e "print join(q/,/,@INC)"')
  else
    let perlpath = system("perl -e 'print join(q/,/,@INC)'")
  endif
  let perlpath = substitute(perlpath,',.$',',,','')
endif

let &l:path=perlpath
