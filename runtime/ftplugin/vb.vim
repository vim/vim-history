" Vim filetype plugin file
" Language:	VisualBasic (ft=vb)
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Tue, 18 Sep 2001 23:00:50 +0200

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

setlocal nocindent
setlocal autoindent
setlocal foldmethod=syntax
setlocal com=sr:'\ -,mb:'\ \ ,el:'\ \ ,:'

" we need this wrapper, as call doesn't allow a count
fun! <SID>VbSearch(pattern, flags)
    let cnt = v:count1
    while cnt > 0
	call search(a:pattern, a:flags)
	let cnt = cnt - 1
    endwhile
endfun

let s:cpo_save = &cpo
set cpo&vim

" NOTE the double escaping \\|
nnoremap <buffer> <silent> [[ :call <SID>VbSearch('^\s*\(private\s\+\)\=\(function\\|sub\)', 'bW')<cr>
nnoremap <buffer> <silent> ]] :call <SID>VbSearch('^\s*\(private\s\+\)\=\(function\\|sub\)', 'W')<cr>
nnoremap <buffer> <silent> [] :call <SID>VbSearch('^\s*\<end\>\s\+\(function\\|sub\)', 'bW')<cr>
nnoremap <buffer> <silent> ][ :call <SID>VbSearch('^\s*\<end\>\s\+\(function\\|sub\)', 'W')<cr>

" matchit support
if exists("loaded_matchit")
    let b:match_ignorecase=1
    let b:match_words=
    \ '\%(^\s*\)\@<=\<if\>:\%(^\s*\)\@<=\<else\>:\%(^\s*\)\@<=\<elseif\>:\%(^\s*\)\@<=\<end\>\s\+\<if\>,' .
    \ '\%(^\s*\)\@<=\<for\>:\%(^\s*\)\@<=\<next\>,' .
    \ '\%(^\s*\)\@<=\<while\>:\%(^\s*\)\@<=\<wend\>,' .
    \ '\%(^\s*\)\@<=\<select\>\s\+\<case\>:\%(^\s*\)\@<=\<case\>:\%(^\s*\)\@<=\<end\>\s\+\<select\>,' .
    \ '\%(^\s*\)\@<=\%(\<private\>\s\+\)\=\<function\>\s\+\([^ \t(]\+\):\%(^\s*\)\@<=\<\1\>\s*=:\%(^\s*\)\@<=\<end\>\s\+\<function\>,' .
    \ '\%(^\s*\)\@<=\%(\<private\>\s\+\)\=\<sub\>\s\+:\%(^\s*\)\@<=\<end\>\s\+\<sub\>'
endif

let &cpo = s:cpo_save
unlet s:cpo_save
