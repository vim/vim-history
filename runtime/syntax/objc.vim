" Vim syntax file
" Language:	    Objective C
" Maintainer:	    Kazunobu Kuriyama <kazunobu.kuriyama@nifty.com>
" Ex-maintainer:    Anthony Hodsdon <ahodsdon@fastmail.fm>
" First Author:	    Valentino Kyriakides <1kyriaki@informatik.uni-hamburg.de>
" Last Change:	    2004 May 15

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

if &filetype != 'objcpp'
  " Read the C syntax to start with
  if version < 600
    source <sfile>:p:h/c.vim
  else
    runtime! syntax/c.vim
  endif
endif

" Objective C extentions follow below
"
" NOTE: Objective C is abbreviated to ObjC/objc
" and uses *.h, *.m as file extensions!


" ObjC keywords, types, type qualifiers etc.
syn keyword objcStatement	self super _cmd
syn keyword objcType		id Class SEL IMP BOOL nil Nil
syn keyword objcTypeModifier	bycopy in out inout oneway

" Match the ObjC #import directive (like C's #include)
syn region objcImported display contained start=+"+  skip=+\\\\\|\\"+  end=+"+
syn match  objcImported display contained "<[_0-9a-zA-Z.\/]*>"
syn match  objcImport display "^\s*\(%:\|#\)\s*import\>\s*["<]" contains=objcImported

" Match the important ObjC directives
syn match  objcScopeDecl    "@public\|@private\|@protected"
syn match  objcDirective    "@interface\|@implementation"
syn match  objcDirective    "@class\|@end\|@defs"
syn match  objcDirective    "@encode\|@protocol\|@selector"

" Match the ObjC method types
"
" NOTE: here I match only the indicators, this looks
" much nicer and reduces cluttering color highlightings.
" However, if you prefer full method declaration matching
" append .* at the end of the next two patterns!
"
syn match objcInstMethod    "^\s*-\s*"
syn match objcFactMethod    "^\s*+\s*"

" To distinguish labels from method's parameters
syn region objcKeyForMethodParam display
    \ start="^\s*[_a-zA-Z0-9]*\s*:\s*("
    \ end="\()\s*)\s*[_a-zA-Z0-9]*\|)\s*[_a-zA-Z0-9]\)"
    \ contains=objcType,objcTypeModifier,cType,cStructure,cStorageClass,Type

" For Objective-C constant strings
syn match objcSpecial display "%@" contained
syn region objcString start=+\(@"\|"\)+ skip=+\\\\\|\\"+ end=+"+ contains=cFormat,cSpecial,objcSpecial

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_objc_syntax_inits")
  if version < 508
    let did_objc_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink objcImport		Include
  HiLink objcImported		cString
  HiLink objcTypeModifier	objcType
  HiLink objcType		Type
  HiLink objcScopeDecl		Statement
  HiLink objcInstMethod		Function
  HiLink objcFactMethod		Function
  HiLink objcStatement		Statement
  HiLink objcDirective		Statement
  HiLink objcKeyForMethodParam	None
  HiLink objcString		cString
  HiLink objcSpecial		Special

  delcommand HiLink
endif

let b:current_syntax = "objc"

" vim: ts=8
