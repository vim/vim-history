" Description:	html indenter
" Author:	Johannes Zellner <johannes@zellner.org>
" URL:		http://www.zellner.org/vim/indent/html.vim
" Last Change:	Son, 28 Jan 2001 02:45:38 +0100
" Globals:	g:html_indent_tags         -- indenting tags
"		g:html_indent_strict       -- inhibit 'O O' elements
"		g:html_indent_strict_table -- inhibit 'O -' elements

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1


" [-- local settings (must come before aborting the script) --]
setlocal indentexpr=HtmlIndentGet(v:lnum)
setlocal indentkeys=o,O,*<Return>,<>>,<bs>,{,}


" [-- avoid multiple sourcing --]
if exists("*HtmlIndentGet") | finish | endif


if exists('g:html_indent_tags')
    unlet g:html_indent_tags
endif

" [-- helper function to assemble tag list --]
fun! <SID>HtmlIndentPush(tag)
    if exists('g:html_indent_tags')
	let g:html_indent_tags = g:html_indent_tags.'\|'.a:tag
    else
	let g:html_indent_tags = a:tag
    endif
endfun


" [-- <ELEMENT ? - - ...> --]
call <SID>HtmlIndentPush('A')
call <SID>HtmlIndentPush('ABBR')
call <SID>HtmlIndentPush('ACRONYM')
call <SID>HtmlIndentPush('ADDRESS')
call <SID>HtmlIndentPush('B')
call <SID>HtmlIndentPush('BDO')
call <SID>HtmlIndentPush('BIG')
call <SID>HtmlIndentPush('BLOCKQUOTE')
call <SID>HtmlIndentPush('BUTTON')
call <SID>HtmlIndentPush('CAPTION')
call <SID>HtmlIndentPush('CENTER')
call <SID>HtmlIndentPush('CITE')
call <SID>HtmlIndentPush('CODE')
call <SID>HtmlIndentPush('DEL')
call <SID>HtmlIndentPush('DFN')
call <SID>HtmlIndentPush('DIR')
call <SID>HtmlIndentPush('DIV')
call <SID>HtmlIndentPush('DL')
call <SID>HtmlIndentPush('EM')
call <SID>HtmlIndentPush('FIELDSET')
call <SID>HtmlIndentPush('FONT')
call <SID>HtmlIndentPush('FORM')
call <SID>HtmlIndentPush('FRAMESET')
call <SID>HtmlIndentPush('H1')
call <SID>HtmlIndentPush('H2')
call <SID>HtmlIndentPush('H3')
call <SID>HtmlIndentPush('H4')
call <SID>HtmlIndentPush('H5')
call <SID>HtmlIndentPush('H6')
call <SID>HtmlIndentPush('I')
call <SID>HtmlIndentPush('IFRAME')
call <SID>HtmlIndentPush('INS')
call <SID>HtmlIndentPush('KBD')
call <SID>HtmlIndentPush('LABEL')
call <SID>HtmlIndentPush('LEGEND')
call <SID>HtmlIndentPush('MAP')
call <SID>HtmlIndentPush('MENU')
call <SID>HtmlIndentPush('NOFRAMES')
call <SID>HtmlIndentPush('NOSCRIPT')
call <SID>HtmlIndentPush('OBJECT')
call <SID>HtmlIndentPush('OL')
call <SID>HtmlIndentPush('OPTGROUP')
call <SID>HtmlIndentPush('PRE')
call <SID>HtmlIndentPush('Q')
call <SID>HtmlIndentPush('S')
call <SID>HtmlIndentPush('SAMP')
call <SID>HtmlIndentPush('SCRIPT')
call <SID>HtmlIndentPush('SELECT')
call <SID>HtmlIndentPush('SMALL')
call <SID>HtmlIndentPush('SPAN')
call <SID>HtmlIndentPush('STRONG')
call <SID>HtmlIndentPush('STYLE')
call <SID>HtmlIndentPush('SUB')
call <SID>HtmlIndentPush('SUP')
call <SID>HtmlIndentPush('TABLE')
call <SID>HtmlIndentPush('TEXTAREA')
call <SID>HtmlIndentPush('TITLE')
call <SID>HtmlIndentPush('TT')
call <SID>HtmlIndentPush('U')
call <SID>HtmlIndentPush('UL')
call <SID>HtmlIndentPush('VAR')


