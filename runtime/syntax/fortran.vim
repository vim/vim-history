" Vim syntax file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	6.01
" Last Change:	2001 Mar 09
" Maintainer:	Ajit J. Thakkar <ajit@unb.ca>; <http://www.unb.ca/chem/ajit/>
" For the latest version of this file, see <http://www.unb.ca/chem/ajit/vim.htm>
" For instructions on use, do :help fortran from vim
" Credits:
"  Some items based on the fortran syntax file by Mario Eusebio and
"   Preben Guldberg, and some on suggestions by Andrej Panjkov,
"   Bram Moolenaar, Thomas Olsen, Michael Sternberg, Christian Reile,
"   Walter Dieudonné and Alexander Wagner.

" Quit if a syntax file is already loaded
if exists("b:current_syntax")
  finish
endif

" let b:fortran_dialect = fortran_dialect if set correctly by user
if exists("fortran_dialect")
  if fortran_dialect =~ '\<\(f\(9[05]\|77\)\|elf\|F\)\>'
    let b:fortran_dialect = matchstr(fortran_dialect,'\<\(f\(9[05]\|77\)\|elf\|F\)\>')
  else
    echohl WarningMsg | echo "Unknown value of fortran_dialect" | echohl None
    let b:fortran_dialect = "unknown"
  endif
else
  let b:fortran_dialect = "unknown"
endif

" fortran_dialect not set or set incorrectly by user,
if b:fortran_dialect == "unknown"
  " set b:fortran_dialect from directive in first three lines of file
  let s:fortran_retype = getline(1)." ".getline(2)." ".getline(3)
  if s:fortran_retype =~ '\<fortran_dialect\s*=\s*F\>'
    let b:fortran_dialect = "F"
  elseif s:fortran_retype =~ '\<fortran_dialect\s*=\s*elf\>'
    let b:fortran_dialect = "elf"
  elseif s:fortran_retype =~ '\<fortran_dialect\s*=\s*f90\>'
    let b:fortran_dialect = "f90"
  elseif s:fortran_retype =~ '\<fortran_dialect\s*=\s*f95\>'
    let b:fortran_dialect = "f95"
  elseif s:fortran_retype =~ '\<fortran_dialect\s*=\s*f77\>'
    let b:fortran_dialect = "f77"
  else
    " no directive found, so guess b:fortran_dialect from file extension
    let s:extfname = expand("%:e")
    if s:extfname ==? "f95"
      let b:fortran_dialect = "f95"
    elseif s:extfname ==? "f90"
      let b:fortran_dialect = "f90"
    elseif s:extfname ==? "f" ||s:extfname ==? "for" || s:extfname ==? "fpp" || s:extfname ==? "f77" || s:extfname ==? "ftn"
      let b:fortran_dialect = "f77"
    else
      " Unrecognized extension
      echohl WarningMsg | echo "Unknown extension. Setting fortran_dialect=f90" | echohl None
      let b:fortran_dialect = "f90"
    endif
  endif
endif

" Choose between fixed and free source form
" if this hasn't been done yet
if !exists("b:fortran_fixed_source")
  if b:fortran_dialect == "elf" || b:fortran_dialect == "F"
    " elf and F require free source form
    let b:fortran_fixed_source = 0
  elseif b:fortran_dialect == "f77"
    " f77 requires fixed source form
    let b:fortran_fixed_source = 1
  elseif exists("fortran_free_source")
    " user guarantees free source form for all f90 and f95 files
    let b:fortran_fixed_source = 0
  else
    " f90 and f95 allow both fixed and free source form
    " assume fixed source form unless signs of free source form
    " are detected in the first five columns of the first 25 lines
    " Detection becomes more accurate and time-consuming if more lines
    " are checked. Increase the limit below if you keep lots of comments at
    " the very top of each file and you have a fast computer
    let s:lmax = 25
    if ( s:lmax > line("$") )
      let s:lmax = line("$")
    endif
    let b:fortran_fixed_source = 1
    let s:ln=1
    while s:ln <= s:lmax
      let s:test = strpart(getline(s:ln),0,5)
      if s:test[0] !~ '[Cc*!#]' && s:test !~ "^ \+!" && s:test =~ '[^ 0-9\t]'
	let b:fortran_fixed_source = 0
	break
      endif
      let s:ln = s:ln + 1
    endwhile
  endif
endif

syn case ignore

