" Vim filetype plugin file
" Language:	Java
" Maintainer:	Dan Sharp <vimuser@crosswinds.net>
" Last Change:	Mon, 17 Sep 2001 11:28:40 Eastern Daylight Time
" Current version is at http://sites.netscape.net/sharppeople/vim/ftplugin

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

" Go ahead and set this to get decent indenting even if the indent files
" aren't being used.
setlocal cindent

"---------------------
" From Johannes Zellner <johannes@zellner.org>
setlocal cinoptions+=j1		" Correctly indent anonymous classes
setlocal cinoptions+=+0		" Align continuation lines with the previous line
"---------------------

" For filename completion, prefer the .java extension over the .class
" extension.
setlocal suffixes+=.class

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" Set 'comments' to format dashed lists in comments
setlocal com& com^=sO:*\ -,mO:*\ \ ,exO:*/  " Behaves just like C

" Make sure the continuation lines below do not cause problems in
" compatibility mode.
set cpo-=C

" Change the :browse e filter to primarily show Java-related files.
if has("gui_win32") && !exists("b:browsefilter")
    let  b:browsefilter="Java Files (*.java)\t*.java\n" .
		\	"Properties Files (*.prop*)\t*.prop*\n" .
		\	"Manifest Files (*.mf)\t*.mf\n" .
		\	"All Files (*.*)\t*.*\n"
endif
