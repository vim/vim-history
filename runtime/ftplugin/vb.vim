" Vim filetype plugin file
" Language:	VisualBasic (ft=vb)
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Thu, 23 Aug 2001 11:18:55 W. Europe Standard Time

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

setlocal nocindent
setlocal autoindent
setlocal foldmethod=syntax

" we need this wrapper, as call doesn't allow a count
fun! <SID>VbSearch(pattern, flags)
    let cnt = v:count1
    while cnt > 0
	call search(a:pattern, a:flags)
	let cnt = cnt - 1
    endwhile
endfun

" NOTE the double escaping \\|
nnoremap <silent> [[ :call <SID>VbSearch('^\s*\(private\s\+\)\=\(function\\|sub\)', 'bW')<cr>
nnoremap <silent> ]] :call <SID>VbSearch('^\s*\(private\s\+\)\=\(function\\|sub\)', 'W')<cr>
nnoremap <silent> [] :call <SID>VbSearch('^\s*\<end\>\s\+\(function\\|sub\)', 'bW')<cr>
nnoremap <silent> ][ :call <SID>VbSearch('^\s*\<end\>\s\+\(function\\|sub\)', 'W')<cr>

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
