"  vim: set sw=4 sts=4:
"  File		: make.vim
"  Maintainer	: Nikolai 'pcp' Weibull <da.box@home.se>
"  Revised on	: Tue, 24 Jul 2001 17:52:45 CEST
"  Language	: Makefile

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
    finish
endif

let b:did_indent = 1

setlocal indentexpr=GetMakeIndent()
setlocal indentkeys=!^F,o,O

" Only define the function once.
if exists("*GetMakeIndent")
    finish
endif

function GetMakeIndent()
    if v:lnum == 1
	return 0
    endif

    let ind = indent(v:lnum - 1)
    let line = getline(v:lnum - 1)

    if line == ''
	let ind = 0
    elseif line =~ '^[^ \t#:][^#:]*:\{1,2}\([^=:]\|$\)'
	let ind = ind + &ts
    elseif line =~ '^\s*\h\w*\s*=\s*.\+\\$'
	let ind = ind + matchend(line, '=\s*.')
    else
	let ind = indent(v:lnum)
    endif

    return ind
endfunction
