" Vim syntax file
" Language   : TeX
" Maintainer : Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change: December 4, 1997
"
" Notes:
" 1. If you have a \begin{verbatim} that appears to overrun its boundaries,
"    use %stopzone.

" Removes any old syntax stuff hanging around
syn clear

" Some type styles
syn match texTypeStyle	"\\rm\>"
syn match texTypeStyle	"\\em\>"
syn match texTypeStyle	"\\bf\>"
syn match texTypeStyle	"\\it\>"
syn match texTypeStyle	"\\sl\>"
syn match texTypeStyle	"\\sf\>"
syn match texTypeStyle	"\\sc\>"
syn match texTypeStyle	"\\tt\>"

" Some type sizes
syn match texTypeSize	"\\tiny\>"
syn match texTypeSize	"\\scriptsize\>"
syn match texTypeSize	"\\footnotesize\>"
syn match texTypeSize	"\\small\>"
syn match texTypeSize	"\\normalsize\>"
syn match texTypeSize	"\\large\>"
syn match texTypeSize	"\\Large\>"
syn match texTypeSize	"\\LARGE\>"
syn match texTypeSize	"\\huge\>"
syn match texTypeSize	"\\Huge\>"

" Try to flag {}, [], and () mismatches
syn region texMatcher matchgroup=Delimiter start="{" skip="\(\\\\\)*\\[{}]" end="}"	contains=ALLBUT,texError,texCmdName,texCmdArgs,texCmdBody,texDefName,texMathMatcher,texMathParen,texMathOper
syn region texMatcher matchgroup=Delimiter start="\["	end="]"	              contains=ALLBUT,texError,texCmdName,texCmdArgs,texCmdBody,texDefName,texMathMatcher,texMathParen,texMathOper
syn region texParen	start="("		end=")"	              contains=ALLBUT,texError,texCmdName,texCmdArgs,texCmdBody,texDefName,texMathMatcher,texMathParen,texMathOper
syn match  texError	"[}\])]"

syn region texMathMatcher   matchgroup=Delimiter start="{"  skip="\(\\\\\)*\\}"  end="}"	contained contains=ALLBUT,texError,texCmdName,texCmdArgs,texCmdBody,texDefName,texOnlyMath,texMatcher,texParen,texBadMath
syn region texMathMatcher   matchgroup=Delimiter start="\[" skip="\(\\\\\)*\\\]" end="]"	contained contains=ALLBUT,texError,texCmdName,texCmdArgs,texCmdBody,texDefName,texOnlyMath,texMatcher,texParen,texBadMath
syn region texMathParen	start="(" end=")"				contained contains=ALLBUT,texError,texCmdName,texCmdArgs,texCmdBody,texDefName,texOnlyMath,texMatcher,texParen,texBadMath

" TeX/LaTeX keywords
" Instead of trying to be All Knowing, I just match \..alphameric..
syn match texStatement	"\\[a-zA-Z]\+"
if expand("%:e") == "sty"
  syn match texStatement	"\\[a-zA-Z]*@[a-zA-Z@]*"
else
  syn match texError	"\\[a-zA-Z]*@[a-zA-Z@]*"
endif
syn match texDelimiter	"&"
syn match texDelimiter	"\\\\"

" Sections, subsections, etc
syn match texSection "\\\(sub\)*section\*\="
syn match texSection "\\\(title\|author\|part\|chapter\|paragraph\|subparagraph\)"
syn match texSection "\\begin{abstract}\|\\end{abstract}"

" Math Zones
syn region texMathZone	start="\\begin{equation}"	end="\\end{equation}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone	start="\\begin{equation\*}"	end="\\end{equation\*}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone	start="\\begin{eqnarray}"	end="\\end{eqnarray}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone	start="\\begin{eqnarray\*}"	end="\\end{eqnarray\*}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone	start="\\begin{math}"	end="\\end{math}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone	start="\\begin{math\*}"	end="\\end{math\*}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone start="\\begin{displaymath}"   end="\\end{displaymath}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone start="\\begin{displaymath\*}" end="\\end{displaymath\*}"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texComment,texRefZone
syn region texMathZone	matchgroup=Delimiter start="\\("		matchgroup=Delimiter end="\\)\|%stopzone"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texRefZone
syn region texMathZone	matchgroup=Delimiter start="\\\["		matchgroup=Delimiter end="\\]\|%stopzone"	keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texRefZone
syn region texMathZone	matchgroup=Delimiter start="\$"		skip="\(\\\\\)*\\\$"	matchgroup=Delimiter end="\$\|%stopzone" keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texRefZone
syn region texMathZone	matchgroup=Delimiter start="\$\$"		matchgroup=Delimiter end="\$\$\|%stopzone" keepend contains=texTypeStyle,texTypeSize,texStatement,texDelimiter,texComment,texLength,texMathMatcher,texMathParen,texMathOper,texError,TexMathDelim,texRefZone
syn match texMathOper	"[_^=]" contained
syn match TexMathDelim	"\\\(\left\|\right\)\([()[\]\.|]\|\\[uU]parrow\|\\[dD]ownarrow\|\\[uU]pdownarrow\|\\[lr]floor\|\\[lr]ceil\|\\[lr]\angle\|\\backslash\|\\|\)\>" contained
syn match texBadMath        "\\end{\(equation\|eqnarray\|displaymath\)\*\=}"
syn match texBadMath	"\\[\])]"

