" Vim filetype plugin file
" Language:	Perl
" Maintainer:	Dan Sharp <vimuser@crosswinds.net>
" Last Change:	Tue, 31 Jul 2001 14:00:43 Eastern Daylight Time
" Current version is at http://sites.netscape.net/sharppeople/vim/scripts

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

setlocal com& com+=:#

if has("gui") && !exists("b:browsefilter")
    let b:browsefilter = "Perl Source Files (*.pl)\t*.pl\n" .
		       \ "Perl Modules (*.pm)\t*.pm\n" .
		       \ "Perl Documentation Files (*.pod)\t*.pod\n" .
		       \ "All Files (*.*)\t*.*\n"
endif
