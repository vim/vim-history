" Vim filetype plugin file
" Language:	Perl
" Maintainer:	Dan Sharp <vimuser@crosswinds.net>
" Last Change:	Tue, 21 Aug 2001 16:54:23 
" Current version is at http://sites.netscape.net/sharppeople/vim/scripts

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

setlocal com=:#

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
setlocal isfname=A-Z,a-z,:,48-57,_
setlocal keywordprg=perldoc
setlocal iskeyword=48-57,_,A-Z,a-z,:
setlocal isident=48-57,_,A-Z,a-z
setlocal define=[^A-Za-z_]

" Set this once, globally.
if !exists("perlpath")
  let perlpath = system('perl -e "print join(q/,/,@INC)"')
  let perlpath = substitute(perlpath,',.$',',,','')
endif

let &l:path=perlpath
