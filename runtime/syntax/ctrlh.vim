" Vim syntax file
" Language:	CTRL-H (e.g., ASCII manpages)
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Nov 04

" Existing syntax is kept, this file can be used as an addition

" Recognize underlined text: _^Hx
syntax match CtrlHUnderline /_\b./  contains=CtrlHHide

" Recognize bold text: x^Hx
syntax match CtrlHBold /\(.\)\b\1/  contains=CtrlHHide

" Hide the CTRL-H (backspace)
syntax match CtrlHHide /.\b/  contained

" The default highlighting.
hi def link CtrlHHide Ignore
hi def CtrlHUnderline term=underline cterm=underline gui=underline
hi def CtrlHBold term=bold cterm=bold gui=bold

" vim: ts=8
