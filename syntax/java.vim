" Vim syntax file
" Language:	Java
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/java.vim
" Last change:	1998 Jul 22

" Please check :help java.vim for comments on some of the options available.

" Remove any old syntax stuff hanging around
syn clear

" some characters that cannot be in a java program (outside a string)
syn match javaError "[\\@`]"
syn match javaError "<<<\|\.\.\|=>\|<>\|||=\|&&=\|[^-]->\|\*\/"

" use separte name so that it can be deleted in javacc.vim
syn match javaError2 "#\|=<"
hi link javaError2 javaError

" we define it here so that included files can test for it
if !exists("main_syntax")
  let main_syntax='java'
endif


" keyword definitions
syn keyword javaExternal        import native package
syn keyword javaBranch          break continue
syn keyword javaError           goto 
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
syn keyword javaClassDecl       extends implements interface
" to differentiate the keyword class from MyClass.class we use a match here
syn match   javaTypedef         "class"
syn match   javaClassDecl       "^class\>"
syn match   javaClassDecl       "[^.]\s*\<class\>"ms=s+1
syn keyword javaScopeDecl       public protected private abstract

syn region  javaLabelRegion     transparent matchgroup=javaLabel start="case" matchgroup=NONE end=":" contains=javaNumber
syn match   javaUserLabel       "^\s*[_$a-zA-Z][_$a-zA-Z0-9_]*\s*:"he=e-1 contains=javaLabel
syn keyword javaLabel           default

if !exists("java_allow_cpp_keywords")
  syn keyword javaError auto const delete enum extern friend inline redeclared
  syn keyword javaError register signed sizeof struct template typedef union
  syn keyword javaError unsigned operator
endif

" The following cluster contains all java groups except the contained ones
syn cluster javaTop contains=javaExternal,javaError,javaError,javaBranch,javaLabelRegion,javaLabel,javaConditional,javaRepeat,javaBoolean,javaConstant,javaTypedef,javaOperator,javaType,javaType,javaStatement,javaStorageClass,javaExceptions,javaMethodDecl,javaClassDecl,javaClassDecl,javaClassDecl,javaScopeDecl,javaError,javaError2,javaUserLabel

" Comments
syn region  javaCommentString    contained start=+"+  skip=+\\\\\|\\"+  end=+"+ end=+\*/+me=s-1,he=s-1 contains=javaSpecial,javaCommentStar
syn region  javaComment2String   contained start=+"+  skip=+\\\\\|\\"+  end=+$\|"+  contains=javaSpecial
syn match   javaCommentCharacter contained "'\\[^']\{1,6\}'" contains=javaSpecialChar
syn match   javaCommentCharacter contained "'\\''" contains=javaSpecialChar
syn match   javaCommentCharacter contained "'[^\\]'"
syn region  javaComment          start="/\*"  end="\*/" contains=javaCommentString,javaCommentCharacter,javaNumber
syn match   javaCommentStar contained "^\s*\*[^/]"me=e-1
syn match   javaCommentStar contained "^\s*\*$"
syn match   javaLineComment      "//.*" contains=javaComment2String,javaCommentCharacter,javaNumber
hi link javaCommentString javaString
hi link javaComment2String javaString
hi link javaCommentCharacter javaCharacter

syn cluster javaTop add=javaComment,javaLineComment

if !exists("java_ignore_javadoc") 
  syntax case ignore
  " syntax coloring for javadoc comments (HTML)
  syntax include @javaHtml <sfile>:p:h/html.vim
  syn region  javaComment       start="/\*\*"  end="\*/" keepend contains=javaCommentTitle,@javaHtml,javaDocTags
  syn region  javaCommentTitle  contained matchgroup=javaComment start="/\*\*"   matchgroup=javaCommentTitle keepend end="\.$" end="\.[ \t\r<&]"me=e-1 end="@"me=s-1,he=s-1 end="\*/"me=s-1,he=s-1 contains=@javaHtml,javaCommentStar
  
  syn region javaDocTags contained start="{@link" end="}"
  syn match javaDocTags contained "@\(see\|param\|exception\)\s\+\S\+" contains=javaDocParam
  syn match javaDocParam contained "\s\S\+"
  syn match javaDocTags contained "@\(version\|author\|return\|deprecated\|since\)\>"
  syntax case match
endif

" match the special comment /**/
syn match   javaComment          "/\*\*/"

" Strings and constants
syn match   javaSpecialError     contained "\\."
syn match   javaSpecialCharError contained "[^']"
syn match   javaSpecialChar      contained "\\\([4-9]\d\|[0-3]\d\d\|[\"\\'ntbrf]\|u\x\{4\}\)"
syn region  javaString           start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaSpecialChar,javaSpecialError
syn match   javaCharacter        "'[^']*'" contains=javaSpecialChar,javaSpecialCharError
syn match   javaCharacter        "'\\''" contains=javaSpecialChar
syn match   javaCharacter        "'[^\\]'"
syn match   javaNumber           "\<\(0[0-7]*\|0[xX]\x\+\|\d\+\)[lL]\=\>"
syn match   javaNumber           "\(\<\d\+\.\d*\|\.\d\+\)\([eE][-+]\=\d\+\)\=[fFdD]\="
syn match   javaNumber           "\<\d\+[eE][-+]\=\d\+[fFdD]\=\>"
syn match   javaNumber           "\<\d\+\([eE][-+]\=\d\+\)\=[fFdD]\>"

