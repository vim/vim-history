" Vim syntax file
" Language:    WML
" Maintainer:  Gerfried Fuchs <alfie@innocent.com>
" URL:         http://alfie.ist.org/vim/syntax/wml.vim
" Last Change: 2000 Sep 21
"
" Comments are very welcome - but please make sure that you are commenting on
" the latest version of this file.
"
" Original Version: Craig Small <csmall@eye-net.com.au>

" A lot of the web stuff looks like HTML so we load that first
so $VIMRUNTIME/syntax/html.vim

if !exists("main_syntax")
  let main_syntax = 'wml'
endif

" Redfine htmlTag
syn clear htmlTag
syn region  htmlTag  start=+<[^/<]+ end=+>+  contains=htmlTagN,htmlString,htmlArg,htmlValue,htmlTagError,htmlEvent,htmlCssDefinition

"
" Add in extra Arguments used by wml
syn keyword htmlTagName contained gfont imgbg imgdot lowsrc 
syn keyword htmlTagName contained navbar:define navbar:header
syn keyword htmlTagName contained navbar:footer navbar:prolog
syn keyword htmlTagName contained navbar:epilog navbar:button
syn keyword htmlTagName contained navbar:filter navbar:debug
syn keyword htmlTagName contained navbar:render
syn keyword htmlTagName contained preload rollover 
syn keyword htmlTagName contained space hspace vspace over
syn keyword htmlTagName contained ps ds pi ein big sc spaced headline
syn keyword htmlTagName contained ue subheadline zwue verbcode
syn keyword htmlTagName contained isolatin pod sdf text url verbatim
syn keyword htmlTagName contained xtable
syn keyword htmlTagName contained csmap fsview import box 
syn keyword htmlTagName contained case:upper case:lower
syn keyword htmlTagName contained grid cell info lang: logo page
syn keyword htmlTagName contained set-var restore
syn keyword htmlTagName contained array:push array:show set-var ifdef
syn keyword htmlTagName contained say m4 symbol dump enter divert
syn keyword htmlTagName contained toc 
" added due to request by Alfie [2000-09-19]
syn keyword htmlTagName contained wml card do refresh oneevent catch spawn

"
" The wml arguments
syn keyword htmlArg     contained adjust background base bdcolor bdspace
syn keyword htmlArg     contained bdwidth complete copyright created crop
syn keyword htmlArg     contained direction description domainname eperlfilter
syn keyword htmlArg     contained file hint imgbase imgstar interchar interline
syn keyword htmlArg     contained keephr keepindex keywords layout spacing
syn keyword htmlArg     contained padding nonetscape noscale notag notypo
syn keyword htmlArg     contained onload oversrc pos select slices style
syn keyword htmlArg     contained subselected txtcol_select txtcol_normal
syn keyword htmlArg     contained txtonly via
" added due to request by Alfie [2000-09-19]
syn keyword htmlArg     contained mode columns localsrc ordered


" Comments fixed   -- Alfie [2000-02-25]
" Lines starting with an # are usually comments
syn match   wmlComment  "#.*"
" The different exceptions to comments
syn match   wmlSharpBang   "^#!.*"
syn match   wmlUsed     contained "\s\s*[A-Za-z:]*"
syn match   wmlUse      "^\s*#\s*use\s*" contains=wmlUsed
syn match   wmlInclude  "^\s*#\s*include.*"

syn region  wmlBody     contained start=+<<+ end=+>>+

syn match   wmlLocationed  contained "[A-Za-z]*"
syn region  wmlLocation start=+<<+ end=+>>+ contains=wmlLocationed

" fixed Divert   -- Alfie [2000-02-21]
syn match   wmlDivert   "\.\.[a-zA-Z_]\+>>"
syn match   wmlDivertEnd   "<<\.\."

syn match   wmlDefineName  contained "\s\+[A-Za-z-]\+"
syn region  htmlTagName start="\<\(define-tag\|define-region\)" end="\>" contains=wmlDefineName

" The perl include stuff
if main_syntax != 'perl'
  " Perl script
  syn include @wmlPerlScript $VIMRUNTIME/syntax/perl.vim
  syn region perlScript start=+<perl>+ keepend end=+</perl>+ contains=@wmlPerlScript,wmlPerlTag
" eperl between '<:' and ':>'  -- Alfie [1999-12-26]
  syn region perlScript start=+<:+ keepend end=+:>+ contains=@wmlPerlScript,wmlPerlTag
  syn match    wmlPerlTag  contained "</*perl>" contains=wmlPerlTagN
  "syn match   wmlPerlTag  contained "</perl>" contains=wmlPerlTagN
  syn keyword  wmlPerlTagN contained perl 

  hi link   wmlPerlTag  htmlTag
  hi link   wmlPerlTagN htmlStatement
endif

if main_syntax == "html"
  syn sync match wmlHighlight groupthere NONE "</a-zA-Z]"
  syn sync match wmlHighlight groupthere perlScript "<perl>"
  syn sync match wmlHighlightSkip "^.*['\"].*$"
  syn sync minlines=10
endif

if !exists("did_wml_syntax_inits")
  let did_wml_syntax_hints = 1
  hi link wmlUse         Include
  hi link wmlUsed        String
  hi link wmlBody        Special
  hi link wmlDiverted    Label
  hi link wmlDivert      Delimiter
  hi link wmlDivertEnd   Delimiter
  hi link wmlLocationed  Label
  hi link wmlLocation    Delimiter
  hi link wmlDefineName  String
  hi link wmlComment     Comment
  hi link wmlInclude     Include
  hi link wmlSharpBang   PreProc
endif

let b:current_syntax = "wml"
