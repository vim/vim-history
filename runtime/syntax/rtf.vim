" Vim syntax file
" Language:	Rich Text Format
" 		"*.rtf" files 
" 
" The Rich Text Format (RTF) Specification is a method of encoding formatted
" text and graphics for easy transfer between applications.
" .hlp (windows help files) use compiled rtf files
" rtf documentation at http://night.primate.wisc.edu/software/RTF/
"
" Maintainer:	Dominique Stéphan (dstephan@my-deja.com)
" URL: http://www.geocities.com/SiliconValley/Bit/1809/vim/syntax/rtf.zip
" Last change:	2000 Sep 02

" TODO: render underline, italic, bold

" clear any unwanted syntax defs
syn clear

" case on (all controls must be lower case)
syn case match

" Control Words
syn match rtfControlWord	"\\[a-z]\+[\-]\=[0-9]*"

" New Control Words (not in the 1987 specifications)
syn match rtfNewControlWord	"\\\*\\[a-z]\+[\-]\=[0-9]*"

" Control Symbol : any \ plus a non alpha symbol, *, \, { and } and '
syn match rtfControlSymbol	"\\[^a-zA-Z\*\{\}\\']"

" { } and \ are special characters, to use them
" we add a backslash \
syn match rtfCharacter		"\\\\"
syn match rtfCharacter		"\\{"
syn match rtfCharacter		"\\}"
" Escaped characters (for 8 bytes characters upper than 127)
syn match rtfCharacter		"\\'[A-Za-z0-9][A-Za-z0-9]"
" Unicode
syn match rtfUnicodeCharacter	"\\u[0-9][0-9]*"

" Color values, we will put this value in Red, Green or Blue
syn match rtfRed		"\\red[0-9][0-9]*"
syn match rtfGreen		"\\green[0-9][0-9]*"
syn match rtfBlue		"\\blue[0-9][0-9]*"

" Some stuff for help files
syn match rtfFootNote "[#$K+]{\\footnote.*}" contains=rtfControlWord,rtfNewControlWord

if !exists("did_rtf_syntax_inits")
	let did_rtf_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi link rtfControlWord		Statement
	hi link rtfNewControlWord	Special
	hi link rtfControlSymbol	Constant
	hi link rtfCharacter		Character
	hi link rtfUnicodeCharacter	SpecialChar
	hi link rtfFootNote		Comment

	" Define colors for the syntax file
      	hi rtfRed          term=underline cterm=underline ctermfg=DarkRed gui=underline guifg=DarkRed
      	hi rtfGreen        term=underline cterm=underline ctermfg=DarkGreen gui=underline guifg=DarkGreen
      	hi rtfBlue         term=underline cterm=underline ctermfg=DarkBlue gui=underline guifg=DarkBlue

	hi link rtfRed		rtfRed
	hi link rtfGreen	rtfGreen
	hi link rtfBlue		rtfBlue
endif

let b:current_syntax = "rtf"

" vim:ts=8
