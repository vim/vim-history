" Vim syntax file
" Language:	ncf NCF Batch File
" Maintainer:	Jonathan J. Miner <jon.miner@doit.wisc.edu>
" Last Change:	Thursday 13 May 1999 10:29 Central Daylight Time

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

syn match ncfComment	"^\ *rem.*$"
syn match ncfComment	"^\ *;.*$"
syn match ncfComment	"^\ *#.*$"

syn match ncfPath	"search \(add\|del\)"
syn match ncfServerName	"^file server name .*$"
syn match ncfIPXNet	"^ipx internal net"
syn keyword ncfCommands	set load mount
syn keyword ncfBoolean	on off

" String
syn region ncfString	start=+"+  end=+"+

syn match ncfNumber	"\<\d\(\d\+\|[A-F]\+\)*\>"

syn match ncfLogins	"^\([Dd]is\|[Ee]n\)able login[s]*"
syn match ncfScript	"[^ ]*\.ncf"


if !exists("did_ncf_syntax_inits")
	let did_ncf_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi link ncfCommands	Statement
	hi link ncfLogins	ncfCommands
	hi link ncfString	String
	hi link ncfComment	Comment
	hi link ncfImplicit	Type
	hi link ncfBoolean	Boolean
	hi link ncfScript	Identifier
	hi link ncfNumber	Number
	hi link ncfPath		Constant
	hi link ncfServerName	Special
	hi link ncfIPXNet	ncfServerName

endif

let b:current_syntax = "ncf"

" vim: ts=8
