" Vim syntax file
" Language:	Dylan
" Authors:	Justus Pendleton <justus@acm.org>
"		Brent A. Fulgham <bfulgham@debian.org>
" Last Change:	Fri Sep 29 13:45:55 PDT 2000
"
" This syntax file is based on the Haskell, Perl, Scheme, and C
" syntax files.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Part 1:  Syntax definition

syn case ignore

setlocal lisp

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

" The default highlighting.
hi def link dylanComment	Comment
hi def link dylanConstant	Constant
hi def link dylanString		String
hi def link dylanCharacter	Character
hi def link dylanNumber		Number
hi def link dylanBoolean	Boolean
hi def link dylanClass		Structure
hi def link dylanVariable	Identifier
hi def link dylanConditional	Conditional
hi def link dylanRepeat		Repeat
hi def link dylanLabel		Label
hi def link dylanOperator	Operator
hi def link dylanUnnamedDefs	Keyword
hi def link dylanNamedDefs	Keyword
hi def link dylanParamDefs	Keyword
hi def link dylanBlock		Structure
hi def link dylanSimpleDefs	Keyword
hi def link dylanStatement	Statement
hi def link dylanOther		Keyword
hi def link dylanException	Exception
hi def link dylanClassMods	StorageClass
hi def link dylanMethMods	StorageClass
hi def link dylanMiscMods	StorageClass
hi def link dylanImport		Include
hi def link dylanPreProc	PreProc
hi def link dylanPrecondit	PreCondit
hi def link dylanHeader		Macro

let b:current_syntax = "dylan"

" vim:ts=8
