" Vim syntax file
" Language:	XML
" Maintainer:	Johannes Zellner <johannes@zellner.org>
"		Author and previous maintainer:
"		Paul Siegmann <pauls@euronet.nl>
" Last Change:	Mon, 30 Apr 2001 07:45:38 +0200
" Filenames:	*.xml
" URL:		http://www.zellner.org/vim/syntax/xml.vim
" $Id$

" CREDITS:
" - The original version was derived by Paul Siegmann from
"   Claudio Fleiner's html.vim.
"
" - suggestions and patches by:
"       Rafael Garcia-Suarez
"       Akbar Ibrahim <akbar.ibrahim@wipro.com>
"       Devin Weaver
"
" REFERENCES:
"   http://www.w3.org/TR/1998/REC-xml-19980210
"   http://www.w3.org/XML/1998/06/xmlspec-report-19980910.htm

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

let s:xml_cpo_save = &cpo
set cpo&vim

syn case match

" mark illegal characters
syn match xmlError "[<&]"


" strings (inside tags) aka VALUES
"
" EXAMPLE:
"
" <tag foo.attribute = "value">
"                      ^^^^^^^
syn region  xmlString contained start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=xmlEntity display
syn region  xmlString contained start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=xmlEntity display


" punctuation (within attributes) e.g. <tag xml:foo.attribute ...>
"                                              ^   ^
syn match   xmlAttribPunct +[:.]+ contained display


" no highlighting for xmlEqual (xmlEqual has no highlighting group)
syn match   xmlEqual +=+


