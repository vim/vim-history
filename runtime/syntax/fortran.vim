" Vim syntax file
" Language:	Fortran
" Maintainer:	Preben "Peppe" Guldberg <c928400@student.dtu.dk>
"		Original author: Mario Eusebio
" Last Change:	1999 Jul 05

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

syn keyword fortranStatement		return continue
syn keyword fortranLabel		go to goto
syn keyword fortranConditional		if else elseif endif then
syn keyword fortranRepeat		do

syn keyword fortranTodo			contained TODO

syn region fortranString		start=+"+ end=+"+	oneline
syn region fortranString		start=+'+ end=+'+	oneline

" Format strings are used with READ, WRITE and PRINT
" READ and PRINT can be used without paretheses (and '(...)' is caught as '...' )
" FMTs can span several lines. This is not supoorted here for simple READ and PRINT
syn match  fortranReadWrite		"\<\(read\|print\)\>"
syn region fortranFormatString		contained start=+'(+ end=+)'+ contains=fortranContinueLine
syn region fortranReadWriteFunc		matchgroup=fortranReadWrite start="\(read\|write\)\s*("rs=e-1,he=e-1 skip=")'" matchgroup=NONE end=")" contains=fortranFormatString

" If you don't like initial tabs in fortran (or at all)
"syn match fortranIniTab		"^\t.*$"
"syn match fortranTab			"\t"

" This is valid with Gnu Fortran
"syn match  fortranSpecial contained "\\\d\{3}\|\\."
"syn region fortranString	start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=fortranSpecial
"syn region fortranString	start=+'+ skip=+\\\\\|\\"+ end=+'+ contains=fortranSpecial

" Any integer
syn match fortranNumber		"-\=\<\d\+\>"
" floating point number, with dot, optional exponent
syn match fortranFloat		"\<\d\+\.\d*\([edED][-+]\=\d\+\)\=\>"
" floating point number, starting with a dot, optional exponent
syn match fortranFloat		"\.\d\+\([edED][-+]\=\d\+\)\=\>"
" floating point number, without dot, with exponent
syn match fortranFloat		"\<\d\+[edED][-+]\=\d\+\>"

syn match fortranOperator		"\.\(gt\|ge\|lt\|le\)\."
syn match fortranOperator		"\.\(eq\|ne\|eqv\|neqv\)\."
syn match fortranOperator		"\.\(and\|or\|not\)\."

syn match fortranLogicalConstant	"\.\(true\|false\)\."

"syn match fortranIdentifier		"\<[a-zA-Z_][a-zA-Z0-9_]*\>"

"syn match fortranDelimiter		"[()]"

" any character other than a 'c' or '!' in the first column is an error
" (labelnumbers iare handled below, though)
syn match fortranCommentError		"^\ \{,4}[^\ \t0-9].*$"
syn match fortranComment	"^[cC*].*$"	contains=fortranTodo,fortranTab
syn match fortranComment	"!.*$"		contains=fortranTodo,fortranTab
syn match fortranContinueLine		"^\ \{5}\S"ms=e

syn match fortranNoLabelNumber		"^\d\{5}\S.*$"
syn match fortranNoLabelNumber		"^\ \d\{4}\S.*$"
syn match fortranNoLabelNumber		"^\ \{2}\d\{3}\S.*$"
syn match fortranNoLabelNumber		"^\ \{3}\d\{2}\S.*$"
syn match fortranNoLabelNumber		"^\ \{4}\d\S.*$"

syn match fortranLabelNumber		"^\d\{1,5}\>"
syn match fortranLabelNumber		"^\ \d\{1,4}\>"
syn match fortranLabelNumber		"^\ \{2}\d\{1,3}\>"
syn match fortranLabelNumber		"^\ \{3}\d\{1,2}\>"
syn match fortranLabelNumber		"^\ \{4}\d\>"

"syn match fortranPreCondit		"^#define\>"
"syn match fortranPreCondit		"^#include\>"

" Treat all past 72nd column as a comment. Do not work with tabs!
" Breaks down when 72-73rd column is in another match (eg number or keyword)
"syn match  fortranComment		"^.\{-72}.*$"lc=72

syn keyword fortranType		character complex double implicit integer
syn keyword fortranType		logical precision real real*8

syn keyword fortranStructure	common dimension equivalence external
syn keyword fortranStructure	intrinsic parameter save

syn keyword fortranUnitHeader	call data end function program subroutine

" READ, WRITE and PRINT are handled above (as they take FMT arguments)
syn keyword fortranReadWrite	backspace close inquire open rewind

