" Vim syntax file
" Language   : TeX
" Maintainer : Dr. Charles E. Campbell, Jr. <Charles.Campbell@gsfc.nasa.gov>
" Last change: August 21, 1998
"
" Notes:
" 1. If you have a \begin{verbatim} that appears to overrun its boundaries,
"    use %stopzone.
" 2. Run-on equations ($..$ and $$..$$, particularly) can also be stopped
"    by suitable use of %stopzone.
" 3. If you have a slow computer, you may wish to modify
"
"    syn sync maxlines=200
"    syn sync minlines=50
"
"    to values that are more to your liking.
" 4. There is no match-syncing for $...$ and $$...$$; hence large
"    equation blocks constructed that way may exhibit syncing problems.
"    (there's no difference between begin/end patterns)

" Removes any old syntax stuff hanging around
syn clear

" Try to flag {}, [], and () mismatches
syn cluster texMatchGroup	contains=texCmdArgs,texCmdBody,texCmdName,texDefName,texError,texMathDelimBad,texMathDelimBs,texMathDelimChr,texMathDelimKey,texMathMatcher,texMathOper,texMathParen,texSectionMarker,texDocumentTypeArgs
syn region texMatcher	matchgroup=Delimiter start="{" skip="\(\\\\\)*\\[{}]" end="}"	contains=ALLBUT,@texMatchGroup
syn region texMatcher	matchgroup=Delimiter start="\["	end="]"		contains=ALLBUT,@texMatchGroup
syn region texParen	start="("		end=")"		contains=ALLBUT,@texMatchGroup
syn match  texError	"[}\])]"

syn cluster texMathMatchGroup	contains=texAccent,texBadMath,texCmdArgs,texCmdBody,texCmdName,texDefName,texError,texMatcher,texMathDelimBad,texMathDelimBs,texMathDelimChr,texMathDelimKey,texOnlyMath,texParen,texSectionMarker,texDocumentTypeArgs
syn region texMathMatcher	matchgroup=Delimiter start="{"  skip="\(\\\\\)*\\}"  end="}"	contained contains=ALLBUT,@texMathMatchGroup
syn region texMathMatcher	matchgroup=Delimiter start="\[" skip="\(\\\\\)*\\\]" end="]"	contained contains=ALLBUT,@texMathMatchGroup
syn region texMathParen	start="("	end=")"			contained contains=ALLBUT,@texMathMatchGroup

" TeX/LaTeX keywords
" Instead of trying to be All Knowing, I just match \..alphameric..
syn match texStatement	"\\[a-zA-Z]\+"
let b:extfname=expand("%:e")
if b:extfname == "sty" || b:extfname == "cls" || b:extfname == "clo"
  syn match texStatement	"\\[a-zA-Z]*@[a-zA-Z@]*"
else
  syn match texError	"\\[a-zA-Z]*@[a-zA-Z@]*"
endif
unlet b:extfname
syn match texDelimiter	"&"
syn match texDelimiter	"\\\\"

" \begin{}/\end{} section markers
syn match texSectionMarker "\\begin\|\\end" nextgroup=texSectionName
syn region texSectionName matchgroup=Delimiter start="{" end="}" contained

" \documentclass, \documentstyle, \usepackage
syn match texDocumentType "\\documentclass\|\\documentstyle\|\\usepackage" nextgroup=texSectionName,texDocumentTypeArgs
syn region texDocumentTypeArgs matchgroup=Delimiter start="\[" end="]" contained nextgroup=texSectionName

" TeX input
syn match texInput	"\\input\s\+[a-zA-Z/.0-9]\+"hs=s+7	contains=texStatement

" Some type styles
syn match texTypeStyle	"\\rm\>"
syn match texTypeStyle	"\\em\>"
syn match texTypeStyle	"\\bf\>"
syn match texTypeStyle	"\\it\>"
syn match texTypeStyle	"\\sl\>"
syn match texTypeStyle	"\\sf\>"
syn match texTypeStyle	"\\sc\>"
syn match texTypeStyle	"\\tt\>"