" attribute, everything before the '='
"
" PROVIDES: @xmlAttribHook
"
" EXAMPLE:
"
" <tag foo.attribute = "value">
"      ^^^^^^^^^^^^^
"
syn match   xmlAttrib
    \ +[^-'"<]\@<=\<[a-zA-Z0-9.:]\+\>\([^'">]\@=\|$\)+
    \ contained
    \ contains=xmlAttribPunct,@xmlAttribHook
    \ display


" start tag
" use matchgroup=xmlTag to skip over the leading '<'
" see also xmlEmptyTag below.
"
" PROVIDES: @xmlTagHook
"
syn region   xmlTag
    \ matchgroup=xmlTag start=+<[^ /!?"']\@=+
    \ matchgroup=xmlTag end=+>+
    \ contained
    \ contains=xmlError,xmlAttrib,xmlEqual,xmlString,@xmlTagHook


" tag content for empty tags. This is the same as xmlTag
" above, except the `matchgroup=xmlEndTag for highlighting
" the end '/>' differently.
"
" PROVIDES: @xmlTagHook
"
syn region   xmlEmptyTag
    \ matchgroup=xmlTag start=+<[^ /!?"']\@=+
    \ matchgroup=xmlEndTag end=+/>+
    \ contained
    \ contains=xmlError,xmlAttrib,xmlEqual,xmlString,@xmlTagHook


" end tag
" highlight everything but not the trailing '>' which
" was already highlighted by the containing xmlRegion.
"
" PROVIDES: @xmlTagHook
" (should we provide a separate @xmlEndTagHook ?)
"
syn match   xmlEndTag
    \ +</[^ /!?>"']\+>+
    \ contained
    \ contains=@xmlTagHook


" real (non-empty) elements with syntax-folding.
" No highlighing, highlighing is done by contained elements.
"
" PROVIDES: @xmlRegionHook
"
" EXAMPLE:
"
" <tag id="whoops">
"   <!-- comment -->
"   <another.tag></another.tag>
"   <another.tag/>
"   some data
" </tag>
"
syn region   xmlRegion
    \ start=+<\z([^ /!?>"']\+\)\(\(\_[^>]*[^/!?]>\)\|\_[>]\)+
    \ end=+</\z1>+
    \ fold
    \ contains=xmlTag,xmlEndTag,xmlCdata,@xmlRegionCluster,xmlComment,xmlEntity,xmlProcessing,@xmlRegionHook
    \ keepend
    \ extend


" empty tags. Just a container, no highlighting.
" Compare this with xmlTag.
"
" EXAMPLE:
"
" <tag id="lola"/>
"
" TODO use xmlEmptyTag intead of xmlTag
syn match    xmlEmptyRegion
    \ +<[^ /!?>"']\(\_[^"'<>]\|"\_[^"]*"\|'\_[^']*'\)*/>+
    \ contains=xmlEmptyTag


" cluster which contains the above two elements
syn cluster xmlRegionCluster contains=xmlRegion,xmlEmptyRegion


" &entities; compare with dtd
syn match   xmlEntity                 "&[^; \t]*;" contains=xmlEntityPunct
syn match   xmlEntityPunct  contained "[&.;]"


" The real comments (this implements the comments as defined by xml,
" but not all xml pages actually conform to it. Errors are flagged.
syn region  xmlComment                start=+<!+        end=+>+ contains=xmlCommentPart,xmlString,xmlCommentError,xmlTodo,@xmlCommentHook
syn keyword xmlTodo         contained TODO FIXME XXX
syn match   xmlCommentError contained "[^><!]"
syn region  xmlCommentPart  contained start=+--+        end=+--+


" CData sections
"
" PROVIDES: @xmlCdataHook
"
syn region    xmlCdata
    \ start=+<!\[CDATA\[+
    \ end=+]]>+
    \ contains=xmlCdataStart,xmlCdataEnd,@xmlCdataHook
    \ keepend
    \ extend

" using the following line instead leads to corrupt folding at CDATA regions
" syn match    xmlCdata      +<!\[CDATA\[\_.\{-}]]>+  contains=xmlCdataStart,xmlCdataEnd,@xmlCdataHook
syn match    xmlCdataStart +<!\[CDATA\[+  contained contains=xmlCdataCdata
syn keyword  xmlCdataCdata CDATA          contained
syn match    xmlCdataEnd   +]]>+          contained


" Processing instructions
" This allows "?>" inside strings -- good idea?
syn region  xmlProcessing matchgroup=xmlProcessingDelim start="<?" end="?>" contains=xmlAttrib,xmlEqual,xmlString


" DTD -- we use dtd.vim here
syn region  xmlDocType matchgroup=xmlDocTypeDecl
    \ start="<!DOCTYPE"he=s+2,rs=s+2 end=">"
    \ fold
    \ contains=xmlDocTypeKeyword,xmlInlineDTD,xmlString
syn keyword xmlDocTypeKeyword contained DOCTYPE PUBLIC SYSTEM
syn region  xmlInlineDTD contained matchgroup=xmlDocTypeDecl start="\[" end="]" contains=@xmlDTD
syn include @xmlDTD syntax/dtd.vim
unlet b:current_syntax


" synchronizing
" TODO !!! to be improved !!!

syn sync match xmlSyncDT grouphere  xmlDocType +\_.\(<!DOCTYPE\)\@=+
" syn sync match xmlSyncDT groupthere  NONE       +]>+

syn sync match xmlSync grouphere   xmlRegion  +\_.\(<[^ /!?>"']\+\)\@=+
" syn sync match xmlSync grouphere  xmlRegion "<[^ /!?>"']*>"
syn sync match xmlSync groupthere  xmlRegion  +</[^ /!?>"']\+>+

syn sync minlines=100


" The default highlighting.
hi def link xmlTodo                      Todo
hi def link xmlTag                       Function
hi def link xmlEndTag                    Identifier
hi def link xmlEmptyTag                  Function
hi def link xmlEntity                    Statement
hi def link xmlEntityPunct               Type

hi def link xmlAttribPunct               Comment
hi def link xmlAttrib                    Type

hi def link xmlString                    String
hi def link xmlComment                   Comment
hi def link xmlCommentPart               Comment
hi def link xmlCommentError              Error
hi def link xmlError                     Error

hi def link xmlProcessingDelim           Comment
hi def link xmlProcessing                Type

hi def link xmlCdata                     String
hi def link xmlCdataCdata                Statement
hi def link xmlCdataStart                Type
hi def link xmlCdataEnd                  Type

hi def link xmlDocTypeDecl               Function
hi def link xmlDocTypeKeyword            Statement
hi def link xmlInlineDTD                 Function

let b:current_syntax = "xml"

let &cpo = s:xml_cpo_save
unlet s:xml_cpo_save

" vim: ts=8
