" Vim syntax file
" Language:	PostScript
" Maintainer:	Mario Eusebio <bio@dq.fct.unl.pt>
" Last change:	1997 April 25

" Remove any old syntax stuff hanging around
syn clear

syn case match

syn keyword postscrTodo contained	TODO

" String
syn region  postscrString	start=+(+  end=+)+

syn match  postscrDelimiter	"[{}]"

syn match  postscrComment		"%.*$" contains=postscrTodo

syn keyword postscrFunction	add aload and arc arcn arcto array
syn keyword postscrFunction	ashow awidthshow begin charpath
syn keyword postscrFunction	clear closepath copy copy copypage
syn keyword postscrFunction	def definefont dict div dup end eq
syn keyword postscrFunction	exch exec exit false fill findfont
syn keyword postscrFunction	for forall ge get grestore gsave
syn keyword postscrFunction	gt idiv if ifelse kshow le length
syn keyword postscrFunction	lineto loop lt makefont maxlength
syn keyword postscrFunction	moveto mul ne neg newpath not or
syn keyword postscrFunction	pop put repeat restore rlineto
syn keyword postscrFunction	rmoveto roll rotate round save
syn keyword postscrFunction	scale search setgray setlinewidth
syn keyword postscrFunction	show showpage keyword string stroke
syn keyword postscrFunction	sub translate true widthshow xor

if !exists("did_postscr_syntax_inits")
  let did_postscr_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link postscrTodo		Todo
  hi link postscrString		String
  hi link postscrFunction	Statement
  hi link postscrComment		Comment
  hi link postscrDelimiter	Identifier
endif

let b:current_syntax = "postscr"

" vim: ts=8
