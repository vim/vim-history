" Vim syntax file
" Language:	generic ChangeLog file
" Written By:	Gediminas Paulauskas <menesis@delfi.lt>
" Maintainer:	Corinna Vinschen <vinschen@redhat.com>
" Last Change:	Sep 3, 2001

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn case ignore

syn match	changelogText	"^\s.*$" contains=changelogMail,changelogNumber,changelogMonth,changelogDay
syn match	changelogHeader	"^\S.*$" contains=changelogNumber,changelogMonth,changelogDay,changelogMail
if version < 600
  syn region	changelogFiles	start="^\s\+[+*]\s" end=":\s" end="^$" contains=changelogBullet,changelogColon keepend
  syn region	changelogFiles	start="^\s\+[([]" end=":\s" end="^$" contains=changelogBullet,changelogColon keepend
  syn match	changelogColon	contained ":\s"
else
  syn region	changelogFiles	start="^\s\+[+*]\s" end=":" end="^$" contains=changelogBullet,changelogColon,changeLogFuncs keepend
  syn region	changelogFiles	start="^\s\+[([]" end=":" end="^$" contains=changelogBullet,changelogColon,changeLogFuncs keepend
  syn match	changeLogFuncs  contained "(.\{-})" extend
  syn match	changeLogFuncs  contained "\[.\{-}]" extend
  syn match	changelogColon	contained ":"
endif
syn match	changelogBullet	contained "^\s\+[+*]\s"
syn match	changelogMail	contained "<[A-Za-z0-9\._:+-]\+@[A-Za-z0-9\._-]\+>"
syn keyword	changelogMonth	contained jan feb mar apr may jun jul aug sep oct nov dec
syn keyword	changelogDay	contained mon tue wed thu fri sat sun
syn match	changelogNumber	contained "[.-]*[0-9]\+"

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_changelog_syntax_inits")
  if version < 508
    let did_changelog_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink changelogText		Normal
  HiLink changelogBullet	Type
  HiLink changelogColon		Type
  HiLink changelogFiles		Comment
  if version >= 600
    HiLink changelogFuncs		Comment
  endif
  HiLink changelogHeader	Statement
  HiLink changelogMail		Special
  HiLink changelogNumber	Number
  HiLink changelogMonth		Number
  HiLink changelogDay		Number

  delcommand HiLink
endif

let b:current_syntax = "changelog"

" vim: ts=8
