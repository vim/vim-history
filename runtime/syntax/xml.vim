" Vim syntax file
" Language:	XML
" Maintainer:	Johannes Zellner <johannes@zellner.org>
"		Author and previous maintainer:
"		Paul Siegmann <pauls@euronet.nl>
" Last Change:	Fre, 10 Nov 2000 16:04:07 +0100
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
"

syn clear

" Following items are case-sensitive
" Case-insensitive rules can be specified by 'syn case ignore' later
syn case match

" mark illegal characters
syn match xmlError "[<>&]"


" tags
syn region  xmlString   contained start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=xmlEntity
syn region  xmlString   contained start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=xmlEntity
syn region  xmlEndTag             start=+</+                 end=+>+ contains=xmlTagError,@xmlTagHook
syn region  xmlTag                start=+<[^/]+              end=+>+ contains=xmlString,xmlTagError,@xmlTagHook
syn match   xmlTagError                                              contained "[^>]<"ms=s+1

" syntax-folding
if v:version >= 600
    syn cluster xmlFoldCluster
	\ contains=xmlTag,xmlEndTag,xmlProcessing,xmlComment,xmlFold,xmlCdata,xmlEntity
    syn region  xmlFold 
	\ start=+<\z([^ /!?>]\+\)\(\(\_[^>]*[^/!?]>\)\|>\)+ 
	\ end=+</\z1>+ 
	\ transparent fold contains=@xmlFoldCluster keepend
endif

" &entities; compare with dtd
syn match   xmlEntity                 "&[^; \t]*;" contains=xmlEntityPunct
syn match   xmlEntityPunct  contained "[&.;]"

syn keyword xmlTodo         contained TODO FIXME XXX

" The real comments (this implements the comments as defined by xml,
" but not all xml pages actually conform to it. Errors are flagged.
syn region  xmlComment                start=+<!+        end=+>+ contains=xmlCommentPart,xmlString,xmlCommentError,xmlTodo
syn match   xmlCommentError contained "[^><!]"
syn region  xmlCommentPart  contained start=+--+        end=+--+

" DTD -- we use dtd.vim here
syn region  xmlDocType matchgroup=xmlDocTypeDecl start="<!DOCTYPE"he=s+2,rs=s+2 end=">" contains=xmlDocTypeKeyword,xmlInlineDTD,xmlString
syn keyword xmlDocTypeKeyword contained DOCTYPE PUBLIC SYSTEM
syn region  xmlInlineDTD contained start="\[" end="]" contains=@xmlDTD
syn include @xmlDTD <sfile>:p:h/dtd.vim

" Processing instructions
" This allows "?>" inside strings -- good idea?
syn region  xmlProcessing matchgroup=xmlProcessingDelim start="<?" end="?>" contains=xmlString

" CData sections
syn region  xmlCdata matchgroup=xmlCdataDecl start="<!\[CDATA\[" keepend end="]]>"

" synchronizing (does not always work if a comment includes legal
" xml tags, but doing it right would mean to always start
" at the first line, which is too slow)
syn sync match xmlHighlight groupthere NONE "<[/a-zA-Z]"
syn sync match xmlHighlightSkip "^.*['\"].*$"
syn sync minlines=10

" The default highlighting.
hi def link xmlTodo                      Todo
hi def link xmlTag                       Function
hi def link xmlEndTag                    Identifier
hi def link xmlEntity                    Statement
hi def link xmlEntityPunct               Type
hi def link xmlString                    String
hi def link xmlComment                   Comment
hi def link xmlCommentPart               Comment
hi def link xmlCommentError              xmlError
hi def link xmlTagError                  xmlError
hi def link xmlError                     Error

hi def link xmlProcessingDelim           Comment
hi def link xmlProcessing                Type
hi def link xmlCdata                     Normal
hi def link xmlCdataDecl                 String
hi def link xmlDocType                   Normal
hi def link xmlDocTypeDecl               Function
hi def link xmlDocTypeKeyword            Statement
hi def link xmlInlineDTD                 Function

let b:current_syntax = "xml"

" vim: ts=8
