" Vim settings file
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Jul 15

" Only do these settings when not done yet for this buffer
if exists("b:did_settings")
  finish
endif

" Don't do other file type settings for this buffer
let b:did_settings = 1

setlocal cindent

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" Set 'comments' to format dashed lists in comments and not see preprocessor
" lines as comments.
setlocal com& com^=sO:*\ -,mO:*\ \ ,exO:*/ com-=b:#

" load a "types.vim" file if it exists; should highlight non-standard typedefs
let s:fname = expand('<afile>:p:h') . '/types.vim'
if file_readable(s:fname)
  exe 'so ' . s:fname
endif
