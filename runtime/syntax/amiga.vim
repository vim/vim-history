" Vim syntax file
" Language:	AmigaDos
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	September 26, 2000
" Version:     1.01

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore

" Amiga Devices
syn match amiDev "\(par\|ser\|prt\|con\|nil\):"

" Amiga aliases and paths
syn match amiAlias	"\<[a-zA-Z][a-zA-Z0-9]\+:"
syn match amiAlias	"\<[a-zA-Z][a-zA-Z0-9]\+:[a-zA-Z0-9/]*/"

" strings
syn region amiString	start=+"+ end=+"+ oneline

" numbers
syn match amiNumber	"\<\d\+\>"

" Logic flow
syn region	amiFlow	matchgroup=Statement start="if"	matchgroup=Statement end="endif"	contains=ALL
syn keyword	amiFlow	skip endskip
syn match	amiError	"else\|endif"
syn keyword	amiElse contained	else

syn keyword	amiTest contained	not warn error fail eq gt ge val exists

" echo exception
syn region	amiEcho	matchgroup=Statement start="\<echo\>" end="$" oneline contains=amiComment
syn region	amiEcho	matchgroup=Statement start="^\.[bB][rR][aA]" end="$" oneline
syn region	amiEcho	matchgroup=Statement start="^\.[kK][eE][tT]" end="$" oneline

" commands
syn keyword	amiKey	addbuffers	copy	fault	join	pointer	setdate
syn keyword	amiKey	addmonitor	cpu	filenote	keyshow	printer	setenv
syn keyword	amiKey	alias	date	fixfonts	lab	printergfx	setfont
syn keyword	amiKey	ask	delete	fkey	list	printfiles	setmap
syn keyword	amiKey	assign	dir	font	loadwb	prompt	setpatch
syn keyword	amiKey	autopoint	diskchange	format	lock	protect	sort
syn keyword	amiKey	avail	diskcopy	get	magtape	quit	stack
syn keyword	amiKey	binddrivers	diskdoctor	getenv	makedir	relabel	status
syn keyword	amiKey	bindmonitor	display	graphicdump	makelink	remrad	time
syn keyword	amiKey	blanker		iconedit	more	rename	type
syn keyword	amiKey	break	ed	icontrol	mount	resident	unalias
syn keyword	amiKey	calculator	edit	iconx	newcli	run	unset
syn keyword	amiKey	cd	endcli	ihelp	newshell	say	unsetenv
syn keyword	amiKey	changetaskpri	endshell	info	nocapslock	screenmode	version
syn keyword	amiKey	clock	eval	initprinter	nofastmem	search	wait
syn keyword	amiKey	cmd	exchange	input	overscan	serial	wbpattern
syn keyword	amiKey	colors	execute	install	palette	set	which
syn keyword	amiKey	conclip	failat	iprefs	path	setclock	why

" comments
syn cluster	amiCommentGroup contains=amiTodo
syn case ignore
syn keyword	amiTodo	contained	todo
syn case match
syn match	amiComment	";.*$" contains=amiCommentGroup

" sync
syn sync lines=50

" The default highlighting.
hi def link amiAlias	Type
hi def link amiComment	Comment
hi def link amiDev	Type
hi def link amiEcho	String
hi def link amiElse	Statement
hi def link amiError	Error
hi def link amiKey	Statement
hi def link amiNumber	Number
hi def link amiString	String
hi def link amiTest	Special

let b:current_syntax = "amiga"

" vim:ts=15
