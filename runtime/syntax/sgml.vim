" Vim syntax file
" Language:	SGML-DTD (DocBook 3.1 and LinuxDoc)
" Maintainer:	Lorance Stinson <madlinux@yahoo.com>
" Last Change:	Thu Sep 8 14:39 EDT 2000
" Filenames:    *.ent *.sgml *.sgm

" Adapted from the HTML syntax file by Claudio Fleiner <claudio@fleiner.com>
" and the SGML syntax files by Sung-Hyun Nam <namsh@kldp.org>.

" The option "sgml_no_rendering" can be set to turnrendering off.
" The option "sgml_my_rendering" can be set to use custome rendering.
" If custome rendering is used the color definitions sgmlLink, sgmlBold
" sgmlBoldItalic, sgmlUnderline and sgmlItalic should be set to taste.

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" mark illegal characters
syn match sgmlErr       "[<>&]"

if !exists("main_syntax")
  let main_syntax = 'sgml'
endif

" tags
syn region  sgmlString  contained start=+"+ end=+"+ contains=sgmlSpecialChar,@sgmlPreproc
syn match   sgmlValue   contained "=[\t ]*[^'" \t>][^ \t>]*"hs=s+1   contains=@sgmlPreproc
syn region  sgmlEndTag  start=+</+ end=+>+ contains=sgmlTagN,sgmlTagError
syn region  sgmlTag     start=+<[^/]+ end=+>+ contains=sgmlTagN,sgmlString,sgmlArg,sgmlValue,sgmlTagErr,sgmlEvent,@sgmlPreproc,@sgmlArgCluster
syn match   sgmlTagN    contained +<\s*[-a-zA-Z0-9]\++hs=s+1 contains=sgmlTagName,sgmlSpecialTagName,@sgmlTagNameCluster
syn match   sgmlTagN    contained +</\s*[-a-zA-Z0-9]\++hs=s+2 contains=sgmlTagName,sgmlSpecialTagName,@sgmlTagNameCluster
syn match   sgmlTagErr  contained "[^>]<"ms=s+1
syn region  sgmlDocEnt  contained start="<!\(entity\|element\)\s" end=">" contains=sgmlStr
syn region  sgmlDocEntI contained start=+\[+ end=+]+ contains=sgmlDocEnt,sgmlComment
syn region  sgmlDocType start=+<!doctype\s+ end=+>+ contains=sgmlDocEntI,sgmlStr


" tag names
syn match   sgmlTagName contained "sect\d\+"
syn match   sgmlTagName contained "sect\d\+info"
syn match   sgmlTagName contained "refsect\d\+"
syn keyword sgmlTagName contained abbrev abstract accel acronym action address
				\ affiliation alt anchor answer appendix
				\ application
				\ area areaset areaspec arg artheader article
				\ artpagenums attribution author authorblurb
				\ authorgroup authorinitials bibliodiv
				\ biblioentry bibliography bibliomixed
				\ bibliomset biblioset blockquote book
				\ bookbiblio bookinfo bridgehead callout
				\ calloutlist caption caution chapter citation
				\ citerefentry citetitle city classname
				\ cmdsynopsis colophon colspec command comment
				\ computeroutput constant copyright corpauthor
				\ corpname country database date dedication
				\ docinfo edition editor email emphasis entry
				\ envar epigraph equation errorcode errorname
				\ errortype example fax figure filename
				\ firstname firstterm footnote footnoteref
				\ foreignphrase formalpara funcdef funcparams
				\ funcprototype funcsynopsis funcsynopsisinfo
				\ function glossary glossdef glossdiv
				\ glossentry glosslist glosssee glossseealso
				\ glossterm graphic group guibutton guiicon
				\ guilabel guimenu guimenuitem guisubmenu
				\ hardware holder honorific imagedata
				\ imageobject important index indexdiv
				\ indexentry indexterm informalequation
				\ informalexample informalfigure informaltable
				\ inlineequation inlinegraphic
				\ inlinemediaobject interface
				\ interfacedefinition isbn issn issuenum
				\ itemizedlist jobtitle keycap keycode
				\ keycombo keysym legalnotice lineannotation
				\ link listitem literal literallayout manvolnum
				\ markup medialabel mediaobject member
				\ mousebutton msg msgaud msgentry msgexplan
				\ msginfo msglevel msgmain msgorig msgrel
				\ msgset msgsub msgtext note objectinfo option
				\ optional orderedlist orgdiv orgname
				\ otheraddr othername pagenums para paramdef
				\ parameter part partintro phone phrase
				\ postcode preface primary primaryie procedure
				\ productname programlisting programlistingco
				\ prompt property pubdate publisher
				\ publishername qandadiv qandaentry qandaset
				\ question quote refdescriptor refentry
				\ refentrytitle reference refmeta refmiscinfo
				\ refname
				\ refnamediv refpurpose refsynopsisdiv
				\ releaseinfo replaceable returnvalue
				\ revhistory revision revnumber revremark row
				\ sbr screen screeninfo screenshot secondary
				\ secondaryie section sectioninfo
				\ see seealso seealsoie
				\ seeie seg seglistitem segmentedlist segtitle
				\ seriesinfo set setinfo sgmltag shortaffil
				\ sidebar simpara simplelist simplesect
				\ spanspec state step street structfield
				\ structname subscript substeps subtitle
				\ superscript surname symbol synopsis
				\ systemitem table tbody term tertiaryie
				\ textobject tgroup thead tip title
				\ titleabbrev toc token trademark type ulink
				\ userinput varargs variablelist varlistentry
				\ varname videodata videoobject void volumenum
				\ warning wordasword xref year

