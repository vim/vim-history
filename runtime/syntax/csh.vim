" Vim syntax file
" Language:	C-shell (csh)
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Version:	1.02
" Last Change:	September 19, 2000

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" clusters:
syn cluster cshQuoteList	contains=cshDblQuote,cshSnglQuote,cshBckQuote

" Variables which affect the csh itself
syn match cshSetVariables	contained "argv\|histchars\|ignoreeof\|noglob\|prompt\|status"
syn match cshSetVariables	contained "cdpath\|history\|mail\|nonomatch\|savehist\|time"
syn match cshSetVariables	contained "cwd\|home\|noclobber\|path\|shell\|verbose"
syn match cshSetVariables	contained "echo"

syn case ignore
syn keyword cshTodo	contained todo
syn case match

" Variable Name Expansion Modifiers
syn match cshModifier	contained ":\(h\|t\|r\|q\|x\|gh\|gt\|gr\)"

" Strings and Comments
syn match   cshNoEndlineDQ	contained "[^\"]\(\\\\\)*$"
syn match   cshNoEndlineSQ	contained "[^\']\(\\\\\)*$"
syn match   cshNoEndlineBQ	contained "[^\`]\(\\\\\)*$"

syn region  cshDblQuote	start=+[^\\]"+lc=1 skip=+\\\\\|\\"+ end=+"+	contains=cshSpecial,cshShellVariables,cshExtVar,cshSelector,cshQtyWord,cshArgv,cshSubst,cshNoEndlineDQ,cshBckQuote
syn region  cshSnglQuote	start=+[^\\]'+lc=1 skip=+\\\\\|\\'+ end=+'+	contains=cshNoEndlineSQ
syn region  cshBckQuote	start=+[^\\]`+lc=1 skip=+\\\\\|\\`+ end=+`+	contains=cshNoEndlineBQ
syn region  cshDblQuote	start=+^"+ skip=+\\\\\|\\"+ end=+"+		contains=cshSpecial,cshExtVar,cshSelector,cshQtyWord,cshArgv,cshSubst,cshNoEndlineDQ
syn region  cshSnglQuote	start=+^'+ skip=+\\\\\|\\'+ end=+'+		contains=cshNoEndlineSQ
syn region  cshBckQuote	start=+^`+ skip=+\\\\\|\\`+ end=+`+		contains=cshNoEndlineBQ
syn cluster cshCommentGroup	contains=cshTodo
syn match   cshComment	"#.*$" contains=@cshCommentGroup

" A bunch of useful csh keywords
syn keyword cshStatement	alias	end	history	onintr	setenv	unalias
syn keyword cshStatement	cd	eval	kill	popd	shift	unhash
syn keyword cshStatement	chdir	exec	login	pushd	source
syn keyword cshStatement	continue	exit	logout	rehash	time	unsetenv
syn keyword cshStatement	dirs	glob	nice	repeat	umask	wait
syn keyword cshStatement	echo	goto	nohup

syn keyword cshConditional	break	case	else	endsw	switch
syn keyword cshConditional	breaksw	default	endif
syn keyword cshRepeat	foreach

" Special environment variables
syn keyword cshShellVariables	HOME	LOGNAME	PATH	TERM	USER

" Modifiable Variables without {}
syn match cshExtVar	"\$[a-zA-Z_][a-zA-Z0-9_]*\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\="		contains=cshModifier
syn match cshSelector	"\$[a-zA-Z_][a-zA-Z0-9_]*\[[a-zA-Z_]\+\]\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\="	contains=cshModifier
syn match cshQtyWord	"\$#[a-zA-Z_][a-zA-Z0-9_]*\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\="		contains=cshModifier
syn match cshArgv	"\$\d\+\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\="			contains=cshModifier
syn match cshArgv	"\$\*\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\="			contains=cshModifier

" Modifiable Variables with {}
syn match cshExtVar	"\${[a-zA-Z_][a-zA-Z0-9_]*\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\=}"		contains=cshModifier
syn match cshSelector	"\${[a-zA-Z_][a-zA-Z0-9_]*\[[a-zA-Z_]\+\]\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\=}"	contains=cshModifier
syn match cshQtyWord	"\${#[a-zA-Z_][a-zA-Z0-9_]*\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\=}"	contains=cshModifier
syn match cshArgv	"\${\d\+\(:h\|:t\|:r\|:q\|:x\|:gh\|:gt\|:gr\)\=}"			contains=cshModifier

