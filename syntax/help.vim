" Vim syntax file
" Language:	Vim help file
" Maintainer:	Bram Moolenaar (Bram@vim.org)
" Last change:	1998 January 11

" Remove any old syntax stuff hanging around
syn clear

syn match helpHeadline		"^[A-Z ]\+[ ]\+\*"me=e-1
syn match helpSectionDelim	"^=\{3,}"
syn match helpSectionDelim	"^-\{3,}"
syn match helpExampleStart	"^>" nextgroup=helpExample
syn match helpExample		".*" contained
syn match helpHyperTextJump	"|[#-)!+-~]\+|"
syn match helpHyperTextEntry	"\*[#-)!+-~]\+\*\s"he=e-1
syn match helpHyperTextEntry	"\*[#-)!+-~]\+\*$"
syn match helpVim		"Vim version [0-9.a-z]\+"
syn match helpVim		"VIM REFERENCE.*"
syn match helpOption		"'[a-z]\{2,\}'"
syn match helpHeader		".*\~$"me=e-1 nextgroup=helpIgnore
syn match helpIgnore		"." contained
syn keyword helpNote		note
syn region helpNotVi		start="{Vi[: ]" start="{not" start="{only" end="}" contains=helpLeadBlank
syn match helpLeadBlank		"^\s\+"

if !exists("did_help_syntax_inits")
  let did_help_syntax_inits = 1

  hi helpExampleStart guifg=bg
  hi helpIgnore guifg=bg
  hi link helpHyperTextJump	Subtitle
  hi link helpHyperTextEntry	String
  hi link helpHeadline		Statement
  hi link helpHeader		PreProc
  hi link helpSectionDelim	PreProc
  hi link helpVim		Identifier
  hi link helpExample		Comment
  hi link helpOption		Type
  hi link helpNotVi		Special
  hi link helpNote		Todo
  hi link Subtitle		Identifier
endif

let b:current_syntax = "help"

" vim: ts=8
