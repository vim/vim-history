" Vim syntax file
" Language:	shell (sh) Korn shell (ksh) bash (sh)
" Maintainer:	Lennart Schultz <Lennart.Schultz@ecmwf.int>
" Last change:	Aug 26, 1998
"
" Using the following VIM variables:
" is_kornshell               if defined enhance with kornshell syntax
" is_bash                    if defined enhance with bash syntax
"
" This is a complete redesign including many ideas from
" Éric Brunet (eric.brunet@ens.fr)
"
" Updated 1997 November 21
"	With many good inputs from Éric Brunet, especially he got the
"	case statement to work!
" Updated 1997 November 24
" 	Sync included with thanks to Dr. Charles E. Campbell Jr <Charles.Campbell@gsfc.nasa.gov>
"	Echo included with thanks to Éric Brunet
"	[ \t] removed using \s instead
" Updated 1998 March 24
"	Error in case statement corrected with thanks to  Ronald.Schild rs@dps.de
" Updated 1998 March 25
"	Error correction from Dr. Charles E. Campbell Jr 
" Updated 1998 March 29
"	added sh_minlines (Bram Moolenaar)
" Updated 1998 April 16
"	added check for ksh and bash for $( .. ) command substitution
"	Error correction from Dr. Charles E. Campbell Jr 
" Updated 1998 August 20 (tnx to Donovan Rebbechi) (cec)
"	embedded $(...$(...)) now handled
"	embedded echo fixed (sequence problem)
"	bash's declare allowed ${} and $()
"	case/esac if/fi do/done  now shConditional -> Conditional highlighting
"	numerous bash keywords
"	Wierd: problems fixed with $() shCommandSub having matchgroup=shDeref
" Updated 1998 August 21 (cec)
"	case ... in ... esac now handles embedded case...in...esac
" Updated 1998 August 24,25,26 (cec)
"	fixed: case-esac, export `...`, export $VAR, set $VAR
"	new: kshStatement, shCaseBar
"	commented out a case now works right

" Remove any old syntax stuff hanging around
syn clear
" sh syntax is case sensitive
syn case match

" This one is needed INSIDE a CommandSub, so that
" `echo bla` be correct
syn region shEcho matchgroup=shStatement start="\<echo\>" skip="\\$" matchgroup=shOperator end="$" matchgroup=NONE end="[<>;&|]"me=e-1 end="\d[<>]"me=e-2 end="#"me=e-1 contains=shNumber,shCommandSub,shSinglequote,shDeref,shSpecialVar,shSpecial,shOperator,shDoubleQuote

" This must be after the strings, so that bla \" be correct
syn region shEmbeddedEcho contained matchgroup=shStatement start="\<echo\>" skip="\\$" matchgroup=shOperator end="$" matchgroup=NONE end="[<>;&|`)]"me=e-1 end="\d[<>]"me=e-2 end="#"me=e-1 contains=shNumber,shSinglequote,shDeref,shSpecialVar,shSpecial,shOperator,shDoubleQuote

"Error Codes
syn match   shDoError "\<done\>"
syn match   shIfError "\<fi\>"
syn match   shInError "\<in\>"
syn match   shCaseError ";;"
syn match   shEsacError "\<esac\>"
syn match   shCurlyError "}"
syn match   shParenError ")"
if exists("is_kornshell")
 syn match     shDTestError "]]"
endif
syn match     shTestError "]"
  
" Options interceptor
syn match   shOption  "[\-+][a-zA-Z0-9]\+\>"

" Tests
"======
syn cluster shTestList	contains=shFunction,shTestError,shIdentifier,shCase,shDTestError,shCaseBar
if exists("is_kornshell")
 syn region  shNone transparent matchgroup=shOperator start="\[\[" skip=+\\\\\|\\$+ end="\]\]" contains=ALLBUT,@shTestList
endif
syn region  shNone transparent matchgroup=shOperator start="\[" skip=+\\\\\|\\$+ end="\]" contains=ALLBUT,@shTestList
syn region  shNone transparent matchgroup=shStatement start="\<test\>" skip=+\\\\\|\\$+ matchgroup=NONE end="[;&|]"me=e-1 end="$" contains=ALLBUT,shFunction,shIdentifier,shCase,shCaseBar
syn match   shTestOpr contained "[!=]\|-.\>\|-\(nt\|ot\|ef\|eq\|ne\|lt\|le\|gt\|ge\)\>"

