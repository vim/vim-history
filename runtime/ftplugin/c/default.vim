" Vim filetype plugin file
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Oct 15

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

setlocal cindent

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" Set 'comments' to format dashed lists in comments and not see preprocessor
" lines as comments.
setlocal com& com^=sO:*\ -,mO:*\ \ ,exO:*/ com-=b:#