syn keyword fortranFormat	access blank err file fmt form format
syn keyword fortranFormat	iostat rec recl status unit

syn keyword fortranStopPause	stop pause

syn keyword fortranImplicit	abs acos aint atan asin cos cosh aimag
syn keyword fortranImplicit	anint atan2 char cmplx conjg dble dim
syn keyword fortranImplicit	dprod exp ichar index int len lge lgt
syn keyword fortranImplicit	lle llt log log10 max min mod nint sin
syn keyword fortranImplicit	sinh sign sqrt tan tanh

syn keyword fortranSpecific	cabs ccos cexp clog csin csqrt dabs
syn keyword fortranSpecific	dacos dasin datan datan2 dcos dcosh
syn keyword fortranSpecific	ddim dexp dint dlog dlog10 dmod
syn keyword fortranSpecific	dnint dsign dsin dsinh dsqrt dtan
syn keyword fortranSpecific	dtanh iabs idim idnint isign

" This syntax highlighting file support Fortran 77 by default
"syn keyword fortranExtended	allocate assign block case contains cycle
"syn keyword fortranExtended	deallocate default elsewhere enddo endfile
"syn keyword fortranExtended	endwhile entry equivalence exit interface
"syn keyword fortranExtended	module nullify only operator procedure
"syn keyword fortranExtended	recursive select use where while allocatable
"syn keyword fortranExtended	in include inout intent kind namelist none
"syn keyword fortranExtended	optional out pointer private public result
"syn keyword fortranExtended	sequence target type achar adjustl adjustr
"syn keyword fortranExtended	all allocated any bit_size break btest carg
"syn keyword fortranExtended	ceiling conjg count cshift date_and_time
"syn keyword fortranExtended	digits dim dot_product dvchk eoshift epsilon
"syn keyword fortranExtended	error exponent floor flush fraction getcl
"syn keyword fortranExtended	huge iachar iand ibclr ibits ibset ichar
"syn keyword fortranExtended	ieor intrup invalop ior iostat_msg ishft
"syn keyword fortranExtended	ishftc lbound len_trim matmul maxexponent
"syn keyword fortranExtended	maxloc maxval merge minexponent minloc minval
"syn keyword fortranExtended	modulo mvbits nbreak ndperr ndpexc nearest
"syn keyword fortranExtended	nint not offset ovefl pack precfill present
"syn keyword fortranExtended	product prompt radix random_number random_seed
"syn keyword fortranExtended	range repeat reshape rrspacing scale scan
"syn keyword fortranExtended	segment selected_int_kind selected_real_kind
"syn keyword fortranExtended	set_exponent shape signsize spacing spread
"syn keyword fortranExtended	sum system system_clock timer tiny transfer
"syn keyword fortranExtended	transpose trim ubound undfl unpack val
"syn keyword fortranExtended	verify action blank blocksize carriagecontrol
"syn keyword fortranExtended	direct exist form formatted from location
"syn keyword fortranExtended	name named nextrec number opened recl
"syn keyword fortranExtended	recordtype sequential stat unformatted

if !exists("did_fortran_syntax_inits")
  let did_fortran_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link fortranStatement		Statement
  hi link fortranLabel			Special
  hi link fortranConditional		Conditional
  hi link fortranRepeat			Repeat
  hi link fortranTodo			Todo
  hi link fortranString			String
  hi link fortranFormatString		fortranString
  hi link fortranNumber			Number
  hi link fortranFloat			Float
  hi link fortranOperator		Operator
  hi link fortranLogicalConstant	Constant
  hi link fortranCommentError		Error
  hi link fortranComment		Comment
  hi link fortranContinueLine		Todo
  hi link fortranLabelNumber		Special
  hi link fortranNoLabelNumber		Error
  hi link fortranType			Type
  hi link fortranStructure		fortranType
  hi link fortranUnitHeader		fortranPreCondit
  hi link fortranReadWrite		fortranImplicit
  hi link fortranFormat			fortranImplicit
  hi link fortranStopPause		fortranImplicit
  hi link fortranImplicit		Identifier
  hi link fortranSpecific		fortranImplicit

  " optional highlighting
  "hi link fortranIdentifier		Identifier
  "hi link fortranDelimiter		Identifier
  "hi link fortranPreCondit		PreCondit
  "hi link fortranIniTab		Error
  "hi link fortranTab			Error
  "hi link fortranExtended		fortranImplicit
  "hi link fortranSpecial		Special
endif

let b:current_syntax = "fortran"

"EOF	vim: ts=8 noet tw=120 sw=8 sts=0