" unicode characters
syn match   javaSpecial "\\u\d\{4\}"

syn cluster javaTop add=javaString,javaCharacter,javaNumber,javaSpecial

if exists("java_highlight_functions")
  syn match  javaFuncDef "^\(\t\| \{8\}\)[_$a-zA-Z][_$a-zA-Z0-9_. \[\]]*([^-+*/()]*)" contains=javaScopeDecl,javaType,javaStorageClass
  syn region javaFuncDef start=+^\(\t\| \{8\}\)[$_a-zA-Z][$_a-zA-Z0-9_. \[\]]*([^-+*/()]*,\s*+ end=+)+ contains=javaScopeDecl,javaType,javaStorageClass
  syn match  javaFuncDef "^  [$_a-zA-Z][$_a-zA-Z0-9_. \[\]]*([^-+*/()]*)" contains=javaScopeDecl,javaType,javaStorageClass
  syn region javaFuncDef start=+^  [$_a-zA-Z][$_a-zA-Z0-9_. \[\]]*([^-+*/()]*,\s*+ end=+)+ contains=javaScopeDecl,javaType,javaStorageClass
  syn match  javaBraces  "[{}]"
  syn cluster javaTop add=javaFuncDef,javaBraces
endif

if exists("java_highlight_debug")

  " Strings and constants
  syn match   javaDebugSpecial          contained "\\\d\d\d\|\\."
  syn region  javaDebugString           contained start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaDebugSpecial
  syn match   javaDebugCharacter        contained "'[^\\]'"
  syn match   javaDebugSpecialCharacter contained "'\\.'"
  syn match   javaDebugSpecialCharacter contained "'\\''"
  syn match   javaDebugNumber           contained "\<\(0[0-7]*\|0[xX]\x\+\|\d\+\)[lL]\=\>"
  syn match   javaDebugNumber           contained "\(\<\d\+\.\d*\|\.\d\+\)\([eE][-+]\=\d\+\)\=[fFdD]\="
  syn match   javaDebugNumber           contained "\<\d\+[eE][-+]\=\d\+[fFdD]\=\>"
  syn match   javaDebugNumber           contained "\<\d\+\([eE][-+]\=\d\+\)\=[fFdD]\>"
  syn keyword javaDebugBoolean          contained true false
  syn keyword javaDebugType             contained null this super

  " to make this work you must define the highlighting for these groups
  syn region javaDebug start="System\.\(out\|err\)\.print\(ln\)*\s*" end=";" contains=javaDebug.*
  syn region javaDebug  start="trace[SL]\=[ \t]*(" end=";" contains=javaDebug.*

  syn cluster javaTop add=javaDebug

  hi link javaDebug                 Debug
  hi link javaDebugString           DebugString
  hi link javaDebugType             DebugType
  hi link javaDebugBoolean          DebugBoolean
  hi link javaDebugNumber           Debug
  hi link javaDebugSpecial          DebugSpecial
  hi link javaDebugSpecialCharacter DebugSpecial
  hi link javaDebugCharacter        DebugString

  hi link DebugString               String
  hi link DebugSpecial              Special
  hi link DebugBoolean              Boolean
  hi link DebugType                 Type
endif

if exists("java_mark_braces_in_parens_as_errors")
  syn match javaInParen          contained "[{}]"
  hi link   javaInParen          javaError
  syn cluster javaTop add=javaInParen
endif

" catch errors caused by wrong parenthesis
syn region  javaParen            transparent start="(" end=")" contains=@javaTop,javaParen
syn match   javaParenError       ")"
hi link     javaParenError       javaError

if !exists("java_minlines")
  let java_minlines = 10
endif
exec "syn sync ccomment javaComment minlines=" . java_minlines

if !exists("did_java_syntax_inits")
  let did_java_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link javaFuncDef                       Function
  hi link javaBraces                        Function
  hi link javaBranch                        Conditional
  hi link javaLabel                         Label
  hi link javaUserLabel                     Label
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
  hi link javaNumber                        Number
  hi link javaError                         Error
  hi link javaStatement                     Statement
  hi link javaOperator                      Operator
  hi link javaComment                       Comment
  hi link javaLineComment                   Comment
  hi link javaConstant			    javaBoolean
  hi link javaTypedef			    Typedef

  hi link javaCommentTitle                  SpecialComment
  hi link javaDocTags			    Special
  hi link javaDocParam			    Function
  hi link javaCommentStar                   javaComment

  hi link javaType                          Type
  hi link javaExternal                      Include

  hi link htmlComment                       Special
  hi link htmlCommentPart                   Special
endif

let b:current_syntax = "java"

if main_syntax == 'java'
  unlet main_syntax
endif

" vim: ts=8
