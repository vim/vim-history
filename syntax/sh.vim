" Vim syntax file
" Language:	shell (sh) Korn shell (ksh) bash (sh)
" Maintainer:	Lennart Schultz <Lennart.Schultz@ecmwf.int>
" Last change:	1997 November 24
"
" Using the following VIM variables:
" is_kornshell               if defined enhance with kornshell syntax
" is_bash                    if defined enhance with bash syntax
"
" This is a complete redesign including many ideas from
" Éric Brunet (eric.brunet@ens.fr)
"
" Updated 1997 November 21
"	With many good inputs from Éric Brunet, especially he got the
"	case statement to work!
" Updated 1997 November 24
" 	Sync included with thanks to Dr. Charles E. Campbell Jr <cec@gryphon.gsfc.nasa.gov>
"	Echo included with thanks to Éric Brunet
"	[ \t] removed using \s instead

" Remove any old syntax stuff hanging around
syn clear
" sh syntax is case sensitive
syn case match

syn keyword	shTodo		contained TODO
syn match	shComment		"#.*$" contains=shTodo

" String and Character constants
"===============================
syn match   shNumber       "-\=\<[0-9]\+\>"
syn match   shSpecial      contained "\\[0-9][0-9][0-9]\|\\[abcfnrtv]"
syn region  shSinglequote matchgroup=shOperator start=+'+ end=+'+
syn region  shDoubleQuote      matchgroup=shOperator start=+"+ skip=+\\"+ end=+"+ contains=shDeref,shCommandSub,shSpecialShellVar,shSpecial
syn match  shSpecial  "\\[\\\"\'`$]"
	" This must be after the strings, so that bla \" be correct
syn region shEmbeddedEcho contained matchgroup=shStatement start="\<echo\>" skip="\\$" matchgroup=shOperator end="$" matchgroup=NONE end="[<>;&|`]"me=e-1 end="[0-9][<>]"me=e-2 end="#"me=e-1 contains=shNumber,shSinglequote,shDeref,shSpecialVar,shSpecial,shOperator,shDoubleQuote
 	" This one is needed INSIDE a CommandSub, so that
 	" `echo bla` be correct
syn region shEcho matchgroup=shStatement start="\<echo\>" skip="\\$" matchgroup=shOperator end="$" matchgroup=NONE end="[<>;&|]"me=e-1 end="[0-9][<>]"me=e-2 end="#"me=e-1 contains=shNumber,shCommandSub,shSinglequote,shDeref,shSpecialVar,shSpecial,shOperator,shDoubleQuote

" Tests
"======
if exists("is_kornshell")
syn region  shNone transparent matchgroup=shOperator start="\[\[" skip=+\\\\\|\\$+ end="\]\]" contains=ALLBUT,shFunction,shTestError,shIdentifier,shCase,shDTestError
endif
syn region  shNone transparent matchgroup=shOperator start="\[" skip=+\\\\\|\\$+ end="\]" contains=ALLBUT,shFunction,shTestError,shIdentifier,shCase,shDTestError
syn region  shNone transparent matchgroup=shStatement start="\<test\>" skip=+\\\\\|\\$+ matchgroup=NONE end="[;&|]"me=e-1 end="$" contains=ALLBUT,shFunction,shIdentifier,shCase
syn match   shTestOpr contained "[!=]\|-.\>\|-\(nt\|ot\|ef\|eq\|ne\|lt\|le\|gt\|ge\)\>"

syn region  shDo transparent matchgroup=shStatement start="\<do\>" end="\<done\>" contains=ALLBUT,shFunction,shDoError,shCase
syn region  shIf transparent matchgroup=shStatement start="\<if\>" end="\<fi\>" contains=ALLBUT,shFunction,shIfError,shCase
syn region  shFor  matchgroup=shStatement start="\<for\>" end="\<in\>" contains=ALLBUT,shFunction,shInError,shCase
syn region shCaseEsac transparent matchgroup=shStatement start="\<case\>" matchgroup=NONE end="\<in\>"me=s-1 contains=ALLBUT,shFunction,shCaseError nextgroup=shCaseEsac
syn region shCaseEsac matchgroup=shStatement start="\<in\>" end="\<esac\>" contains=ALLBUT,shFunction,shCaseError
syn region shCase matchgroup=shStatement contained start="\s*.\+)"  end=";;" contains=ALLBUT,shFunction,shCaseError,shCase
syn region  shNone transparent matchgroup=shOperator start="{" end="}" contains=ALLBUT,shCurlyError,shCase
syn region  shSubSh transparent matchgroup=shOperator start="(" end=")" contains=ALLBUT,shParenError,shCase

" Misc
"=====
syn match   shOperator		"[!&;|=]"
syn match   shWrapLineOperator	"\\$"
syn region  shCommandSub   start="`" skip="\\`" end="`" contains=ALLBUT,shFunction,shCommandSub,shTestOpr,shCase,shEcho

syn match   shSource	"^\.\s"
syn match   shSource	"\s\.\s"
syn region  shColon	start="^\s*:" end="$\|" end="#"me=e-1 contains=ALLBUT,shFunction,shTestOpr,shCase