" do, if
syn cluster shLoopList	contains=shFunction,shDoError,shCase,shInEsac,shCaseBar
syn region shDo  transparent matchgroup=shConditional start="\<do\>" matchgroup=shConditional end="\<done\>" contains=ALLBUT,@shLoopList
syn region shIf  transparent matchgroup=shConditional start="\<if\>" matchgroup=shConditional end="\<fi\>"   contains=ALLBUT,@shLoopList,shCommandSub
syn region shFor matchgroup=shStatement start="\<for\>" end="\<in\>" contains=ALLBUT,@shLoopList

" case
syn region  shCaseEsac matchgroup=shConditional start="\<case\>" matchgroup=shConditional end="\<esac\>" contains=shCaseIn,shCase,shCaseBar,shDeref,shComment
syn keyword shCaseIn  contained skipwhite skipnl in			nextgroup=shCase,shCaseBar,shComment
syn region  shCase    contained skipwhite skipnl matchgroup=shConditional start="[^)]\{-})"ms=s,hs=e  end=";;" end="esac"me=s-1 contains=ALLBUT,shFunction,shCaseError,shCase,shCaseBar nextgroup=shCase,shCaseBar,shComment
syn match   shCaseBar contained "[^|)]\{-}|"hs=e			nextgroup=shCase,shCaseBar

syn region shNone  transparent matchgroup=shOperator start="{" end="}"		contains=ALLBUT,shCurlyError,shCase,shCaseBar
syn region shSubSh transparent matchgroup=shOperator start="(" end=")"		contains=ALLBUT,shParenError,shCase,shCaseBar

" Misc
"=====
syn match   shOperator	 "[!&;|=]"
syn match   shWrapLineOperator "\\$"
syn region  shCommandSub   start="`" skip="\\`" end="`" contains=ALLBUT,shFunction,shCommandSub,shTestOpr,shCase,shEcho,shCaseBar
if exists("is_kornshell") || exists("is_bash")
 syn region  shCommandSub matchgroup=shDeref start="$(" end=")" contains=ALLBUT,shFunction,shTestOpr,shCase,shEcho,shCaseBar
endif

if exists("is_bash")
 syn keyword bashSpecialVariables contained	BASH	HISTCONTROL	LANG	OPTERR	PWD
 syn keyword bashSpecialVariables contained	BASH_ENV	HISTFILE	LC_ALL	OPTIND	RANDOM
 syn keyword bashSpecialVariables contained	BASH_VERSINFO	HISTFILESIZE	LC_COLLATE	OSTYPE	REPLY
 syn keyword bashSpecialVariables contained	BASH_VERSION	HISTIGNORE	LC_MESSAGES	PATH	SECONDS
 syn keyword bashSpecialVariables contained	CDPATH	HISTSIZE	LINENO	PIPESTATUS	SHELLOPTS
 syn keyword bashSpecialVariables contained	DIRSTACK	HOME	MACHTYPE	PPID	SHLVL
 syn keyword bashSpecialVariables contained	EUID	HOSTFILE	MAIL	PROMPT_COMMAND	TIMEFORMAT
 syn keyword bashSpecialVariables contained	FCEDIT	HOSTNAME	MAILCHECK	PS1	TIMEOUT
 syn keyword bashSpecialVariables contained	FIGNORE	HOSTTYPE	MAILPATH	PS2	UID
 syn keyword bashSpecialVariables contained	GLOBIGNORE	IFS	OLDPWD	PS3	auto_resume
 syn keyword bashSpecialVariables contained	GROUPS	IGNOREEOF	OPTARG	PS4	histchars
 syn keyword bashSpecialVariables contained	HISTCMD	INPUTRC
 syn keyword bashStatement		chmod	fgrep	install	rm	sort
 syn keyword bashStatement		clear	find	less	rmdir	strip
 syn keyword bashStatement		du	gnufind	ls	rpm	tail
 syn keyword bashStatement		egrep	gnugrep	mkdir	sed	touch
 syn keyword bashStatement		expr	grep	mv	sleep
 syn keyword bashAdminStatement	daemon	killproc	reload	start	stop
 syn keyword bashAdminStatement	killall	nice	restart	status
