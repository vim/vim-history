" Description:	html indenter
" Author:	Johannes Zellner <johannes@zellner.org>
" URL:		http://www.zellner.org/vim/indent/html.vim
" Last Change:	Son, 01 Okt 2000 07:57:36 +0200
" Globals:	g:html_indent_tags         -- indenting tags
"		g:html_indent_strict       -- inhibit 'O O' elements
"		g:html_indent_strict_table -- inhibit 'O -' elements

" $Id$

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1


" [-- local settings (must come before aborting the script) --]
setlocal indentexpr=HtmlIndentGet()
setlocal indentkeys=o,O,*<Return>,<>>,<bs>,{,}


" [-- avoid multiple sourcing --]
if exists("*HtmlIndentGet") | finish | endif


if exists('g:html_indent_tags')
    unlet g:html_indent_tags
endif

" [-- helper function to assemble tag list --]
fun! HtmlIndentPush(tag)
    if exists('g:html_indent_tags')
	let g:html_indent_tags = g:html_indent_tags.'\|'.a:tag
    else
	let g:html_indent_tags = a:tag
    endif
endfun


" [-- <ELEMENT ? - - ...> --]
call HtmlIndentPush('A')
call HtmlIndentPush('ABBR')
call HtmlIndentPush('ACRONYM')
call HtmlIndentPush('ADDRESS')
call HtmlIndentPush('B')
call HtmlIndentPush('BDO')
call HtmlIndentPush('BIG')
call HtmlIndentPush('BLOCKQUOTE')
call HtmlIndentPush('BUTTON')
call HtmlIndentPush('CAPTION')
call HtmlIndentPush('CENTER')
call HtmlIndentPush('CITE')
call HtmlIndentPush('CODE')
call HtmlIndentPush('DEL')
call HtmlIndentPush('DFN')
call HtmlIndentPush('DIR')
call HtmlIndentPush('DIV')
call HtmlIndentPush('DL')
call HtmlIndentPush('EM')
call HtmlIndentPush('FIELDSET')
call HtmlIndentPush('FONT')
call HtmlIndentPush('FORM')
call HtmlIndentPush('FRAMESET')
call HtmlIndentPush('H1')
call HtmlIndentPush('H2')
call HtmlIndentPush('H3')
call HtmlIndentPush('H4')
call HtmlIndentPush('H5')
call HtmlIndentPush('H6')
call HtmlIndentPush('I')
call HtmlIndentPush('IFRAME')
call HtmlIndentPush('INS')
call HtmlIndentPush('KBD')
call HtmlIndentPush('LABEL')
call HtmlIndentPush('LEGEND')
call HtmlIndentPush('MAP')
call HtmlIndentPush('MENU')
call HtmlIndentPush('NOFRAMES')
call HtmlIndentPush('NOSCRIPT')
call HtmlIndentPush('OBJECT')
call HtmlIndentPush('OL')
call HtmlIndentPush('OPTGROUP')
call HtmlIndentPush('PRE')
call HtmlIndentPush('Q')
call HtmlIndentPush('S')
call HtmlIndentPush('SAMP')
call HtmlIndentPush('SCRIPT')
call HtmlIndentPush('SELECT')
call HtmlIndentPush('SMALL')
call HtmlIndentPush('SPAN')
call HtmlIndentPush('STRONG')
call HtmlIndentPush('STYLE')
call HtmlIndentPush('SUB')
call HtmlIndentPush('SUP')
call HtmlIndentPush('TABLE')
call HtmlIndentPush('TEXTAREA')
call HtmlIndentPush('TITLE')
call HtmlIndentPush('TT')
call HtmlIndentPush('U')
call HtmlIndentPush('UL')
call HtmlIndentPush('VAR')


" [-- <ELEMENT ? O O ...> --]
if !exists('g:html_indent_strict')
    call HtmlIndentPush('BODY')
    call HtmlIndentPush('HEAD')
    call HtmlIndentPush('HTML')
    call HtmlIndentPush('TBODY')
endif


" [-- <ELEMENT ? O - ...> --]
if !exists('g:html_indent_strict_table')
    call HtmlIndentPush('TH')
    call HtmlIndentPush('TD')
    call HtmlIndentPush('TR')
    call HtmlIndentPush('TFOOT')
    call HtmlIndentPush('THEAD')
endif

delfun HtmlIndentPush


" [-- count indent-increasing tags of line a:lnum --]
fun! HtmlIndentOpen(lnum)
    let s = substitute('x'.getline(a:lnum),
    \ '.\{-}\(\(<\)\('.g:html_indent_tags.'\)\>\)', '§', 'g')
    let s = substitute(s, '[^§].*$', '', '')
    return strlen(s)
endfun

" [-- count indent-decreasing tags of line a:lnum --]
fun! HtmlIndentClose(lnum)
    let s = substitute('x'.getline(a:lnum),
    \ '.\{-}\(\(<\)/\('.g:html_indent_tags.'\)\>>\)', '§', 'g')
    let s = substitute(s, '[^§].*$', '', '')
    return strlen(s)
endfun

" [-- count indent-increasing '{' of (java|css) line a:lnum --]
fun! HtmlIndentOpenAlt(lnum)
    return strlen(substitute(getline(a:lnum), '[^{]\+', '', 'g'))
endfun

" [-- count indent-decreasing '}' of (java|css) line a:lnum --]
fun! HtmlIndentCloseAlt(lnum)
    return strlen(substitute(getline(a:lnum), '[^}]\+', '', 'g'))
endfun

" [-- return the sum of indents respecting the syntax of a:lnum --]
fun! HtmlIndentSum(lnum, style)
    if '' != &syntax &&
	\ synIDattr(synID(a:lnum, 1, 1), 'name') =~ '\(css\|java\).*' &&
	\ synIDattr(synID(a:lnum, strlen(getline(a:lnum)) - 1, 1), 'name')
	\ =~ '\(css\|java\).*'
	if a:style == match(getline(a:lnum), '^\s*}')
	    return HtmlIndentOpenAlt(a:lnum) - HtmlIndentCloseAlt(a:lnum)
	else
	    return 0
	endif
    else
	" do this double check, because the first check is much faster
	if a:style == match(getline(a:lnum), '^\s*</')
	    if a:style == match(getline(a:lnum), '^\s*</\<\('.g:html_indent_tags.'\)\>') 
		return HtmlIndentOpen(a:lnum) - HtmlIndentClose(a:lnum)
	    endif
	else
	    return 0
	endif
    endif
endfun

fun! HtmlIndentGet()

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

    let restore_ic = 'let &ic='.&ic
    set noic

    let ind = HtmlIndentSum(lnum, -1)
    let ind = ind + HtmlIndentSum(v:lnum, 0)

    exe restore_ic

    return indent(lnum) + (&sw * ind)
endfun

" [-- EOF <runtime>/indent/html.vim --]
