" Vim syntax file
" Language:	JAL
" Version: 	0.1
" Last Change:	2003 Apr 27
" Maintainer:	Mark Gross <mark@thegnar.org>
" Previous Maintainer:
" This is a Hack job on the Pascal.vim syntax file.
" I'm leaving some of the pascal syntax declarations for
" a cheat sheet as I add to this syntax declaration.

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif


syn case ignore
syn sync lines=250

syn keyword jalBoolean		true false
syn keyword jalBit		high low
syn keyword jalConstant		Input Output all_input all_output
syn keyword jalConditional	if else then elsif end if
syn keyword jalLabel		goto
syn keyword jalRepeat		for while forever loop
syn keyword jalStatement	procedure function
syn keyword jalStatement	return end const var
syn keyword jalType		bit byte

syn keyword jalModifier		interrupt volatile
syn keyword jalStatement	out in is begin
syn keyword jalDirective	pragma
syn keyword jalPredefined	blabla

syn keyword jalFunction		Abs Arctan Cos Exp Ln Sin Sqr Sqrt

syn keyword jalPIC		local

syn region  jalAsm		matchgroup=jalAsmKey start="\<assembler\>" end="\<end assembler\>" contains=jalComment,jalPreProc,jalLabel,jalPIC
syn region  jalAsm		matchgroup=jalAsmKey start="\<asm\>" end=/$/ contains=jalComment,jalPreProc,jalPIC


syn match  jalStringEscape	contained "#[12][0-9]\=[0-9]\="
syn match   jalIdentifier		"\<[a-zA-Z_][a-zA-Z0-9_]*\>"
syn match   jalSymbolOperator		"[+\-/*=]"
syn match   jalSymbolOperator		"!"
syn match   jalSymbolOperator		"<"
syn match   jalSymbolOperator		">"
syn match   jalSymbolOperator		"<="
syn match   jalSymbolOperator		">="
syn match   jalSymbolOperator		"!="
syn match   jalSymbolOperator		"=="
syn match   jalSymbolOperator		"<<"
syn match   jalSymbolOperator		">>"
syn match   jalSymbolOperator		"|"
syn match   jalSymbolOperator		"&"
syn match   jalSymbolOperator		"%"
syn match   jalSymbolOperator		"[()]"
syn match   jalSymbolOperator		"[\^.]"
syn match   jalLabel			"[\^]*:"

syn match  jalNumber		"-\=\<\d\+\>"
syn match  jalNumber		"-\=\<\d[0-9_]\+\>"
syn match  jalFloat		"-\=\<\d\+\.\d\+\>"
syn match  jalFloat		"-\=\<\d\+\.\d\+[eE]-\=\d\+\>"
syn match  jalHexNumber		"0x[0-9A-Fa-f_]\+\>"
syn match  jalBinNumber		"0b[01_]\+\>"

" String
"wrong strings
syn region  jalStringError matchgroup=jalStringError start=+'+ end=+'+ end=+$+ contains=jalStringEscape

"right strings
syn region  jalString matchgroup=jalString start=+'+ end=+'+ oneline contains=jalStringEscape
" To see the start and end of strings:
syn region  jalString matchgroup=jalString start=+"+ end=+"+ oneline contains=jalStringEscapeGPC

syn keyword jalTodo contained	TODO
syn region jalComment		start=/-- /  end=/$/ oneline contains=jalTodo
syn match  jalComment		/--\_$/
syn region jalPreProc		start="include"  end=/$/ contains=JalComment,jalToDo


if exists("jal_no_tabs")
	syn match jalShowTab "\t"
endif


" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_jal_syn_inits")
if version < 508
  let did_jal_syn_inits = 1
  command -nargs=+ HiLink hi link <args>
else
  command -nargs=+ HiLink hi def link <args>
endif

  HiLink jalAcces		jalStatement
  HiLink jalBoolean		Boolean
  HiLink jalBit			Boolean
  HiLink jalComment		Comment
  HiLink jalConditional		Conditional
  HiLink jalConstant		Constant
  HiLink jalDelimiter		Identifier
  HiLink jalDirective		jalStatement
  HiLink jalException		Exception
  HiLink jalFloat		Float
  HiLink jalFunction		Function
  HiLink jalLabel		Label
  HiLink jalMatrixDelimiter	Identifier
  HiLink jalModifier		Type
  HiLink jalNumber		Number
  HiLink jalBinNumber		Number
  HiLink jalHexNumber		Number
  HiLink jalOperator		Operator
  HiLink jalPredefined		jalStatement
  HiLink jalPreProc		PreProc
  HiLink jalRepeat		Repeat
  HiLink jalStatement		Statement
  HiLink jalString		String
  HiLink jalStringEscape	Special
  HiLink jalStringEscapeGPC	Special
  HiLink jalStringError		Error
  HiLink jalStruct		jalStatement
  HiLink jalSymbolOperator	jalOperator
  HiLink jalTodo		Todo
  HiLink jalType		Type
  HiLink jalUnclassified	jalStatement
  HiLink jalAsm			Assembler
  HiLink jalError		Error
  HiLink jalAsmKey		jalStatement
  HiLink jalPIC			jalStatement

  HiLink jalShowTab		Error

  delcommand HiLink
endif


let b:current_syntax = "jal"

" vim: ts=8 sw=2
