" Vim syntax file
" Filename:     tidy.vim 
" Language:     HMTL Tidy configuration file (~/.tidyrc)
" Maintainer:   Doug Kearns <djkea2@mugca.cc.monash.edu.au>
" URL:          http://mugca.cc.monash.edu.au/~djkea2/vim/syntax/tidy.vim
" Last Change:  2001 June 18

" TODO: color values for new*tags options

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

if version < 600
  set iskeyword=@,48-57,-
else
  setlocal iskeyword=@,48-57,-
endif

syn match   tidyComment    "^\s*//.*$" contains=tidyTodo

syn keyword tidyTodo       TODO NOTE FIXME XXX contained

syn match   tidyAssignment "^[a-z0-9-]\+:\s*.*$" contains=tidyOption,tidyDelimiter,tidyString,tidyBoolean,tidyNumber,tidyValue

syn match   tidyDelimiter  ":" contained

syn region  tidyString     start=+"+ skip=+\\\\\|\\"+ end=+"+ contained oneline
syn region  tidyString     start=+'+ skip=+\\\\\|\\'+ end=+'+ contained oneline

" ignore 1,0 as boolean values since they are undocumented
syn case ignore
syn keyword tidyBoolean    t[rue] f[alse] y[es] n[o] contained
syn case match

syn match   tidyNumber     "\<\d\+\>" contained

syn keyword tidyValue      omit auto strict loose raw ascii latin1 utf8 iso2022 contained

syn match   tidyOption     "^add-xml-decl" contained
syn match   tidyOption     "^add-xml-pi" contained
syn match   tidyOption     "^add-xml-space" contained
syn match   tidyOption     "^alt-text" contained
syn match   tidyOption     "^assume-xml-procins" contained
syn match   tidyOption     "^break-before-br" contained
syn match   tidyOption     "^char-encoding" contained
syn match   tidyOption     "^clean" contained
syn match   tidyOption     "^doctype" contained
syn match   tidyOption     "^drop-empty-paras" contained
syn match   tidyOption     "^drop-font-tags" contained
syn match   tidyOption     "^enclose-block-text" contained
syn match   tidyOption     "^enclose-text" contained
syn match   tidyOption     "^error-file" contained
syn match   tidyOption     "^fix-backslash" contained
syn match   tidyOption     "^fix-bad-comments" contained
syn match   tidyOption     "^gnu-emacs" contained
syn match   tidyOption     "^hide-endtags" contained
syn match   tidyOption     "^indent" contained
syn match   tidyOption     "^indent-attributes" contained
syn match   tidyOption     "^indent-spaces" contained
syn match   tidyOption     "^input-xml" contained
syn match   tidyOption     "^keep-time" contained
syn match   tidyOption     "^literal-attributes" contained
syn match   tidyOption     "^logical-emphasis" contained
syn match   tidyOption     "^markup" contained
syn match   tidyOption     "^new-blocklevel-tags" contained
syn match   tidyOption     "^new-empty-tags" contained
syn match   tidyOption     "^new-inline-tags" contained
syn match   tidyOption     "^new-pre-tags" contained
syn match   tidyOption     "^numeric-entities" contained
syn match   tidyOption     "^output-xhtml" contained
syn match   tidyOption     "^output-xml" contained
syn match   tidyOption     "^quiet" contained
syn match   tidyOption     "^quote-ampersand" contained
syn match   tidyOption     "^quote-marks" contained
syn match   tidyOption     "^quote-nbsp" contained
syn match   tidyOption     "^show-warnings" contained
syn match   tidyOption     "^split" contained
syn match   tidyOption     "^tab-size" contained
syn match   tidyOption     "^tidy-mark" contained
syn match   tidyOption     "^uppercase-attributes" contained
syn match   tidyOption     "^uppercase-tags" contained
syn match   tidyOption     "^word-2000" contained
syn match   tidyOption     "^wrap" contained
syn match   tidyOption     "^wrap-asp" contained
syn match   tidyOption     "^wrap-attributes" contained
syn match   tidyOption     "^wrap-jste" contained
syn match   tidyOption     "^wrap-php" contained
syn match   tidyOption     "^wrap-script-literals" contained
syn match   tidyOption     "^write-back" contained

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_tidy_syn_inits")
  if version < 508
    let did_tidy_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink tidyBoolean    Boolean
  HiLink tidyComment    Comment
  HiLink tidyDelimiter  Special
  HiLink tidyNumber     Number
  HiLink tidyOption     Identifier
  HiLink tidyString     String
  HiLink tidyTodo       Todo
  HiLink tidyValue      Constant

  delcommand HiLink
endif

let b:current_syntax = "tidy"