" this is for LinuxDoc
syn keyword sgmlTagName contained bf code descrip enum em htmlurl itemize item
				\ p ref sect tag tscreen tt url verb

" legal arg names
syn keyword sgmlArg     contained arch conformance filename graphics
                                \ id idreq keyaction label lang linespecific
                                \ linkend linkendreq linkends linkendsreq
                                \ mark moreinfo name os pagenum remap revision
                                \ revisionflag role status url userlevel vendor
                                \ width xreflabel

" special characters
syn match sgmlSpecialChar "&[^;]*;"

" Comments
syn region sgmlComment  start=+<!--+ end=+-->+

" rendering
if !exists("sgml_no_rendering")
    syn cluster sgmlTop     contains=@Spell,sgmlTag,sgmlEndTag,sgmlSpecialChar,sgmlPreProc,sgmlComment,sgmlLink,@sgmlPreproc

    syn region sgmlUnderline  start="<varname\>" end="</varname>"me=e-10 contains=@sgmlTop
    syn region sgmlBoldItalic start="<emphasis\>" end="</emphasis>"me=e-11 contains=@sgmlTop
    syn region sgmlBold       start="<command\>" end="</command>"me=e-10 contains=@sgmlTop
    syn region sgmlBold       start="<function\>" end="</function>"me=e-11 contains=@sgmlTop
    syn region sgmlItalic     start="<literal\>" end="</literal>"me=e-10 contains=@sgmlTop
    syn region sgmlItalic     start="<replaceable\>" end="</replaceable>"me=e-14 contains=@sgmlTop
    syn region sgmlLink       start="<ulink\>" end="</ulink>"me=e-8 contains=@Spell,sgmlTag,sgmlEndTag,sgmlSpecialChar,sgmlPreProc,sgmlComment,@sgmlPreproc
    syn region sgmlLink       start="<link\>" end="</link>"me=e-7 contains=@Spell,sgmlTag,sgmlEndTag,sgmlSpecialChar,sgmlPreProc,sgmlComment,@sgmlPreproc
endif

if main_syntax == "sgml"
  " synchronizing (does not always work if a comment includes legal
  " sgml tags, but doing it right would mean to always start
  " at the first line, which is too slow)
  syn sync match sgmlHighlight groupthere NONE "<[/a-zA-Z]"
  syn sync match sgmlHighlightSkip "^.*['\"].*$"
  syn sync minlines=10
endif

" The default highlighting.
hi def link sgmlTag                       Function
hi def link sgmlEndTag                    Identifier
hi def link sgmlArg                       Type
hi def link sgmlTagName                   sgmlStatement
hi def link sgmlSpecialTagName            Exception
hi def link sgmlValue                     Value
hi def link sgmlSpecialChar               Special
hi def link sgmlDocEnt                    Type
hi def link sgmlDocType                   PreProc
hi def link sgmlTitle                     Title
hi def link sgmlPreStmt                   PreProc
hi def link sgmlPreErr                    Error
hi def link sgmlPreProc                   PreProc
hi def link sgmlPreAttr                   String
hi def link sgmlPreProcAttrName           PreProc
hi def link sgmlPreProcAttrErr            Error
hi def link sgmlSpecial                   Special
hi def link sgmlSpecialChar               Special
hi def link sgmlString                    String
hi def link sgmlStatement                 Statement
hi def link sgmlComment                   Comment
hi def link sgmlValue                     String
hi def link sgmlTagErr                    sgmlErr
hi def link sgmlErr                       Error

if !exists("sgml_no_rendering")
  if !exists("sgml_my_rendering")
    if &background == "dark"
      hi def sgmlLink              term=underline cterm=underline ctermfg=cyan gui=underline guifg=#80a0ff
    else
      hi def sgmlLink              term=underline cterm=underline ctermfg=DarkBlue gui=underline guifg=Blue
    endif
    hi def sgmlBold                term=bold cterm=bold gui=bold
    hi def sgmlBoldItalic          term=bold,italic cterm=bold,italic gui=bold,italic
    hi def sgmlUnderline           term=underline cterm=underline gui=underline
    hi def sgmlItalic              term=italic cterm=italic gui=italic
  endif
endif

let b:current_syntax = "sgml"

if main_syntax == 'sgml'
  unlet main_syntax
endif

" vim: ts=8
