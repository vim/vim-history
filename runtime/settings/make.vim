" Vim settings file
" Language:	Make
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Jul 21

" Only do these settings when not done yet for this buffer
if exists("b:did_settings")
  finish
endif

" Don't do other file type settings for this buffer
let b:did_settings = 1

" Make sure a hard tab is used, required for most make programs
setlocal noexpandtab

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" Set 'comments' to format dashed lists in comments
setlocal com=sO:#\ -,mO:#\ \ ,b:#
