" Vim filetype plugin file
" Language:	Verilog HDL
" Maintainer:	Chih-Tsun Huang <cthuang@larc.ee.nthu.edu.tw>
" Last Change:	Wed Aug  1 21:06:49 CST 2001
" URL:		http://larc.ee.nthu.edu.tw/~cthuang/vim/ftplugin/verilog.vim

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croqlm1

" Set 'comments' to format dashed lists in comments.
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://

" Format comments to be up to 78 characters long
setlocal tw=75