if b:fortran_dialect == "f77"
  syn match fortranIdentifier		"\<\a\(\a\|\d\)*\>" contains=fortSerialNumber
else
  syn match fortran90Identifier		"\<\a\w*\>" contains=fortSerialNumber
  if b:fortran_fixed_source == 1
    syn match fortranConstructName	"^\s\{6,}\a\w*\s*:[^:]"
  else
    syn match fortranConstructName	"^\s*\a\w*\s*:[^:]"
  endif
  syn match fortranConstructName "\(\<end\s*do\s\+\)\@<=\a\w*\>"
  syn match fortranConstructName "\(\<end\s*if\s\+\)\@<=\a\w*\>"
  syn match fortranConstructName "\(\<end\s*select\s\+\)\@<=\a\w*\>"
endif

syn match   fortranUnitHeader	"\<end\>"

syn keyword fortranType		character complex integer
syn keyword fortranType		intrinsic implicit
syn keyword fortranStructure	dimension
syn keyword fortranStorageClass	parameter save
syn match fortranUnitHeader	"\<subroutine\>"
syn keyword fortranUnitHeader	call
syn match fortranUnitHeader	"\<function\>"
syn match fortranUnitHeader	"\<program\>"
syn keyword fortranStatement	return stop
syn keyword fortranConditional	if else then
syn match fortranRepeat	"\<do\>"

syn keyword fortranTodo		contained todo fixme

"Catch errors caused by too many right parentheses
syn region fortranParen transparent start="(" end=")" contains=ALLBUT,fortranParenError,@fortranCommentGroup
syn match  fortranParenError   ")"

syn match fortranOperator	"\.\s*n\=eqv\s*\."
syn match fortranOperator	"\.\s*\(and\|or\|not\)\s*\."
syn match fortranOperator	"\(+\|-\|/\|\*\)"

syn match fortranBoolean	"\.\s*\(true\|false\)\s*\."

syn keyword fortranReadWrite	backspace close inquire open rewind endfile
syn keyword fortranReadWrite	read write print

"If tabs are allowed then the left margin checks do not work
if exists("fortran_have_tabs")
  syn match fortranTab		"\t"  transparent
else
  syn match fortranTab		"\t"
endif

syn keyword fortranIO		unit file iostat access blank err fmt form
syn keyword fortranIO		recl status exist opened number named name
syn keyword fortranIO		sequential direct rec
syn keyword fortranIO		formatted unformatted nextrec

syn keyword fortran66Intrinsic		cabs ccos cexp clog csin csqrt dabs
syn keyword fortran66Intrinsic		dacos dasin datan datan2 dcos dcosh
syn keyword fortran66Intrinsic		ddim dexp dint dlog dlog10 dmod
syn keyword fortran66Intrinsic		dnint dsign dsin dsinh dsqrt dtan
syn keyword fortran66Intrinsic		dtanh iabs idim idnint isign idint ifix
syn keyword fortran66Intrinsic		amax0 amax1 dmax1 max0 max1
syn keyword fortran66Intrinsic		amin0 amin1 dmin1 min0 min1
syn keyword fortran66Intrinsic		amod float sngl alog alog10

syn keyword fortran77Intrinsic	abs acos aimag aint anint asin atan atan2
syn keyword fortran77Intrinsic	cos sin tan sinh cosh tanh exp log log10
syn keyword fortran77Intrinsic	sign sqrt int cmplx nint min max conjg
syn keyword fortran77Intrinsic	char ichar index
syn match fortran77Intrinsic	"\<len\s*[(,]"me=s+3
syn match fortran77Intrinsic	"\(implicit\s\+\)\@<!\<real\s*(\(\s*kind\>\)\@!"me=s+4
syn match fortranType		"\<real\(\*\d\+\)\=\(\s\+[^(]\|\s*(\s*kind\)\@="me=s+4
syn match fortranType		"\(implicit\s\+\)\@<=real\(\*\d\+\)\="me=s+4

