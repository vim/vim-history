" Vim syntax file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	0.4
" Last Change:	2000 May 21
" Maintainer:	Ajit Thakkar <ajit@unb.ca>; <http://www.unb.ca/chem/ajit/>
"  Some items based on the fortran syntax file by Mario Eusebio and
"   Preben Guldberg, some on changes suggested by Andrej Panjkov, and some
"   on suggestions by Bram Moolenaar

" let b:fortran_dialect = fortran_dialect if set correctly by user
if exists("fortran_dialect")
  if fortran_dialect =~ '\<\(f95\|f90\|f77\|elf\|F\)\>'
    let b:fortran_dialect = matchstr(fortran_dialect,'\<\(f95\|f90\|f77\|elf\|F\)\>')
  else
    echohl WarningMsg | echo "Unknown value of fortran_dialect" | echohl None
    let b:fortran_dialect = "unknown"
  endif
else
  let b:fortran_dialect = "unknown"
endif

" If fortran_dialect not set or set incorrectly by user,
" then guess b:fortran_dialect from file extension
" and, in some cases, content of first three lines of file
if b:fortran_dialect == "unknown"
  let b:extfname = expand("%:e")
  if b:extfname ==? "f95"
    let b:fortran_dialect = "f95"
  elseif b:extfname ==? "f90"
    let b:fortran_retype = getline(1)." ".getline(2)." ".getline(3)
    if b:fortran_retype =~ '\<fortran_dialect\s*=\s*F\>'
      let b:fortran_dialect = "F"
    elseif b:fortran_retype =~ '\<fortran_dialect\s*=\s*elf\>'
      let b:fortran_dialect = "elf"
    else
      let b:fortran_dialect = "f90"
    endif
    unlet b:fortran_retype
  elseif b:extfname ==? "f"
    if getline(1)." ".getline(2)." ".getline(3) =~ '\<fortran_dialect\s*=\s*F\>'
      let b:fortran_dialect = "F"
    else
      let b:fortran_dialect = "f77"
    endif
  elseif b:extfname ==? "for" || b:extfname ==? "fpp"
    let b:fortran_dialect = "f77"
  else
    let b:fortran_dialect = "f90"
  endif
  unlet b:extfname
endif

"Choose between fixed and free source form
" elf and F require free source form
if b:fortran_dialect == "elf" || b:fortran_dialect == "F"
  let b:fortran_fixed_source = 0
" f77 requires fixed source form
elseif b:fortran_dialect == "f77"
  let b:fortran_fixed_source = 1
" f90 and f95 allow both fixed and free source form
" check first fuve columns of first five lines for signs of free source form
else
  let b:fortran_fixed_source = 1
  let b:ln=1
  while b:ln < 5
    let b:test = strpart(getline(b:ln),0,5)
    if b:test =~ '\S' && b:test[0] !~ '[Cc*]' && b:test =~ '[^ 0-9\t]'
      let b:fortran_fixed_source = 0
      break
    endif
    let b:ln = b:ln + 1
  endwhile
  unlet b:ln b:test
endif

syn clear
syn case ignore

if b:fortran_dialect == "f77"
  syn match fortranIdentifier		"\<\a\(\a\|\d\)*\>"
else
  syn match fortranIdentifier		"\<\a\w*\>"
  if b:fortran_fixed_source == 1
    syn match fortranConstructName	"^\s\{6,}\a\w\+\s*:[^:]"
  else
    syn match fortranConstructName	"^\s*\a\w\+\s*:[^:]"
  endif
endif

syn keyword fortranType		character complex integer logical real
syn keyword fortranType		intrinsic implicit
syn keyword fortranStructure	dimension
syn keyword fortranStorageClass	parameter save
syn keyword fortranUnitHeader	call function program subroutine
syn keyword fortranStatement	return stop
syn keyword fortranConditional	if else then
syn keyword fortranRepeat	do

syn keyword fortranTodo		contained TODO

"attempt to catch errors caused by too many right parentheses
syn region fortranParen transparent start="(" end=")" contains=ALLBUT,fortranParenError
syn match  fortranParenError   ")"