" LaTeX2E type styles
syn match texTypeStyle	"\\textrm\>"
syn match texTypeStyle	"\\textem\>"
syn match texTypeStyle	"\\textbf\>"
syn match texTypeStyle	"\\textit\>"
syn match texTypeStyle	"\\textsl\>"
syn match texTypeStyle	"\\textsf\>"
syn match texTypeStyle	"\\textsc\>"
syn match texTypeStyle	"\\texttt\>"

syn match texTypeStyle	"\\mathrm\>"
syn match texTypeStyle	"\\mathem\>"
syn match texTypeStyle	"\\mathbf\>"
syn match texTypeStyle	"\\mathit\>"
syn match texTypeStyle	"\\mathsl\>"
syn match texTypeStyle	"\\mathsf\>"
syn match texTypeStyle	"\\mathsc\>"
syn match texTypeStyle	"\\mathtt\>"

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

" Sections, subsections, etc
syn match texSection	"\\\(sub\)*section\*\=\>"
syn match texSection	"\\\(title\|author\|part\|chapter\|paragraph\|subparagraph\)\>"
syn match texSection	"\\begin{abstract}\|\\end{abstract}"

" Bad Math (mismatched)
syn match texBadMath	"\\end{\(split\|align\|gather\|alignat\|flalign\|multline\)"
syn match texBadMath	"\\end{\(equation\|eqnarray\|displaymath\)\*\=}"
syn match texBadMath	"\\[\])]"

" Math Zones and Clusters
syn cluster texMathZoneGroup	contains=texComment,texDelimiter,texError,texLength,texMathDelim,texMathMatcher,texMathOper,texMathParen,texRefZone,texStatement,texTypeSize,texTypeStyle
syn cluster texMathZones	contains=texMathZoneA,texMathZoneB,texMathZoneC,texMathZoneD,texMathZoneE,texMathZoneF,texMathZoneG,texMathZoneH,texMathZoneI,texMathZoneJ,texMathZoneK,texMathZoneL,texMathZoneM,texMathZoneN,texMathZoneO,texMathZoneP,texMathZoneQ,texMathZoneR,texMathZoneS,texMathZoneT,texMathZoneU,texMathZoneV,texMathZoneW

