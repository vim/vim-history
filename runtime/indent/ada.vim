" Vim indent file
" Language:	Ada
" Maintainer:	Neil Bird <neil@fnxweb.com>
" Last Change:	2001 June 08
" Version:	$Id$
"
" ToDo:  Correctly indent multi-line exprs. and recover upon the final ';'.

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
   finish
endif
let b:did_indent = 1

setlocal indentexpr=GetAdaIndent()
setlocal indentkeys-=0{,0}
setlocal indentkeys+=0=~then,0=~end,0=~elsif,0=~when,0=~exception

" Only define the functions once.
if exists("*GetAdaIndent")
   finish
endif


" Try to find indent of the block we're in (and about to complete)
" current_indent = the current line's indent
" prev_lnum      = line to start looking on
" blockstart     = expr. that indicates a possible start of this block
" No recursive previous block analysis: simply look for a valid line
" with a lesser indent than we currently ought to have (checking against one
" sw less than where we are, with the assumption that we're outdenting at leat
" by 'one').
" This shouldn't work as well as it appears to with lines that are currently
" nowhere near the correct indent (e.g., start of line)!
function s:MainBlockIndent( current_indent, prev_lnum, blockstart )
   let lnum  = a:prev_lnum
   while lnum > 0
      let lnum = prevnonblank(lnum - 1)
      if getline(lnum) =~ '^\s*' . a:blockstart
         let ind = indent(lnum)
         if ind < a:current_indent - &sw
            return ind
         endif
      endif
   endwhile
   " Fallback - just move back one
   return a:current_indent - &sw
endfunction


" Find correct indent of a new line based upon what went before
function GetAdaIndent()
   " Find a non-blank line above the current line.
   let lnum = prevnonblank(v:lnum - 1)

   " Hit the start of the file, use zero indent.
   if lnum == 0
      return 0
   endif

   " Add a 'shiftwidth' after block start
   let ind = indent(lnum)
   let line = getline(lnum)
   if line =~ '^\s*\(if\|while\|else\|elsif\|loop\|for\|declare\|begin\|record\|procedure\|function\|accept\|do\|task\|package\|then\|when\)\>'  ||
   \  line =~ '(\s*$'
      " Move indent in
      let ind = ind + &sw
   elseif line =~ '^\s*\(case\|exception\)\>'
      " Move indent in twice (next 'when' will move back)
      let ind = ind + 2 * &sw
   elseif line =~ ')\s*[;,]\s*$'
      " Revert to indent of line that started this parenthesis pair
      let b:jobby = 'normal! ' . lnum . 'G$F)%'
      exe 'normal! ' . lnum . 'G$F)%'
      let ind = indent('.')
      exe 'normal! ' . v:lnum . 'G'
   endif

   " Check current line; search for simplistic matching start-of-block
   let line = getline(v:lnum)
   if line =~ '^\s*begin\>'
      let ind = s:MainBlockIndent( ind, lnum, '\(procedure\|function\|declare\|package\|task\)\>' )
   elseif line =~ '^\s*\(else\|elsif\)\>'
      let ind = s:MainBlockIndent( ind, lnum, 'if\>' )
   elseif line =~ '^\s*when\>'
      let ind = s:MainBlockIndent( ind, lnum, '\(case\|exception\)\>' )
   elseif line =~ '^\s*end\>'
      let ind = s:MainBlockIndent( ind, lnum, '\(if\|while\|loop\|accept\|begin\|record\)\>' )
   elseif line =~ '^\s*exception\>'
      let ind = s:MainBlockIndent( ind, lnum, 'begin\>' )
   elseif line =~ '^\s*then\>'
      let ind = s:MainBlockIndent( ind, lnum, 'if\>' )
   endif

   return ind
endfunction
