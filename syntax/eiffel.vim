" Vim syntax file
" Language:	Eiffel
" Maintainer:	Reimer Behrends <behrends@student.uni-kl.de>
" Last change:	1998 Feb 5
" Note: There should be better handling of multi-line strings

" Remove any old syntax stuff hanging around
syn clear

" keyword definitions
syn keyword eiffelKeyword        indexing expanded class obsolete
syn keyword eiffelKeyword        separate
syn keyword eiffelKeyword        inherit rename redefine undefine as
syn keyword eiffelKeyword        select export
syn keyword eiffelKeyword        external alias
syn keyword eiffelKeyword        if else elseif end inspect
syn keyword eiffelKeyword        when then
syn keyword eiffelKeyword        check debug require ensure
syn keyword eiffelKeyword        from until invariant variant loop
syn keyword eiffelKeyword        feature creation is do once
syn keyword eiffelKeyword        deferred frozen unique local
syn keyword eiffelKeyword        prefix infix
syn keyword eiffelKeyword        rescue retry

syn keyword eiffelValue          Current Void Result Precursor
" Uncomment the following line if you want to be warned against accidentally
" using certain reserved words as variables.

" syntax keyword eiffelError	    current result precursor void

" Operators
syn match eiffelOperator         "\<and\([ \t]\+then\)\=\>"
syn match eiffelOperator         "\<or\([ \t]\+else\)\=\>"
syn keyword eiffelOperator       xor not
syn keyword eiffelOperator       strip old
syn match eiffelOperator         "[!{}[\]]"
syn match eiffelOperator         "<<"
syn match eiffelOperator         ">>"
syn match eiffelOperator         "->"
syn match eiffelOperator         "[@#|&][^ \e\t\b%]*"

" Classes
syn keyword eiffelKeyword        like BIT
syn match   eiffelClassName      "\<[A-Z][A-Z0-9_]*\>"

" Constants
syn keyword eiffelConst          true false
syn region  eiffelString         start=+"+  skip=+%"+  end=+"+ contains=eiffelSpecial,eiffelStringError
syn match   eiffelSpecial	    contained "%[^/]"
syn match   eiffelSpecial	    contained "%/[0-9]\+/"
syn match   eiffelSpecial	    contained "^[ \t]*%"
syn match   eiffelSpecial	    contained "%[ \t]*$"
syn match   eiffelStringError    contained "%/[^0-9]"
syn match   eiffelStringError    contained "%/[0-9]\+[^0-9/]"
syn match   eiffelStringError    "'\(%[^/]\|%/[0-9]\+/\|[^'%]\)\+'"
syn match   eiffelCharacter      "'\(%[^/]\|%/[0-9]\+/\|[^'%]\)'" contains=eiffelSpecial
syn match   eiffelNumber         "-\=\<[0-9]\+"
syn match   eiffelNumber         "\<[01]\+[bB]\>"
syn match   eiffelNumber         "-\=\<[0-9]\+\.[0-9]*\([eE]-\=[0-9]\+\)\="
syn match   eiffelNumber         "-\=\.[0-9]\+\([eE]-\=[0-9]\+\)\="
syn match   eiffelComment        "--.*"

" Catch mismatched parentheses
syn region eiffelGeneric         transparent start="\[" end="\]" contains=ALLBUT,eiffelBracketError
syn region eiffelParen           transparent start="(" end=")" contains=ALLBUT,eiffelParenError
syn match eiffelParenError       ")"
syn match eiffelBracketError     "]"

syn sync lines=10

if !exists("did_eiffel_syntax_inits")
  let did_eiffel_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link eiffelClassName 	Type
  hi link eiffelKeyword		Statement
  hi link eiffelBoolValue 	Boolean
  hi link eiffelString 		String
  hi link eiffelCharacter	Character
  hi link eiffelSpecial 	Special
  hi link eiffelNumber 		Number
  hi link eiffelOperator	Special
  hi link eiffelComment		Comment
  hi link eiffelType		Statement
  hi link eiffelValue		String
  hi link eiffelConst		String
  hi link eiffelError		Error
  hi link eiffelStringError	Error
  hi link eiffelParenError	Error
  hi link eiffelBracketError	Error
endif

let b:current_syntax = "eiffel"

" vim: ts=8
