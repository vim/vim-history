" Vim syntax file
" Language:     DNS Zone File
" Maintainer:   jehsom@jehsom.com
" URL:          http://scripts.jehsom.com
" Last Change:  2001 Apr 16

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Last match is taken!
syn match	dnsKeyword          "\<\(IN\|A\|SOA\|NS\|CNAME\|MX\|PTR\|SOA\|MB\|MG\|MR\|NULL\|WKS\|HINFO\|TXT\|CS\|CH\|CPU\|OS\)\>"
syn match   dnsDataFQDN         "\<[^ 	]*\.[ 	]*$"
syn match   dnsPreProc          "^\$.*"
syn match   dnsConstant			"\<\([0-9][0-9.]*\|[0-9.]*[0-9]\)\>"
syn match   dnsIPaddr           "\<\(\([0-2]\)\{0,1}\([0-9]\)\{1,2}\.\)\{3}\([0-2]\)\{0,1}\([0-9]\)\{1,2}\>[ 	]*$"
syn match   dnsComment          ";.*$"
syn match   dnsRecordName       "^[^;$ 	]*"

hi def link dnsComment		Comment
hi def link dnsDataFQDN		Identifier
hi def link dnsPreProc		PreProc
hi def link dnsKeyword		Keyword
hi def link dnsRecordName	Type
hi def link dnsIPaddr		Type
hi def link dnsIPerr		Error
hi def link dnsConstant		Constant

let b:current_syntax = "dns"
