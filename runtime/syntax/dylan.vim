" Vim syntax file
" Language:	Dylan
" Authors:	Justus Pendleton <justus@acm.org>
"		Brent A. Fulgham <bfulgham@debian.org>
" Last Change:	Fri Sep 29 13:45:55 PDT 2000
"
" This syntax file is based on the Haskell, Perl, Scheme, and C
" syntax files.

" Part 1:  Syntax definition
syn clear
syn case ignore

set lisp

" Highlight special characters (those that have backslashes) differently
syn match	dylanSpecial		display contained "\\\(x\x\+\|\o\{1,3}\|.\|$\)"

" Keywords
syn keyword	dylanUnnamedDefs	interface
syn keyword	dylanNamedDefs		library macro
syn keyword	dylanParamDefs		method class function
syn keyword	dylanBlock		cleanup block afterwards end
syn keyword	dylanSimpleDefs		constant variable generic primary
syn keyword	dylanStatement		begin method
syn keyword	dylanOther		above below from by in instance local
syn keyword	dylanOther		slot subclass then to virtual
syn keyword	dylanConditional	if when select case else elseif unless
syn keyword	dylanConditional	finally otherwise then
syn keyword	dylanRepeat		for until while from to
syn keyword	dylanStatement		define let
" how to match labels within select and case?
" finally is a label for for loops
syn keyword	dylanLabel		otherwise finally
" signal is a method, not really a keyword
syn keyword	dylanException		exception handler
syn keyword	dylanException		signal
syn keyword	dylanImport		import rename create
syn keyword	dylanPreProc		use export exclude
syn keyword	dylanClassMods		abstract concrete primary inherited
syn keyword	dylanMethMods		inline
syn keyword	dylanMiscMods		open sealed domain singleton

" Matching rules for special forms
syn match	dylanOperator		"\s[-!%&\*\+/=\?@\\^|~.]\=[-!%&\*\+=\?@\\^|~:.]\=[->!%&\*\+=\?@\\^|~:.]"
syn match	dylanOperator		"\(\<[A-Z][a-zA-Z0-9_']*\.\)\=:[-!#$%&\*\+./=\?@\\^|~:]*"
" Numbers
syn match	dylanNumber		"\<[0-9]\+\>\|\<0[xX][0-9a-fA-F]\+\>\|\<0[oO][0-7]\+\>"
syn match	dylanNumber		"\<[0-9]\+\.[0-9]\+\([eE][-+]\=[0-9]\+\)\=\>"
" Booleans
syn match	dylanBoolean		"#t\|#f"
" Comments
syn match	dylanComment		"//.*"
syn region	dylanComment		start="/\*" end="\*/"
" Strings
syn region	dylanString		start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=dySpecial
syn match	dylanCharacter		"'[^\\]'"
" Constants, classes, and variables
syn match	dylanConstant		"$\<[a-zA-Z0-9\-]\+\>"
syn match	dylanClass		"<\<[a-zA-Z0-9\-]\+\>>"
syn match	dylanVariable		"\*\<[a-zA-Z0-9\-]\+\>\*"
" Preconditions
syn region	dylanPrecondit		start="^\s*#\s*\(if\>\|else\>\|endif\>\)" skip="\\$" end="$"

" These appear at the top of files (usually).  I like to highlight the whole line
" so that the definition stands out.  They should probably really be keywords, but they
" don't generally appear in the middle of a line of code.
syn match	dylanHeader	"^[Mm]odule:.*$"
syn match	dylanHeader	"^[Aa]uthor:.*$"
syn match	dylanHeader	"^[Cc]opyright:.*$"
syn match	dylanHeader	"^[Ss]ynopsis:.*$"

" Part 2:  Syntax highlighting rules
if !exists("did_dylan_syntax_inits")
  let did_dylan_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later.
  hi link dylanComment		Comment
  hi link dylanConstant		Constant
  hi link dylanString		String
  hi link dylanCharacter	Character
  hi link dylanNumber		Number
  hi link dylanBoolean		Boolean
  hi link dylanClass		Structure
  hi link dylanVariable		Identifier
  hi link dylanConditional	Conditional
  hi link dylanRepeat		Repeat
  hi link dylanLabel		Label
  hi link dylanOperator		Operator
  hi link dylanUnnamedDefs	Keyword
  hi link dylanNamedDefs	Keyword
  hi link dylanParamDefs	Keyword
  hi link dylanBlock		Structure
  hi link dylanSimpleDefs	Keyword
  hi link dylanStatement	Statement
  hi link dylanOther		Keyword
  hi link dylanException	Exception
  hi link dylanClassMods	StorageClass
  hi link dylanMethMods		StorageClass
  hi link dylanMiscMods		StorageClass
  hi link dylanImport		Include
  hi link dylanPreProc		PreProc
  hi link dylanPrecondit	PreCondit
  hi link dylanHeader		Macro
endif

let b:current_syntax = "dylan"

" vim:ts=8
