" Vim syntax file
" Language: Web2C TeX texmf.cnf configuration file
" Maintainer: David Necas (Yeti) <yeti@physics.muni.cz>
" Last Change: 2001 Apr 20
" URL: http://physics.muni.cz/~yeti/download/texmf.vim
"
" Remove any old syntax stuff hanging around
" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case match
setlocal iskeyword=a-z,A-Z,48-57,-,_

syn match texmfComment "%..\+$" contains=texmfTodo
syn match texmfComment "%\s*$" contains=texmfTodo
syn match texmfPassedParameter "[-+]\=%\w\W"
syn match texmfPassedParameter "[-+]\=%\w$"
syn keyword texmfTodo TODO FIXME contained
syn match texmfNumber "\<\d\+\>"
syn match texmfVariable "\$\(\w\k*\|{\w\k*}\)"
syn region texmfString start=+"+ end=+"+ skip=+\\"\\\\+ contains=texmfVariable,texmfSpecial,texmfPassedParameter
syn match texmfSpecial +\\"\|\\$+
syn match texmfLHSStart "^\s*\w\k*" nextgroup=texmfLHSDot,texmfEquals
syn match texmfLHSVariable "\w\k*" contained nextgroup=texmfLHSDot,texmfEquals
syn match texmfLHSDot "\." contained nextgroup=texmfLHSVariable
syn match texmfEquals "\s*=" contained
syn region texmfBrace matchgroup=texmfBraceBrace start="{" end="}" contains=ALLBUT,texmfTodo,texmfBraceError,texmfLHSVariable,texmfLHSDot transparent
syn match texmfComma "," contained
syn match texmfColons ":\|;"
syn match texmfDoubleExclam "!!" contained
syn match texmfBraceError "}"

" the default highlighting.
hi def link texmfComment Comment
hi def link texmfTodo Todo
hi def link texmfVariable Identifier
hi def link texmfNumber Number
hi def link texmfString String
hi def link texmfLHSStart texmfLHS
hi def link texmfLHSVariable texmfLHS
hi def link texmfLHSDot texmfLHS
hi def link texmfLHS Type
hi def link texmfEquals Normal
hi def link texmfBraceBrace texmfDelimiter
hi def link texmfComma texmfDelimiter
hi def link texmfColons texmfDelimiter
hi def link texmfDelimiter Preproc
hi def link texmfDoubleExclam Statement
hi def link texmfPassedParameter texmfVariable
hi def link texmfSpecial Special
hi def link texmfBraceError texmfError
hi def link texmfError Error

let b:current_syntax = "texmf"

" vim: ts=8