endif

if exists("is_kornshell")
 syn keyword kshSpecialVariables contained	CDPATH	HISTFILE	MAILCHECK	PPID	RANDOM
 syn keyword kshSpecialVariables contained	COLUMNS	HISTSIZE	MAILPATH	PS1	REPLY
 syn keyword kshSpecialVariables contained	EDITOR	HOME	OLDPWD	PS2	SECONDS
 syn keyword kshSpecialVariables contained	ENV	IFS	OPTARG	PS3	SHELL
 syn keyword kshSpecialVariables contained	ERRNO	LINENO	OPTIND	PS4	TMOUT
 syn keyword kshSpecialVariables contained	FCEDIT	LINES	PATH	PWD	VISUAL
 syn keyword kshSpecialVariables contained	FPATH	MAIL
 syn keyword kshStatement		cat	expr	less	printenv	strip
 syn keyword kshStatement		chmod	fgrep	ls	rm	stty
 syn keyword kshStatement		clear	find	mkdir	rmdir	tail
 syn keyword kshStatement		cp	grep	mv	sed	touch
 syn keyword kshStatement		du	install	nice	sort	tput
 syn keyword kshStatement		egrep	killall
endif

syn match   shSource	"^\.\s"
syn match   shSource	"\s\.\s"
syn region  shColon	start="^\s*:" end="$\|" end="#"me=e-1 contains=ALLBUT,shFunction,shTestOpr,shCase,shCaseBar

" Comments
"=========
syn keyword	shTodo    contained	TODO
syn match	shComment		"#.*$" contains=shTodo

" String and Character constants
"===============================
syn match   shNumber	"-\=\<\d\+\>"
syn match   shSpecial	contained "\\\d\d\d\|\\[abcfnrtv]"
syn region  shSinglequote	matchgroup=shOperator start=+'+ end=+'+
syn region  shDoubleQuote     matchgroup=shOperator start=+"+ skip=+\\"+ end=+"+ contains=shDeref,shCommandSub,shSpecialShellVar,shSpecial
syn match   shSpecial	"\\[\\\"\'`$]"

" File redirection highlighted as operators
"==========================================
syn match	shRedir	"\d\=>\(&[-0-9]\)\="
syn match	shRedir	"\d\=>>-\="
syn match	shRedir	"\d\=<\(&[-0-9]\)\="
syn match	shRedir	"\d<<-\="

" Shell Input Redirection (Here Documents)
syn region shHereDoc matchgroup=shRedir start="<<-\=\s*\**END[a-zA-Z_0-9]*\**" matchgroup=shRedir end="^END[a-zA-Z_0-9]*$"
syn region shHereDoc matchgroup=shRedir start="<<-\=\s*\**EOF\**" matchgroup=shRedir end="^EOF$"

" Identifiers
"============
syn match  shIdentifier "\<[a-zA-Z_][a-zA-Z0-9_]*\>="me=e-1
syn cluster shIdList	contains=shTestError,shCurlyError,shWrapLineOperator,shDeref,shCommandSub
if exists("is_bash")
 syn region shIdentifier matchgroup=shStatement start="\<\(declare\|typeset\|local\|export\|set\|unset\)\>[^/]"me=e-1 matchgroup=shOperator end="$\|[;&|]" matchgroup=NONE end="#\|="me=e-1 contains=@shIdList
elseif exists("is_kornshell")
 syn region shIdentifier matchgroup=shStatement start="\<\(typeset\|set\|export\|unset\)\>[^/]"me=e-1 matchgroup=shOperator end="$\|[;&|]" matchgroup=NONE end="#\|="me=e-1 contains=@shIdList
else
 syn region shIdentifier matchgroup=shStatement start="\<\(set\|export\|unset\)\>[^/]"me=e-1 matchgroup=shOperator end="$\|[;&|]" matchgroup=NONE end="#\|="me=e-1 contains=@shIdList
endif