" Comments:
syn match texComment	"%.*$"

" separate lines used for verb` and verb# so that the end conditions
" will appropriately terminate.  Ideally vim would let me save a
" character from the start pattern and re-use it in the end-pattern.
syn region texZone	start="\\begin{verbatim}"		end="\\end{verbatim}\|%stopzone"
syn region texZone	start="\\verb`"		end="`\|%stopzone"
syn region texZone	start="\\verb#"		end="#\|%stopzone"
syn region texZone	start="@samp{"		end="}\|%stopzone"
syn region texRefZone	matchgroup=texStatement start="\\ref{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter
syn region texRefZone	matchgroup=texStatement start="\\label{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter
syn region texRefZone	matchgroup=texStatement start="\\cite{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter

" special TeX characters  ( \$ \& \% \# \{ \} \_ \S \P )
syn match texSpecialChar	"\\[$&%#{}_]"
syn match texSpecialChar	"\\[SP@][^a-zA-Z]"me=e-1
syn match texSpecialChar	"\\\\"
syn match texOnlyMath	"[_^]"

" handle newcommands
syn match  texNewCmd		"\\newcommand"		nextgroup=texCmdName skipwhite skipnl
syn region texCmdName contained matchgroup=Delimiter start="{"rs=s+1  end="}"	nextgroup=texCmdArgs,texCmdBody skipwhite skipnl
syn region texCmdArgs contained matchgroup=Delimiter start="\["rs=s+1 end="]"	nextgroup=texCmdBody skipwhite skipnl
syn region texCmdBody contained matchgroup=Delimiter start="{"rs=s+1 skip="\(\\\\\)\\[{}]" matchgroup=Delimiter end="}" contains=ALLBUT,texZone,texMatcher,texError,texCmdName,texCmdArgs,texDefCmd,texDefName,texMathZone,texParen,texMathParen,texMathMatcher,texBadMath,texOnlyMath

syn match texDefCmd		"\\def"			nextgroup=texDefName skipwhite skipnl
syn match texDefName contained	"\\[a-zA-Z]\+"		nextgroup=texCmdBody skipwhite skipnl

" TeX input
syn match texInput	"\\input [a-zA-Z]\+"hs=s+7	contains=texStatement

" TeX Lengths
syn match  texLength	"[0-9]\+\(\.[0-9]\+\)\=\(cm\|em\|ex\|in\|pc\|pt\|mm\)"

" TeX String Delimiters
syn match texString	"\(``\|''\)"

" synchronizing
syn sync maxlines=50

if !exists("did_tex_syntax_inits")
 let did_tex_syntax_inits = 1
 " easy-for-user-to-modify highlighting groups
 hi link texMath	Special
 hi link texDef	Statement
 hi link texType	Type

 " The default methods for highlighting. Can be overridden later
 hi link texBadMath         Error
 hi link texCmdArgs	Number
 hi link texCmdName	Statement
 hi link texComment	Comment
 hi link texDefCmd	texDef
 hi link texDefName	texDef
 hi link texDelimiter	Delimiter
 hi link texError	Error
 hi link texInput	Todo
 hi link TexMathDelim	Delimiter
 hi link texLength	Number
 hi link texMathMatcher     texMath
 hi link texMathParen       texMath
 hi link texMathOper	Operator
 hi link texMathZone	texMath
 hi link texNewCmd	Statement
 hi link texRefZone	Special
 hi link texSection	Todo
 hi link texSpecialChar	SpecialChar
 hi link texStatement	Statement
 hi link texString	String
 hi link texTypeSize	texType
 hi link texTypeStyle	texType
 hi link texOnlyMath	Error
 hi link texZone	PreCondit
endif

let b:current_syntax = "tex"

" vim: ts=14
