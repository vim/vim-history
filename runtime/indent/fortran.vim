" Vim indent file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	0.2
" Last Change:	2000 Sep 30
" Maintainer:	Ajit J. Thakkar <ajit@unb.ca>; <http://www.unb.ca/chem/ajit/>
" For the latest version of this file, see <http://www.unb.ca/chem/ajit/vim.htm>

setlocal nosmartindent
setlocal indentkeys+==end,=case,=if,=else,=do,=where,=elsewhere,=select

"Define the appropriate indent function but only once
if (b:fortran_fixed_source == 1)
  setlocal indentexpr=GetFortranFixedIndent()
  if exists("*GetFortranFixedIndent")
    finish
  endif
else
  setlocal indentexpr=GetFortranFreeIndent()
  if exists("*GetFortranFreeIndent")
    finish
  endif
endif

function GetFortranFreeIndent()
  "Find the previous non-blank line
  let lnum = v:lnum - 1
  while lnum > 0
    let prevline=getline(lnum)
    if prevline !~ '^\s*$'
      break
    endif
    let lnum = lnum - 1
  endwhile

  "Use zero indent at the top of the file
  if lnum == 0
    return 0
  endif

  let ind = indent(lnum)
  "Add a shiftwidth to statements following if, else, do, case,
  "where and elsewhere statements
  if prevline =~ '^\s*\(if\|else\|do\|case\|where\|elsewhere\)\>'
    let ind = ind + &sw
  endif

  "Subtract a shiftwidth from else, elsewhere, case, end if, end do,
  " end where and end select statements
  if getline(v:lnum) =~ '^\s*\(else\|elsewhere\|case\|end\s*\(if\|do\|where\|select\)\)\>'
    let ind = ind - &sw
  endif

  return ind
endfunction

function GetFortranFixedIndent()
  let currline=getline(v:lnum)
  "Don't indent comments, continuation lines and labelled lines
  if strpart(currline,0,6) =~ '[^ \t]'
    let ind = indent(v:lnum)
    return ind
  endif

  "Find the previous line which is not blank, not a comment,
  "not a continuation line, and does not have a label
  let lnum = v:lnum - 1
  while lnum > 0
    let prevline=getline(lnum)
    if prevline !~ '^\([C*!]\|\s*$\)'
      if !(strpart(prevline,0,6) =~ '[^ \t]') 
        break
      endif
    endif
    let lnum = lnum - 1
  endwhile

  "First line must begin at column 7
  if lnum == 0
    return 6
  endif

  "Add a shiftwidth to statements following if, else, do, case,
  "where and elsewhere statements
  let ind = indent(lnum)
  if getline(lnum) =~ '^\s*\(if\|else\|do\|case\|where\|elsewhere\)\>'
    let ind = ind + &sw
  endif

  "Subtract a shiftwidth from else, elsewhere, case, end if, end do,
  " end where and end select statements
  if getline(v:lnum) =~ '^\s*\(else\|elsewhere\|case\|end\s*\(if\|do\|where\|select\)\)\>'
    let ind = ind - &sw
  endif

  return ind
endfunction

" vim:sw=2 tw=130
