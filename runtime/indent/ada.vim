" Vim indent file
" Language:	Ada
" Maintainer:	Neil Bird <neil@fnxweb.com>
" Last Change:	2001 June 20
" Version:	$Id$
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
let s:AdaComment = "\v^(\"[^\"]*\"|'.'|[^\"']){-}\zs\s*--.*"


" Try to find indent of the block we're in (and about to complete)
" prev_indent = the previous line's indent
" prev_lnum   = previous line (to start looking on)
" blockstart  = expr. that indicates a possible start of this block
" No recursive previous block analysis: simply look for a valid line
" with a lesser or equal indent than we currently (on prev_lnum) have.
" This shouldn't work as well as it appears to with lines that are currently
" nowhere near the correct indent (e.g., start of line)!
" Seems to work OK as it 'starts' with the indent of the /previous/ line.
function s:MainBlockIndent( prev_indent, prev_lnum, blockstart )
   let lnum = a:prev_lnum
   let line = getline(lnum)
   while lnum > 1
      if getline(lnum) =~ '^\s*' . a:blockstart
         let ind = indent(lnum)
         if ind <= a:prev_indent
            return ind
         endif
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


" As per MainBlockIndent, but return indent of previous startement-start
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
         let line = getline(lnum)
         let line = substitute( line, s:AdaComment, '', '' )
         if line !~ '^\s*$'
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

   " Get previous non-blank/non-comment-only line
   while 1
      let line = getline(lnum)
      let line = substitute( line, s:AdaComment, '', '' )
      if line !~ '^\s*$'
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
      let ind = s:MainBlockIndent( ind, lnum, 'type\>' )
   elseif line =~ ')\s*[;,]\s*$'
      " Revert to indent of line that started this parenthesis pair
      let b:jobby = 'normal! ' . lnum . 'G$F)%'
      exe 'normal! ' . lnum . 'G$F)%'
      if getline('.') =~ '^\s*('
         " Dire layout - use previous indent (could check for AdaComment here)
         let ind = indent( prevnonblank( line('.')-1 ) )
      else
         let ind = indent('.')
      endif
      exe 'normal! ' . v:lnum . 'G'
   elseif line =~ '[.=(]\s*$'
      " A statement continuation - move in one
      let ind = ind + &sw
   elseif line =~ ';\s*$'
      " Statement end - try to find current statement-start indent
      let ind = s:StatementIndent( ind, lnum )
   endif

   " Check current line; search for simplistic matching start-of-block
   let line = getline(v:lnum)
   if line =~ '^\s*\(begin\|is\)\>'
      let ind = s:MainBlockIndent( ind, lnum, '\(procedure\|function\|declare\|package\|task\)\>' )
   elseif line =~ '^\s*record\>'
      let ind = s:MainBlockIndent( ind, lnum, 'type\>' ) + &sw
   elseif line =~ '^\s*\(else\|elsif\)\>'
      let ind = s:MainBlockIndent( ind, lnum, 'if\>' )
   elseif line =~ '^\s*when\>'
      " Align 'when' one /in/ from matching block start
      let ind = s:MainBlockIndent( ind, lnum, '\(case\|exception\)\>' ) + &sw
   elseif line =~ '^\s*end\>'
      let ind = s:MainBlockIndent( ind, lnum, '\(if\|while\|loop\|accept\|begin\|record\)\>' )
   elseif line =~ '^\s*exception\>'
      let ind = s:MainBlockIndent( ind, lnum, 'begin\>' )
   elseif line =~ '^\s*then\>'
      let ind = s:MainBlockIndent( ind, lnum, 'if\>' )
   endif

   return ind
endfunction
