" Vim syntax file
" Language:	Haskell
" Maintainer:	John Williams <jrw@pobox.com>
" Last Change:	2000 Nov 04
" Thanks to Ryan Crumley for suggestions and John Meacham for
" pointing out bugs.
"
" Options-assign a value to these variables to turn the option on:
"
" hs_highlight_delimiters - Highlight delimiter characters--users
"                           with a light-colored background will
"                           probably want to turn this on.
" hs_highlight_boolean - Treat True and False as keywords.
" hs_highlight_types - Treat names of primitive types as keywords.
" hs_highlight_more_types - Treat names of other common types as keywords.
" hs_highlight_debug - Highlight names of debugging functions.

" Remove any old syntax stuff hanging around
syn clear

" (Qualified) identifiers (no default highlighting)
syn match ConId "\(\<[A-Z][a-zA-Z0-9_']*\.\)\=\<[A-Z][a-zA-Z0-9_']*\>"
syn match VarId "\(\<[A-Z][a-zA-Z0-9_']*\.\)\=\<[a-z][a-zA-Z0-9_']*\>"

" Infix operators--most punctuation characters and any (qualified) identifier
" enclosed in `backquotes`. An operator starting with : is a constructor,
" others are variables (e.g. functions).
syn match hsVarSym "\(\<[A-Z][a-zA-Z0-9_']*\.\)\=[-!#$%&\*\+/<=>\?@\\^|~.][-!#$%&\*\+/<=>\?@\\^|~:.]*"
syn match hsConSym "\(\<[A-Z][a-zA-Z0-9_']*\.\)\=:[-!#$%&\*\+./<=>\?@\\^|~:]*"
syn match hsVarSym "`\(\<[A-Z][a-zA-Z0-9_']*\.\)\=[a-z][a-zA-Z0-9_']*`"
syn match hsConSym "`\(\<[A-Z][a-zA-Z0-9_']*\.\)\=[A-Z][a-zA-Z0-9_']*`"

" Reserved symbols--cannot be overloaded.
syn match hsDelimiter  "(\|)\|\[\|\]\|,\|;\|_\|{\|}"

" Strings and constants
syn match   hsSpecialChar      contained "\\\([0-9]\+\|o[0-7]\+\|x[0-9a-fA-F]\+\|[\"\\'&\\abfnrtv]\|^[A-Z^_\[\\\]]\)"
syn match   hsSpecialChar      contained "\\\(NUL\|SOH\|STX\|ETX\|EOT\|ENQ\|ACK\|BEL\|BS\|HT\|LF\|VT\|FF\|CR\|SO\|SI\|DLE\|DC1\|DC2\|DC3\|DC4\|NAK\|SYN\|ETB\|CAN\|EM\|SUB\|ESC\|FS\|GS\|RS\|US\|SP\|DEL\)"
syn match   hsSpecialCharError contained "\\&\|'''\+"
syn region  hsString           start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=hsSpecialChar
syn match   hsCharacter        "[^a-zA-Z0-9_']'\([^\\]\|\\[^']\+\|\\'\)'"lc=1 contains=hsSpecialChar,hsSpecialCharError
syn match   hsCharacter        "^'\([^\\]\|\\[^']\+\|\\'\)'" contains=hsSpecialChar,hsSpecialCharError
syn match   hsNumber           "\<[0-9]\+\>\|\<0[xX][0-9a-fA-F]\+\>\|\<0[oO][0-7]\+\>"
syn match   hsFloat            "\<[0-9]\+\.[0-9]\+\([eE][-+]\=[0-9]\+\)\=\>"

" Keyword definitions. These must be patters instead of keywords
" because otherwise they would match as keywords at the start of a
" "literate" comment (see lhs.vim).
syn match hsModule          "\<module\>"
syn match hsImport          "\<import\>.*"he=s+6 contains=hsImportMod
syn match hsImportMod       contained "\<\(as\|qualified\|hiding\)\>"
syn match hsInfix           "\<\(infix\|infixl\|infixr\)\>"
syn match hsStructure       "\<\(class\|data\|deriving\|instance\|default\|where\)\>"
syn match hsTypedef         "\<\(type\|newtype\)\>"
syn match hsStatement       "\<\(do\|case\|of\|let\|in\)\>"
syn match hsConditional     "\<\(if\|then\|else\)\>"

" Not real keywords, but close.
if exists("hs_highlight_boolean")
  " Boolean constants from the standard prelude.
  syn match hsBoolean "\<\(True\|False\)\>"
endif
if exists("hs_highlight_types")
  " Primitive types from the standard prelude and libraries.
  syn match hsType "\<\(Int\|Integer\|Char\|Bool\|Float\|Double\|IO\|Void\|Addr\|Array\|String\)\>"
endif
if exists("hs_highlight_more_types")
  " Types from the standard prelude libraries.
  syn match hsType "\<\(Maybe\|Either\|Ratio\|Complex\|Ordering\|IOError\|IOResult\|ExitCode\)\>"
  syn match hsMaybe    "\<Nothing\>"
  syn match hsExitCode "\<\(ExitSuccess\)\>"
  syn match hsOrdering "\<\(GT\|LT\|EQ\)\>"
endif
if exists("hs_highlight_debug")
  " Debugging functions from the standard prelude.
  syn match hsDebug "\<\(undefined\|error\|trace\)\>"
endif


" Comments
syn match   hsLineComment      "--.*"
syn region  hsBlockComment     start="{-"  end="-}" contains=hsBlockComment
syn region  hsPragma           start="{-#" end="#-}"

" Literate comments--any line not starting with '>' is a comment.
if exists("b:hs_literate_comments")
  syn region  hsLiterateComment   start="^" end="^>"
endif

if !exists("hs_minlines")
  let hs_minlines = 50
endif
exec "syn sync lines=" . hs_minlines

" The default highlighting.
hi def link hsModule           hsStructure
hi def link hsImport           Include
hi def link hsImportMod        hsImport
hi def link hsInfix            PreProc
hi def link hsStructure        Structure
hi def link hsStatement        Statement
hi def link hsConditional      Conditional
hi def link hsSpecialChar      SpecialChar
hi def link hsTypedef          Typedef
hi def link hsVarSym           hsOperator
hi def link hsConSym           hsOperator
hi def link hsOperator         Operator
if exists("hs_highlight_delimiters")
  " Some people find this highlighting distracting.
  hi def link hsDelimiter      Delimiter
endif
hi def link hsSpecialCharError Error
hi def link hsString           String
hi def link hsCharacter        Character
hi def link hsNumber           Number
hi def link hsFloat            Float
hi def link hsConditional      Conditional
hi def link hsLiterateComment  hsComment
hi def link hsBlockComment     hsComment
hi def link hsLineComment      hsComment
hi def link hsComment          Comment
hi def link hsPragma           SpecialComment
hi def link hsBoolean          Boolean
hi def link hsType             Type
hi def link hsMaybe            hsEnumConst
hi def link hsOrdering         hsEnumConst
hi def link hsEnumConst        Constant
hi def link hsDebug            Debug

let b:current_syntax = "haskell"

" vim: ts=8
