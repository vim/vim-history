" Vim syntax file
" Language:	Perl POD format
" Maintainer:	Scott Bigham <dsb@cs.duke.edu>
" Last change:	1997 Nov 13

" Clever Hack(TM):  This file can be sourced from other syntax files to
" handle embedded POD documentation.  Set the variable b:embedded_pod
" when doing so.

" Remove any old syntax stuff hanging around -- unless we were called from
" another syntax file.
if !exists("b:embedded_pod")
  syn clear
endif

" POD commands
" For embedded POD, all of these must be marked "contained" so as to only
" appear within a region such as perlEmbeddedPod.
if exists("b:embedded_pod")
  syn match podCommand	"^=head[12]"	contained nextgroup=podCmdText
  syn match podCommand	"^=item"	contained nextgroup=podCmdText
  syn match podCommand	"^=over"	contained nextgroup=podOverIndent skipwhite
  syn match podCommand	"^=back"	contained
  " Subtle:  If this is declared here, it won't match as the end of an
  " embeddedPod region.  If you want it highlighted, use a matchgroup; see
  " perl.vim for an example.
  "syn match podCommand	"^=cut"		contained
  syn match podCommand	"^=pod"		contained
  syn match podCommand	"^=for"		contained nextgroup=podForKeywd skipwhite
  syn match podCommand	"^=begin"	contained nextgroup=podForKeywd skipwhite
  syn match podCommand	"^=end"		contained nextgroup=podForKeywd skipwhite
else
  syn match podCommand	"^=head[12]"	nextgroup=podCmdText
  syn match podCommand	"^=item"	nextgroup=podCmdText
  syn match podCommand	"^=over"	nextgroup=podOverIndent skipwhite
  syn match podCommand	"^=back"
  syn match podCommand	"^=cut"
  syn match podCommand	"^=pod"
  syn match podCommand	"^=for"		nextgroup=podForKeywd skipwhite
  syn match podCommand	"^=begin"	nextgroup=podForKeywd skipwhite
  syn match podCommand	"^=end"		nextgroup=podForKeywd skipwhite
endif

" Text of a =head1, =head2 or =item command
syn match podCmdText	".*$" contained

" Indent amount of =over command
syn match podOverIndent	"[0-9]\+" contained

" Formatter identifier keyword for =for, =begin and =end commands
syn match podForKeywd	"[^ ]\+" contained

" An indented line, to be displayed verbatim
if exists("b:embedded_pod")
  syn match podVerbatimLine	"^[ \t].*$" contained
else
  syn match podVerbatimLine	"^[ \t].*$"
endif

" Inline textual items handled specially by POD
"syn match podSpecial	"\<[A-Z_]\+\>"
if exists("b:embedded_pod")
  syn match podSpecial	"\<[A-Za-z_][A-Za-z0-9_]*([^)]*)" contained
  syn match podSpecial	"[$@%][A-Za-z_][A-Za-z0-9_]*\>" contained
else
  syn match podSpecial	"\<[A-Za-z_][A-Za-z0-9_]*([^)]*)"
  syn match podSpecial	"[$@%][A-Za-z_][A-Za-z0-9_]*\>"
endif

" Special formatting sequences
if exists("b:embedded_pod")
  syn region podFormat	start="[IBSCLFXEZ]<" end=">" oneline contained contains=podFormat
else
  syn region podFormat	start="[IBSCLFXEZ]<" end=">" oneline contains=podFormat
endif

if !exists("did_pod_syntax_inits")
  let did_pod_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later.
  hi link podCommand		Statement
  hi link podCmdText		String
  hi link podOverIndent		Number
  hi link podForKeywd		Identifier
  hi link podFormat		Identifier
  hi link podVerbatimLine	PreProc
  hi link podSpecial		Identifier
endif

let b:current_syntax = "pod"

" vim: ts=8
