" Vim filetype plugin file
" Language:	Kimwitu++
" Maintainer:	Michael Piefel <piefel@informatik.hu-berlin.de>
" Last Change:	2000 Dec 20

" Behaves almost like C++
runtime! ftplugin/cpp/*.vim

" Setting the errorformat for the Kimwitu++ compiler
set efm+=kc%.%#:\ error\ at\ %f:%l:\ %m
