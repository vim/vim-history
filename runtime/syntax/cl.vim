" Vim syntax file
" Language:	cl (by Multibase, http://www.mbase.com.au)
" Filename extensions: *.ent, *.eni
" Maintainer:	Philip Uren <philu@system77.com>
" Last update:	Mon Jan  8 17:03:11 EST 2001

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

setlocal isk=@,48-57,_,-,
syn case ignore

syn sync lines=300

"If/else/elsif/endif and while/wend mismatch errors
syn match   clifError		"\<wend\>"
syn match   clifError		"\<elsif\>"
syn match   clifError		"\<else\>"
syn match   clifError		"\<endif\>"

" If and while regions
syn region clLoop	transparent matchgroup=clWhile start="\<while\>" matchgroup=clWhile end="\<wend\>" contains=ALLBUT,clBreak,clProcedure
syn region clIf		transparent matchgroup=clConditional start="\<if\>" matchgroup=clConditional end="\<endif\>"   contains=ALLBUT,clBreak,clProcedure

" Make those TODO notes and debugging stand out!
syn keyword	clTodo			contained	TODO BUG DEBUG FIX
syn keyword clDebug			contained	debug
syn match	clNeedsWork		contained	"NEED[S]*[ 	][ 	]*WORK"

syn match	clComment		"#.*$"		contains=clTodo,clNeedsWork
syn region	clProcedure		oneline		start="^[ 	]*[{}]" end="$"
syn match	clInclude					"^[ 	]*include[ 	].*"

" We don't put "debug" in the clSetOptions;
" we contain it in clSet so we can make it stand out.
syn keyword clSetOptions	transparent aauto abort align convert E fill fnum goback hangup justify null_exit output rauto rawprint rawdisplay repeat skip tab trim
syn match	clSet			"^[ 	]*set[ 	].*" contains=clSetOptions,clDebug

syn match clPreProc			"^[ 	]*#P.*"

syn keyword clConditional	else elsif continue endloop
" 'break' needs to be a region so we can sync on it above.
syn region clBreak			oneline start="^[ 	]*break" end="$"

syn match clOperator		"[!;|)(:.><+*=-]"

syn match clNumber			"\<\d\+\(u\=l\=\|lu\|f\)\>"

syn region clString	matchgroup=clQuote	start=+"+ end=+"+	skip=+\\"+
syn region clString matchgroup=clQuote	start=+'+ end=+'+	skip=+\\'+

syn keyword clReserved		ERROR EXIT INTERRUPT LOCKED LREPLY MODE MCOL MLINE MREPLY NULL REPLY V1 V2 V3 V4 V5 V6 V7 V8 V9 ZERO BYPASS GOING_BACK AAUTO ABORT ABORT ALIGN BIGE CONVERT FNUM GOBACK HANGUP JUSTIFY NEXIT OUTPUT RAUTO RAWDISPLAY RAWPRINT REPEAT SKIP TAB TRIM LCOUNT PCOUNT PLINES SLINES SCOLS MATCH LMATCH

syn keyword clFunction 		asc asize chr name random slen srandom day getarg getcgi getenv lcase scat sconv sdel skey smult srep substr sword trim ucase match

syn keyword clStatement		clear clear_eol clear_eos close copy create unique with where empty define define ldefine delay_form delete escape exit_block exit_do exit_process field fork format get getfile getnext getprev goto head join maintain message no_join on_eop on_key on_exit on_delete openin openout openapp pause popenin popenout popenio print put range read redisplay refresh restart_block screen select sleep text unlock write and not or do

" The default highlighting.
hi def link clifError		Error
hi def link clWhile			Repeat
hi def link clConditional	Conditional
hi def link clDebug			Debug
hi def link clNeedsWork		Todo
hi def link clTodo			Todo
hi def link clComment		Comment
hi def link clProcedure		Procedure
hi def link clBreak			Procedure
hi def link clInclude		Include
hi def link clSetOption		Statement
hi def link clSet			Identifier
hi def link clPreProc		PreProc
hi def link clOperator		Operator
hi def link clNumber		Number
hi def link clString		String
hi def link clQuote			Delimiter
hi def link clReserved		Identifier
hi def link clFunction		Function
hi def link clStatement		Statement

let b:current_syntax = "cl"

" vim: ts=4 sw=4