" UnModifiable Substitutions
syn match cshSubstError	"\$?[a-zA-Z_][a-zA-Z0-9_]*:\(h\|t\|r\|q\|x\|gh\|gt\|gr\)"
syn match cshSubstError	"\${?[a-zA-Z_][a-zA-Z0-9_]*:\(h\|t\|r\|q\|x\|gh\|gt\|gr\)}"
syn match cshSubstError	"\$?[0$<]:\(h\|t\|r\|q\|x\|gh\|gt\|gr\)"
syn match cshSubst	"\$?[a-zA-Z_][a-zA-Z0-9_]*"
syn match cshSubst	"\${?[a-zA-Z_][a-zA-Z0-9_]*}"
syn match cshSubst	"\$?[0$<]"

" I/O redirection
syn match cshRedir	">>&!\|>&!\|>>&\|>>!\|>&\|>!\|>>\|<<\|>\|<"

" Handle set expressions
syn keyword cshSetStmt	contained set unset
syn region  cshSetExpr	transparent start="set\|unset" end="$\|;" contains=cshSetVariables,cshSetStmt,@cshQuoteList

" Operators and Expression-Using constructs
syn keyword cshExprUsing	contained if while exit then
syn match   cshOperator	contained "\(&&\|!\~\|!=\|<<\|<=\|==\|=\~\|>=\|>>\|\*\|\^\|\~\|||\|!\|\|%\|&\|+\|-\|/\|<\|>\||\)"
syn match   cshOperator	contained "[(){}]"
syn region  cshTest	transparent start="if\|while\|exit" skip="\\$" end="$\|;\|then" contains=cshOperator,cshExprUsing,@cshQuoteList


" Highlight special characters (those which have a backslash) differently
syn match cshSpecial	contained "\\\d\d\d\|\\[abcfnrtv\\]"
syn match cshNumber	"-\=\<\d\+\>"

" All other identifiers
"syn match cshIdentifier	"\<[a-zA-Z._][a-zA-Z0-9._]*\>"

" Shell Input Redirection (Here Documents)
if version < 600
  syn region cshHereDoc matchgroup=cshRedir start="<<-\=\s*\**END[a-zA-Z_0-9]*\**" matchgroup=cshRedir end="^END[a-zA-Z_0-9]*$"
  syn region cshHereDoc matchgroup=cshRedir start="<<-\=\s*\**EOF\**" matchgroup=cshRedir end="^EOF$"
else
  syn region cshHereDoc matchgroup=cshRedir start="<<-\=\s*\**\z(\h\w*\)\**" matchgroup=cshRedir end="^\z1$"
endif

" The default highlighting.
hi def link cshArgv	cshVariables
hi def link cshBckQuote	cshCommand
hi def link cshDblQuote	cshString
hi def link cshExprUsing	cshStatement
hi def link cshExtVar	cshVariables
hi def link cshHereDoc	cshString
hi def link cshNoEndlineBQ	cshNoEndline
hi def link cshNoEndlineDQ	cshNoEndline
hi def link cshNoEndlineSQ	cshNoEndline
hi def link cshQtyWord	cshVariables
hi def link cshRedir	cshOperator
hi def link cshSelector	cshVariables
hi def link cshSetStmt	cshStatement
hi def link cshSetVariables	cshVariables
hi def link cshSnglQuote	cshString
hi def link cshSubst	cshVariables

hi def link cshCommand	Statement
hi def link cshComment	Comment
hi def link cshConditional	Conditional
hi def link cshIdentifier	Error
hi def link cshModifier	Special
hi def link cshNoEndline	Error
hi def link cshNumber	Number
hi def link cshOperator	Operator
hi def link cshRedir	Statement
hi def link cshRepeat	Repeat
hi def link cshShellVariables	Special
hi def link cshSpecial	Special
hi def link cshStatement	Statement
hi def link cshString	String
hi def link cshSubstError	Error
hi def link cshTodo	Todo
hi def link cshVariables	Type

let b:current_syntax = "csh"

" vim: ts=18
