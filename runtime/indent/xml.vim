" Language:	xml
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" URL:		http://www.zellner.org/vim/indent/xml.vim
" Last Change:	Sat, 28 Apr 2001 14:16:23 +0200
" Notes:	1) does not indent pure non-xml code (e.g. embedded scripts)
"		2) will be confused by unbalanced tags in comments
"		or CDATA sections.
" TODO: 	implement pre-like tags, see xml_indent_open / xml_indent_close

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
    finish
endif
let b:did_indent = 1

" [-- local settings (must come before aborting the script) --]
setlocal indentexpr=XmlIndentGet(v:lnum)
setlocal indentkeys=o,O,*<Return>,<>>,<<>,/,<bs>,{,}

if !exists('b:xml_indent_open')
    let b:xml_indent_open = '.\{-}<\a'
    " pre tag, e.g. <address>
    " let b:xml_indent_open = '.\{-}<[/]\@!\(address\)\@!'
endif

if !exists('b:xml_indent_close')
    let b:xml_indent_close = '.\{-}</'
    " end pre tag, e.g. </address>
    " let b:xml_indent_close = '.\{-}</\(address\)\@!'
endif

" [-- finish, if the function already exists --]
if exists('*XmlIndentGet') | finish | endif

fun! <SID>XmlIndentWithPattern(line, pat)
    let s = substitute('x'.a:line, a:pat, '§', 'g')
    return strlen(substitute(s, '[^§].*$', '', ''))
endfun

" [-- check if it's xml --]
fun! <SID>XmlIndentSynCheck(lnum)
    if '' != &syntax &&
	let syn1 = synIDattr(synID(a:lnum, 1, 1), 'name')
	let syn2 = synIDattr(synID(a:lnum, strlen(getline(a:lnum)) - 1, 1), 'name')
	if '' != syn1 && syn1 !~ 'xml' && '' != syn2 && syn2 !~ 'xml'
	    " don't indent pure non-xml code
	    return 0
	endif
    endif
    return 1
endfun

" [-- return the sum of indents of a:lnum --]
fun! <SID>XmlIndentSum(lnum, style, add)
    let line = getline(a:lnum)
    if a:style == match(line, '^\s*</')
	return (&sw *
	\  (<SID>XmlIndentWithPattern(line, b:xml_indent_open)
	\ - <SID>XmlIndentWithPattern(line, b:xml_indent_close)
	\ - <SID>XmlIndentWithPattern(line, '.\{-}/>'))) + a:add
    else
	return a:add
    endif
endfun

fun! XmlIndentGet(lnum)
    " Find a non-empty line above the current line.
    let lnum = prevnonblank(a:lnum - 1)

    " Hit the start of the file, use zero indent.
    if lnum == 0
	return 0
    endif

    if 0 == <SID>XmlIndentSynCheck(lnum) || 0 == <SID>XmlIndentSynCheck(a:lnum)
	return indent(a:lnum)
    endif

    let ind = <SID>XmlIndentSum(lnum, -1, indent(lnum))
    let ind = <SID>XmlIndentSum(a:lnum, 0, ind)

    return ind
endfun

" vim:ts=8
