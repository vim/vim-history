" Vim syntax file
" Language:	Simula
" Maintainer:	Haakon Riiser <hakonrk@fys.uio.no>
" Last change:	1998 Mar 26

" Clear old syntax defs
syn clear

" No case sensitivity in Simula
syn case	ignore

syn match	simulaComment		"^%.*$" contains=simulaTodo
syn region	simulaComment		start="!\|comment" end=";" contains=simulaTodo

" Text between the keyword 'end' and either a semicolon or one of the
" keywords 'end', 'else', 'when' or 'otherwise' is also a comment
syn region	simulaComment		start="\<end\>"lc=3 matchgroup=simulaKeyword end=";\|\<\(end\|else\|when\|otherwise\)\>"

syn match	simulaCharacter		"'.\{-}'" contains=simulaSpecialChar
syn match	simulaString		'".\{-}"' contains=simulaSpecialChar,simulaTodo

" Integer number (or float without `.')
syn match	simulaNumber		"\<[0-9]\+\>"
" Real with optional exponent
syn match	simulaReal		"\<[0-9]\+\(\.[0-9]\+\)\=\(&&\=[+-]\=[0-9]\+\)\=\>"
" Real starting with a `.', optional exponent
syn match	simulaReal		"\.[0-9]\+\(&&\=[+-]\=[0-9]\+\)\=\>"

syn keyword	simulaBoolean		true false
syn keyword	simulaConditional	else if otherwise then until when
syn keyword	simulaConstant		none notext
syn keyword	simulaFunction		procedure
syn keyword	simulaKeyword		do begin end step this qua inner
syn keyword	simulaOperator		eq eqv ge gt imp in is le lt ne not
syn keyword	simulaRepeat		while for
syn keyword	simulaStatement		inspect new
syn keyword	simulaStorageClass	external hidden name protected
syn keyword	simulaStructure		class
syn keyword	simulaType		array boolean character integer long real short text value virtual
syn match	simulaAssigned		"\<[a-z_][a-z0-9_]*\s*\((.*)\)\=\s*:\(=\|-\)"me=e-2
syn match	simulaOperator		"\<and\(\s\+then\)\=\>"
syn match	simulaOperator		"\<or\(\s\+else\)\=\>"
syn match	simulaReferenceType	"\<ref\s*(.\{-})"
syn match	simulaSemicolon		";"
syn match	simulaSpecial		"[(),.&:=<>+\-*/]"
syn match	simulaSpecialChar	"![0-9]\+!" contained
syn match	simulaTodo		"XXX\+" contained

if !exists("did_simula_syntax_inits")
	let did_simula_syntax_inits = 1
	hi link simulaAssigned		Identifier
	hi link simulaBoolean		Boolean
	hi link simulaCharacter		Character
	hi link simulaComment		Comment
	hi link simulaConditional	Conditional
	hi link simulaConstant		Constant
	hi link simulaFunction		Function
	hi link simulaKeyword		Keyword
	hi link simulaNumber		Number
	hi link simulaOperator		Special
	hi link simulaReal		Float
	hi link simulaReferenceType	Type
	hi link simulaRepeat		Repeat
	hi link simulaSemicolon		Statement
	hi link simulaSpecial		Special
	hi link simulaSpecialChar	SpecialChar
	hi link simulaStatement		Statement
	hi link simulaStorageClass	StorageClass
	hi link simulaString		String
	hi link simulaStructure		Structure
	hi link simulaTodo		Todo
	hi link simulaType		Type
endif

let b:current_syntax = "simula"
" vim: ts=8 nowrap
