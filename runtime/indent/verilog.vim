" Vim indent file
" Language:	Verilog HDL
" Maintainer:	Chih-Tsun Huang <cthuang@larc.ee.nthu.edu.tw>
" Last Change:	Wed Jul 25 22:29:03 CST 2001
" URL:          http://larc.ee.nthu.edu.tw/~cthuang/vim/indent/verilog.vim
"
" Buffer Variables:
"     b:verilog_indent_modules : indenting after the declaration
"                                of module blocks
"     b:verilog_indent_width   : indenting width
"

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetVerilogIndent()
setlocal indentkeys=!^F,o,O,0),=\*/,=begin,=end,=join,=endcase
setlocal indentkeys+==endmodule,=endfunction,=endtask,=endspecify
setlocal indentkeys+==`else,=`endif

" Only define the function once.
if exists("*GetVerilogIndent")
  finish
endif

function GetVerilogIndent()

  let offset_comment  = 3
  let offset_comment1 = 1
  let offset_comment2 = 2
  if exists('b:verilog_indent_width')
    let offset = b:verilog_indent_width
  else
    let offset = &sw
  endif
  if exists('b:verilog_indent_modules')
    let indent_modules = offset
  else
    let indent_modules = 0
  endif

  " Find a non-blank line above the current line.
  let lnum = prevnonblank(v:lnum - 1)

  " At the start of the file use zero indent.
  if lnum == 0
    return 0
  endif

  let lnum2 = prevnonblank(lnum - 1)
  let curr_line  = getline(v:lnum)
  let last_line  = getline(lnum)
  let last_line2 = getline(lnum2)
  let ind  = indent(lnum)
  let ind2 = indent(lnum - 1)

  " Indent accoding to last line
  " Multiple-line comment
  if last_line =~ '^\s*/\*'
    if last_line !~ '\*/\s*$'
      let ind = ind + offset_comment
    endif
  elseif last_line =~ '\*/\s*$'
    let ind = ind - offset_comment1
    if last_line !~ '^\s*\*/\s*$'
      let ind = ind - offset_comment2
    endif
  " One-line comment
  elseif last_line =~ '^\s*//'
    ind = ind

  " Indent after if/else/for/case/always/initial/specify/fork blocks
  elseif last_line =~ '\<\(if\|else\)\>' ||
    \ last_line =~ '^\s*\<\(for\|case\%[[zx]]\)\>' ||
    \ last_line =~ '^\s*\<\(always\|initial\)\>' ||
    \ last_line =~ '^\s*\<\(specify\|fork\)\>'
    if last_line !~ '\(;\|\<end\>\)\s*$'
      let ind = ind + offset
    endif
  " Indent after function/task blocks
  elseif last_line =~ '^\s*\<\(function\|task\)\>'
    if last_line !~ '\<end\>\s*$'
      let ind = ind + offset
    endif

  " Indent after each condition in case block
  elseif last_line =~
    \ '^\s*\(default\|[a-zA-Z0-9_]\+\|[0-9]\+.[dDhHbBoO][0-9a-fA-F_xXzZ?]\+\)\s*[,:]'
    if last_line !~ '\(;\|\<end\>\|,\)\s*$'
      let ind = ind + offset
    endif

  " Indent after module/function/task/specify/fork blocks
  elseif last_line =~ '^\s*\<module\>'
    let ind = ind + indent_modules
    if last_line =~ '[(,]\s*$'
      let ind = ind + offset
    endif

  " Indent after a 'begin' statement
  elseif last_line =~ '\(\<begin\>\)\(\s*:\s*\w\+\)*$'
    let ind = ind + offset

  " De-indent for the end of one-line block
  elseif last_line !~ '\<begin\>' &&
    \ last_line2 =~ '^\s*\<\(if\|else\|for\|always\|initial\)\>' &&
    \ last_line2 !~ '\(\<or\>\|[*(,{><+-/%^&|!=?:]\)\s*$' &&
    \ last_line2 !~ '\<begin\>'
    let ind = ind - offset

  " Multiple-line statement
  " Open statement
  elseif last_line =~ '[*(,{><+-/%^&|!=?:]\s*$' &&
    \ last_line2 !~ '[*(,{><+-/%^&|!=?:]\s*$'
    let ind = ind + offset
  " Close statement
  elseif last_line =~ '\();\|;\|)\)\s*$' &&
    \ last_line !~ '^\s*\();\|;\|)\)\s*$' &&
    \ last_line2 =~ '\(\<or\>\|[*(,{><+-/%^&|!=?:]\)\s*$' &&
    \ last_line2 !~ '\(//\|\*/\)\s*$'
    let ind = ind - offset

  " `ifdef and `else
  elseif last_line =~ '^\s*`\<\(ifdef\|else\)\>'
    let ind = ind + offset

  endif

  " Re-indent current line
  " Multiple-line comment
  if curr_line =~ '^\s*\*/'
    let ind = ind - offset_comment2

  " De-indent on the end of the block
  " join/end/endcase/endfunction/endtask/endspecify
  elseif curr_line =~ '^\s*\<\(join\|end\|endcase\)\>' ||
    \ curr_line =~ '^\s*\<\(endfunction\|endtask\|endspecify\)\>'
    let ind = ind - offset
  elseif curr_line =~ '^\s*\<endmodule\>'
    let ind = ind - indent_modules
  " De-indent on a stand-alone 'begin'
  elseif curr_line =~ '^\s*\<begin\>'
    if last_line =~ '\<\(if\|else\|for\|case\%[[zx]]\|always\|initial\)\>' ||
      \ last_line =~
      \ '^\s*\(default\|[a-zA-Z0-9_]\+\|[0-9]\+.[dDhHbBoO][0-9a-fA-F_xXzZ?]\+\)\s*:'
      let ind = ind - offset
    endif

  " De-indent after the end of multiple-line statement
  elseif curr_line =~ '^\s*)' && last_line !~ ')\s*$'
    let ind = ind - offset

  " De-indent `else and `endif
  elseif curr_line =~ '^\s*`\<\(else\|endif\)\>'
    let ind = ind - offset

  endif

  " Return the indention
  return ind
endfunction

" vim:sw=2
