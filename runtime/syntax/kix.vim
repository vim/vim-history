" Vim syntax file
" Language:	Kixtart
" Maintainer:	Nigel Gibbs <nigel@gibbsoft.com>
" Based on:     KiXtart 95 Version 3.61, from Windows NT® 4.0 Resource Kit
" Last change:	November 2000

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

syn keyword kixStatement : ; ? BEEP BIG BREAK CALL CD CLS COLOR COOKIE1 COPY 
syn keyword kixStatement DEL DIM DISPLAY DO UNTIL EXIT FLUSHKB GET GETS GLOBAL 
syn keyword kixStatement GO GOSUB GOTO IF ELSE ENDIF MD PASSWORD PLAY QUIT RD 
syn keyword kixStatement RETURN RUN SELECT CASE … ENDSELECT SET SETL SETM 
syn keyword kixStatement SETTIME SHELL SLEEP SMALL USE WHILE LOOP

syn keyword kixFunction ADDKEY ADDPRINTERCONNECTION ADDPROGRAMGROUP ADDPROGRAMITEM 
syn keyword kixFunction ASC AT BOX CHR CLOSE COMPAREFILETIMES DECTOHEX DELKEY 
syn keyword kixFunction DELPRINTERCONNECTION DELPROGRAMGROUP DELPROGRAMITEM DELTREE 
syn keyword kixFunction DELVALUE DIR ENUMGROUP ENUMKEY ENUMLOCALGROUP ENUMVALUE EXECUTE 
syn keyword kixFunction EXIST EXISTKEY GETDISKSPACE GETFILEATTR GETFILESIZE GETFILETIME 
syn keyword kixFunction GETFILEVERSION INGROUP INSTR LCASE LEN LOADHIVE LOADKEY LOGEVENT 
syn keyword kixFunction LOGOFF LTRIM MESSAGEBOX OLECALLFUNC OLECALLPROC OLECREATEOBJECT 
syn keyword kixFunction OLEGETOBJECT OLEGETPROPERTY OLEGETSUBOBJECT OLEPUTPROPERTY 
syn keyword kixFunction OLERELEASEOBJECT OPEN READLINE READPROFILESTRING READTYPE 
syn keyword kixFunction READVALUE REDIRECTOUTPUT RND RTRIM SAVEKEY SENDKEYS SENDMESSAGE 
syn keyword kixFunction SETCONSOLE SETDEFAULTPRINTER SETFILEATTR SETFOCUS SETWALLPAPER 
syn keyword kixFunction SHOWPROGRAMGROUP SHUTDOWN SRND SUBSTR UCASE UNLOADHIVE VAL 
syn keyword kixFunction WRITELINE WRITEPROFILESTRING WRITEVALUE

syn keyword kixConditional IF ELSE ENDIF

syn keyword kixTodo contained	TODO

"integer number, or floating point number without a dot.
syn match  kixNumber		"\<[0-9]\+\>"

"floating point number, with dot
syn match  kixNumber		"\<[0-9]\+\.[0-9]*\>"

"floating point number, starting with a dot
syn match  kixNumber		"\.[0-9]\+\>"


" String and Character contstants
syn region  kixString		start=+"+ end=+"+ contains=kixSpecial
syn match   kixSpecial   "\(\$#\|\$\+\|@\$*\|\&\$*\)[a-zA-Z_][a-zA-Z0-9_]*\(\(::\|'\)[a-zA-Z_][a-zA-Z0-9_]*\)*\>"

syn region  kixComment	start=";" end="$" contains=kixTodo
syn match   kixTypeSpecifier  "[a-zA-Z0-9][\$%&!#]"ms=s+1

" Used with OPEN statement
syn match   kixFilenumber  "#[0-9]\+"

syn match   kixMathsOperator   "-\|=\|[:<>+\*^/\\]\|AND\|OR"

" The default highlighting.
hi def link kixLabel		Label
hi def link kixConditional	Conditional
hi def link kixRepeat		Repeat
hi def link kixNumber		Number
hi def link kixError		Error
hi def link kixStatement        Statement
hi def link kixString		String
hi def link kixComment		Comment
hi def link kixSpecial		Special
hi def link kixTodo		Todo
hi def link kixFunction		Identifier
hi def link kixTypeSpecifier	Type
hi def link kixFilenumber	kixTypeSpecifier
hi def kixMathsOperator term=bold cterm=bold gui=bold

let b:current_syntax = "kix"

" vim: ts=8
