" Language:	xml
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" URL:		http://www.zellner.org/vim/indent/xml.vim
" Last Change:	Son, 01 Okt 2000 23:40:31 +0200
" Notes:	1) does not indent pure non-xml code (e.g. embedded scripts)
"		2) will be confused by unbalanced tags in comments
"		or CDATA sections.
" $Id$


" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

" [-- local settings (must come before aborting the script) --]
setlocal indentexpr=XmlIndentGet()
setlocal indentkeys=o,O,*<Return>,<>>,/,<bs>,{,}


" [-- avoid multiple sourcing --]
if exists("*XmlIndentGet") | finish | endif


fun! XmlIndentWithPattern(line, pat)
    let s = substitute('x'.a:line, a:pat, '§', 'g')
    let s = substitute(s, '[^§].*$', '', '')
    return strlen(s)
endfun

" [-- check if it's xml --]
fun! XmlIndentSynCheck(lnum)
    if '' != &syntax &&
	let syn1 = synIDattr(synID(a:lnum, 1, 1), 'name')
	let syn2 = synIDattr(synID(a:lnum, strlen(getline(a:lnum)) - 1, 1), 'name')
	if '' != syn1 && syn1 !~ 'xml.*' && '' != syn2 && syn2 !~ 'xml.*'
	    " don't indent pure non-xml code
	    return 0
	endif
    endif
    return 1
endfun

" [-- return the sum of indents of a:lnum --]
fun! XmlIndentSum(lnum, style, add)
    let line = getline(a:lnum)
    if a:style == match(line, '^\s*</')
	return (&sw *
	\  (XmlIndentWithPattern(line, '.\{-}<\a')
	\ - XmlIndentWithPattern(line, '.\{-}</')
	\ - XmlIndentWithPattern(line, '.\{-}/>'))) + a:add
    else
	return a:add
    endif
endfun

fun! XmlIndentGet()

    " Find a non-empty line above the current line.
    let lnum = v:lnum - 1
    while lnum > 0
	if getline(lnum) !~ '^\s*$'
	    break
	endif
	let lnum = lnum - 1
    endwhile

    " Hit the start of the file, use zero indent.
    if lnum == 0
	return 0
    endif

    if 0 == XmlIndentSynCheck(lnum) || 0 == XmlIndentSynCheck(v:lnum)
	return indent(v:lnum)
    endif

    let ind = XmlIndentSum(lnum, -1, indent(lnum))
    let ind = XmlIndentSum(v:lnum, 0, ind)

    return ind
endfun

" vim:ts=8
