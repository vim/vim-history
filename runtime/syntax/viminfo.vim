" Vim syntax file
" Language:	Vim .viminfo file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Feb 07

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" The lines that are NOT recognized
syn match viminfoError "^[^\t].*"

" The one-character one-liners that are recognized
syn match viminfoStatement "^[/&$@:?=%!<]"

" The two-character one-liners that are recognized
syn match viminfoStatement "^[-'>"]."
syn match viminfoStatement +^"".+
syn match viminfoStatement "^\~[/&]"
syn match viminfoStatement "^\~[hH]"
syn match viminfoStatement "^\~[mM][sS][lL][eE]\d\+\~\=[/&]"

syn match viminfoOption "^\*.*=" contains=viminfoOptionName
syn match viminfoOptionName "\*\a*"ms=s+1 contained

" Comments
syn match viminfoComment "^#.*"

" The default highlighting.
hi def link viminfoComment	Comment
hi def link viminfoError	Error
hi def link viminfoStatement	Statement
hi def link viminfoOption	viminfoStatement
hi def link viminfoOptionName	PreProc

let b:current_syntax = "viminfo"

" vim: ts=8
