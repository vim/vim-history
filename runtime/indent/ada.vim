" Vim indent file
" Language:	Ada
" Maintainer:	Neil Bird <neil@fnxweb.com>
" Last Change:	2001 November 6
" Version:	$Id$
" Look for the latest version at http://vim.sourceforge.net/
"
" ToDo:
"  Verify handling of multi-line exprs. and recovery upon the final ';'.
"  Correctly find comments given '"' and "" ==> " syntax.

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
   finish
endif
let b:did_indent = 1

setlocal indentexpr=GetAdaIndent()
setlocal indentkeys-=0{,0}
setlocal indentkeys+=0=~then,0=~end,0=~elsif,0=~when,0=~exception,0=~begin,0=~is,0=~record

" Only define the functions once.
if exists("*GetAdaIndent")
   finish
endif

let s:AdaBlockStart = '^\s*\(if\|while\|else\|elsif\|loop\|for\>.*\<loop\|declare\|begin\|record\|procedure\|function\|accept\|do\|task\|package\|then\|when\|is\)\>'
let s:AdaComment = "\\v^(\"[^\"]*\"|'.'|[^\"']){-}\\zs\\s*--.*"


" Try to find indent of the block we're in (and about to complete)
" prev_indent = the previous line's indent
" prev_lnum   = previous line (to start looking on)
" blockstart  = expr. that indicates a possible start of this block
" No recursive previous block analysis: simply look for a valid line
" with a lesser or equal indent than we currently (on prev_lnum) have.
" This shouldn't work as well as it appears to with lines that are currently
" nowhere near the correct indent (e.g., start of line)!
" Seems to work OK as it 'starts' with the indent of the /previous/ line.
function s:MainBlockIndent( prev_indent, prev_lnum, blockstart, stop_at )
   let lnum = a:prev_lnum
   let line = getline(lnum)
   while lnum > 1
      if a:stop_at != ''  &&  getline(lnum) =~ '^\s*' . a:stop_at
         return -1
      elseif getline(lnum) =~ '^\s*' . a:blockstart
         let ind = indent(lnum)
         if ind < a:prev_indent
            return ind
         endif
      endif

      let lnum = prevnonblank(lnum - 1)
      " Get previous non-blank/non-comment-only line
      while 1
         let line = substitute( getline(lnum), s:AdaComment, '', '' )
         if line !~ '^\s*$' && line !~ '^\s*#'
            break
         endif
         let lnum = prevnonblank(lnum - 1)
         if lnum <= 0
            return a:prev_indent
         endif
      endwhile
   endwhile
   " Fallback - just move back one
   return a:prev_indent - &sw
endfunction

" Try to find indent of the block we're in (and about to complete),
" including handling of nested blocks. Works on the 'end' of a block.
" prev_indent = the previous line's indent
" prev_lnum   = previous line (to start looking on)
" blockstart  = expr. that indicates a possible start of this block
" blockend    = expr. that indicates a possible end of this block
function s:EndBlockIndent( prev_indent, prev_lnum, blockstart, blockend )
   let lnum = a:prev_lnum
   let line = getline(lnum)
   let ends = 0
   while lnum > 1
      if getline(lnum) =~ '^\s*' . a:blockstart
	 let ind = indent(lnum)
         if ends <= 0
            if ind < a:prev_indent
	       return ind
            endif
         else
            let ends = ends - 1
	 endif
      elseif getline(lnum) =~ '^\s*' . a:blockend
         let ends = ends + 1
      endif

      let lnum = prevnonblank(lnum - 1)
      " Get previous non-blank/non-comment-only line
      while 1
	 let line = getline(lnum)
	 let line = substitute( line, s:AdaComment, '', '' )
	 if line !~ '^\s*$'
	    break
	 endif
	 let lnum = prevnonblank(lnum - 1)
	 if lnum <= 0
	    return a:prev_indent
	 endif
      endwhile
   endwhile
   " Fallback - just move back one
   return a:prev_indent - &sw
endfunction

" As per MainBlockIndent, but return indent of previous statement-start
" (after we've indented due to multi-line statements).
" This time, we start searching on the line *before* the one given (which is
" the end of a statement - we want the previous beginning).
function s:StatementIndent( current_indent, prev_lnum )
   let lnum  = a:prev_lnum
   while lnum > 0
      let prev_lnum = lnum
      let lnum = prevnonblank(lnum - 1)
      " Get previous non-blank/non-comment-only line
      while 1
         let line = substitute( getline(lnum), s:AdaComment, '', '' )
         if line !~ '^\s*$' && line !~ '^\s*#'
            break
         endif
         let lnum = prevnonblank(lnum - 1)
         if lnum <= 0
            return a:current_indent
         endif
      endwhile
      " Leave indent alone if our ';' line is part of a ';'-delineated
      " aggregate (e.g., procedure args.) or first line after a block start.
      if line =~ s:AdaBlockStart
         return a:current_indent
      endif
      if line !~ '[.=(]\s*$'
         let ind = indent(prev_lnum)
         if ind < a:current_indent
            return ind
         endif
      endif
   endwhile
   " Fallback - just use current one
   return a:current_indent
