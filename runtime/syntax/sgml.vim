" Vim syntax file
" Language:	SGML
" Maintainer:	Sung-Hyun Nam <namsh@lgic.co.kr>
"		Currently it looks not good, but I believe it is better than
"               no syntax. If you want to enhance and maintain,
"		You can remove my name and insert yours.
" Last change:	1999 Jun 29

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" tags
syn cluster sgmlTagGroup contains=sgmlTagError,sgmlSpecial,sgmlComment,sgmlDocType
syn match   sgmlTagError ">"
syn match   sgmlErrInTag contained "<"
syn region  sgmlEndTag	start=+</+    end=+>+	contains=ALLBUT,@sgmlTagGroup,sgmlErrInTag
syn region  sgmlTag	start=+<[^/]+ end=+>+	contains=ALLBUT,@sgmlTagGroup,sgmlErrInTag
syn match   sgmlTagN	contained +<\s*[-a-zA-Z0-9]\++ms=s+1	contains=ALLBUT,@sgmlTagGroup,sgmlErrInTag,sgmlStr
syn match   sgmlTagN	contained +</\s*[-a-zA-Z0-9]\++ms=s+2	contains=ALLBUT,@sgmlTagGroup,sgmlErrInTag

syn region  sgmlStr	contained start=+L\="+ end=+"+

syn region  sgmlSpecial	oneline start="&" end=";"

" tag names for DTD DocBook V3.0
syn match   sgmlTagName contained "sect\d\+"
syn keyword sgmlTagName contained address affiliation area areaspec author
syn keyword sgmlTagName contained authorblurb authorgroup book bookinfo
syn keyword sgmlTagName contained chapter copyright docinfo email emphasis
syn keyword sgmlTagName contained entry example firstname funcdef funcsynopsis
syn keyword sgmlTagName contained function glossdef glossentry glosslist
syn keyword sgmlTagName contained glossterm graphic guimenuitem holder
syn keyword sgmlTagName contained informaltable itemizedlist legalnotice
syn keyword sgmlTagName contained listitem literallayout para paramdef
syn keyword sgmlTagName contained parameter preface programlisting
syn keyword sgmlTagName contained programlistingco prompt row screenshot
syn keyword sgmlTagName contained simpara structfield structname subtitle
syn keyword sgmlTagName contained surname symbol tbody tgroup thead title
syn keyword sgmlTagName contained toc type ulink userinput warning xref year

syn region  sgmlEntity  oneline start="^\s\+&" end=";$"
syn region  sgmlComment start=+<!--+ end=+-->+
syn region  sgmlDocEnt  contained start="<!entity" end=">"
syn region  sgmlDocType start=+<!doctype+ end=+>+ contains=sgmlDocEnt

if !exists("did_sgml_syntax_inits")
  let did_sgml_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link sgmlTag	Special
  hi link sgmlEndTag	Special
  hi link sgmlEntity	Type
  hi link sgmlDocEnt    Type
  hi link sgmlTagName	Statement
  hi link sgmlComment	Comment
  hi link sgmlSpecial	Special
  hi link sgmlDocType   PreProc
  hi link sgmlStr	String
  hi link sgmlTagError	Error
  hi link sgmlErrInTag	Error
endif

let b:current_syntax = "sgml"

" vim: ts=8 sts=8 sw=8 noet:
