" Vim syntax file
" Language:	XML
" Maintainer:	Paul Siegmann <pauls@euronet.nl>
" URL:		http://www.euronet.nl/~pauls/vim/syntax/xml.vim
" Last change:	1999 Jun 14

" This syntax file will highlight xml tags and arguments.
"
" Currently this is a pre-alpha 'better-then-nothing' version, and I probably
" not going to make any improvements, because it servers my purpose in its
" current form
" In other words, I copied Claudio Fleiner's <claudio@fleiner.com> html.vim
" available from http://www.fleiner.com/vim/syntax/html.vim,
" and did some global search and replace and stuff.

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" Only tags and special chars (&auml;) are highlighted
" Known tag names and arg names are colored the same way
" as statements and types, while unknwon ones as function.

" mark illegal characters
syn match xmlError "[<>&]"


" tags
syn match   xmlSpecial  contained "\\\d\d\d\|\\."
syn region  xmlString   contained start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=xmlSpecial
syn region  xmlString   contained start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=xmlSpecial
syn region  xmlEndTag             start=+</+    end=+>+              contains=xmlTagName,xmlTagError
syn region  xmlTag                start=+<[^/]+ end=+>+              contains=xmlString,xmlTagName,xmlArg,xmlValue,xmlTagError
syn match   xmlTagError contained "[^>]<"ms=s+1

" special characters
syn match xmlSpecialChar "&[^;]*;"

" server-parsed commands
syn region xmlPreProc start=+<!--#+ end=+-->+

" The real comments (this implements the comments as defined by xml,
" but not all xml pages actually conform to it. Errors are flagged.
syn region xmlComment                start=+<!+        end=+>+ contains=xmlCommentPart,xmlCommentError
syn region xmlComment                start=+<!DOCTYPE+ end=+>+
syn match  xmlCommentError contained "[^><!]"
syn region xmlCommentPart  contained start=+--+        end=+--+

" synchronizing (does not always work if a comment includes legal
" xml tags, but doing it right would mean to always start
" at the first line, which is too slow)
syn sync match xmlHighlight groupthere NONE "<[/a-zA-Z]"
syn sync match xmlHighlightSkip "^.*['\"].*$"
syn sync minlines=10

if !exists("did_xml_syntax_inits")
  let did_xml_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link xmlTag                       Function
  hi link xmlEndTag                    Identifier
  hi link xmlArg                       Type
  hi link xmlTagName                   xmlStatement
  hi link xmlValue                     Value
  hi link xmlSpecialChar               Special

  hi link xmlSpecial                   Special
  hi link xmlSpecialChar               Special
  hi link xmlString                    String
  hi link xmlStatement                 Statement
  hi link xmlComment                   Comment
  hi link xmlCommentPart               Comment
  hi link xmlPreProc                   PreProc
  hi link xmlValue                     String
  hi link xmlCommentError              xmlError
  hi link xmlTagError                  xmlError
  hi link xmlError			Error

endif

let b:current_syntax = "xml"

" vim: ts=8