"Numbers of various sorts
" Integers
syn match fortranNumber	display "\<\d\+\(_\a\w*\)\=\>"
" floating point number, without a decimal point
syn match fortranFloatNoDec	display	"\<\d\+[de][-+]\=\d\+\(_\a\w*\)\=\>"
" floating point number, starting with a decimal point
syn match fortranFloatIniDec	display	"\.\d\+\([de][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
" floating point number, no digits after decimal
syn match fortranFloatEndDec	display	"\d\+\.\([de][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
" floating point number, [Dd] exponents
syn match fortranFloatDExp	display	"\<\d\+\.\d\+\([d][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
" floating point number
syn match fortranFloat	display	"\<\d\+\.\d\+\([e][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
" Numbers in formats
syn match fortranFormatSpec	display	"\d\{,2}[d-g]\d\d\=\.\d\d\="
syn match fortranFormatSpec	display	"\d\d\+[ai]\d\d\="
" Numbers as labels
syn match fortranLabelNumber	display	"^\d\{1,5}\>"
syn match fortranLabelNumber	display	"^ \d\{1,4}\>"ms=s+1
syn match fortranLabelNumber	display	"^  \d\{1,3}\>"ms=s+2
syn match fortranLabelNumber	display	"^   \d\d\=\>"ms=s+3
syn match fortranLabelNumber	display	"^    \d\>"ms=s+4
" Numbers as targets
syn match fortranTarget	display	"\(\<do\s\+\)\@<=\d\+\>"
syn match fortranTarget	display	"\(\<go\s*to\s\+\)\@<=\d\+\>"
syn match fortranTarget	display	"\(\<end\s*=\s*\)\@<=\d\+\>"
syn match fortranTarget	display	"\(\<err\s*=\s*\)\@<=\d\+\>"

syn keyword fortranTypeEx	external
syn keyword fortranIOEx		format
syn keyword fortranStatementEx	continue go to
syn region fortranStringEx	start=+'+ end=+'+ contains=fortranContinueMark,fortranLeftMargin,fortSerialNumber
syn keyword fortran77IntrinsicEx	dim lge lgt lle llt mod
syn keyword fortranStatementOb	assign pause
syn match fortranType		"\<logical\(\*\d\+\)\=\(\s\+[^(]\|\s*(\s*kind\)\@="me=s+7
syn match fortranType		"\(implicit\s\+\)\@<=logical\(\*\d\+\)\="me=s+7

if b:fortran_dialect != "f77"
  syn match fortran90Intrinsic	"\(implicit\s\+\)\@<!logical\s*(\(\s*kind\>\)\@!"me=s+7

  syn keyword fortranType	type none

  syn keyword fortranStructure	private public intent optional
  syn keyword fortranStructure	pointer target allocatable
  syn keyword fortranStorageClass	in out
  syn match fortranStorageClass	"\<kind\s*="me=s+4
  syn match fortranStorageClass	"\<len\s*="me=s+3

  syn match fortranUnitHeader	"\<module\>"
  syn keyword fortranUnitHeader	use only contains
  syn keyword fortranUnitHeader	recursive result interface operator
  syn keyword fortranStatement	allocate deallocate nullify cycle exit
  syn match fortranConditional	"\<select\>"
  syn keyword fortranConditional	case default where elsewhere

  syn match fortranOperator	"\(\(>\|<\)=\=\|==\|/=\|=\)"
  syn match fortranOperator	"=>"

  syn region fortranString	start=+"+ end=+"+	contains=fortranLeftMargin,fortranContinueMark,fortSerialNumber
  syn keyword fortranIO		pad position action delim readwrite
  syn keyword fortranIO		eor advance nml

  syn keyword fortran90Intrinsic	adjustl adjustr all allocated any
  syn keyword fortran90Intrinsic	associated bit_size btest ceiling
  syn keyword fortran90Intrinsic	count cshift date_and_time
  syn keyword fortran90Intrinsic	digits dot_product eoshift epsilon exponent
  syn keyword fortran90Intrinsic	floor fraction huge iand ibclr ibits ibset ieor
  syn keyword fortran90Intrinsic	ior ishft ishftc lbound len_trim
  syn keyword fortran90Intrinsic	matmul maxexponent maxloc maxval merge
  syn keyword fortran90Intrinsic	minexponent minloc minval modulo mvbits nearest
  syn keyword fortran90Intrinsic	pack present product radix random_number
  syn match fortran90Intrinsic		"\<not\>\(\s*\.\)\@!"me=s+3
  syn keyword fortran90Intrinsic	random_seed range repeat reshape rrspacing scale
  syn keyword fortran90Intrinsic	selected_int_kind selected_real_kind scan
  syn keyword fortran90Intrinsic	shape size spacing spread set_exponent
  syn keyword fortran90Intrinsic	tiny transpose trim ubound unpack verify
  syn keyword fortran90Intrinsic	precision sum system_clock
  syn match fortran90Intrinsic	"\<kind\>\s*[(,]"me=s+4

  syn match  fortranUnitHeader	"\<end\s*function"
  syn match  fortranUnitHeader	"\<end\s*interface"
  syn match  fortranUnitHeader	"\<end\s*module"
  syn match  fortranUnitHeader	"\<end\s*program"
  syn match  fortranUnitHeader	"\<end\s*subroutine"
  syn match  fortranRepeat	"\<end\s*do"
  syn match  fortranConditional	"\<end\s*where"
  syn match  fortranConditional	"\<select\s*case"
  syn match  fortranConditional	"\<end\s*select"
  syn match  fortranType	"\<end\s*type"
  syn match  fortranType	"\<in\s*out"

  syn keyword fortranUnitHeaderEx	procedure
  syn keyword fortranIOEx		namelist
  syn keyword fortranConditionalEx	while
  syn keyword fortran90IntrinsicEx	achar iachar transfer

  syn keyword fortranInclude		include
  syn keyword fortran90StorageClassR	sequence
