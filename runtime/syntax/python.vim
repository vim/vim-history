" Vim syntax file
" Language:	Python
" Maintainer:	Neil Schemenauer <nascheme@enme.ucalgary.ca>
" Last Change:	Sun 03 Oct 1999

" remove old syntax
syn clear

syn keyword pythonStatement		break continue del
syn keyword pythonStatement		except exec finally
syn keyword pythonStatement		pass print raise
syn keyword pythonStatement		return try
syn keyword pythonStatement		global assert
syn keyword pythonStatement		lambda
syn keyword pythonRepeat		for while
syn keyword pythonConditional		if elif else then
syn keyword pythonOperator		and in is not or
syn region  pythonFunction		matchgroup=pythonStatement start=+\<\(def\|class\) + end=+\>+ oneline
syn keyword pythonPreCondit		import from
syn match   pythonComment		"#.*$" contains=pythonTodo
syn keyword pythonTodo			contained TODO FIXME XXX

" numbers (including longs and complex)
syn match   pythonNumber		"\<0x\x\+[LlJj]\=\>"
syn match   pythonNumber		"\<\d\+[LljJ]\=\>"
syn match   pythonNumber		"\<\d\+\.\d\+[jJ]\=\>"
syn match   pythonNumber		"\<\d\+[eE][+-]\=\d\+[jJ]\=\>"
syn match   pythonNumber		"\<\d\+\.\d\+[eE][+-]\=\d\+[jJ]\=\>"

" strings
syn region  pythonString		matchgroup=Normal start=+'+  end=+'+ skip=+\\\\\|\\'+ contains=pythonEscape
syn region  pythonString		matchgroup=Normal start=+"+  end=+"+ skip=+\\\\\|\\"+ contains=pythonEscape
syn region  pythonString		matchgroup=Normal start=+"""+  end=+"""+ contains=pythonEscape
syn region  pythonString		matchgroup=Normal start=+'''+  end=+'''+ contains=pythonEscape
syn match  pythonEscape contained +\\[abfnrtv'"\\]+
syn match  pythonEscape contained "\\\o\o\=\o\="
syn match  pythonEscape contained "\\x\x\+"
syn match  pythonEscape "\\$"

" raw strings
syn region pythonRawString matchgroup=Normal start=+[rR]'+    end=+'+ skip=+\\\\\|\\'+
syn region pythonRawString matchgroup=Normal start=+[rR]"+    end=+"+ skip=+\\\\\|\\"+
syn region pythonRawString matchgroup=Normal start=+[rR]"""+ end=+"""+
syn region pythonRawString matchgroup=Normal start=+[rR]'''+ end=+'''+

" This is fast but code inside triple quoted strings screws it up. It
" is impossible to fix because the only way to know if you are inside a
" triple quoted string is to start from the beginning of the file. If
" you have a fast machine you can try uncommenting the "sync minlines"
" and commenting out the rest.
syn sync match pythonSync grouphere NONE "):$"
syn sync maxlines=100
"syn sync minlines=2000

if !exists("did_python_syntax_inits")
  let did_python_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link pythonStatement		Statement
  hi link pythonConditional		Conditional
  hi link pythonRepeat			Repeat
  hi link pythonString			String
  hi link pythonRawString		String
  hi link pythonOperator		Operator
  hi link pythonFunction		Function
"  hi link pythonNumber			Number
  hi link pythonPreCondit		PreCondit
  hi link pythonComment			Comment
  hi link pythonTodo			Todo
  hi link pythonEscape			Special
endif

let b:current_syntax = "python"

" vim: ts=8
