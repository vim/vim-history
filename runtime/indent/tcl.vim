" Tcl indent file
" Language:	Tcl
" Author:	Nikolai 'pcp' Weibull <da.box@home.se>
" URL:		http://slackedit.sourceforge.net/vim/indent/tcl.vim (not yet)
" Last Change:	2001 Feb 06

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
	finish
endif
let b:did_indent = 1

setlocal indentexpr=GetTclIndent()
setlocal indentkeys-=0#

" Only define the function once.
if exists("*GetTclIndent")
	finish
endif

function GetTclIndent()
	" Find a non-blank line above the current line.
	let lnum = prevnonblank(v:lnum - 1)

	" Hit the start of the file, use zero indent.
	if lnum == 0
		return 0
	endif

	let line	= getline(lnum)
	let ind		= indent(lnum)

	let	plnum = lnum - 1
	let pline	= getline(plnum)
	
	" Check for comments
	if line =~ '^\s*#'
		return ind
	elseif pline =~ '\\\s*$'
		" Uh, oh...might be multi-line comment..gotta check
		while plnum > 0 && pline =~ '\\\s*$'
			if pline =~ '^\s*#'
				return ind
			endif
			let	plnum	= plnum - 1
			let pline	= getline(plnum)
		endwhile
	endif
	
	" Check for opening brace at end of previous line
	if line =~ '{\s*$'
		let ind	= ind + &sw
	endif
	
	let line	= getline(v:lnum)
	
	" Check for closing brace on current line
	if line =~ '^\s*}'
		let ind	= ind - &sw
	endif

	return ind
endfunction
