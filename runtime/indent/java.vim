" Vim indent file
" Language:	Java
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2002 Feb 28

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
   finish
endif
let b:did_indent = 1

" Indent Java anonymous classes correctly.
setlocal cinoptions& cinoptions+=j1

" The "extends" and "implements" lines start off with the wrong indent.
setlocal indentkeys& indentkeys+=0=extends indentkeys+=0=implements

" Set the function to do the work.
setlocal indentexpr=GetJavaIndent()

" Only define the function once.
if exists("*GetJavaIndent")
    finish
endif

function GetJavaIndent()

  " Java is just like C; use the built-in C indenting and then correct a few
  " specific cases.
  let theIndent = cindent(v:lnum)

  " find start of previous line, in case it was a continuation line
  let prev = v:lnum - 1
  while prev > 1
    if getline(prev - 1) !~ ',\s*$'
      break
    endif
    let prev = prev - 1
  endwhile

  " Try to align "throws" lines for methods and "extends" and "implements" for
  " classes.
  if getline(v:lnum) =~ '^\s*\(extends\|implements\)\>'
	\ && getline(prev) !~ '^\s*\(extends\|implements\)\>'
    let theIndent = theIndent + &sw
  endif

  " correct for continuation lines of "throws" and "implements"
  if getline(prev) =~ '^\s*\(throws\|implements\)\>.*,\s*$'
    if getline(prev) =~ '^\s*throws'
      let amount = &sw + 7	" add length of 'throws '
    else
      let amount = 11		" length of 'implements'.
    endif
    if getline(v:lnum - 1) !~ ',\s*$'
      let theIndent = theIndent - amount
      if theIndent < 0
	let theIndent = 0
      endif
    elseif prev == v:lnum - 1
      let theIndent = theIndent + amount
    endif
  elseif getline(v:lnum - 1) =~ '^\s*throws\>'
    let theIndent = theIndent - &sw
  endif

  " Below a line starting with "}" never indent more.  Needed for a method
  " below a method with an indented "throws" clause.
  let lnum = prevnonblank(v:lnum - 1)
  if getline(lnum) =~ '^\s*}\s*\(//.*\|/\*.*\)\=$' && indent(lnum) < theIndent
    let theIndent = indent(lnum)
  endif

  return theIndent
endfunction