syn region texMathZoneA	start="\\begin{align\*}"	end="\\end{align\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneB	start="\\begin{alignat\*}"	end="\\end{alignat\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneC	start="\\begin{alignat}"	end="\\end{alignat}"	keepend contains=@texMathZoneGroup
syn region texMathZoneD	start="\\begin{align}"	end="\\end{align}"	keepend contains=@texMathZoneGroup
syn region texMathZoneE	start="\\begin{eqnarray\*}"	end="\\end{eqnarray\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneF	start="\\begin{eqnarray}"	end="\\end{eqnarray}"	keepend contains=@texMathZoneGroup
syn region texMathZoneG	start="\\begin{equation\*}"	end="\\end{equation\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneH	start="\\begin{equation}"	end="\\end{equation}"	keepend contains=@texMathZoneGroup
syn region texMathZoneI	start="\\begin{flalign\*}"	end="\\end{flalign\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneJ	start="\\begin{flalign}"	end="\\end{flalign}"	keepend contains=@texMathZoneGroup
syn region texMathZoneK	start="\\begin{gather\*}"	end="\\end{gather\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneL	start="\\begin{gather}"	end="\\end{gather}"	keepend contains=@texMathZoneGroup
syn region texMathZoneM	start="\\begin{math\*}"	end="\\end{math\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneN	start="\\begin{math}"	end="\\end{math}"	keepend contains=@texMathZoneGroup
syn region texMathZoneO	start="\\begin{multline}"	end="\\end{multline}"	keepend contains=@texMathZoneGroup
syn region texMathZoneP	start="\\begin{split}"	end="\\end{split}"	keepend contains=@texMathZoneGroup
syn region texMathZoneQ	start="\\begin{displaymath\*}"	end="\\end{displaymath\*}"	keepend contains=@texMathZoneGroup
syn region texMathZoneR	start="\\begin{displaymath}"	end="\\end{displaymath}"	keepend contains=@texMathZoneGroup
syn region texMathZoneS	start="\\begin{multline\*}"	end="\\end{multline\*}"	keepend contains=@texMathZoneGroup
                       
syn region texMathZoneT	matchgroup=Delimiter start="\\("	matchgroup=Delimiter end="\\)\|%stopzone"	keepend contains=@texMathZoneGroup
syn region texMathZoneU	matchgroup=Delimiter start="\\\["	matchgroup=Delimiter end="\\]\|%stopzone"	keepend contains=@texMathZoneGroup
syn region texMathZoneV	matchgroup=Delimiter start="\$"	skip="\(\\\\\)*\\\$"	matchgroup=Delimiter end="\$\|%stopzone" keepend contains=@texMathZoneGroup
syn region texMathZoneW	matchgroup=Delimiter start="\$\$"	matchgroup=Delimiter end="\$\$\|%stopzone" keepend contains=@texMathZoneGroup

syn match texMathOper	"[_^=]" contained

" \left..something.. and \right..something.. support
syn match   texMathDelimBad contained	"[^a-zA-Z0-9]"
syn match   texMathDelim	"\\\(left\|right\)[^a-zA-Z0-9]"me=e-1	nextgroup=texMathDelimBs,texMathDelimChr,texMathDelimBad
syn match   texMathDelimChr contained	"[().|\[\]]"
syn match   texMathDelimBs  contained	"\\."me=e-1	nextgroup=texMathDelimKey
syn keyword texMathDelimKey contained	Downarrow	Uparrow	downarrow	lceil	rangle	uparrow
syn keyword texMathDelimKey contained	Rfloor	backslash	langle	lfloor	rceil
syn match   texMathDelimKey contained	"[{}]"

" Comments:
syn match texComment	"%.*$"

" separate lines used for verb` and verb# so that the end conditions
" will appropriately terminate.  Ideally vim would let me save a
" character from the start pattern and re-use it in the end-pattern.
syn region texZone	start="\\begin{verbatim}"		end="\\end{verbatim}\|%stopzone"
syn region texZone	start="\\verb`"		end="`\|%stopzone"
syn region texZone	start="\\verb#"		end="#\|%stopzone"
syn region texZone	start="@samp{"		end="}\|%stopzone"
syn region texRefZone	matchgroup=texStatement start="\\cite{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter
syn region texRefZone	matchgroup=texStatement start="\\label{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter
syn region texRefZone	matchgroup=texStatement start="\\pageref{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter
syn region texRefZone	matchgroup=texStatement start="\\ref{"	keepend end="}\|%stopzone"  contains=texComment,texDelimiter

" special TeX characters  ( \$ \& \% \# \{ \} \_ \S \P )
syn match texSpecialChar	"\\[$&%#{}_]"
syn match texSpecialChar	"\\[SP@][^a-zA-Z]"me=e-1
syn match texSpecialChar	"\\\\"
syn match texOnlyMath	"[_^]"

" texAccent (tnx to Karim Belabas) avoids annoying highlighting for accents
syn match texAccent	"\\[bcdvuH][^a-zA-Z]"me=e-1
syn match texAccent	"\\[bcdvuH]$"
syn match texAccent	+\\[=^.\~"`']+
syn match texLigature	"\\\([ijolL]\|ae\|oe\|ss\|AA\|AE\|OE\)[^a-zA-Z]"me=e-1
syn match texLigature	"\\\([ijolL]\|ae\|oe\|ss\|AA\|AE\|OE\)$"

" handle newcommands
syn match  texNewCmd		"\\newcommand"		nextgroup=texCmdName skipwhite skipnl
syn region texCmdName contained matchgroup=Delimiter start="{"rs=s+1  end="}"	nextgroup=texCmdArgs,texCmdBody skipwhite skipnl
syn region texCmdArgs contained matchgroup=Delimiter start="\["rs=s+1 end="]"	nextgroup=texCmdBody skipwhite skipnl
syn region texCmdBody contained matchgroup=Delimiter start="{"rs=s+1 skip="\(\\\\\)\\[{}]" matchgroup=Delimiter end="}" contains=ALLBUT,texAccent,texBadMath,texCmdArgs,texCmdName,texDefCmd,texDefName,texError,texMatcher,texMathDelimBad,texMathDelimBs,texMathDelimChr,texMathDelimKey,texMathMatcher,texMathParen,@texMathZones,texOnlyMath,texParen,texZone

syn match texDefCmd	"\\def"		nextgroup=texDefName skipwhite skipnl
syn match texDefName contained	"\\[a-zA-Z]\+"		nextgroup=texCmdBody skipwhite skipnl

" TeX Lengths
syn match  texLength	"\d\+\(\.\d\+\)\=\(cm\|em\|ex\|in\|pc\|pt\|mm\)"

" TeX String Delimiters
syn match texString	"\(``\|''\)"

" LaTeX synchronization
syn sync maxlines=200
syn sync minlines=50

syn sync match texSyncMathZoneA	grouphere texMathZoneA	"\\begin{align\*}"
syn sync match texSyncMathZoneB	grouphere texMathZoneB	"\\begin{alignat\*}"
syn sync match texSyncMathZoneC	grouphere texMathZoneC	"\\begin{alignat}"
syn sync match texSyncMathZoneD	grouphere texMathZoneD	"\\begin{align}"
syn sync match texSyncMathZoneE	grouphere texMathZoneE	"\\begin{eqnarray\*}"
syn sync match texSyncMathZoneF	grouphere texMathZoneF	"\\begin{eqnarray}"
syn sync match texSyncMathZoneG	grouphere texMathZoneG	"\\begin{equation\*}"
syn sync match texSyncMathZoneH	grouphere texMathZoneH	"\\begin{equation}"
syn sync match texSyncMathZoneI	grouphere texMathZoneI	"\\begin{flalign\*}"
syn sync match texSyncMathZoneJ	grouphere texMathZoneJ	"\\begin{flalign}"
syn sync match texSyncMathZoneK	grouphere texMathZoneK	"\\begin{gather\*}"
syn sync match texSyncMathZoneL	grouphere texMathZoneL	"\\begin{gather}"
syn sync match texSyncMathZoneM	grouphere texMathZoneM	"\\begin{math\*}"
syn sync match texSyncMathZoneN	grouphere texMathZoneN	"\\begin{math}"
syn sync match texSyncMathZoneO	grouphere texMathZoneO	"\\begin{multline}"
syn sync match texSyncMathZoneP	grouphere texMathZoneP	"\\begin{split}"
syn sync match texSyncMathZoneQ	grouphere texMathZoneQ	"\\begin{displaymath\*}"
syn sync match texSyncMathZoneR	grouphere texMathZoneR	"\\begin{displaymath}"
syn sync match texSyncMathZoneS	grouphere texMathZoneS	"\\begin{multline\*}"
syn sync match texSyncMathZoneT	grouphere texMathZoneT	"\\("
syn sync match texSyncMathZoneU	grouphere texMathZoneU	"\\\["

syn sync match texSyncMathZoneA	groupthere NONE	"\\end{align\*}"
syn sync match texSyncMathZoneB	groupthere NONE	"\\end{alignat\*}"
syn sync match texSyncMathZoneC	groupthere NONE	"\\end{alignat}"
syn sync match texSyncMathZoneD	groupthere NONE	"\\end{align}"
syn sync match texSyncMathZoneE	groupthere NONE	"\\end{eqnarray\*}"
syn sync match texSyncMathZoneF	groupthere NONE	"\\end{eqnarray}"
syn sync match texSyncMathZoneG	groupthere NONE	"\\end{equation\*}"
syn sync match texSyncMathZoneH	groupthere NONE	"\\end{equation}"
syn sync match texSyncMathZoneI	groupthere NONE	"\\end{flalign\*}"
syn sync match texSyncMathZoneJ	groupthere NONE	"\\end{flalign}"
syn sync match texSyncMathZoneK	groupthere NONE	"\\end{gather\*}"
syn sync match texSyncMathZoneL	groupthere NONE	"\\end{gather}"
syn sync match texSyncMathZoneM	groupthere NONE	"\\end{math\*}"
syn sync match texSyncMathZoneN	groupthere NONE	"\\end{math}"
syn sync match texSyncMathZoneO	groupthere NONE	"\\end{multline}"
syn sync match texSyncMathZoneP	groupthere NONE	"\\end{split}"
syn sync match texSyncMathZoneQ	groupthere NONE	"\\end{displaymath\*}"
syn sync match texSyncMathZoneR	groupthere NONE	"\\end{displaymath}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{multline\*}"
syn sync match texSyncMathZoneT	groupthere NONE	"\\)"
syn sync match texSyncMathZoneU	groupthere NONE	"\\\]"
syn sync match texSyncStop	groupthere NONE	"%stopzone"

" The $..$ and $$..$$ make for impossible sync patterns.
" The following grouptheres coupled with minlines above
" help improve the odds of good syncing.
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{abstract}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{center}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{description}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{enumerate}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{itemize}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{table}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\end{tabular}"
syn sync match texSyncMathZoneS	groupthere NONE	"\\\(sub\)*section"

if !exists("did_tex_syntax_inits")
 let did_tex_syntax_inits = 1

 " TeX highlighting groups which should share similar highlighting
 hi link texBadMath	texError
 hi link texDefCmd	texDef
 hi link texDefName	texDef
 hi link texDocumentType	texCmdName
 hi link texDocumentTypeArgs	texCmdArgs
 hi link texInput	Todo
 hi link texLigature	texSpecialChar
 hi link texMathDelimBad	texError
 hi link texMathDelimBs	texMathDelim
 hi link texMathDelimChr	texMathDelim
 hi link texMathDelimKey	texMathDelim
 hi link texMathMatcher	texMath
 hi link texMathParen	texMath
 hi link texMathZoneA	texMath
 hi link texMathZoneB	texMath
 hi link texMathZoneC	texMath
 hi link texMathZoneD	texMath
 hi link texMathZoneE	texMath
 hi link texMathZoneF	texMath
 hi link texMathZoneG	texMath
 hi link texMathZoneH	texMath
 hi link texMathZoneI	texMath
 hi link texMathZoneJ	texMath
 hi link texMathZoneK	texMath
 hi link texMathZoneL	texMath
 hi link texMathZoneM	texMath
 hi link texMathZoneN	texMath
 hi link texMathZoneO	texMath
 hi link texMathZoneP	texMath
 hi link texMathZoneQ	texMath
 hi link texMathZoneR	texMath
 hi link texMathZoneS	texMath
 hi link texMathZoneT	texMath
 hi link texMathZoneU	texMath
 hi link texMathZoneV	texMath
 hi link texMathZoneW	texMath
 hi link texOnlyMath	texError
 hi link texSectionMarker	texCmdName
 hi link texSectionName	texSection
 hi link texTypeSize	texType
 hi link texTypeStyle	texType

 " Basic TeX highlighting groups
 hi link texCmdArgs	Number
 hi link texCmdName	Statement
 hi link texComment	Comment
 hi link texDef		Statement
 hi link texDelimiter	Delimiter
 hi link texError	Error
 hi link texLength	Number
 hi link texMath	Special
 hi link texMathDelim	Statement
 hi link texMathOper	Operator
 hi link texNewCmd	Statement
 hi link texRefZone	Special
 hi link texSection	PreCondit
 hi link texSpecialChar	SpecialChar
 hi link texStatement	Statement
 hi link texString	String
 hi link texType	Type
 hi link texZone	PreCondit
endif

let b:current_syntax = "tex"

" vim: ts=16