syn match fortranOperator	"\.\(eqv\|neqv\)\."
syn match fortranOperator	"\.\(and\|or\|not\)\."

syn match fortranBoolean	"\.\(true\|false\)\."

syn keyword fortranReadWrite	backspace close inquire open rewind endfile
syn keyword fortranReadWrite	read write print

syn match fortranIniTab		"^\t.*$"
syn match fortranTab		"\t"

syn keyword fortranI_O		unit file iostat access blank err fmt form
syn keyword fortranI_O		recl status exist opened number named name
syn keyword fortranI_O		sequential direct rec
syn keyword fortranI_O		formatted unformatted nextrec

syn keyword fortran77Intrinsic	abs acos aimag aint anint asin atan atan2
syn keyword fortran77Intrinsic	cos sin tan sinh cosh tanh exp log log10
syn keyword fortran77Intrinsic	sign sqrt int cmplx nint min max conjg
syn keyword fortran77Intrinsic	char ichar len index
"syn keyword fortran77Intrinsic	real

if b:fortran_dialect == "f77"
  syn match fortranNumber	"\<\d\+\>"
  syn keyword fortranStatement	assign pause
else
  syn match fortranNumber	"\<\d\+\(_\a\w*\)\=\>"
  syn keyword fortranObsolete	assign pause

  syn keyword fortranType	type none
  if b:fortran_dialect == "elf"
    syn match fortranType	"end\s\+type"
  else
    syn match fortranType	"end\s*type"
  endif

  syn keyword fortranStructure	private public intent optional
  syn keyword fortranStructure	pointer target allocatable
  syn keyword fortranStorageClass	in out kind

  syn keyword fortranUnitHeader	module use only contains
  syn keyword fortranUnitHeader	recursive result interface operator
  syn keyword fortranStatement	allocate deallocate nullify cycle exit
  syn keyword fortranConditional	select case default where elsewhere

  syn match fortranComment	"!.*$"		contains=fortranTodo,fortranTab

  syn match fortranOperator	"\(>=\=\|<=\=\|==\|/=\)"
  syn match fortranOperator	"=>"

  syn region fortranString	start=+"+ end=+"+	contains=fortranContinueLine
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
" syn keyword fortranIntrinsic	kind logical
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
  syn match fortranObsolete	"\<\d\+[edED][-+]\=\d\+\>"
  "No digit before decimal
  syn match fortranObsolete	"\D\.\d\+"
  "No digit after decimal
  syn match fortranObsolete	"\d\+\.\([dDeE][-+]\=\d\+\)\="
  "[Dd] exponents
  syn match fortranObsolete	"\<\d\+\.\d\+\([dD][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
  " floating point number
  syn match fortranFloat	"\<\d\+\.\d\+\([eE][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
else
  syn keyword fortranType	external
  syn keyword fortranI_O	format
  syn keyword fortranStatement	continue go to
  syn region fortranString	start=+'+ end=+'+	oneline
  syn keyword fortran77Intrinsic	dim lge lgt lle llt mod
  " floating point number, without a decimal point
  syn match fortranFloat	"\<\d\+[edED][-+]\=\d\+\(_\a\w*\)\=\>"
  " floating point number, starting with a decimal point
  syn match fortranFloat	"\.\d\+\([edED][-+]\=\d\+\)\=\(_\a\w*\)\=\>"
  " floating point number, starting with digits followed by a decimal point
  syn match fortranFloat	"\<\d\+\.\d*\([edED][-+]\=\d\+\)\=\(_\a\w*\)\=\>"

  syn match fortranLabelNumber	"^\d\{1,5}\>"
  syn match fortranLabelNumber	"^\ \d\{1,4}\>"ms=s+1
  syn match fortranLabelNumber	"^\ \{2}\d\{1,3}\>"ms=s+2
  syn match fortranLabelNumber	"^\ \{3}\d\{1,2}\>"ms=s+3
  syn match fortranLabelNumber	"^\ \{4}\d\>"ms=s+4
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
else
  syn match   fortranConditional	"end\s*if"
  syn match   fortranConditional	"else\s*if"
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
    syn keyword fortranType		inout
  endif
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
  syn match   fortranObsolete		"\.\(gt\|ge\|lt\|le\)\."
  syn match   fortranObsolete		"\.\(eq\|ne\)\."
