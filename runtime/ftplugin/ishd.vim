" Vim filetype plugin file
" Language:	InstallShield (ft=ishd)
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Wed, 08 Aug 2001 20:48:49 +0200

if exists("b:did_ftplugin") | finish | endif
let b:did_ftplugin = 1

setlocal nocindent
setlocal autoindent

set cpo-=C

" matchit support
if exists("loaded_matchit")
    let b:match_ignorecase=0
    let b:match_words=
    \ '\%(^\s*\)\@<=\<function\>\s\+[^()]\+\s*(:\%(^\s*\)\@<=\<begin\>\s*$:\%(^\s*\)\@<=\<return\>:\%(^\s*\)\@<=\<end\>\s*;\s*$,' .
    \ '\%(^\s*\)\@<=\<repeat\>\s*$:\%(^\s*\)\@<=\<until\>\s\+.\{-}\s*;\s*$,' .
    \ '\%(^\s*\)\@<=\<switch\>\s*(.\{-}):\%(^\s*\)\@<=\<\%(case\|default\)\>:\%(^\s*\)\@<=\<endswitch\>\s*;\s*$,' .
    \ '\%(^\s*\)\@<=\<while\>\s*(.\{-}):\%(^\s*\)\@<=\<endwhile\>\s*;\s*$,' .
    \ '\%(^\s*\)\@<=\<for\>.\{-}\<\%(to\|downto\)\>:\%(^\s*\)\@<=\<endfor\>\s*;\s*$,' .
    \ '\%(^\s*\)\@<=\<if\>\s*(.\{-})\s*then:\%(^\s*\)\@<=\<else\s*if\>\s*([^)]*)\s*then:\%(^\s*\)\@<=\<else\>:\%(^\s*\)\@<=\<endif\>\s*;\s*$'
endif