" The [^/] in the start pattern is a kludge to avoid bad
" highlighting with cd /usr/local/lib...
syn region  shFunction transparent matchgroup=shFunctionName 	start="^\s*\<[a-zA-Z_][a-zA-Z0-9_]*\>\s*()\s*{" end="}" contains=ALLBUT,shFunction,shCurlyError,shCase,shCaseBar
syn region shDeref	start="\${" end="}"
syn match  shDeref	"\$\<[a-zA-Z_][a-zA-Z0-9_]*\>"
syn match  shSpecialShellVar "\$[-#@*$?!0-9]"

" A bunch of useful sh keywords
syn keyword shStatement	break	eval	newgrp	return	ulimit
syn keyword shStatement	cd	exec	pwd	shift	umask
syn keyword shStatement	chdir	exit	read	test	wait
syn keyword shStatement	continue	kill	readonly	trap
syn keyword shConditional	elif	else	then	while

if exists("is_kornshell") || exists("is_bash")
 syn keyword shFunction	function
 syn keyword shRepeat	select	until
 syn keyword shStatement	alias	getopts	let	suspend	type
 syn keyword shStatement	bg	hash	print	time	unalias
 syn keyword shStatement	fc	history	stop	times	whence
 syn keyword shStatement	fg	jobs
 if exists("is_bash")
 syn keyword shStatement	bind	disown	history	popd	shopt
 syn keyword shStatement	builtin	enable	logout	pushd	source
 syn keyword shStatement	dirs	help
 else
  syn keyword shStatement	login	newgrp
 endif
endif

" Syncs
" =====
if !exists("sh_minlines")
  let sh_minlines = 100
endif
if !exists("sh_maxlines")
  let sh_maxlines = 2 * sh_minlines
endif
exec "syn sync minlines=" . sh_minlines . " maxlines=" . sh_maxlines
syn sync match shDoSync       grouphere  shDo       "\<do\>"
syn sync match shDoSync       groupthere shDo       "\<done\>"
syn sync match shIfSync       grouphere  shIf       "\<if\>"
syn sync match shIfSync       groupthere shIf       "\<fi\>"
syn sync match shForSync      grouphere  shFor      "\<for\>"
syn sync match shForSync      groupthere shFor      "\<in\>"
syn sync match shCaseEsacSync grouphere  shCaseEsac "\<case\>"
syn sync match shCaseEsacSync groupthere shCaseEsac "\<esac\>"

if !exists("did_sh_syntax_inits")
 " The default methods for highlighting. Can be overridden later
 let did_sh_syntax_inits = 1

 hi link shCaseBar		shConditional
 hi link shCaseIn		shConditional
 hi link shColon		shStatement
 hi link shDeref		shShellVariables
 hi link shDoubleQuote		shString
 hi link shEcho		shString
 hi link shEmbeddedEcho		shString
 hi link shHereDoc		shString
 hi link shRedir		shOperator
 hi link shSinglequote		shString
 hi link shSource		shOperator
 hi link shSpecialShellVar		shShellVariables
 hi link shTestOpr		shConditional
 hi link shWrapLineOperator		shOperator

 if exists("is_bash")
  hi link bashAdminStatement		shStatement
  hi link bashSpecialVariables	shShellVariables
  hi link bashStatement		shStatement
 endif
 if exists("is_kornshell")
  hi link kshSpecialVariables		shShellVariables
  hi link kshStatement		shStatement
 endif

 hi link shCaseError		Error
 hi link shCurlyError		Error
 hi link shDoError		Error
 hi link shEsacError		Error
 hi link shIfError		Error
 hi link shInError		Error
 hi link shParenError		Error
 hi link shTestError		Error
 if exists("is_kornshell")
  hi link shDTestError		Error
 endif

 hi link shCommandSub		Special
 hi link shComment		Comment
 hi link shConditional		Conditional
 hi link shFunction		Function
 hi link shFunctionName		Function
 hi link shIdentifier		Identifier
 hi link shNumber		Number
 hi link shOperator		Operator
 hi link shRepeat		Repeat
 hi link shShellVariables		PreProc
 hi link shSpecial		Special
 hi link shStatement		Statement
 hi link shString		String
 hi link shTodo		Todo
endif

let b:current_syntax = "sh"

" vim: ts=15