endfunction


" Find correct indent of a new line based upon what went before
function GetAdaIndent()
   " Find a non-blank line above the current line.
   let lnum = prevnonblank(v:lnum - 1)
   let ind = indent(lnum)

   " Get previous non-blank/non-comment-only/non-cpp line
   while 1
      let line = substitute( getline(lnum), s:AdaComment, '', '' )
      if line !~ '^\s*$' && line !~ '^\s*#'
         break
      endif
      let lnum = prevnonblank(lnum - 1)
      if lnum <= 0
         return ind
      endif
   endwhile

   " Get default indent (from prev. line)
   let ind = indent(lnum)

   " Now check what's on the previous line
   if line =~ s:AdaBlockStart  ||  line =~ '(\s*$'
      " Move indent in
      let ind = ind + &sw
   elseif line =~ '^\s*\(case\|exception\)\>'
      " Move indent in twice (next 'when' will move back)
      let ind = ind + 2 * &sw
   elseif line =~ '^\s*end\s*record\>'
      " Move indent back to tallying 'type' preceeding the 'record'.
      let ind = s:MainBlockIndent( ind, lnum, 'type\>', '' )
   elseif line =~ ')\s*[;,]\s*$'
      " Revert to indent of line that started this parenthesis pair
      lnum
      exe 'normal! $F)%'
      if getline('.') =~ '^\s*('
         " Dire layout - use previous indent (could check for AdaComment here)
         let ind = indent( prevnonblank( line('.')-1 ) )
      else
         let ind = indent('.')
      endif
      v:lnum
   elseif line =~ '[.=(]\s*$'
      " A statement continuation - move in one
      let ind = ind + &sw
   elseif line =~ ';\s*$'
      " Statement end - try to find current statement-start indent
      let ind = s:StatementIndent( ind, lnum )
   endif


   " Check current line; search for simplistic matching start-of-block
   let line = getline(v:lnum)
   if line =~ '^\s*#'
      " Start of line for ada-pp
      let ind = 0
   elseif line =~ '^\s*\(begin\|is\)\>'
      let ind = s:MainBlockIndent( ind, lnum, '\(procedure\|function\|declare\|package\|task\)\>', 'begin\>' )
   elseif line =~ '^\s*record\>'
      let ind = s:MainBlockIndent( ind, lnum, 'type\>', '' ) + &sw
   elseif line =~ '^\s*\(else\|elsif\)\>'
      let ind = s:MainBlockIndent( ind, lnum, 'if\>', '' )
   elseif line =~ '^\s*when\>'
      " Align 'when' one /in/ from matching block start
      let ind = s:MainBlockIndent( ind, lnum, '\(case\|exception\)\>', '' ) + &sw
   elseif line =~ '^\s*end\>\s*\<if\>'
      " End of if statements
      let ind = s:EndBlockIndent( ind, lnum, 'if\>', 'end\>\s*\<if\>' )
   elseif line =~ '^\s*end\>\s*\<loop\>'
      " End of loops
      let ind = s:EndBlockIndent( ind, lnum, '\(while\|for\)\>.*\<loop\>', 'end\>\s*\<loop\>' )
   elseif line =~ '^\s*end\>\s*\<record\>'
      " End of records
      let ind = s:EndBlockIndent( ind, lnum, 'record\>.*\<is\>', 'end\>\s*\<record\>' )
   elseif line =~ '^\s*end\>\s*\<procedure\>'
      " End of procedures
      let ind = s:EndBlockIndent( ind, lnum, 'procedure\>.*\<is\>', 'end\>\s*\<procedure\>' )
   elseif line =~ '^\s*end\>\s*\<case\>'
      " End of case statement
      let ind = s:EndBlockIndent( ind, lnum, 'case\>.*\<is\>', 'end\>\s*\<case\>' )
   elseif line =~ '^\s*end\>'
      " General case for end
      let ind = s:MainBlockIndent( ind, lnum, '\(if\|while\|for\|loop\|accept\|begin\|record\|case\)\>', '' )
   elseif line =~ '^\s*exception\>'
      let ind = s:MainBlockIndent( ind, lnum, 'begin\>', '' )
   elseif line =~ '^\s*then\>'
      let ind = s:MainBlockIndent( ind, lnum, 'if\>', '' )
   endif

   return ind
endfunction

" vim: set sw=3 sts=3 :
