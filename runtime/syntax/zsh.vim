" Vim syntax file
" Language:	Z shell (zsh)
" Maintainer:	Felix von Leitner <leitner@math.fu-berlin.de>
" Heavily based on sh.vim by Lennart Schultz
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn region	zshSinglequote	start=+'+ skip=+\\'+ end=+'+
" A bunch of useful zsh keywords
" syn keyword	zshFunction	function
syn keyword	zshStatement	bg break cd chdir continue echo eval exec
syn keyword	zshStatement	exit export fg getopts hash jobs kill
syn keyword	zshStatement	pwd read readonly return set zshift function
syn keyword	zshStatement	stop suspend test times trap type ulimit
syn keyword	zshStatement	umask unset wait setopt compctl source
syn keyword	zshStatement	whence disown shift which unhash unalias
syn keyword	zshStatement	alias functions unfunction getln disable
syn keyword	zshStatement	vared getopt enable unsetopt autoload
syn keyword	zshStatement	bindkey pushln command limit unlimit fc
syn keyword	zshStatement	print builtin noglob sched r time
syn keyword	zshStatement	typeset declare local integer

syn keyword	zshConditional	if else esac case then elif fi in
syn keyword	zshRepeat	while for do done

" Following is worth to notice: command substitution, file redirection and functions (so these features turns red)
syn match	zshFunctionName	"\h\w*\s*()"
syn region	zshCommandSub	start=+`+ skip=+\\`+ end=+`+
" contains=ALLBUT,zshFunction
syn match	zshRedir	"\d\=\(<\|<<\|>\|>>\)\(|\|&\d\)\="

syn keyword	zshTodo contained TODO

syn keyword	zshShellVariables	USER LOGNAME HOME PATH CDPATH SHELL
syn keyword	zshShellVariables	LC_TYPE LC_MESSAGE MAIL MAILCHECK
syn keyword	zshShellVariables	PS1 PS2 IFS EGID EUID ERRNO GID UID
syn keyword	zshShellVariables	HOST LINENO MACHTYPE OLDPWD OPTARG
syn keyword	zshShellVariables	OPTIND OSTYPE PPID PWD RANDOM SECONDS
syn keyword	zshShellVariables	SHLVL TTY signals TTYIDLE USERNAME
syn keyword	zshShellVariables	VENDOR ZSH_NAME ZSH_VERSION ARGV0
syn keyword	zshShellVariables	BAUD COLUMNS cdpath DIRSTACKSIZE
syn keyword	zshShellVariables	FCEDIT fignore fpath histchars HISTCHARS
syn keyword	zshShellVariables	HISTFILE HISTSIZE KEYTIMEOUT LANG
syn keyword	zshShellVariables	LC_ALL LC_COLLATE LC_CTYPE LC_MESSAGES
syn keyword	zshShellVariables	LC_TIME LINES LISTMAX LOGCHECK mailpath
syn keyword	zshShellVariables	MAILPATH MANPATH manpath module_path
syn keyword	zshShellVariables	MODULE_PATH NULLCMD path POSTEDIT
syn keyword	zshShellVariables	PS3 PS4 PROMPT PROMPT2 PROMPT3 PROMPT4
syn keyword	zshShellVariables	psvar PSVAR prompt READNULLCMD
syn keyword	zshShellVariables	REPORTTIME RPROMPT RPS1 SAVEHIST
syn keyword	zshShellVariables	SPROMPT STTY TIMEFMT TMOUT TMPPREFIX
syn keyword	zshShellVariables	watch WATCH WATCHFMT WORDCHARS ZDOTDIR
syn match	zshSpecialShellVar	"\$[-#@*$?!0-9]"
syn keyword	zshSetVariables		ignoreeof noclobber
syn region	zshDerefOpr	start="\${" end="}" contains=zshShellVariables
syn match	zshDerefIdentifier	"\$[a-zA-Z_][a-zA-Z0-9_]*\>"
syn match	zshOperator		"[][}{&;|)(]"

" String and Character contstants
" Highlight special characters (those which have a backslash) differently
syn match   zshSpecial	contained "\\\d\d\d\|\\[abcfnrtv\\]"


syn match  zshNumber		"-\=\<\d\+\>"
syn match  zshComment	"#.*$" contains=zshNumber,zshTodo


syn match zshTestOpr	"-\<[oeaznlg][tfqet]\=\>\|!\==\|-\<[b-gkLprsStuwjxOG]\>"
"syn region zshTest           start="\[" skip="\\$" end="\]" contains=zshString,zshTestOpr,zshDerefIdentifier,zshDerefOpr
syn region  zshString	start=+"+  skip=+\\"+  end=+"+  contains=zshSpecial,zshOperator,zshDerefIdentifier,zshDerefOpr,zshSpecialShellVar,zshSinglequote,zshCommandSub

" The default highlighting.
hi def link zshSinglequote	zshString
hi def link zshConditional	zshStatement
hi def link zshRepeat		zshStatement
hi def link zshFunctionName	zshFunction
hi def link zshCommandSub	zshOperator
hi def link zshRedir		zshOperator
hi def link zshSetVariables	zshShellVariables
hi def link zshSpecialShellVar	zshShellVariables
hi def link zshTestOpr		zshOperator
hi def link zshDerefOpr		zshSpecial
hi def link zshDerefIdentifier	zshShellVariables
hi def link zshOperator		Operator
hi def link zshStatement	Statement
hi def link zshNumber		Number
hi def link zshString		String
hi def link zshComment		Comment
hi def link zshSpecial		Special
hi def link zshTodo		Todo
hi def link zshShellVariables	Special
"  hi def zshOperator		term=underline ctermfg=6 guifg=Purple gui=bold
"  hi def zshShellVariables	term=underline ctermfg=2 guifg=SeaGreen gui=bold
"  hi def zshFunction		term=bold ctermbg=1 guifg=Red

let b:current_syntax = "zsh"

" vim: ts=8
