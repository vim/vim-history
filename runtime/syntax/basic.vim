" Vim syntax file
" Language:	BASIC
" Maintainer:	Allan Kelly <Allan.Kelly@ed.ac.uk>
" Last change:	1999 Jun 14

" First version based on Micro$soft QBASIC circa 1989, as documented in
" 'Learn BASIC Now' by Halvorson&Rygmyr. Microsoft Press 1989.
" This syntax file not a complete implementation yet.  Send suggestions to the
" maintainer.

" Remove any old syntax stuff hanging around
syn clear
" A bunch of useful BASIC keywords
syn keyword basicStatement	BEEP BLOAD BSAVE CALL ABSOLUTE CHAIN CHDIR CIRCLE
syn keyword basicStatement	CLEAR CLOSE CLS COLOR COM COMMON CONST DATA
syn keyword basicStatement	DECLARE DEF DEFDBL DEFINT DEFLNG DEFSNG DEFSTR DIM
syn keyword basicStatement	DO LOOP DRAW END ENVIRON ERASE ERROR EXIT FIELD
syn keyword basicStatement	FILES FOR NEXT FUNCTION GET GOSUB GOTO IF THEN ELSE
syn keyword basicStatement	INPUT INPUT# IOCTL KEY KILL LET LINE LOCATE
syn keyword basicStatement	LOCK UNLOCK LPRINT USING LSET MKDIR NAME
syn keyword basicStatement	ON ERROR OPEN OPTION BASE OUT PAINT PALETTE PCOPY
syn keyword basicStatement	PEN PLAY PMAP POKE PRESET PRINT PRINT# USING PSET
syn keyword basicStatement	PUT RANDOMIZE READ REDIM RESET RESTORE RESUME
syn keyword basicStatement	RETURN RMDIR RSET RUN SEEK SELECT CASE
syn keyword basicStatement	SHARED SHELL SLEEP SOUND STATIC STOP STRIG SUB
syn keyword basicStatement	SWAP SYSTEM TIMER TROFF TRON TYPE UNLOCK
syn keyword basicStatement	VIEW WAIT WHILE WEND WIDTH WINDOW WRITE
syn keyword basicStatement	DATE$ MID$ TIME$

syn keyword basicFunction	ABS ASC ATN CDBL CINT CLNG COS CSNG CSRLIN CVD CVDMBF
syn keyword basicFunction	CVI CVL CVS CVSMBF EOF ERDEV ERL ERR EXP FILEATTR
syn keyword basicFunction	FIX FRE FREEFILE INP INSTR INT LBOUND LEN LOC LOF
syn keyword basicFunction	LOG LPOS PEEK PEN POINT POS RND SADD SCREEN SEEK
syn keyword basicFunction	SETMEM SGN SIN SPC SQR STICK STRIG TAB TAN UBOUND
syn keyword basicFunction	VAL VALPTR VALSEG VARPTR VARSEG
syn keyword basicFunction	CHR\$ COMMAND$ DATE$ ENVIRON$ ERDEV$ HEX$ INKEY$
syn keyword basicFunction	INPUT$ IOCTL$ LCASES$ LAFT$ LTRIM$ MID$ MKDMBF$ MKD$
syn keyword basicFunction	MKI$ MKL$ MKSMBF$ MKS$ OCT$ RIGHT$ RTRIM$ SPACE$
syn keyword basicFunction	STR$ STRING$ TIME$ UCASE$ VARPTR$
syn keyword basicTodo contained	TODO

"integer number, or floating point number without a dot.
syn match  basicNumber		"\<\d\+\>"
"floating point number, with dot
syn match  basicNumber		"\<\d\+\.\d*\>"
"floating point number, starting with a dot
syn match  basicNumber		"\.\d\+\>"

" String and Character contstants
syn match   basicSpecial contained "\\\d\d\d\|\\."
syn region  basicString		  start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=basicSpecial

syn region  basicComment	start="REM" end="$" contains=basicTodo
syn region  basicLineNumber	start="^\d" end="\s"
syn match   basicTypeSpecifier  "[a-zA-Z0-9][\$%&!#]"ms=s+1
" Used with OPEN statement
syn match   basicFilenumber  "#\d\+"
"syn sync ccomment basicComment
" syn match   basicMathsOperator "[<>+\*^/\\=-]"
syn match   basicMathsOperator   "-\|=\|[:<>+\*^/\\]\|AND\|OR"

if !exists("did_basic_syntax_inits")
  let did_basic_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link basicLabel		Label
  hi link basicConditional	Conditional
  hi link basicRepeat		Repeat
  hi link basicLineNumber	Comment
  hi link basicNumber		Number
  hi link basicError		Error
  hi link basicStatement	Statement
  hi link basicString		String
  hi link basicComment		Comment
  hi link basicSpecial		Special
  hi link basicTodo		Todo
  hi link basicFunction		Identifier
  hi link basicTypeSpecifier Type
  hi link basicFilenumber basicTypeSpecifier
  hi basicMathsOperator term=bold cterm=bold gui=bold
endif

let b:current_syntax = "basic"

" vim: ts=8
