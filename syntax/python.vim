" Vim syntax file
" Language:	Python
" Maintainer:	Neil Schemenauer <nascheme@acs.ucalgary.ca>
" Last change:	1998 January 17

" Remove any old syntax stuff hanging around
syn clear

syn keyword pythonStatement		break continue del
syn keyword pythonStatement		except exec finally
syn keyword pythonStatement		pass print raise
syn keyword pythonStatement		return try
syn keyword pythonRepeat		for while
syn keyword pythonConditional		if elif else then
syn keyword pythonOperator		and in is not or
syn region  pythonString		start=+'+  end=+'+ skip=+\\\\\|\\'+
syn region  pythonString		start=+"+  end=+"+ skip=+\\\\\|\\"+
syn region  pythonString		start=+"""+  end=+"""+
syn region  pythonString		start=+'''+  end=+'''+
syn keyword pythonFunction		def class lambda
syn keyword pythonPreCondit		import from
syn match   pythonComment		"#.*$" contains=pythonTodo
syn keyword pythonTodo			contained TODO FIXME XXX

syn sync lines=100

if !exists("did_python_syntax_inits")
  let did_python_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link pythonStatement		Statement
  hi link pythonConditional		Conditional
  hi link pythonRepeat			Repeat
  hi link pythonString			String
  hi link pythonOperator		Operator
  hi link pythonFunction		Function
  hi link pythonPreCondit		PreCondit
  hi link pythonComment			Comment
  hi link pythonTodo			Todo
endif

let b:current_syntax = "python"

" vim: ts=8
