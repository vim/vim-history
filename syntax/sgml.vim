" Vim syntax file
" Language:	SGML
" Maintainer:	Sung-Hyun Nam <namsh@lgic.co.kr>
"		Currently it looks bad. If you want to enhance and maintain,
"		You can remove my name and insert yours.
" Last change:	1998/06/01

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" tags
syn region sgmlEndTag	start=+</+    end=+>+	contains=sgmlTagN,sgmlTagError
syn region sgmlTag	start=+<[^/]+ end=+>+	contains=sgmlTagN,sgmlTagError
syn match  sgmlTagN	contained +<\s*[-a-zA-Z0-9]\++ms=s+1	contains=sgmlTagName
syn match  sgmlTagN	contained +</\s*[-a-zA-Z0-9]\++ms=s+2	contains=sgmlTagName
syn match  sgmlTagError contained "[^>]<"ms=s+1

syn region sgmlTag2	start=+<\s*[a-zA-Z]\+/+ keepend end=+/+	contains=sgmlTagN2
syn match  sgmlTagN2	contained +/.*/+ms=s+1,me=e-1

syn match  sgmlSpecial	"&\(dollar\|num\|lt\|amp\);"

" tag names
syn keyword sgmlTagName contained article author date toc title sect verb
syn keyword sgmlTagName contained abstract tscreen p itemize item enum
syn keyword sgmlTagName contained descrip quote htmlurl code
syn keyword sgmlTagName contained tt tag bf
syn match   sgmlTagName contained "sect\d\+"

" Comments
syn region sgmlComment start=+<!--+ end=+-->+ 
syn region sgmlComment start=+<!doctype+ keepend end=+>+

if !exists("did_sgml_syntax_inits")
  let did_sgml_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link sgmlTag	Function
  hi link sgmlTag2	Function
  hi link sgmlTagN2	PreProc
  hi link sgmlEndTag	Identifier
  hi link sgmlTagName	Statement
  hi link sgmlComment	Comment
  hi link sgmlSpecial	Special
  hi link sgmlTagError	Error
endif

let b:current_syntax = "sgml"

" vim: ts=8 sts=8 sw=8 noet:
