" Vim syntax file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	0.77
" Last Change:	2000 Dec 14
" Maintainer:	Ajit J. Thakkar <ajit@unb.ca>; <http://www.unb.ca/chem/ajit/>
" For the latest version of this file, see <http://www.unb.ca/chem/ajit/vim.htm>
" Credits:
"  Some items based on the fortran syntax file by Mario Eusebio and
"   Preben Guldberg, and some on suggestions by Andrej Panjkov,
"   Bram Moolenaar, Thomas Olsen, Michael Sternberg, Christian Reile,
"   and Walter Dieudonné.

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
    let b:fortran_fixed_source = 1
    let s:ln=1
    " Detection becomes more accurate and time-consuming if more lines
    " are checked. Increase the limit below if you keep lots of comments at
    " the very top of each file and you have a fast computer
    while s:ln < 25
      let s:test = strpart(getline(s:ln),0,5)
      if s:test[0] !~ '[Cc*#]' && s:test !~ '^\s*!' && s:test =~ '[^ 0-9\t]'
	let b:fortran_fixed_source = 0
	break
      endif
      let s:ln = s:ln + 1
    endwhile
  endif
endif

syn clear
syn case ignore

if b:fortran_dialect == "f77"
  syn match fortranIdentifier		"\<\a\(\a\|\d\)*\>" contains=fortSerialNumber
else
  syn match fortranIdentifier		"\<\a\w*\>" contains=fortSerialNumber
  if b:fortran_fixed_source == 1
    syn match fortranConstructName	"^\s\{6,}\a\w*\s*:[^:]"
  else
    syn match fortranConstructName	"^\s*\a\w*\s*:[^:]"
  endif
endif

syn match   fortranUnitHeader	"\<end\>"

syn keyword fortranType		character complex integer
syn keyword fortranType		intrinsic implicit
syn keyword fortranStructure	dimension
syn keyword fortranStorageClass	parameter save
syn keyword fortranUnitHeader	call function program subroutine
syn keyword fortranStatement	return stop
syn keyword fortranConditional	if else then
syn keyword fortranRepeat	do

syn keyword fortranTodo		contained TODO

"Catch errors caused by too many right parentheses
syn region fortranParen transparent start="(" end=")" contains=ALLBUT,fortranParenError
syn match  fortranParenError   ")"

syn match fortranOperator	"\.n\=eqv\."
syn match fortranOperator	"\.\(and\|or\|not\)\."

syn match fortranBoolean	"\.\(true\|false\)\."

syn keyword fortranReadWrite	backspace close inquire open rewind endfile
syn keyword fortranReadWrite	read write print

"If tabs are allowed then the left margin checks do not work
if exists("fortran_have_tabs")
  syn match fortranTab		"\t"  transparent
else
  syn match fortranTab		"\t"
endif

syn keyword fortranI_O		unit file iostat access blank err fmt form
syn keyword fortranI_O		recl status exist opened number named name
syn keyword fortranI_O		sequential direct rec
syn keyword fortranI_O		formatted unformatted nextrec

syn keyword fortran77Intrinsic	abs acos aimag aint anint asin atan atan2
syn keyword fortran77Intrinsic	cos sin tan sinh cosh tanh exp log log10
syn keyword fortran77Intrinsic	sign sqrt int cmplx nint min max conjg
syn keyword fortran77Intrinsic	char ichar index
syn match fortran77Intrinsic	"\<len\>\s*[(,]"me=s+3
syn match fortran77Intrinsic	"\<real\s*("me=e-1
syn match fortranType		"\<real\>\s\+[^(]"me=e-1

if b:fortran_dialect == "f77"
  syn match fortranNumber	"\<\d\+\>"
  syn keyword fortranStatement	assign pause
  syn keyword fortranType	logical
else
  syn match fortranNumber	"\<\d\+\(_\a\w*\)\=\>"
  syn keyword fortranObsolete	assign pause
  syn match fortranIntrinsic	"\<logical\s*("me=e-1
  syn match fortranType		"\<logical\s\+[^(]"me=e-1

  syn keyword fortranType	type none

  syn keyword fortranStructure	private public intent optional
  syn keyword fortranStructure	pointer target allocatable
  syn keyword fortranStorageClass	in out
  syn match fortranStorageClass	"\<kind\s*="me=s+4
  syn match fortranStorageClass	"\<len\s*="me=s+4

  syn keyword fortranUnitHeader	module use only contains
  syn keyword fortranUnitHeader	recursive result interface operator
  syn keyword fortranStatement	allocate deallocate nullify cycle exit
  syn keyword fortranConditional	select case default where elsewhere

  syn match fortranComment	"!.*$" excludenl contains=fortranTodo,fortranTab

  syn match fortranOperator	"\(\(>\|<\)=\=\|==\|/=\)"
  syn match fortranOperator	"=>"

  syn region fortranString	start=+"+ end=+"+	contains=fortranLeftMargin,fortranContinueMark,fortSerialNumber
  syn keyword fortranI_O	pad position action delim readwrite
  syn keyword fortranI_O	eor advance nml

  syn keyword fortranIntrinsic	adjustl adjustr all allocated any
  syn keyword fortranIntrinsic	associated bit_size btest ceiling
  syn keyword fortranIntrinsic	count cshift date_and_time
  syn keyword fortranIntrinsic	digits dot_product eoshift epsilon exponent
  syn keyword fortranIntrinsic	floor fraction huge iand ibclr ibits ibset ieor
  syn keyword fortranIntrinsic	ior ishft ishftc lbound len_trim
  syn keyword fortranIntrinsic	matmul maxexponent maxloc maxval merge
  syn keyword fortranIntrinsic	minexponent minloc minval modulo mvbits nearest
  syn keyword fortranIntrinsic	not pack present product radix random_number
  syn keyword fortranIntrinsic	random_seed range repeat reshape rrspacing scale
  syn keyword fortranIntrinsic	selected_int_kind selected_real_kind scan
  syn keyword fortranIntrinsic	shape size spacing spread set_exponent
  syn keyword fortranIntrinsic	tiny transpose trim ubound unpack verify
  syn keyword fortranIntrinsic	precision sum system_clock
  syn match fortranIntrinsic	"\<kind\>\s*[(,]"me=s+4
endif

if b:fortran_dialect == "F"
  syn keyword fortranObsolete	external
  syn keyword fortranObsolete	procedure
  syn keyword fortranObsolete	format namelist
  syn keyword fortranObsolete	continue go to
  syn keyword fortranObsolete	while
  syn region fortranObsolete	start=+'+ end=+'+
  syn keyword fortranObsolete	achar dim iachar lge lgt lle llt mod transfer
  "No decimal
  syn match fortranObsolete	"\<\d\+[de][-+]\=\d\+\(_\a\w*\)\=\>"
  "No digit before decimal
  syn match fortranObsolete	"\.\d\+\([de][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
  "No digit after decimal
  syn match fortranObsolete	"\d\+\.\([de][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
  "[Dd] exponents
  syn match fortranObsolete	"\<\d\+\.\d\+\([d][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
  " floating point number
  syn match fortranFloat	"\<\d\+\.\d\+\([e][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
else
  syn keyword fortranType	external
  syn keyword fortranI_O	format
  syn keyword fortranStatement	continue go to
  syn region fortranString	start=+'+ end=+'+ contains=fortranContinueMark,fortranLeftMargin,fortSerialNumber
  syn keyword fortran77Intrinsic	dim lge lgt lle llt mod
  " floating point number, without a decimal point
  syn match fortranFloat	"\<\d\+[de][-+]\=\d\+\(_\a\w*\)\=\>"
  " floating point number, starting with a decimal point
  syn match fortranFloat	"\.\d\+\([de][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
  " floating point number, starting with digits followed by a decimal point
  syn match fortranFloat	"\<\d\+\.\d*\([de][-+]\=\d\+\)\=\(_\a\w*\)\=\>"

  syn match fortranFormatSpec   "\d\{,2}[d-g]\d\d\=\.\d\d\="
  syn match fortranFormatSpec   "\d\d\+[ai]\d\d\="
  syn match fortranLabelNumber	"^\d\{1,5}\>"
  syn match fortranLabelNumber	"^ \d\{1,4}\>"ms=s+1
  syn match fortranLabelNumber	"^  \d\{1,3}\>"ms=s+2
  syn match fortranLabelNumber	"^   \d\d\=\>"ms=s+3
  syn match fortranLabelNumber	"^    \d\>"ms=s+4
  if b:fortran_dialect != "f77"
    syn keyword fortranUnitHeader	procedure
    syn keyword fortranI_O		namelist
    syn keyword fortranConditional	while
    syn keyword fortranIntrinsic	achar iachar transfer
  endif
endif

if b:fortran_dialect == "elf"
  syn keyword fortranObsolete		enddo endfunction endif endinterface
  syn keyword fortranObsolete		endmodule endprogram endselect endsubroutine
  syn keyword fortranObsolete		endtype endwhere elseif selectcase
  syn keyword fortranObsolete		inout
  syn match   fortranUnitHeader		"end\s\+function"
  syn match   fortranUnitHeader		"end\s\+interface"
  syn match   fortranUnitHeader		"end\s\+module"
  syn match   fortranUnitHeader		"end\s\+program"
  syn match   fortranUnitHeader		"end\s\+subroutine"
  syn match   fortranRepeat		"end\s\+do"
  syn match   fortranConditional	"end\s\+if"
  syn match   fortranConditional	"end\s\+where"
  syn match   fortranConditional	"else\s\+if"
  syn match   fortranConditional	"select\s\+case"
  syn match   fortranConditional	"end\s\+select"
  syn match   fortranType		"end\s\+type"
else
  if b:fortran_dialect != "f77"
    syn match   fortranUnitHeader	"end\s*function"
    syn match   fortranUnitHeader	"end\s*interface"
    syn match   fortranUnitHeader	"end\s*module"
    syn match   fortranUnitHeader	"end\s*program"
    syn match   fortranUnitHeader	"end\s*subroutine"
    syn match   fortranRepeat		"end\s*do"
    syn match   fortranConditional	"end\s*where"
    syn match   fortranConditional	"select\s*case"
    syn match   fortranConditional	"end\s*select"
    syn match	fortranType		"end\s*type"
    syn keyword fortranType		inout
  endif
  syn match   fortranConditional	"end\s*if"
  syn match   fortranI_O        	"end\s*="
  syn match   fortranConditional	"else\s*if"
endif

if ( b:fortran_dialect == "elf" || b:fortran_dialect == "F" )
  syn keyword fortranObsolete		include
  syn keyword fortranObsolete		goto
  syn keyword fortranObsolete		entry
  syn keyword fortranObsolete		double
  syn keyword fortranObsolete		common equivalence sequence block data
  syn keyword fortranObsolete		cabs ccos cexp clog csin csqrt dabs
  syn keyword fortranObsolete		dacos dasin datan datan2 dcos dcosh
  syn keyword fortranObsolete		ddim dexp dint dlog dlog10 dmod
  syn keyword fortranObsolete		dnint dsign dsin dsinh dsqrt dtan
  syn keyword fortranObsolete		dtanh iabs idim idnint isign idint ifix
  syn keyword fortranObsolete		amax0 amax1 dmax1 max0 max1
  syn keyword fortranObsolete		amin0 amin1 dmin1 min0 min1
  syn keyword fortranObsolete		amod float sngl alog alog10
  syn keyword fortranObsolete		dble dprod
  syn match   fortranObsolete		"\.[gl][et]\."
  syn match   fortranObsolete		"\.\(eq\|ne\)\."
else
  syn keyword fortranStatement		goto
  syn keyword fortranUnitHeader		entry
  syn match fortranType			"double\s\+precision"
  syn match fortranUnitHeader		"block\s\+data"
  syn keyword fortranStorageClass	common equivalence data
  syn keyword fortran66Specific		cabs ccos cexp clog csin csqrt dabs
  syn keyword fortran66Specific		dacos dasin datan datan2 dcos dcosh
  syn keyword fortran66Specific		ddim dexp dint dlog dlog10 dmod
  syn keyword fortran66Specific		dnint dsign dsin dsinh dsqrt dtan
  syn keyword fortran66Specific		dtanh iabs idim idnint isign idint ifix
  syn keyword fortran66Specific		amax0 amax1 dmax1 max0 max1
  syn keyword fortran66Specific		amin0 amin1 dmin1 min0 min1
  syn keyword fortran66Specific		amod float sngl alog alog10
  syn keyword fortran77Intrinsic	dble dprod
  syn match   fortran77Operator		"\.[gl][et]\."
  syn match   fortran77Operator		"\.\(eq\|ne\)\."
  if b:fortran_dialect != "f77"
    syn keyword fortranInclude		include
    syn keyword fortranStorageClass	sequence
  endif
endif

if (b:fortran_fixed_source == 1)
"Flag items beyond column 72
  syn match fortSerialNumber	        "^.\{73,}$"lc=72 excludenl

"Flag left margin errors -- does not do anything if you allow tabs
  syn match fortranLabelError		"^[^\t]\{-,4}[^0-9 \t]" contains=fortranTab
  syn match fortranLabelError	"^\d\{5}\S"
  syn match fortranLabelError	"^ \d\{4}\S"
  syn match fortranLabelError	"^  \d\d\d\S"
  syn match fortranLabelError	"^   \d\d\S"
  syn match fortranLabelError	"^    \d\S"

  syn match fortranComment		"^[!c*].*$" excludenl contains=fortranTodo,fortranTab
  syn match fortranLeftMargin		"^     "
  syn match fortranContinueMark		"^     \S"lc=5
else
  syn match fortranContinueMark		"&"
endif

if b:fortran_dialect == "f95"
  syn keyword fortranRepeat		forall endforall
  syn keyword fortranIntrinsic		null cpu_time
  syn keyword fortranType		elemental pure
endif

"Synchronising limits assume that comment and continuation lines are not mixed
if (b:fortran_fixed_source == 0)
  syn sync linecont "&" maxlines=40
else
  syn sync minlines=20
endif

"cpp is often used with Fortran
syn match	cPreProc		"^\s*#\s*\(define\|ifdef\)\>.*"
syn match	cPreProc		"^\s*#\s*\(elif\|if\)\>.*"
syn match	cPreProc		"^\s*#\s*\(ifndef\|undef\)\>.*"
syn match	cPreCondit		"^\s*#\s*\(else\|endif\)\>"
syn region	cIncluded		contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	cIncluded		contained "<[^>]*>"
syn match	cInclude		"^\s*#\s*include\>\s*["<]" contains=cIncluded

" The default highlighting.
hi def link fortranStatement		Statement
hi def link fortranConstructName	Special
hi def link fortranConditional		Conditional
hi def link fortranRepeat		Repeat
hi def link fortranTodo			Todo
"hi def fortranLeftMargin		NONE
hi def link fortranContinueMark		Todo
hi def link fortranString		String
hi def link fortranNumber		Number
hi def link fortranFloat		Float
hi def link fortranOperator		Operator
hi def link fortran77Operator		Operator
hi def link fortranBoolean		Boolean
hi def link fortranLabelNumber		Special
hi def link fortranLabelError		Error
hi def link fortranObsolete		Todo
hi def link fortranType			Type
hi def link fortranStructure		Type
hi def link fortranStorageClass		StorageClass
hi def link fortranUnitHeader		fortranPreCondit
hi def link fortranReadWrite		fortranIntrinsic
hi def link fortranI_O			fortranIntrinsic
hi def link fortranIntrinsic		Special
hi def link fortran66Specific		Special
hi def link fortran77Intrinsic		Special
hi def link fortranIdentifier		Identifier
hi def link fortranFormatSpec		Identifier
hi def link fortranSpecial		Special
hi def link fortranPreCondit		PreCondit
hi def link fortrancPreCondit		PreCondit
hi def link fortranInclude		Include
hi def link cIncluded			fortranString
hi def link cInclude			Include
hi def link cPreProc			PreProc
hi def link fortranParenError		Error
hi def link fortranComment		Comment
hi def link fortSerialNumber		Todo
hi def link fortranTab			Error

"For future versions
"hi link fortranExtended		Special

let b:current_syntax = "fortran"

" vim: ts=8 tw=132
