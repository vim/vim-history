" Vim syntax file
" Language:	DocBook
" Maintainer:	Devin Weaver <ktohg@tritarget.com>
" URL:		ftp://tritarget.com/pub/vim/syntax/docbk.vim
" Last Change:	Fre, 10 Nov 2000 16:03:40 +0100
" $Id$

" REFERENCES:
"   http://docbook.org/
"   http://www.open-oasis.org/docbook/
"

syn clear

if exists('b:docbk_type')
    if 'xml' == b:docbk_type
	doau FileType xml
	syn cluster xmlTagHook add=docbkKeyword
    elseif 'sgml' == b:docbk_type
	doau FileType sgml
	syn cluster sgmlTagHook add=docbkKeyword
    endif
endif

syn keyword docbkKeyword abbrev abstract accel ackno acronym action
syn keyword docbkKeyword address affiliation alt anchor answer appendix
syn keyword docbkKeyword application area areaset areaspec arg artheader
syn keyword docbkKeyword article articleinfo artpagenums attribution audiodata
syn keyword docbkKeyword audioobject author authorblurb authorgroup
syn keyword docbkKeyword authorinitials beginpage bibliodiv biblioentry
syn keyword docbkKeyword bibliography bibliomisc bibliomixed bibliomset
syn keyword docbkKeyword biblioset blockquote book bookbiblio bookinfo
syn keyword docbkKeyword bridgehead callout calloutlist caption caution
syn keyword docbkKeyword chapter citation citerefentry citetitle city
syn keyword docbkKeyword classname cmdsynopsis co collab collabname
syn keyword docbkKeyword colophon colspec command comment computeroutput
syn keyword docbkKeyword confdates confgroup confnum confsponsor conftitle
syn keyword docbkKeyword constant contractnum contractsponsor contrib
syn keyword docbkKeyword copyright corpauthor corpname country database
syn keyword docbkKeyword date dedication docinfo edition editor email
syn keyword docbkKeyword emphasis entry entrytbl envar epigraph equation
syn keyword docbkKeyword errorcode errorname errortype example fax figure
syn keyword docbkKeyword filename firstname firstterm footnote footnoteref
syn keyword docbkKeyword foreignphrase formalpara funcdef funcparams
syn keyword docbkKeyword funcprototype funcsynopsis funcsynopsisinfo
syn keyword docbkKeyword function glossary glossdef glossdiv glossentry
syn keyword docbkKeyword glosslist glosssee glossseealso glossterm graphic
syn keyword docbkKeyword graphicco group guibutton guiicon guilabel
syn keyword docbkKeyword guimenu guimenuitem guisubmenu hardware
syn keyword docbkKeyword highlights holder honorific imagedata imageobject
syn keyword docbkKeyword imageobjectco important index indexdiv indexentry
syn keyword docbkKeyword indexterm informalequation informalexample
syn keyword docbkKeyword informalfigure informaltable inlineequation
syn keyword docbkKeyword inlinegraphic inlinemediaobject interface
syn keyword docbkKeyword interfacedefinition invpartnumber isbn issn
syn keyword docbkKeyword issuenum itemizedlist itermset jobtitle keycap
syn keyword docbkKeyword keycode keycombo keysym keyword keywordset label
syn keyword docbkKeyword legalnotice lineage lineannotation link listitem
syn keyword docbkKeyword literal literallayout lot lotentry manvolnum
syn keyword docbkKeyword markup medialabel mediaobject mediaobjectco
syn keyword docbkKeyword member menuchoice modespec mousebutton msg msgaud
syn keyword docbkKeyword msgentry msgexplan msginfo msglevel msgmain
syn keyword docbkKeyword msgorig msgrel msgset msgsub msgtext note
syn keyword docbkKeyword objectinfo olink option optional orderedlist
syn keyword docbkKeyword orgdiv orgname otheraddr othercredit othername
syn keyword docbkKeyword pagenums para paramdef parameter part partintro
syn keyword docbkKeyword phone phrase pob postcode preface primary
syn keyword docbkKeyword primaryie printhistory procedure productname
syn keyword docbkKeyword productnumber programlisting programlistingco
syn keyword docbkKeyword prompt property pubdate publisher publishername
syn keyword docbkKeyword pubsnumber qandadiv qandaentry qandaset question
syn keyword docbkKeyword quote refclass refdescriptor refentry
syn keyword docbkKeyword refentrytitle reference refmeta refmiscinfo
syn keyword docbkKeyword refname refnamediv refpurpose refsect1
syn keyword docbkKeyword refsect1info refsect2 refsect2info refsect3
syn keyword docbkKeyword refsect3info refsynopsisdiv refsynopsisdivinfo
syn keyword docbkKeyword releaseinfo replaceable returnvalue revhistory
syn keyword docbkKeyword revision revnumber revremark row sbr screen
syn keyword docbkKeyword screenco screeninfo screenshot secondary
syn keyword docbkKeyword secondaryie sect1 sect1info sect2 sect2info sect3
syn keyword docbkKeyword sect3info sect4 sect4info sect5 sect5info section
syn keyword docbkKeyword sectioninfo see seealso seealsoie seeie seg
syn keyword docbkKeyword seglistitem segmentedlist segtitle seriesinfo
syn keyword docbkKeyword seriesvolnums set setindex setinfo sgmltag
syn keyword docbkKeyword shortaffil shortcut sidebar simpara simplelist
syn keyword docbkKeyword simplesect spanspec state step street structfield
syn keyword docbkKeyword structname subject subjectset subjectterm
syn keyword docbkKeyword subscript substeps subtitle superscript surname
syn keyword docbkKeyword symbol synopfragment synopfragmentref synopsis
syn keyword docbkKeyword systemitem table tbody term tertiary tertiaryie
syn keyword docbkKeyword textobject tfoot tgroup thead tip title
syn keyword docbkKeyword titleabbrev toc tocback tocchap tocentry tocfront
syn keyword docbkKeyword toclevel1 toclevel2 toclevel3 toclevel4 toclevel5
syn keyword docbkKeyword tocpart token trademark type ulink userinput
syn keyword docbkKeyword varargs variablelist varlistentry varname
syn keyword docbkKeyword videodata videoobject void volumenum warning
syn keyword docbkKeyword wordasword xref year

" The default highlighting.
hi def link docbkKeyword	Statement

let b:current_syntax = "docbk"

" vim: ts=8
