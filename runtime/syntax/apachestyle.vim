" Vim syntax file
" Language:	Apache-style config files (Apache, ProFTPd, etc)
" Maintainer:	Christian Hammers <ch@westend.com>
" URL:		none
" Last Change:	2001 Jan 15

" Apache-style config files look this way:
"
" Option	value
" Option	value1 value2
" <Section Name?>
"	Option	value
"	<SubSection Name?>
"	# Comments...
"	</SubSection>
" </Section>

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore

" specials
syn match  apachestyleComment	"^\s*#.*"

" options and values
syn region apachestyleOption	start="^\s*[A-Za-z]" end="$\|\s"

" tags
syn region apachestyleTag	start=+<+ end=+>+ contains=apachestyleTagN,apachestyleTagError
syn match  apachestyleTagN	contained +<[/\s]*[-a-zA-Z0-9]\++ms=s+1
syn match  apachestyleTagError	contained "[^>]<"ms=s+1


" The default highlighting.
hi def link apachestyleComment		Comment
hi def link apachestyleOption		String
hi def link apachestyleTag		Special
hi def link apachestyleTagN		Identifier
hi def link apachestyleTagError		Error

let b:current_syntax = "apachestyle"
" vim: ts=8
