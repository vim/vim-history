" Vim syntax file
" Language:	HTML
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/html.vim
" Last change:	1998 Jan 12

" This syntax file will highlight all known html tags and arguments.
" Unknwon tag names and unknown argument names are colored differently
" so that the error is immediatly visible.
" Javascript snippets are also colored in a way similar to java, however,
" instead of black text the normal text is colored as 'Special' which makes
" it easy to spot javascript inside html pages. Errors caused by wrong parens
" are flagged too, though for some reason the coloring does not work
" correctly.

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" Only tags and special chars (&auml;) are highlighted
" Known tag names and arg names are colored the same way
" as statements and types, while unknwon ones as function.

" mark illegal characters
syn match htmlError "[<>&]"


" tags
syn match   htmlSpecial  contained "\\[0-9][0-9][0-9]\|\\."
syn region  htmlString   contained start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=htmlSpecial,javaScriptExpression
syn region  htmlString   contained start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=htmlSpecial,javaScriptExpression
syn match   htmlValue    contained "=[\t ]*[^'" \t>][^ \t>]*"hs=s+1   contains=javaScriptExpression
syn region  htmlEndTag             start=+</+    end=+>+              contains=htmlTagName,htmlTagError
syn region  htmlTag                start=+<[^/]+ end=+>+              contains=htmlString,htmlTagName,htmlArg,htmlValue,htmlTagError,htmlEvent
syn match   htmlTagError contained "[^>]<"ms=s+1

" tag names
syn keyword htmlTagName contained address applet area a base basefont
syn keyword htmlTagName contained big blockquote body br b caption center
syn keyword htmlTagName contained cite code dd dfn dir div dl dt em font
syn keyword htmlTagName contained form h1 h2 h3 h4 h5 h6 head hr html img
syn keyword htmlTagName contained input isindex i kbd link li link map menu
syn keyword htmlTagName contained meta ol option param pre p samp
syn keyword htmlTagName contained select small strike strong style sub sup
syn keyword htmlTagName contained table td textarea th title tr tt ul u var

" legal arg names
syn keyword htmlArg contained action
syn keyword htmlArg contained align alink alt archive background bgcolor
syn keyword htmlArg contained border bordercolor cellpadding
syn keyword htmlArg contained cellspacing checked clear code codebase color
syn keyword htmlArg contained cols colspan content coords enctype face
syn keyword htmlArg contained gutter height href hspace
syn keyword htmlArg contained link lowsrc marginheight
syn keyword htmlArg contained marginwidth maxlength method name prompt
syn keyword htmlArg contained rel rev rows rowspan scrolling selected shape
syn keyword htmlArg contained size src start target text type url
syn keyword htmlArg contained usemap ismap valign value vlink vspace width wrap
syn match   htmlArg contained "http-equiv"
" Netscape extensions
syn keyword htmlTagName contained frame frameset nobr span
syn keyword htmlTagName contained layer ilayer nolayer spacer
syn keyword htmlArg     contained frameborder noresize pagex pagey above below
syn keyword htmlArg     contained left top visibility clip id noshade
syn match   htmlArg     contained "z-index"


" special characters
syn match htmlSpecialChar "&[^;]*;"

" server-parsed commands
syn region htmlPreProc start=+<!--#+ end=+-->+

" The real comments (this implements the comments as defined by html,
" but not all html pages actually conform to it. Errors are flagged.
syn region htmlComment                start=+<!+        end=+>+ contains=htmlCommentPart,htmlCommentError
syn region htmlComment                start=+<!DOCTYPE+ end=+>+
syn match  htmlCommentError contained "[^><!]"
syn region htmlCommentPart  contained start=+--+        end=+--+

" JAVA SCRIPT
syn keyword htmlTagName                contained noscript

" html events (i.e. arguments that include javascript commands)
syn region htmlEvent        contained start=+on[a-z]\+\s*=[\t ]*'+ skip=+\\\\\|\\'+ end=+'+ contains=javaScriptSpecial,javaScriptNumber,javaScriptLineComment,javaScriptComment,javaScriptStringD,javaStringCharacter,javaStringSpecialCharacter,javaScriptConditional,javaScriptRepeat,javaScriptBranch,javaScriptOperator,javaScriptType,javaScriptStatement,javaScriptFunction,javaScriptBoolean,javaScriptBraces,javaScriptParen,javaScriptParenError
syn region htmlEvent        contained start=+on[a-z]\+\s*=[\t ]*"+ skip=+\\\\\|\\"+ end=+"+ contains=javaScriptSpecial,javaScriptNumber,javaScriptLineComment,javaScriptComment,javaScriptStringS,javaStringCharacter,javaStringSpecialCharacter,javaScriptConditional,javaScriptRepeat,javaScriptBranch,javaScriptOperator,javaScriptType,javaScriptStatement,javaScriptFunction,javaScriptBoolean,javaScriptBraces,javaScriptParen,javaScriptParenError

" a javascript expression is used as an arg value
syn region  javaScriptExpression                 start=+&{+ end=+};+ contains=javaScriptSpecial,javaScriptNumber,javaScriptLineComment,javaScriptComment,javaScriptStringS,javaScriptStringD,javaStringCharacter,javaScriptSpecialCharacter,javaScriptConditional,javaScriptRepeat,javaScriptBranch,javaScriptOperator,javaScriptType,javaScriptStatement,javaScriptBoolean,javaScriptFunction

