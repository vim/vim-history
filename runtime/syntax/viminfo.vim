" Vim syntax file
" Language:	Vim .viminfo file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Nov 19

" Remove any old syntax stuff hanging around
syn clear

" The lines that are NOT recognized
syn match viminfoError "^[^\t].*"

" The one-character one-liners that are recognized
syn match viminfoStatement "^[/&$@:?=%!<]"

" The two-character one-liners that are recognized
syn match viminfoStatement "^['->"]."
syn match viminfoStatement "^\~[/&]"
syn match viminfoStatement "^\~[hH]"
syn match viminfoStatement "^\~[mM][sS][lL][eE]\d\+\~\=[/&]"

" Comments
syn match viminfoComment "^#.*"

" The default highlighting.
hi def link viminfoComment	Comment
hi def link viminfoError	Error
hi def link viminfoStatement	Statement

let b:current_syntax = "viminfo"

" vim: ts=8
