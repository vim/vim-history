" Vim syntax file
" Filename:     ratpoison.vim
" Language:     Ratpoison configuration/commands file ( /etc/ratpoisonrc ~/.ratpoisonrc )
" Maintainer:   Doug Kearns <djkea2@mugca.cc.monash.edu.au>
" URL:          http://mugca.cc.monash.edu.au/~djkea2/vim/syntax/ratpoison.vim
" Last Change:  2001 Sep 13

" TODO: more intelligent argument highlighting

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn match   ratpoisonComment    "^\s*#.*$"  contains=ratpoisonTodo

syn keyword ratpoisonTodo       TODO NOTE FIXME XXX contained

syn match   ratpoisonNumber     "\<\d\+\>"

syn match   ratpoisonLiteral    "\<\(colon\|exec\|name\|title\|class\|left\|right\|top\|bottom\|center\)\>"

syn match   ratpoisonStringCommand   "^\s*bind\s*"
syn match   ratpoisonStringCommand   "^\s*colon\s*"
syn match   ratpoisonStringCommand   "^\s*echo\s*"
syn match   ratpoisonStringCommand   "^\s*escape\s*"
syn match   ratpoisonStringCommand   "^\s*exec\s*"
syn match   ratpoisonStringCommand   "^\s*newwm\s*"
syn match   ratpoisonStringCommand   "^\s*number\s*"
syn match   ratpoisonStringCommand   "^\s*pos\s*"
syn match   ratpoisonStringCommand   "^\s*rudeness\s*"
syn match   ratpoisonStringCommand   "^\s*select\s*"
syn match   ratpoisonStringCommand   "^\s*setenv\s*"
syn match   ratpoisonStringCommand   "^\s*source\s*"
syn match   ratpoisonStringCommand   "^\s*title\s*"
syn match   ratpoisonStringCommand   "^\s*unbind\s*"
syn match   ratpoisonStringCommand   "^\s*unsetenv\s*"

syn match   ratpoisonVoidCommand     "^\s*abort\s*$"
syn match   ratpoisonVoidCommand     "^\s*banish\s*$"
syn match   ratpoisonVoidCommand     "^\s*clock\s*$"
syn match   ratpoisonVoidCommand     "^\s*curframe\s*$"
syn match   ratpoisonVoidCommand     "^\s*delete"
syn match   ratpoisonVoidCommand     "^\s*focus\s*$"
syn match   ratpoisonVoidCommand     "^\s*help\s*$"
syn match   ratpoisonVoidCommand     "^\s*hsplit\s*$"
syn match   ratpoisonVoidCommand     "^\s*kill\s*$"
syn match   ratpoisonVoidCommand     "^\s*meta\s*$"
syn match   ratpoisonVoidCommand     "^\s*next\s*$"
syn match   ratpoisonVoidCommand     "^\s*only\s*$"
syn match   ratpoisonVoidCommand     "^\s*other\s*$"
syn match   ratpoisonVoidCommand     "^\s*prev\s*$"
syn match   ratpoisonVoidCommand     "^\s*quit\s*$"
syn match   ratpoisonVoidCommand     "^\s*redisplay\s*$"
syn match   ratpoisonVoidCommand     "^\s*remove\s*$"
syn match   ratpoisonVoidCommand     "^\s*split\s*$"
syn match   ratpoisonVoidCommand     "^\s*version\s*$"
syn match   ratpoisonVoidCommand     "^\s*vsplit\s*$"
syn match   ratpoisonVoidCommand     "^\s*windows\s*$"

syn match   ratpoisonDefCommand      "^\s*defbarloc\s*"
syn match   ratpoisonDefCommand      "^\s*msgwait\s*"
syn match   ratpoisonDefCommand      "^\s*defborder\s*"
syn match   ratpoisonDefCommand      "^\s*deffont\s*"
syn match   ratpoisonDefCommand      "^\s*definputwidth\s*"
syn match   ratpoisonDefCommand      "^\s*defmaxsizepos\s*"
syn match   ratpoisonDefCommand      "^\s*defpadding\s*"
syn match   ratpoisonDefCommand      "^\s*deftranspos\s*"
syn match   ratpoisonDefCommand      "^\s*defwaitcursor\s*"
syn match   ratpoisonDefCommand      "^\s*defwinfmt\s*"
syn match   ratpoisonDefCommand      "^\s*defwinname\s*"
syn match   ratpoisonDefCommand      "^\s*defwinpos\s*"
syn match   ratpoisonDefCommand      "^\s*deffgcolor\s*"
syn match   ratpoisonDefCommand      "^\s*defbgcolor\s*"

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_ratpoison_syn_inits")
  if version < 508
    let did_ratpoison_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink ratpoisonComment       Comment
  HiLink ratpoisonDefCommand    Identifier
  HiLink ratpoisonLiteral       Constant
  HiLink ratpoisonNumber        Number
  HiLink ratpoisonStringCommand Identifier
  HiLink ratpoisonTodo          Todo
  HiLink ratpoisonVoidCommand   Identifier

  delcommand HiLink
endif

let b:current_syntax = "ratpoison"