" [-- <ELEMENT ? O O ...> --]
if !exists('g:html_indent_strict')
    call <SID>HtmlIndentPush('BODY')
    call <SID>HtmlIndentPush('HEAD')
    call <SID>HtmlIndentPush('HTML')
    call <SID>HtmlIndentPush('TBODY')
endif


" [-- <ELEMENT ? O - ...> --]
if !exists('g:html_indent_strict_table')
    call <SID>HtmlIndentPush('TH')
    call <SID>HtmlIndentPush('TD')
    call <SID>HtmlIndentPush('TR')
    call <SID>HtmlIndentPush('TFOOT')
    call <SID>HtmlIndentPush('THEAD')
endif

delfun <SID>HtmlIndentPush


" [-- count indent-increasing tags of line a:lnum --]
fun! <SID>HtmlIndentOpen(lnum)
    let s = substitute('x'.getline(a:lnum),
    \ '.\{-}\(\(<\)\('.g:html_indent_tags.'\)\>\)', '§', 'g')
    let s = substitute(s, '[^§].*$', '', '')
    return strlen(s)
endfun

" [-- count indent-decreasing tags of line a:lnum --]
fun! <SID>HtmlIndentClose(lnum)
    let s = substitute('x'.getline(a:lnum),
    \ '.\{-}\(\(<\)/\('.g:html_indent_tags.'\)\>>\)', '§', 'g')
    let s = substitute(s, '[^§].*$', '', '')
    return strlen(s)
endfun

" [-- count indent-increasing '{' of (java|css) line a:lnum --]
fun! <SID>HtmlIndentOpenAlt(lnum)
    return strlen(substitute(getline(a:lnum), '[^{]\+', '', 'g'))
endfun

" [-- count indent-decreasing '}' of (java|css) line a:lnum --]
fun! <SID>HtmlIndentCloseAlt(lnum)
    return strlen(substitute(getline(a:lnum), '[^}]\+', '', 'g'))
endfun

" [-- return the sum of indents respecting the syntax of a:lnum --]
fun! <SID>HtmlIndentSum(lnum, style)
    if '' != &syntax &&
	\ synIDattr(synID(a:lnum, 1, 1), 'name') =~ '\(css\|java\).*' &&
	\ synIDattr(synID(a:lnum, strlen(getline(a:lnum)) - 1, 1), 'name')
	\ =~ '\(css\|java\).*'
	if a:style == match(getline(a:lnum), '^\s*}')
	    return <SID>HtmlIndentOpenAlt(a:lnum) - <SID>HtmlIndentCloseAlt(a:lnum)
	else
	    return 0
	endif
    else
	" do this double check, because the first check is much faster
	if a:style == match(getline(a:lnum), '^\s*</')
	    if a:style == match(getline(a:lnum), '^\s*</\<\('.g:html_indent_tags.'\)\>') 
		return <SID>HtmlIndentOpen(a:lnum) - <SID>HtmlIndentClose(a:lnum)
	    endif
	else
	    return 0
	endif
    endif
endfun

fun! HtmlIndentGet(lnum)
    " Find a non-empty line above the current line.
    let lnum = prevnonblank(a:lnum - 1)

    " Hit the start of the file, use zero indent.
    if lnum == 0
	return 0
    endif

    let restore_ic = 'let &ic='.&ic
    set noic

    let ind = <SID>HtmlIndentSum(lnum, -1)
    let ind = ind + <SID>HtmlIndentSum(a:lnum, 0)

    exe restore_ic

    return indent(lnum) + (&sw * ind)
endfun

" [-- EOF <runtime>/indent/html.vim --]
