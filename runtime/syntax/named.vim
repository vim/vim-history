" Vim syntax file
" Language:	BIND 8.x configuration file
" Maintainer:	glory hump <rnd@web-drive.ru>
" Last change:	Sun Dec 24 03:59:03 SAMT 2000
" Filenames:	named.conf
" URL:	http://rnd.web-drive.ru/vim/syntax/named.vim
" $Id$
"
" NOTE
"    it was not widely tested, i just tried it on my simple
"    single-master-single-slave configuration. most syntax was borrowed
"    directly from "BIND Configuration File Guide" without testing.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case match
setlocal iskeyword=.,-,48-58,A-Z,a-z,_

" BIND configuration file

syn match	namedComment	"//.*"
syn region	namedComment	start="/\*" end="\*/"
syn region	namedString	start=/"/ end=/"/ contained
" --- omitted trailing semicolon FIXME
syn match	namedError	/[^;{]$/

" --- top-level keywords

syn keyword	namedInclude	include nextgroup=namedString skipwhite
syn keyword	namedKeyword	acl key nextgroup=namedIntIdent skipwhite
syn keyword	namedKeyword	server nextgroup=namedIdentifier skipwhite
syn keyword	namedKeyword	controls nextgroup=namedSection skipwhite
syn keyword	namedKeyword	trusted-keys nextgroup=namedIntSection skipwhite
syn keyword	namedKeyword	logging nextgroup=namedLogSection skipwhite
syn keyword	namedKeyword	options nextgroup=namedOptSection skipwhite
syn keyword	namedKeyword	zone nextgroup=namedZoneString skipwhite

" --- Identifier: name of following { ... } Section
syn match	namedIdentifier	contained /\k\+/ nextgroup=namedSection skipwhite
" --- IntIdent: name of following IntSection
syn match	namedIntIdent	contained /\k\+/ nextgroup=namedIntSection skipwhite

" --- Section: { ... } clause
syn region	namedSection	contained start=+{+ end=+};+ contains=namedSection,namedIntKeyword

" --- IntSection: section that does not contain other sections
syn region	namedIntSection	contained start=+{+ end=+};+ contains=namedIntKeyword,namedParenError,namedError

" --- IntKeyword: keywords contained within `{ ... }' sections only
" + these keywords are contained within `key' and `acl' sections
syn keyword	namedIntKeyword	contained key algorithm
syn keyword	namedIntKeyword	contained secret nextgroup=namedString skipwhite

" + these keywords are contained within `server' section only
syn keyword	namedIntKeyword	contained bogus support-ixfr nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedIntKeyword	contained transfers nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedIntKeyword	contained transfer-format
syn keyword	namedIntKeyword	contained keys nextgroup=namedIntSection skipwhite

" + these keywords are contained within `controls' section only
syn keyword	namedIntKeyword	contained inet nextgroup=namedIPaddr,namedIPerror skipwhite
syn keyword	namedIntKeyword	contained unix nextgroup=namedString skipwhite
syn keyword	namedIntKeyword	contained port perm owner group nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedIntKeyword	contained allow nextgroup=namedIntSection skipwhite

" --- options
syn region	namedOptSection	contained start=+{+ end=+};+ contains=namedOption,namedCNOption,namedComment,namedParenError

syn keyword	namedOption	contained version directory
\		nextgroup=namedString skipwhite
syn keyword	namedOption	contained named-xfer dump-file pid-file
\		nextgroup=namedString skipwhite
syn keyword	namedOption	contained mem-statistics-file statistics-file
\		nextgroup=namedString skipwhite
syn keyword	namedOption	contained auth-nxdomain deallocate-on-exit
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedOption	contained dialup fake-iquery fetch-glue
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedOption	contained has-old-clients host-statistics
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedOption	contained maintain-ixfr-base multiple-cnames
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedOption	contained notify recursion rfc2308-type1
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedOption	contained use-id-pool treat-cr-as-space
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedOption	contained also-notify forwarders
\		nextgroup=namedIntSection skipwhite
syn keyword	namedOption	contained forward check-names
syn keyword	namedOption	contained allow-query allow-transfer allow-recursion	nextgroup=namedAML skipwhite
syn keyword	namedOption	contained blackhole listen-on
\		nextgroup=namedIntSection skipwhite
syn keyword	namedOption	contained lame-ttl max-transfer-time-in
\		nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedOption	contained max-ncache-ttl min-roots
\		nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedOption	contained serial-queries transfers-in
\		nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedOption	contained transfers-out transfers-per-ns
syn keyword	namedOption	contained transfer-format
syn keyword	namedOption	contained transfer-source
\		nextgroup=namedIPaddr,namedIPerror skipwhite
syn keyword	namedOption	contained max-ixfr-log-size
\		nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedOption	contained coresize datasize files stacksize
syn keyword	namedOption	contained cleaning-interval interface-interval statistics-interval heartbeat-interval
\		nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedOption	contained topology sortlist rrset-order
\		nextgroup=namedIntSection skipwhite

syn match	namedOption	contained /\<query-source\s\+.*;/he=s+12 contains=namedQSKeywords
syn keyword	namedQSKeywords	contained address port
syn match	namedCNOption	contained /\<check-names\s\+.*;/he=s+11 contains=namedCNKeywords
syn keyword	namedCNKeywords	contained fail warn ignore master slave response

" --- logging facilities
syn region	namedLogSection	contained start=+{+ end=+};+ contains=namedLogOption
syn keyword	namedLogOption	contained channel nextgroup=namedIntIdent skipwhite
syn keyword	namedLogOption	contained category nextgroup=namedIntIdent skipwhite
syn keyword	namedIntKeyword	contained syslog null versions size severity
syn keyword	namedIntKeyword	contained file nextgroup=namedString skipwhite
syn keyword	namedIntKeyword	contained print-category print-severity print-time nextgroup=namedBool,namedNotBool skipwhite

" --- zone section
syn region	namedZoneString	contained oneline start=+"+ end=+"+ skipwhite
\		contains=namedDomain,namedIllegalDom
\		nextgroup=namedZoneClass,namedZoneSection
syn keyword	namedZoneClass	contained in hs hesiod chaos
\		nextgroup=namedZoneSection skipwhite

syn region	namedZoneSection	contained start=+{+ end=+};+ contains=namedZoneOpt,namedCNOption,namedComment,namedMasters,namedParenError
syn keyword	namedZoneOpt	contained file ixfr-base
\		nextgroup=namedString skipwhite
syn keyword	namedZoneOpt	contained notify dialup
\		nextgroup=namedBool,namedNotBool skipwhite
syn keyword	namedZoneOpt	contained pubkey forward
syn keyword	namedZoneOpt	contained max-transfer-time-in
\		nextgroup=namedNumber,namedNotNumber skipwhite
syn keyword	namedZoneOpt	contained type nextgroup=namedZoneType skipwhite
syn keyword	namedZoneType	contained master slave stub forward hint

syn keyword	namedZoneOpt	contained masters forwarders
\		nextgroup=namedIPlist skipwhite
syn region	namedIPlist	contained start=+{+ end=+};+ contains=namedIPaddr,namedIPerror,namedParenError,namedComment
syn match	namedZoneOpt	contained "\<allow-\(update\|query\|transfer\)"
\		nextgroup=namedAML skipwhite

" --- boolean parameter
syn match	namedNotBool	contained "[^ 	;]\+"
syn keyword	namedBool	contained yes no true false 1 0

" --- number parameter
syn match	namedNotNumber	contained "[^ 	0-9;]\+"
syn match	namedNumber	contained "\d\+"

" --- address match list
syn region	namedAML	contained start=+{+ end=+};+ contains=namedParenError,namedComment

" --- IPs & Domains
syn match	namedIPaddr	contained /\<[0-9]\{1,3}\(\.[0-9]\{1,3}\)\{3};/he=e-1
syn match	namedDomain	contained /\<[0-9A-Za-z][-0-9A-Za-z.]\+\>/ nextgroup=namedSpareDot
syn match	namedDomain	contained /"\."/ms=s+1,me=e-1
syn match	namedSpareDot	contained /\./

" --- syntax errors
syn match	namedIllegalDom	contained /"\S*[^-A-Za-z0-9.[:space:]]\S*"/ms=s+1,me=e-1
syn match	namedIPerror	contained /\<\S*[^0-9.[:space:];]\S*/
syn match	namedEParenError	contained +{+
syn match	namedParenError	+}\([^;]\|$\)+

" The default highlighting.
hi def link namedComment	Comment
hi def link namedInclude	Include
hi def link namedKeyword	Keyword
hi def link namedIntKeyword	Keyword
hi def link namedIdentifier	Identifier
hi def link namedIntIdent	Identifier

hi def link namedString	String
hi def link namedBool	Type
hi def link namedNotBool	Error
hi def link namedNumber	Number
hi def link namedNotNumber	Error

hi def link namedOption	namedKeyword
hi def link namedLogOption	namedKeyword
hi def link namedCNOption	namedKeyword
hi def link namedQSKeywords	Type
hi def link namedCNKeywords	Type
hi def link namedLogCategory	Type
hi def link namedDomain	Identifier
hi def link namedZoneOpt	namedKeyword
hi def link namedZoneType	Type
hi def link namedParenError	Error
hi def link namedEParenError	Error
hi def link namedIllegalDom	Error
hi def link namedIPerror	Error
hi def link namedSpareDot	Error
hi def link namedError	Error

let b:current_syntax = "named"

" vim: ts=17
