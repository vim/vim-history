" Vim syntax file
" Language:	BIND 8.x zone files (RFC1035)
" Maintainer:	glory hump <rnd@web-drive.ru>
" Last change:	Sun Dec 24 03:59:15 SAMT 2000
" Filenames:	/var/named/*
" URL:	http://rnd.web-drive.ru/vim/syntax/bindzone.vim
" $Id$

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case match
setlocal iskeyword=.,-,48-58,A-Z,a-z,_

" Master File Format (rfc 1035)

" directives
syn region	zoneRRecord	start=+^+ end=+$+ contains=zoneLHSDomain,zoneLHSIP,zoneIllegalDom,zoneWhitespace,zoneComment,zoneParen,zoneSpecial
syn match	zoneDirective	/\$ORIGIN\s\+/ nextgroup=zoneDomain,zoneIllegalDom
syn match	zoneDirective	/\$TTL\s\+/ nextgroup=zoneTTL
syn match	zoneDirective	/\$INCLUDE\s\+/
syn match	zoneDirective	/\$GENERATE\s/

syn match	zoneWhitespace	contained /^\s\+/ nextgroup=zoneTTL,zoneClass,zoneRRType
syn match	zoneError	"\s\+$"
syn match	zoneSpecial	contained /^[@.]\s\+/ nextgroup=zoneTTL,zoneClass,zoneRRType
syn match	zoneSpecial	contained /@$/

" domains and IPs
syn match	zoneLHSDomain	contained /^[-0-9A-Za-z.]\+\s\+/ nextgroup=zoneTTL,zoneClass,zoneRRType
syn match	zoneLHSIP	contained /^[0-9]\{1,3}\(\.[0-9]\{1,3}\)\{,3}\s\+/ nextgroup=zoneTTL,zoneClass,zoneRRType
syn match	zoneIPaddr	contained /\<[0-9]\{1,3}\(\.[0-9]\{1,3}\)\{,3}\>/
syn match	zoneDomain	contained /\<[0-9A-Za-z][-0-9A-Za-z.]\+\>/

syn match	zoneIllegalDom	contained /\S*[^-A-Za-z0-9.[:space:]]\S*\>/
"syn match	zoneIllegalDom	contained /[0-9]\S*[-A-Za-z]\S*/

" keywords
syn keyword	zoneClass	IN CHAOS nextgroup=zoneRRType

syn match	zoneTTL	contained /\<[0-9HhWwDd]\+\s\+/ nextgroup=zoneClass,zoneRRType
syn match	zoneRRType	contained /\s*\<\(NS\|HINFO\)\s\+/ nextgroup=zoneSpecial,zoneDomain
syn match	zoneRRType	contained /\s*\<CNAME\s\+/ nextgroup=zoneDomain,zoneSpecial
syn match	zoneRRType	contained /\s*\<SOA\s\+/ nextgroup=zoneDomain,zoneIllegalDom
syn match	zoneRRType	contained /\s*\<PTR\s\+/ nextgroup=zoneDomain,zoneIllegalDom
syn match	zoneRRType	contained /\s*\<MX\s\+/ nextgroup=zoneMailPrio
syn match	zoneRRType	contained /\s*\<A\s\+/ nextgroup=zoneIPaddr,zoneIllegalDom

" FIXME: catchup serial number
syn match	zoneSerial	contained /\<[0-9]\{9}\>/

syn match	zoneMailPrio	contained /\<[0-9]\+\s*/ nextgroup=zoneDomain,zoneIllegalDom
syn match	zoneErrParen	/)/
syn region	zoneParen	contained start=+(+ end=+)+ contains=zoneSerial,zoneTTL,zoneComment
syn match	zoneComment	";.*"


" The default highlighting.
hi def link zoneComment	Comment
hi def link zoneDirective	Macro
hi def link zoneLHSDomain	Statement
hi def link zoneLHSIP	Statement
hi def link zoneClass	Include
hi def link zoneSpecial	Special
hi def link zoneRRType	Type
hi def link zoneError	Error
hi def link zoneErrParen	Error
hi def link zoneIllegalDom	Error
hi def link zoneSerial	Todo
hi def link zoneIPaddr	Number
hi def link zoneDomain	Identifier

let b:current_syntax = "bindzone"

" vim: ts=17
