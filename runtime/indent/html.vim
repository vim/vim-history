" Description:	html indenter
" Author:	Johannes Zellner <johannes@zellner.org>
" URL:		http://www.zellner.org/vim/indent/html.vim
" Last Change:	Mon, 09 Apr 2001 14:31:56 +0200
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
call <SID>HtmlIndentPush('a')
call <SID>HtmlIndentPush('abbr')
call <SID>HtmlIndentPush('acronym')
call <SID>HtmlIndentPush('address')
call <SID>HtmlIndentPush('b')
call <SID>HtmlIndentPush('bdo')
call <SID>HtmlIndentPush('big')
call <SID>HtmlIndentPush('blockquote')
call <SID>HtmlIndentPush('button')
call <SID>HtmlIndentPush('caption')
call <SID>HtmlIndentPush('center')
call <SID>HtmlIndentPush('cite')
call <SID>HtmlIndentPush('code')
call <SID>HtmlIndentPush('del')
call <SID>HtmlIndentPush('dfn')
call <SID>HtmlIndentPush('dir')
call <SID>HtmlIndentPush('div')
call <SID>HtmlIndentPush('dl')
call <SID>HtmlIndentPush('em')
call <SID>HtmlIndentPush('fieldset')
call <SID>HtmlIndentPush('font')
call <SID>HtmlIndentPush('form')
call <SID>HtmlIndentPush('frameset')
call <SID>HtmlIndentPush('h1')
call <SID>HtmlIndentPush('h2')
call <SID>HtmlIndentPush('h3')
call <SID>HtmlIndentPush('h4')
call <SID>HtmlIndentPush('h5')
call <SID>HtmlIndentPush('h6')
call <SID>HtmlIndentPush('i')
call <SID>HtmlIndentPush('iframe')
call <SID>HtmlIndentPush('ins')
call <SID>HtmlIndentPush('kbd')
call <SID>HtmlIndentPush('label')
call <SID>HtmlIndentPush('legend')
call <SID>HtmlIndentPush('map')
call <SID>HtmlIndentPush('menu')
call <SID>HtmlIndentPush('noframes')
call <SID>HtmlIndentPush('noscript')
call <SID>HtmlIndentPush('object')
call <SID>HtmlIndentPush('ol')
call <SID>HtmlIndentPush('optgroup')
call <SID>HtmlIndentPush('pre')
call <SID>HtmlIndentPush('q')
call <SID>HtmlIndentPush('s')
call <SID>HtmlIndentPush('samp')
call <SID>HtmlIndentPush('script')
call <SID>HtmlIndentPush('select')
call <SID>HtmlIndentPush('small')
call <SID>HtmlIndentPush('span')
call <SID>HtmlIndentPush('strong')
call <SID>HtmlIndentPush('style')
call <SID>HtmlIndentPush('sub')
call <SID>HtmlIndentPush('sup')
call <SID>HtmlIndentPush('table')
call <SID>HtmlIndentPush('textarea')
call <SID>HtmlIndentPush('title')
call <SID>HtmlIndentPush('tt')
call <SID>HtmlIndentPush('u')
call <SID>HtmlIndentPush('ul')
call <SID>HtmlIndentPush('var')


" [-- <ELEMENT ? O O ...> --]
if !exists('g:html_indent_strict')
    call <SID>HtmlIndentPush('body')
    call <SID>HtmlIndentPush('head')
    call <SID>HtmlIndentPush('html')
    call <SID>HtmlIndentPush('tbody')
endif


" [-- <ELEMENT ? O - ...> --]
if !exists('g:html_indent_strict_table')
    call <SID>HtmlIndentPush('th')
    call <SID>HtmlIndentPush('td')
    call <SID>HtmlIndentPush('tr')
    call <SID>HtmlIndentPush('tfoot')
    call <SID>HtmlIndentPush('thead')
endif

delfun <SID>HtmlIndentPush


" [-- count indent-increasing tags of line a:lnum --]
fun! <SID>HtmlIndentOpen(lnum)
    let s = substitute('x'.getline(a:lnum),
    \ '.\{-}\(\(<\)\('.g:html_indent_tags.'\)\>\)', '�', 'g')
    let s = substitute(s, '[^�].*$', '', '')
    return strlen(s)
endfun

" [-- count indent-decreasing tags of line a:lnum --]
fun! <SID>HtmlIndentClose(lnum)
    let s = substitute('x'.getline(a:lnum),
    \ '.\{-}\(\(<\)/\('.g:html_indent_tags.'\)\>>\)', '�', 'g')
    let s = substitute(s, '[^�].*$', '', '')
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
    if a:style == match(getline(a:lnum), '^\s*</')
	let ic = &ic
	set ic " ignore the case of the tags
	if a:style == match(getline(a:lnum), '^\s*</\<\('.g:html_indent_tags.'\)\>')
	    let open = <SID>HtmlIndentOpen(a:lnum)
	    let close = <SID>HtmlIndentClose(a:lnum)
	    if 0 != open || 0 != close
		let &ic = ic " restore case
		return open - close
	    endif
	endif
	let &ic = ic " restore case
    endif
    if '' != &syntax &&
	\ synIDattr(synID(a:lnum, 1, 1), 'name') =~ '\(css\|java\).*' &&
	\ synIDattr(synID(a:lnum, strlen(getline(a:lnum)) - 1, 1), 'name')
	\ =~ '\(css\|java\).*'
	if a:style == match(getline(a:lnum), '^\s*}')
	    return <SID>HtmlIndentOpenAlt(a:lnum) - <SID>HtmlIndentCloseAlt(a:lnum)
	endif
    endif
    return 0
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
