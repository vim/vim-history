" Vim syntax file
" Language:	Verilog
" Maintainer:	Mun Johl <mj@core.rose.hp.com>
" Last change:  Wed Sep 17 12:58:29 PDT 1997

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful Verilog keywords
syn keyword verilogStatement   disable assign deassign force release
syn keyword verilogStatement   parameter function endfunction
syn keyword verilogStatement   always initial module endmodule
syn keyword verilogStatement   task endtask
syn keyword verilogStatement   input output inout reg wire
syn keyword verilogStatement   posedge negedge wait
syn keyword verilogStatement   buf pullup pull0 pull1 pulldown
syn keyword verilogStatement   tri0 tri1 tri trireg
syn keyword verilogStatement   wand wor triand trior
syn keyword verilogStatement   defparam
syn keyword verilogStatement   integer real
syn keyword verilogLabel       begin end fork join
syn keyword verilogConditional if else case casex casez default endcase
"syn keyword verilogConditional   ? :
syn keyword verilogRepeat        forever repeat while for
"syn keyword verilogUnaryOperator ! ~ & ~& | ^| ^ ~^
"syn keyword verilogBinaryOperator + - * / % == != === !== && || < <= > >=
"syn keyword verilogBinaryOperator >> << ^~

syn keyword verilogTodo contained TODO

syn match   verilogOperator "[&|~><!)(*#%@+/=?:;}{,.\^\-\[\]]"

syn region  verilogComment start="/\*" end="\*/"
syn match   verilogComment "//.*"

syn match   verilogGlobal "`[a-zA-Z0-9_]\+\>"
syn match   verilogGlobal "$[a-zA-Z0-9_]\+\>"

syn match   verilogConstant "\<[A-Z][A-Z0-9_]\+\>"

syn match   verilogNumber "\(\<[0-9]\+\|\)'[bdh][0-9a-fxzA-F]\+\>"
syn match   verilogNumber "\<[+-]\=[0-9]\+\>"

syn region  verilogString start=+"+  end=+"+
"Modify the following as needed.  The trade-off is performance versus
"functionality.
syn sync lines=50

if !exists("did_verilog_syntax_inits")
  let did_verilog_syntax_inits = 1
 " The default methods for highlighting.  Can be overridden later

 "hi link verilogBinaryOperator  Operator
  hi link verilogCharacter       Character
 "hi link verilogComment         Comment
  hi link verilogConditional     Conditional
 "hi link verilogLabel           Label
 "hi link verilogNumber          Number
  hi link verilogRepeat          Repeat
 "hi link verilogStatement       Statement
  hi link verilogString          String
  hi link verilogTodo            Todo

  hi link verilogComment   Comment
  hi link verilogConstant  Todo
  hi link verilogLabel     PreCondit
  hi link verilogNumber    Special
  hi link verilogOperator  Type
  hi link verilogStatement Statement
  hi link verilogGlobal    String
endif

let b:current_syntax = "verilog"

" vim: ts=8