endif

syn match   fortranConditional	"\<end\s*if"
syn match   fortranIO		"\<end\s*="
syn match   fortranConditional	"\<else\s*if"

syn keyword fortranStatementR	goto
syn keyword fortranUnitHeaderR	entry
syn match fortranTypeR		display "double\s\+precision"
syn match fortranTypeR		display "double\s\+complex"
syn match fortranUnitHeaderR	display "block\s\+data"
syn keyword fortranStorageClassR	common equivalence data
syn keyword fortran77IntrinsicR	dble dprod
syn match   fortran77OperatorR	"\.\s*[gl][et]\s*\."
syn match   fortran77OperatorR	"\.\s*\(eq\|ne\)\s*\."

if b:fortran_dialect == "f95"
  syn keyword fortranRepeat		forall
  syn match fortranRepeat		"\<end\s*forall"
  syn keyword fortran95Intrinsic	null cpu_time
  syn keyword fortranType		elemental pure
  syn match fortranConstructName "\(\<end\s*forall\s\+\)\@<=\a\w*\>"
endif

syn cluster fortranCommentGroup contains=fortranTodo

if (b:fortran_fixed_source == 1)
  if !exists("fortran_have_tabs")
    "Flag items beyond column 72
    "syn match fortSerialNumber		excludenl "\(^.\{72}\s*\)\@<=\S.*$"
    syn match fortSerialNumber		"^.\{73,}$"lc=72
    "Flag left margin errors
    syn match fortranLabelError	"^.\{-,4}[^0-9 ]" contains=fortranTab
    syn match fortranLabelError	"^.\{4}\d\S"
  endif
  syn match fortranComment		excludenl "^[!c*].*$" contains=@fortranCommentGroup
  syn match fortranLeftMargin		transparent "^ \{5}"
  syn match fortranContinueMark		display "\(^.\{5}\)\@<=\S"
else
  syn match fortranContinueMark		display "&"
endif

if b:fortran_dialect != "f77"
  syn match fortranComment	excludenl "!.*$" contains=@fortranCommentGroup
endif

"cpp is often used with Fortran
syn match	cPreProc		"^\s*#\s*\(define\|ifdef\)\>.*"
syn match	cPreProc		"^\s*#\s*\(elif\|if\)\>.*"
syn match	cPreProc		"^\s*#\s*\(ifndef\|undef\)\>.*"
syn match	cPreCondit		"^\s*#\s*\(else\|endif\)\>"
syn region	cIncluded		contained start=+"+ skip=+\\\\\|\\"+ end=+"+	contains=fortranLeftMargin,fortranContinueMark,fortSerialNumber
syn match	cIncluded		contained "<[^>]*>"
syn match	cInclude		"^\s*#\s*include\>\s*["<]" contains=cIncluded

"Synchronising limits assume that comment and continuation lines are not mixed
if (b:fortran_fixed_source == 0)
  syn sync linecont "&" maxlines=40
else
  syn sync minlines=20
endif