" File redirection highlighted as operators
"==========================================
syn match	shRedir	"[0-9]\=>\(&[-0-9]\)\="
syn match	shRedir	"[0-9]\=>>-\="
syn match	shRedir	"[0-9]\=<\(&[-0-9]\)\="
syn match	shRedir	"[0-9]<<-\="
" Future:
"syn region      shRedir start="<<\([! 	;&|]\+" end="\1"

" Identifiers
"============
syn match  shIdentifier "\<[a-zA-Z_][a-zA-Z0-9_]*\>="me=e-1
if exists("is_bash")
syn region shIdentifier matchgroup=shStatement start="\<\(declare\|typeset\|local\|export\|set\|unset\)\>[^/]"me=e-1 matchgroup=shOperator skip="\\$" end="$\|[;&]" matchgroup=NONE end="#\|="me=e-1 contains=shTestError,shCurlyError,shWrapLineOperator
elseif exists("is_kornshell")
syn region shIdentifier matchgroup=shStatement start="\<\(typeset\|set\|export\|unset\)\>[^/]"me=e-1 matchgroup=shOperator skip="\\$" end="$\|[;&]" matchgroup=NONE end="#\|="me=e-1 contains=shTestError,shCurlyError,shWrapLineOperator
else
syn region shIdentifier matchgroup=shStatement start="\<\(set\|export\|unset\)\>[^/]"me=e-1 matchgroup=shOperator skip="\\$" end="$\|[;&]" matchgroup=NONE end="#\|="me=e-1 contains=shTestError,shCurlyError,shWrapLineOperator
endif
		" The [^/] in the start pattern is a kludge to avoid bad
		" highlighting with cd /usr/local/lib...

syn region  shFunction transparent matchgroup=shFunctionName 	start="^\s*\<[a-zA-Z_][a-zA-Z0-9_]*\>\s*()\s*{" end="}" contains=ALLBUT,shFunction,shCurlyError,shCase
syn region shDeref	     start="\${" end="}"
syn match  shDeref	     "\$\<[a-zA-Z_][a-zA-Z0-9_]*\>"
syn match  shSpecialShellVar "\$[-#@*$?!0-9]"

"Error Codes
syn match   shDoError "\<done\>"
syn match   shIfError "\<fi\>"
syn match   shInError "\<in\>"
syn match     shCaseError ";;"
syn match   shEsacError "\<esac\>"
syn match     shCurlyError "}"
syn match     shParenError ")"
if exists("is_kornshell")
syn match     shDTestError "]]"
endif
syn match     shTestError "]"

" A bunch of useful sh keywords
syn keyword shStatement    break cd chdir continue eval exec
syn keyword shStatement    exit kill newgrp pwd read readonly
syn keyword shStatement    return shift test trap
syn keyword shStatement    ulimit umask wait
syn keyword shConditional  else then elif while

if exists("is_kornshell") || exists("is_bash")
syn keyword shStatement    bg fg jobs stop suspend
syn keyword shFunction     function
syn keyword shStatement    alias fc getopts hash history let print time
syn keyword shStatement    times type whence unalias
syn keyword shRepeat       select until
if exists("is_bash")
syn keyword shStatement    source bind builtin dirs
syn keyword shStatement    disown enable help history
syn keyword shStatement    logout popd pushd shopt
else
syn keyword shStatement    login newgrp
endif
endif

" Syncs
" =====
syn sync minlines=50
syn sync match shDoSync       grouphere  shDo       "\<do\>"
syn sync match shDoSync       groupthere shDo       "\<done\>"
syn sync match shIfSync       grouphere  shIf       "\<if\>"
syn sync match shIfSync       groupthere shIf       "\<fi\>"
syn sync match shForSync      grouphere  shFor      "\<for\>"
syn sync match shForSync      groupthere shFor      "\<in\>"
syn sync match shCaseEsacSync grouphere  shCaseEsac "\<case\>"
syn sync match shCaseEsacSync groupthere shCaseEsac "\<esac\>"

if !exists("did_sh_syntax_inits")
  let did_sh_syntax_inits = 1
" The default methods for highlighting.  Can be overridden later
  hi link shSinglequote		shString
  hi link shDoubleQuote		shString
  hi link shSource		shOperator
  hi link shWrapLineOperator	shOperator
  hi link shColon		shStatement
  hi link shRedir		shOperator
  hi link shSpecialShellVar	shShellVariables
  hi link shDeref		shShellVariables
  hi link shTestOpr		shConditional
  hi link shEcho		shString
  hi link shEmbeddedEcho	shString
  hi link shInError		Error
  hi link shCaseError		Error
  hi link shEsacError		Error
  hi link shDoError		Error
  hi link shIfError		Error
  hi link shCurlyError		Error
  hi link shParenError		Error
  hi link shTestError		Error
if exists("is_kornshell")
  hi link shDTestError		Error
endif
  hi link shCommandSub		Special
  hi link shFunctionName	Function
  hi link shStatement		Statement
  hi link shConditional		Conditional
  hi link shNumber		Number
  hi link shString		String
  hi link shComment		Comment
  hi link shSpecial		Special
  hi link shTodo		Todo
  hi link shIdentifier		Identifier
  hi link shShellVariables	PreProc
  hi link shOperator		Operator
  hi link shFunction	        Function
  hi link shRepeat		Repeat
endif

let b:current_syntax = "sh"

" vim: ts=8
