" Vim syntax file
" Language:	Debian changelog files
" Maintainer:	Wichert Akkerman <wakkerma@debian.org>
" URL:		none
" Last Change:	2000 Dec 25

" Remove old cruft
syn clear

" Case doesn't matter for us
syn case ignore

" Define some common expressions we can use later on
syn match debchangelogName	contained "^[a-z][a-z0-9.-]\+ "
syn match debchangelogUrgency	contained "; urgency=\(low\|medium\|high\)"
syn match debchangelogTarget	contained "\( stable\| frozen\| unstable\)\+"
syn match debchangelogVersion	contained "(.\{-})"
syn match debchangelogCloses	contained "debchangelogoses:\s*\(bug\)\=#\s\=\d\+\(,\s*\(bug\)\=#\s\=\d\+\)*"

" Define the entries that make up the changelog
syn region debchangelogHeader start="^[^ ]" end="$" contains=debchangelogName,debchangelogUrgency,debchangelogTarget,debchangelogVersion oneline
syn region debchangelogFooter start="^ [^ ]" end="$" oneline
syn region debchangelogEntry start="^  " end="$" contains=debchangelogCloses oneline

" Associate our matches and regions with pretty colours
hi def link debchangelogHeader		Error
hi def link debchangelogFooter		Identifier
hi def link debchangelogEntry		String
hi def link debchangelogCloses		Statement
hi def link debchangelogUrgency		Identifier
hi def link debchangelogName		Identifier
hi def link debchangelogVersion		Identifier
hi def link debchangelogTarget		Identifier

let b:current_syntax = "debchangelog"