else
  syn keyword fortranStatement		goto
  syn keyword fortranUnitHeader		entry
  syn match fortranType			"double\s\+precision"
  syn keyword fortranStorageClass	common equivalence block data
  syn keyword fortran66Specific		cabs ccos cexp clog csin csqrt dabs
  syn keyword fortran66Specific		dacos dasin datan datan2 dcos dcosh
  syn keyword fortran66Specific		ddim dexp dint dlog dlog10 dmod
  syn keyword fortran66Specific		dnint dsign dsin dsinh dsqrt dtan
  syn keyword fortran66Specific		dtanh iabs idim idnint isign idint ifix
  syn keyword fortran66Specific		amax0 amax1 dmax1 max0 max1
  syn keyword fortran66Specific		amin0 amin1 dmin1 min0 min1
  syn keyword fortran66Specific		amod float sngl alog alog10
  syn keyword fortran77Intrinsic	dble dprod
  syn match   fortran77Operator		"\.\(gt\|ge\|lt\|le\)\."
  syn match   fortran77Operator		"\.\(eq\|ne\)\."
  if b:fortran_dialect != "f77"
    syn keyword fortranInclude		include
    syn keyword fortranStorageClass	sequence
  endif
endif

if (b:fortran_fixed_source == 1 || b:fortran_dialect == "f77")
  syn match fortranCommentError		"^\ \{,4}[^\ \t0-9].*$"
  syn match fortranComment		"^[cC*].*$"	contains=fortranTodo,fortranTab
  syn match fortranContinueLine		"^\ \{5}\S"ms=e

  syn match fortranNoLabelNumber	"^\d\{5}\S.*$"
  syn match fortranNoLabelNumber	"^\ \d\{4}\S.*$"
  syn match fortranNoLabelNumber	"^\ \{2}\d\{3}\S.*$"
  syn match fortranNoLabelNumber	"^\ \{3}\d\{2}\S.*$"
  syn match fortranNoLabelNumber	"^\ \{4}\d\S.*$"
else
  syn match fortranContinueLine		"\&"
endif

if b:fortran_dialect == "f95"
  syn keyword fortranRepeat		forall endforall
  syn keyword fortranIntrinsic		null cpu_time
  syn keyword fortranType		elemental pure
endif

if !exists("did_fortran_syntax_inits")
  let did_fortran_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link fortranStatement		Statement
  hi link fortranConstructName		Special
  hi link fortranConditional		Conditional
  hi link fortranRepeat			Repeat
  hi link fortranTodo			Todo
  hi link fortranContinueLine		Todo
  hi link fortranString			String
  hi link fortranNumber			Number
  hi link fortranFloat			Float
  hi link fortranOperator		Operator
  hi link fortran77Operator		Operator
  hi link fortranBoolean		Boolean
  hi link fortranComment		Comment
  hi link fortranCommentError		Error
  hi link fortranLabelNumber		Special
  hi link fortranNoLabelNumber		Error
  hi link fortranObsolete		Todo
  hi link fortranType			Type
  hi link fortranStructure		Type
  hi link fortranStorageClass		StorageClass
  hi link fortranUnitHeader		fortranPreCondit
  hi link fortranReadWrite		fortranIntrinsic
  hi link fortranI_O			fortranIntrinsic
  hi link fortranIntrinsic		Special
  hi link fortran66Specific		Special
  hi link fortran77Intrinsic		Special
  hi link fortranIdentifier		Identifier
  hi link fortranSpecial		Special
  hi link fortranPreCondit		PreCondit
  hi link fortranParenError		Error

  " optional highlighting
  " If you like tabs, comment out the next two lines
  hi link fortranIniTab			Error
  hi link fortranTab			Error
  " Uncomment next line if some vendor extensions are defined
  "hi link fortranExtended		Special
endif

let b:current_syntax = "fortran"

" vim: ts=8