" The default highlighting.
" Transparent groups:
" fortranParen, fortranLeftMargin
hi def link fortranStatement		Statement
hi def link fortranConstructName	Special
hi def link fortranConditional		Conditional
hi def link fortranRepeat		Repeat
hi def link fortranTodo			Todo
hi def link fortranContinueMark		Todo
hi def link fortranString		String
hi def link fortranNumber		Number
hi def link fortranOperator		Operator
hi def link fortranBoolean		Boolean
hi def link fortranLabelError		Error
hi def link fortranObsolete		Todo
hi def link fortranType			Type
hi def link fortranStructure		Type
hi def link fortranStorageClass		StorageClass
hi def link fortranUnitHeader		fortranPreCondit
hi def link fortranReadWrite		fortran90Intrinsic
hi def link fortranIO			fortran90Intrinsic
hi def link fortran95Intrinsic		fortran90Intrinsic
hi def link fortran77Intrinsic		fortran90Intrinsic
hi def link fortran90Intrinsic		Special

if ( b:fortran_dialect == "f77" )
  hi def link fortranStatementOb		Statement
else
  hi def link fortranStatementOb		fortranObsolete
endif

if ( b:fortran_dialect == "elf" || b:fortran_dialect == "F" )
  hi def link fortran66Intrinsic	fortranObsolete
  hi def link fortran77IntrinsicR	fortranObsolete
  hi def link fortranStatementR		fortranObsolete
  hi def link fortranUnitHeaderR	fortranObsolete
  hi def link fortranTypeR		fortranObsolete
  hi def link fortranStorageClassR	fortranObsolete
  hi def link fortran90StorageClassR	fortranObsolete
  hi def link fortran77OperatorR	fortranObsolete
  hi def link fortranInclude		fortranObsolete
else
  hi def link fortran66Intrinsic	fortran90Intrinsic
  hi def link fortran77IntrinsicR	fortran90Intrinsic
  hi def link fortranStatementR		fortranStatement
  hi def link fortranUnitHeaderR	fortranPreCondit
  hi def link fortranTypeR		fortranType
  hi def link fortranStorageClassR	fortranStorageClass
  hi def link fortran77OperatorR	fortranOperator
  hi def link fortranInclude		Include
  hi def link fortran90StorageClassR	fortranStorageClass
endif

if ( b:fortran_dialect == "F" )
  hi def link fortranLabelNumber	fortranObsolete
  hi def link fortranTarget		fortranObsolete
  hi def link fortranFormatSpec		fortranObsolete
  hi def link fortranFloatDExp		fortranObsolete
  hi def link fortranFloatNoDec		fortranObsolete
  hi def link fortranFloatIniDec	fortranObsolete
  hi def link fortranFloatEndDec	fortranObsolete
  hi def link fortranTypeEx		fortranObsolete
  hi def link fortranIOEx		fortranObsolete
  hi def link fortranStatementEx	fortranObsolete
  hi def link fortranStringEx		fortranObsolete
  hi def link fortran77IntrinsicEx	fortranObsolete
  hi def link fortranUnitHeaderEx	fortranObsolete
  hi def link fortranIOEx		fortranObsolete
  hi def link fortranConditionalEx	fortranObsolete
  hi def link fortran90IntrinsicEx	fortranObsolete
else
  hi def link fortranLabelNumber	Special
  hi def link fortranTarget		Special
  hi def link fortranFormatSpec		Identifier
  hi def link fortranFloatDExp		fortranFloat
  hi def link fortranFloatNoDec		fortranFloat
  hi def link fortranFloatIniDec	fortranFloat
  hi def link fortranFloatEndDec	fortranFloat
  hi def link fortranTypeEx		fortranType
  hi def link fortranIOEx		fortranIO
  hi def link fortranStatementEx	fortranStatement
  hi def link fortranStringEx		fortranString
  hi def link fortran77IntrinsicEx	fortran90Intrinsic
  hi def link fortranUnitHeaderEx	fortranUnitHeader
  hi def link fortranIOEx		fortranIO
  hi def link fortranConditionalEx	fortranConditional
  hi def link fortran90IntrinsicEx	fortran90Intrinsic
endif
hi def link fortranFloat		Float

hi def link fortran90Identifier		fortranIdentifier
" Uncomment the next line if you want all fortran variables to be highlighted
"hi def link fortranIdentifier		Identifier
hi def link fortranPreCondit		PreCondit
hi def link fortranInclude		Include
hi def link cIncluded			fortranString
hi def link cInclude			Include
hi def link cPreProc			PreProc
hi def link cPreCondit			PreCondit
hi def link fortranParenError		Error
hi def link fortranComment		Comment
"hi def link fortranGoodWord		Comment
hi def link fortSerialNumber		Todo
hi def link fortranTab			Error

"For future versions
"hi link fortranExtended		Special

let b:current_syntax = "fortran"

" vim: ts=8 tw=132