" javascript starts with <SCRIPT and ends with </SCRIPT>
syn region  javaScript                           start=+<script+ end=+</script>+ contains=javaScriptSpecial,javaScriptNumber,javaScriptLineComment,javaScriptComment,javaScriptStringS,javaScriptStringD,javaStringCharacter,javaStringSpecialCharacter,javaScriptConditional,javaScriptRepeat,javaScriptBranch,javaScriptOperator,javaScriptType,javaScriptStatement,javaScriptFunction,javaScriptBoolean,javaScriptBraces,javaScriptParen,javaScriptParenError
syn match   javaScriptLineComment      contained "\/\/.*$"
syn match   javaScriptCommentSkip      contained "^\s*\*\($\|\s\+\)"
syn region  javaScriptCommentString    contained start=+"+  skip=+\\\\\|\\"+  end=+"+ end=+\*/+me=s-1,he=s-1 contains=javaScriptSpecial,javaScriptCommentSkip
syn region  javaScriptComment2String   contained start=+"+  skip=+\\\\\|\\"+  end=+$\|"+  contains=javaScriptSpecial
syn region  javaScriptComment          contained start="/\*"  end="\*/" contains=javaScriptCommentString,javaScriptCharacter,javaScriptNumber
syn match   javaScriptSpecial          contained "\\[0-9][0-9][0-9]\|\\."
syn region  javaScriptStringD          contained start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaScriptSpecial
syn region  javaScriptStringS          contained start=+'+  skip=+\\\\\|\\'+  end=+'+  contains=javaScriptSpecial
syn match   javaScriptSpecialCharacter contained "'\\.'"
syn match   javaScriptNumber           contained "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn keyword javaScriptConditional      contained if else
syn keyword javaScriptRepeat           contained while for
syn keyword javaScriptBranch           contained break continue
syn keyword javaScriptOperator         contained new in
syn keyword javaScriptType             contained this var
syn keyword javaScriptStatement        contained return with
syn keyword javaScriptFunction         contained function
syn keyword javaScriptBoolean          contained true false
syn match   javaScriptBraces           contained "[{}]"
" catch errors caused by wrong parenthesis
syn region  javaScriptParen            contained start="(" end=")" contains=javaScriptSpecial,javaScriptNumber,javaScriptLineComment,javaScriptComment,javaScriptStringS,javaScriptStringD,javaStringCharacter,javaStringSpecialCharacter,javaScriptConditional,javaScriptRepeat,javaScriptBranch,javaScriptOperator,javaScriptType,javaScriptStatement,javaScriptFunction,javaScriptBoolean,javaScriptBraces,javaScriptParen
syn match   javaScriptParenError       contained ")"
syn match   javaScriptInParen          contained "[{}]"

" synchronizing (does not always work if a comment includes legal
" html tags, but doing it right would mean to always start
" at the first line, which is too slow)
syn sync match htmlHighlight groupthere NONE "<[/a-zA-Z]"
syn sync match htmlHighlight groupthere javaScript "<script"
syn sync match htmlHighlightSkip "^.*['\"].*$"
syn sync minlines=10

if !exists("did_html_syntax_inits")
  let did_html_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link htmlTag                       Function
  hi link htmlEndTag                    Identifier
  hi link htmlArg                       Type
  hi link htmlTagName                   htmlStatement
  hi link htmlValue                     Value
  hi link htmlSpecialChar               Special

  hi link htmlSpecial                   Special
  hi link htmlSpecialChar               Special
  hi link htmlString                    String
  hi link htmlStatement                 Statement
  hi link htmlComment                   Comment
  hi link htmlCommentPart               Comment
  hi link htmlPreProc                   PreProc
  hi link htmlValue                     String
  hi link htmlCommentError              htmlError
  hi link htmlTagError                  htmlError
  hi link htmlEvent                     javaScript
  hi link htmlError			Error

  hi link javaScript                    Special
  hi link javaScriptExpression          javaScript
  hi link javaScriptComment             Comment
  hi link javaScriptLineComment         Comment
  hi link javaScriptSpecial             javaScript
  hi link javaScriptStringS             String
  hi link javaScriptStringD             String
  hi link javaScriptCharacter           Character
  hi link javaScriptSpecialCharacter    javaScriptSpecial
  hi link javaScriptNumber              javaScriptValue
  hi link javaScriptConditional         Conditional
  hi link javaScriptRepeat              Repeat
  hi link javaScriptBranch              Conditional
  hi link javaScriptOperator            Operator
  hi link javaScriptType                Type
  hi link javaScriptStatement           Statement
  hi link javaScriptFunction            Function
  hi link javaScriptBoolean             Boolean
  hi link javaScriptError               Error
  hi link javaScriptBraces              Function
  hi link javaScriptParenError          javaScriptError
  hi link javaScriptInParen             javaScriptError
  hi link javaScriptParen               javaScript

endif

let b:current_syntax = "html"

" vim: ts=8
