" Vim syntax file
" Language:    mgp - MaGic Point
" Maintainer:  Gerfried Fuchs <alfie@innocent.com>
" Filenames:   *.mgp
" Last Change: 17 Sep 2000
" URL:         http://alfie.ist.org/vim/syntax/mgp.vim
"
" Comments are very welcome - but please make sure that you are commenting on
" the latest version of this file.

syn match mgpLineSkip "\\$"

" all the commands that are currently recognized
syn keyword mgpCommand contained size fore back bgrad left leftfill center
syn keyword mgpCommand contained right shrink lcutin rcutin cont xfont vfont
syn keyword mgpCommand contained tfont tmfont tfont0 bar image newimage
syn keyword mgpCommand contained prefix icon bimage default tab vgap hgap
syn keyword mgpCommand contained pause mark again system filter endfilter
syn keyword mgpCommand contained vfcap tfdir deffont font embed endembed
syn keyword mgpCommand contained noop pcache include

" charset is not yet supported :-)
" syn keyword mgpCommand contained charset

syn region mgpFile     contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match mgpValue     contained "\d\+"
syn match mgpSize      contained "\d\+x\d\+"
syn match mgpLine      +^%.*$+ contains=mgpCommand,mgpFile,mgpSize,mgpValue

" Comments
syn match mgpPercent   +^%%.*$+
syn match mgpHash      +^#.*$+

" these only work alone
syn match mgpPage      +^%page$+
syn match mgpNoDefault +^%nodefault$+

if !exists("did_mgp_syntax_inits")
  let did_mgp_syntax_hints = 1
  hi link mgpLineSkip   Special

  hi link mgpHash       mgpComment
  hi link mgpPercent    mgpComment
  hi link mgpComment    Comment

  hi link mgpCommand    Identifier

  hi link mgpLine       Type

  hi link mgpFile       String
  hi link mgpSize       Number
  hi link mgpValue      Number

  hi link mgpPage       mgpDefine
  hi link mgpNoDefault  mgpDefine
  hi link mgpDefine     Define
endif

let b:current_syntax = "mgp"
