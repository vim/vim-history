"  File        : css.vim
"  Owner       : Nikolai 'pcp' Weibull <da.box@home.se>
"  Revised on  : Sun, 01 Jul 2001 14:49:01 +0200
"  Revision by : Nikolai 'pcp' Weibull <da.box@home.se>

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
    finish
endif

let b:did_indent = 1

setlocal indentexpr=GetCSSIndent()
setlocal indentkeys-=:,0# indentkeys-=e

" Only define the function once.
if exists("*GetCSSIndent")
    finish
endif

function GetCSSIndent()
    " Find a non-blank line above the current line.
    let lnum = prevnonblank(v:lnum - 1)

    " Hit the start of the file, use zero indent.
    if lnum == 0
	return 0
    endif

    let line	= getline(lnum)
    let ind		= indent(lnum)

    " Check for opening brace at end of previous line
    if line =~ '{' && line !~ '{.*}'
	let ind	= ind + &sw
    endif

    let line	= getline(v:lnum)

    " Check for closing brace on current line
    if line =~ '^\s*}'
	let ind	= ind - &sw
    endif

    return ind
endfunction

"  vim: set sw=4 sts=4:
