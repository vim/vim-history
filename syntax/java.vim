" Vim syntax file
" Language:	Java
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/java.vim
" Last change:	1998 Feb 6

" Note: In Java 1.0.2 it was never possible to have braces inside parens,
" so this was flagged as an error. Now this is possible (with anonymous
" classes), and therefore is no longer marked as an error. If you prefer
" the old way, put the following line into your vim startup file:
"     let java_mark_braces_in_parens_as_errors=1

" Functions are no longer highlighted, as the way to find functions depends
" on how you write java code. To enable it again put the following line
" in your startup file:
"     let java_highlight_functions=1
" This will only work if you either use two spaces for indentation or tabs.

" Remove any old syntax stuff hanging around
syn clear

" some characters that cannot be in a java program (outside a string)
syn match javaError "[\\@$`]"
syn match javaError "<<<\|\.\.\|=>\|<>\|||=\|&&=\|->"

" use separte name so that it can be deleted in javacc.vim
syn match javaError2 "#\|=<"
hi link javaError2 javaError

" keyword definitions
syn keyword javaExternal        import native package
syn keyword javaError           cast const future generic goto inner outer rest
syn keyword javaError           var byvalue operator
syn keyword javaBranch          break continue
syn keyword javaLabel           case default
syn keyword javaConditional     if else switch
syn keyword javaRepeat          while for do
syn keyword javaBoolean         true false
syn keyword javaConstant        null
syn keyword javaTypedef         this super
syn keyword javaOperator        new instanceof
syn keyword javaType            boolean char byte short int long float double
syn keyword javaType            void
syn keyword javaStatement       return
syn keyword javaStorageClass    static synchronized transient volatile final
syn keyword javaExceptions      throw try catch finally
syn keyword javaMethodDecl      synchronized throws
syn keyword javaClassDecl       abstract extends implements interface
" to differentiate the keyword class from MyClass.class we use a match here
syn match   javaClassDecl       "^class\>"
syn match   javaClassDecl       "[^.]\<class\>"ms=s+1
syn keyword javaScopeDecl       public protected private

" Comments
syn region  javaCommentString    contained start=+"+  skip=+\\\\\|\\"+  end=+"+ end=+\*/+me=s-1,he=s-1 contains=javaSpecial,javaCommentStar
syn region  javaComment2String   contained start=+"+  skip=+\\\\\|\\"+  end=+$\|"+  contains=javaSpecial
syn region  javaComment          start="/\*"  end="\*/" contains=javaCommentString,javaCharacter,javaNumber
syn match   javaCommentStar contained "^\s*\*[^/]"me=e-1
syn match   javaCommentStar contained "^\s*\*$"
hi link javaCommentString javaString
hi link javaComment2String javaString
syn match   javaLineComment      "//.*" contains=javaComment2String,javaCharacter,javaNumber

if !exists("java_ignore_javadoc")
  syn region  javaComment       start="/\*\*"  end="\*/" contains=javaCommentTitle,javaHtmlError,javaHtmlTag,javaHtmlEndTag,javaHtmlSpecialChar,javaHtmlComment,javaDocTags
  syn region  javaCommentTitle  contained matchgroup=javaComment start="/\*\*"   matchgroup=javaCommentTitle end="\.$" end="\.[ \t\r<&]"me=e-1 end="@"me=s-1,he=s-1 end="\*/"me=s-1,he=s-1 contains=javaHtmlError,javaHtmlTag,javaHtmlEndTag,javaHtmlSpecialChar,javaHtmlComment,javaCommentStar

  " syntax coloring for javadoc comments (HTML)
  syntax case ignore

  syn match javaDocTags contained "@\(see\|version\|author\|param\|return\|exception\|deprecated\)"

  syn match javaHtmlError contained "[<>&]"

  " tags
  syn match   javaHtmlSpecial  contained "\\[0-9][0-9][0-9]\|\\."
  syn region  javaHtmlString   contained start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=javaHtmlSpecial,javaScriptExpression
  syn region  javaHtmlString   contained start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=javaHtmlSpecial,javaScriptExpression
  syn match   javaHtmlValue    contained "=[\t ]*[^'" \t>][^ \t>]*"hs=s+1   contains=javaScriptExpression
  syn region  javaHtmlEndTag   contained start=+</+    end=+>+              contains=javaHtmlTagName,javaHtmlTagError
  syn region  javaHtmlTag      contained start=+<[a-zA-Z]+ end=+>+ end=+\*/+me=s-1,he=s-1 contains=javaHtmlString,javaHtmlTagName,javaHtmlArg,javaHtmlValue,javaHtmlTagError,javaHtmlEvent
  syn match   javaHtmlTagError contained "[^>]<"ms=s+1

  " tag names
  syn keyword javaHtmlTagName contained address applet area a base basefont
  syn keyword javaHtmlTagName contained big blockquote body br b caption center
  syn keyword javaHtmlTagName contained cite code dd dfn dir div dl dt em font
  syn keyword javaHtmlTagName contained form h1 h2 h3 h4 h5 h6 head hr html img
  syn keyword javaHtmlTagName contained input isindex i kbd link li link map menu
  syn keyword javaHtmlTagName contained meta ol option param pre p samp
  syn keyword javaHtmlTagName contained select small strike strong style sub sup
  syn keyword javaHtmlTagName contained table td textarea th title tr tt ul u var

  " legal arg names
  syn keyword javaHtmlArg contained action
  syn keyword javaHtmlArg contained align alink alt archive background bgcolor
  syn keyword javaHtmlArg contained border bordercolor cellpadding
  syn keyword javaHtmlArg contained cellspacing checked clear code codebase color
  syn keyword javaHtmlArg contained cols colspan content coords enctype face
  syn keyword javaHtmlArg contained gutter height href hspace
  syn keyword javaHtmlArg contained link lowsrc marginheight
  syn keyword javaHtmlArg contained marginwidth maxlength method name prompt
  syn keyword javaHtmlArg contained rel rev rows rowspan scrolling selected shape
  syn keyword javaHtmlArg contained size src start target text type url
  syn keyword javaHtmlArg contained usemap ismap valign value vlink vspace width wrap
  syn match javaHtmlArg contained "http-equiv"
  " Netscape extensions
  syn keyword javaHtmlTagName contained frame frameset nobr span
  syn keyword javaHtmlTagName contained layer ilayer nolayer spacer
  syn keyword javaHtmlArg     contained frameborder noresize pagex pagey above below
  syn keyword javaHtmlArg     contained left top visibility clip id noshade
  syn match   javaHtmlArg     contained "z-index"

  " special characters
  syn match javaHtmlSpecialChar contained "&[^;]*;"

  " The real html comments (this implements the comments as defined by html,
  " but not all html pages actually conform to it. Errors are flagged.
  syn region javaHtmlComment      contained          start=+<!+        end=+>+ contains=javaHtmlCommentPart,javaHtmlCommentError
  syn region javaHtmlComment      contained          start=+<!DOCTYPE+ end=+>+
  syn match  javaHtmlCommentError contained "[^><!]"
  syn region javaHtmlCommentPart  contained start=+--+        end=+--+

  syntax case match
endif

" match the special comment /**/
syn match   javaComment          "/\*\*/"

" Strings and constants
syn match   javaSpecialError     contained "\\."
syn match   javaSpecialCharError contained "[^']"
syn match   javaSpecialChar      contained "\\\([0-9]\{1,2\}\|[0-3][0-9][0-9]\|[\"\\'ntbrf]\|u[0-9a-fA-F]\{4\}\)"
syn region  javaString           start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaSpecialChar,javaSpecialError
syn match   javaCharacter        "'[^']*'" contains=javaSpecialChar,javaSpecialCharError
syn match   javaCharacter        "'\\''" contains=javaSpecialChar
syn match   javaCharacter        "'[^\\]'"
syn match   javaNumber           "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"

" unicode characters
syn match   javaSpecial "\\u[0-9]\{4\}"

" catch errors caused by wrong parenthesis
syn region  javaParen            transparent start="(" end=")" contains=ALLBUT,javaCommentStar,javaSpecial,javaParenError,javaDebugString,javaDebugNumber,javaDebugType,javaDebug,javaDebugBoolean,javaDebugSpecial,javaDebugSpecialCharacter,javaDebugCharacter,javaSpecialError,javaSpecialCharError,javaHtml.*
syn match   javaParenError       ")"
hi link     javaParenError       javaError

if exists("java_highlight_functions")
  " WARNING: the following definitions work only if you use one tab or two spaces
  " to indent function. If you use something different, change the leading
  " white space in all definitions.
  syn match  javaFuncDef "^\t[a-zA-Z][a-zA-Z0-9_. \[\]]*([^-+*/()]*)" contains=javaScopeDecl,javaType,javaStorageClass
  syn region javaFuncDef start=+^\t[a-zA-Z][a-zA-Z0-9_. \[\]]*([^-+*/()]*,\s*+ end=+)+ contains=javaScopeDecl,javaType,javaStorageClass
  syn match  javaFuncDef "^  [a-zA-Z][a-zA-Z0-9_. \[\]]*([^-+*/()]*)" contains=javaScopeDecl,javaType,javaStorageClass
  syn region javaFuncDef start=+^  [a-zA-Z][a-zA-Z0-9_. \[\]]*([^-+*/()]*,\s*+ end=+)+ contains=javaScopeDecl,javaType,javaStorageClass
  syn match  javaBraces  "[{}]"
endif

if exists("java_mark_braces_in_parens_as_errors")
  syn match javaInParen          contained "[{}]"
  hi link   javaInParen          javaError
endif

if exists("java_highlight_debug")
  " to make this work you must define the highlighting for these groups
  syn region javaDebug start="System\.\(out\|err\)\.print\(ln\)*\s*" end=";" contains=javaDebug.*
  syn region javaDebug  start="trace[ \t]*(" end=";" contains=javaDebug.*

  " Strings and constants
  syn match   javaDebugSpecial          contained "\\[0-9][0-9][0-9]\|\\."
  syn region  javaDebugString           contained start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaDebugSpecial
  syn match   javaDebugCharacter        contained "'[^\\]'"
  syn match   javaDebugSpecialCharacter contained "'\\.'"
  syn match   javaDebugSpecialCharacter contained "'\\''"
  syn match   javaDebugNumber           contained "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
  syn keyword javaDebugBoolean          contained true false
  syn keyword javaDebugType             contained null this super

  hi link javaDebug    Debug
  hi link javaDebugString DebugString
  hi link javaDebugType DebugType
  hi link javaDebugBoolean DebugBoolean
  hi link javaDebugNumber Debug
  hi link javaDebugSpecial DebugSpecial
  hi link javaDebugSpecialCharacter DebugSpecial
  hi link javaDebugCharacter DebugString

  " the following lines should actually be in syntax.vim
  hi link DebugString String
  hi link DebugSpecial Special
  hi link DebugBoolean Boolean
  hi link DebugType Type
endif

syn sync ccomment javaComment


if !exists("did_java_syntax_inits")
  let did_java_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link javaFuncDef                       Function
  hi link javaBraces                        Function
  hi link javaBranch                        Conditional
  hi link javaLabel                         Label
  hi link javaConditional                   Conditional
  hi link javaRepeat                        Repeat
  hi link javaExceptions                    Exception
  hi link javaStorageClass                  StorageClass
  hi link javaMethodDecl                    javaStorageClass
  hi link javaClassDecl                     javaStorageClass
  hi link javaScopeDecl                     javaStorageClass
  hi link javaBoolean                       Boolean
  hi link javaSpecial                       Special
  hi link javaSpecialError                  Error
  hi link javaSpecialCharError              Error
  hi link javaString                        String
  hi link javaCharacter                     Character
  hi link javaSpecialChar		    SpecialChar
  hi link javaNumber                        javaValue
  hi link javaError                         Error
  hi link javaStatement                     Statement
  hi link javaOperator                      Operator
  hi link javaComment                       Comment
  hi link javaLineComment                   Comment
  hi link javaConstant			    Constant
  hi link javaTypedef			    Typedef

  hi link javaCommentTitle                  SpecialComment
  hi link javaDocTags			    Special
  hi link javaCommentStar                   javaComment

  hi link javaType                          Type
  hi link javaValue                         Number
  hi link javaExternal                      Include

  hi link javaHtmlTag                       Function
  hi link javaHtmlEndTag                    Identifier
  hi link javaHtmlArg                       Type
  hi link javaHtmlTagName                   javaHtmlStatement
  hi link javaHtmlSpecialChar               SpecialChar
  hi link javaHtmlSpecial                   Special
  hi link javaHtmlString                    String
  hi link javaHtmlStatement                 Statement
  hi link javaHtmlComment                   Special
  hi link javaHtmlCommentPart               Special
  hi link javaHtmlPreProc                   PreProc
  hi link javaHtmlValue                     String
  hi link javaHtmlCommentError              javaHtmlError
  hi link javaHtmlTagError                  javaHtmlError
  hi link javaHtmlEvent                     javaScript
  hi link javaHtmlError			    Error
endif

let b:current_syntax = "java"

" vim: ts=8
