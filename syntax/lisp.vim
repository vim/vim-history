" Vim syntax file
" Language:	Lisp
" Maintainer:	Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	December 9, 1997

" remove any old syntax stuff hanging around
syn clear
set iskeyword=42,43,45,47-58,60-62,64-90,97-122,_

" Lists
syn match	lispSymbol	![^()'`,"; \t]\+!	contained
syn match	lispBarSymbol	!|..\{-}|!		contained
syn region	lispList matchgroup=Delimiter start="(" skip="|.\{-}|" matchgroup=Delimiter end=")" contains=lispAtom,lispBQList,lispConcat,lispDeclaration,lispList,lispNumber,lispSymbol,lispSpecial,lispFunc,lispKey,lispVar,lispAtomMark,lispString,lispComment,lispBarSymbol,lispAtomBarSymbol
syn region	lispBQList	matchgroup=PreProc   start="`("	skip="|.\{-}|" matchgroup=PreProc   end=")" contains=lispAtom,lispBQList,lispConcat,lispDeclaration,lispList,lispNumber,lispSpecial,lispSymbol,lispFunc,lispKey,lispVar,lispAtomMark,lispString,lispComment,lispBarSymbol,lispAtomBarSymbol

" Atoms
syn match	lispAtomMark	"'"
syn match	lispAtom	"'("me=e-1	contains=lispAtomMark	nextgroup=lispAtomList
syn match	lispAtom	"'[^ \t()]\+"	contains=lispAtomMark
syn match	lispAtomBarSymbol	!'|..\{-}|!	contains=lispAtomMark
syn region	lispAtom	start=+'"+	skip=+\\"+ end=+"+
syn region	lispAtomList	matchgroup=Special start="("	skip="|.\{-}|" matchgroup=Special end=")"	contained contains=lispAtomList,lispAtomNmbr0,lispString,lispComment,lispAtomBarSymbol
syn match	lispAtomNmbr	"\<[0-9]\+"			contained

" Standard Lisp Functions and Macros
syn keyword lispFunc	abs	copy-list	gethash	nreconc	setf
syn keyword lispFunc	access	copy-readtable	go	nreverse	setq
syn keyword lispFunc	acons	copy-seq	graphic-char-p	nset-difference	seventh
syn keyword lispFunc	acos	copy-symbol	hash-table-p	nstring-upcase	shadow
syn keyword lispFunc	acosh	copy-tree	host-namestring	nsublis	shiftf
syn keyword lispFunc	adjoin	cos	identity	nsubst	short-site-name
syn keyword lispFunc	adjust-array	cosh	if	nsubst-if	signed-byte
syn keyword lispFunc	alpha-char-p	count	if-exists	nsubst-if-not	signum
syn keyword lispFunc	alphanumericp	count-if	ignore	nsubstitute	simple-string-p
syn keyword lispFunc	and	count-if-not	imagpart	nsubstitute-if	simple-vector-p
syn keyword lispFunc	append	ctypecase	import	nth	sin
syn keyword lispFunc	apply	decf	in-package	nthcdr	sinh
syn keyword lispFunc	applyhook	declaration	in-package	null	sixth
syn keyword lispFunc	apropos	declare	incf	numberp	sleep
syn keyword lispFunc	apropos-list	decode-float	inline	numerator	software-type
syn keyword lispFunc	aref	defconstant	input-stream-p	nunion	some
syn keyword lispFunc	array-dimension		inspect	oddp	sort
syn keyword lispFunc	array-rank	defparameter	int-char	open	special
syn keyword lispFunc	array-rank-limit		integer-length	optimize	special-form-p
syn keyword lispFunc	arrayp	defstruct	integerp	or	sqrt
syn keyword lispFunc	ash		intern	output-stream-p	stable-sort
syn keyword lispFunc	asin		intersection	package-name	standard-char-p
syn keyword lispFunc	asinh	defvar	isqrt	packagep	step
syn keyword lispFunc	assert	delete	keyword	pairlis	streamup
syn keyword lispFunc	assoc	delete-file		parse-integer	string
syn keyword lispFunc	assoc-if	delete-if	last	pathname	string-char
syn keyword lispFunc	assoc-if-not	delete-if-not	lcm	pathname-device	string-char-p
syn keyword lispFunc	atan	denominator	ldb	pathname-host	string-downcase
syn keyword lispFunc	atanh	deposit-field	ldb-test	pathname-name	string-equal
syn keyword lispFunc	bit	describe	ldiff	pathname-type	string-greaterp
syn keyword lispFunc	bit-and	digit-char	length	pathnamep	string-lessp
syn keyword lispFunc	bit-andc1	digit-char-p		peek-char	string-trim
syn keyword lispFunc	bit-andc2	directory	let*	phase	string-upcase
syn keyword lispFunc	bit-eqv	disassemble	lisp	pi	string/=
syn keyword lispFunc	bit-ior	do	list	plusp	string<
syn keyword lispFunc	bit-nand	do*	list*	pop	string<=
syn keyword lispFunc	bit-nor	do-all-symbols	list-length	position	string=
syn keyword lispFunc	bit-not	do-symbols	listen	position-if	string>
syn keyword lispFunc	bit-orc1	documentation	listp	position-if-not	string>=
syn keyword lispFunc	bit-orc2	dolistdotimes	load	pprint	stringp
syn keyword lispFunc	bit-vector-p	dpb		prin1	sublim
syn keyword lispFunc	bit-xor	dribble	log	prin1-to-string	subseq
syn keyword lispFunc	block	ecase	logand	princ	subsetp
syn keyword lispFunc	boole	ed	logandc1	princ-to-string	subst
syn keyword lispFunc	both-case-p	eighth	logandc2	print	subst-if
syn keyword lispFunc	boundp	elt	logcount	probe-file	subst-if-not
syn keyword lispFunc	break	endp	logeqv	proclaim	subtypep
syn keyword lispFunc	butlast	eq	logior	prog	svref
syn keyword lispFunc	byte	eql	lognand	prog*	sxhash
syn keyword lispFunc	byte-poision	equal	lognor	prog1	symbol-function
syn keyword lispFunc	byte-size	equalp	lognot	prog2	symbol-name
syn keyword lispFunc	car	error	logorc1	progn	symbol-package
syn keyword lispFunc	catch	etypecase	logorc2	progv	symbol-plist
syn keyword lispFunc	ccase	eval	logtest	provide	symbol-value
syn keyword lispFunc	cdr	eval-when	logxor	psetf	symbolp
syn keyword lispFunc	ceiling	evalhook	long-site-name	psetq	sys
syn keyword lispFunc	cerror	evenp	loop	push	system
syn keyword lispFunc	char	every	lower-case-p	pushnew	t
syn keyword lispFunc	char-bit	exp	machine-type	putprop	tagbody
syn keyword lispFunc	char-bits	export	machine-version	quote	tailp
syn keyword lispFunc	char-bits-limit	expt	macro-function	random	tan
syn keyword lispFunc	char-code	fboundp	macroexpand	random-state-p	tanh
syn keyword lispFunc	char-code-limit	fceiling	macroexpand-l	rassoc	tenth
syn keyword lispFunc	char-downcase	ffloor		rassoc-if	terpri
syn keyword lispFunc	char-equal	fifth	make-array	rassoc-if-not	the
syn keyword lispFunc	char-font	file-author	make-array	rational	third
syn keyword lispFunc	char-font-limit	file-length	make-char	rationalize	throw
syn keyword lispFunc	char-greaterp	file-namestring	make-hash-table	rationalp	time
syn keyword lispFunc	char-hyper-bit	file-position	make-list	read	trace
syn keyword lispFunc	char-int	file-write-date	make-package	read-byte	tree-equal
syn keyword lispFunc	char-lessp	fill	make-pathname	read-char	truename
syn keyword lispFunc	char-meta-bit	fill-pointer	make-sequence	read-eval-print	truncase
syn keyword lispFunc	char-name	find	make-string	read-line	type
syn keyword lispFunc	char-not-equal	find-if	make-symbol	readtablep	type-of
syn keyword lispFunc	char-not-lessp	find-if-not	makunbound	realpart	typecase
syn keyword lispFunc	char-super-bit	find-package	map	reduce	typep
syn keyword lispFunc	char-upcase	find-symbol	mapc	rem	unexport
syn keyword lispFunc	char/=	finish-output	mapcan	remf	unintern
syn keyword lispFunc	char<	first	mapcar	remhash	union
syn keyword lispFunc	char<=		mapcon	remove	unless
syn keyword lispFunc	char=	float	maphash	remove-if	unread
syn keyword lispFunc	char>	float-digits	mapl	remove-if-not	unsigned-byte
syn keyword lispFunc	char>=	float-precision	maplist	remprop	untrace
syn keyword lispFunc	character	float-radix	mask-field	rename-file	unuse-package
syn keyword lispFunc	characterp	float-sign	max	rename-package	unwind-protect
syn keyword lispFunc	check-type	floatp	member	replace	upper-case-p
syn keyword lispFunc	cis	floor	member-if	require	use-package
syn keyword lispFunc	clear-input	fmakunbound	member-if-not	rest	user
syn keyword lispFunc	clear-output	force-output	merge	return	values
syn keyword lispFunc	close	fourth	merge-pathname	return-from	values-list
syn keyword lispFunc	clrhash	fresh-line	min	revappend	vector
syn keyword lispFunc	code-char	fround	minusp	reverse	vector-pop
syn keyword lispFunc	coerce	ftruncate	mismatch	room	vector-push
syn keyword lispFunc	commonp	ftype	mod	rotatef	vectorp
syn keyword lispFunc	compile	funcall	name-char	round	warn
syn keyword lispFunc	compile-file	function	namestring	rplaca	when
syn keyword lispFunc	compiler-let	functionp	nbutlast	rplacd	with-open-file
syn keyword lispFunc	complex	gbitp	nconc	sbit	write
syn keyword lispFunc	complexp	gcd	nil	scale-float	write-byte
syn keyword lispFunc	concatenate	gensym	nintersection	schar	write-char
syn keyword lispFunc	cond	gentemp	ninth	search	write-line
syn keyword lispFunc	conjugate	get	not	second	write-string
syn keyword lispFunc	cons	get-properties	notany	set	write-to-string
syn keyword lispFunc	consp	get-setf-method	notevery	set-char-bit	y-or-n-p
syn keyword lispFunc	constantp	getf	notinline	set-difference	yes-or-no-p
syn keyword lispFunc	copy-alist	<	>	=	-
syn keyword lispFunc	+	*	/
syn match   lispFunc	"\<c[ad]\+r\>"

syn keyword lispFunc	adjustable-array-p	least-negative-short-float	nstring-capitalize
syn keyword lispFunc	array-dimension-limit	least-negative-single-float	nstring-downcase
syn keyword lispFunc	array-dimensions		least-positive-double-float	nsubstitute-if-not
syn keyword lispFunc	array-element-type	least-positive-long-float	package-nicknames
syn keyword lispFunc	array-has-fill-pointer-p	least-positive-short-float	package-shadowing-symbols
syn keyword lispFunc	array-in-bounds-p	least-positive-single-float	package-use-list
syn keyword lispFunc	array-row-major-index	lisp-implementation-type	package-used-by-list
syn keyword lispFunc	array-total-size		lisp-implementation-version	parse-namestring
syn keyword lispFunc	array-total-size-limit	list-all-packages	pathname-directory
syn keyword lispFunc	call-arguments-limit	long-float-epsilon	pathname-version
syn keyword lispFunc	char-control-bit		long-float-negative-epsilon	read-char-no-hang
syn keyword lispFunc	char-not-greaterp	machine-instance		read-delimited-list
syn keyword lispFunc	compiled-function-p	make-broadcast-stream	read-from-string
syn keyword lispFunc	decode-universal-time	make-concatenated-stream	read-preserving-whitespace
syn keyword lispFunc	define-modify-macro	make-dispatch-macro-character	remove-duplicates
syn keyword lispFunc	define-setf-method	make-echo-stream		set-dispatch-macro-character
syn keyword lispFunc	delete-duplicates	make-random-state	set-exclusive-or
syn keyword lispFunc	directory-namestring	make-string-input-stream	set-macro-character
syn keyword lispFunc	do-exeternal-symbols	make-string-output-stream	set-syntax-from-char
syn keyword lispFunc	double-float-epsilon	make-synonym-stream	shadowing-import
syn keyword lispFunc	double-float-negative-epsilon	make-two-way-stream	short-float-epsilon
syn keyword lispFunc	encode-universal-time	most-negative-double-float	simple-bit-vector-p
syn keyword lispFunc	enough-namestring	most-negative-fixnum	single-flaot-epsilon
syn keyword lispFunc	find-all-symbols		most-negative-long-float	single-float-negative-epsilon
syn keyword lispFunc	get-decoded-time		most-negative-short-float	software-version
syn keyword lispFunc	get-dispatch-macro-character	most-negative-single-float	stream-element-type
syn keyword lispFunc	get-internal-real-time	most-positive-double-float	string-capitalize
syn keyword lispFunc	get-internal-run-time	most-positive-fixnum	string-left-trim
syn keyword lispFunc	get-macro-character	most-positive-long-float	string-not-equal
syn keyword lispFunc	get-output-stream-string	most-positive-short-float	string-not-greaterp
syn keyword lispFunc	get-universal-time	most-positive-single-float	string-not-lessp
syn keyword lispFunc	hash-table-count		multiple-value-bind	string-right-strim
syn keyword lispFunc	integer-decode-float	multiple-value-call	user-homedir-pathname
syn keyword lispFunc	internal-time-units-per-second	multiple-value-list	vector-push-extend
syn keyword lispFunc	lambda-list-keywords	multiple-value-prog1	with-input-from-string
syn keyword lispFunc	lambda-parameters-limit	multiple-value-seteq	with-open-stream
syn keyword lispFunc	least-negative-double-float	multiple-values-limit	with-output-to-string
syn keyword lispFunc	least-negative-long-float	nset-exclusive-or

" Lisp Keywords (modifiers)
syn keyword lispKey	:abort	:element-type	:internal	:probe
syn keyword lispKey	:adjustable	:end	:io	:radix
syn keyword lispKey	:append	:end1	:junk-allowed	:read-only
syn keyword lispKey	:array	:end2	:key	:rehash-size
syn keyword lispKey	:base	:error	:length	:rename
syn keyword lispKey	:case	:escape	:level	:size
syn keyword lispKey	:circle	:external	:name	:start
syn keyword lispKey	:conc-name	:from-end	:named	:start1
syn keyword lispKey	:constructor	:gensym	:new-version	:start2
syn keyword lispKey	:copier	:host	:nicknames	:stream
syn keyword lispKey	:count	:if-exists	:output	:supersede
syn keyword lispKey	:create	:include	:output-file	:test
syn keyword lispKey	:default	:index	:overwrite	:test-not
syn keyword lispKey	:defaults	:inherited	:predicate	:type
syn keyword lispKey	:device	:initial-element	:pretty	:use
syn keyword lispKey	:direction	:initial-offset	:print	:verbose
syn keyword lispKey	:directory	:initial-value	:print-function	:version
syn keyword lispKey	:displaced-to	:input

syn keyword lispKey	:displaced-index-offset	:initial-contents	:rehash-threshold
syn keyword lispKey	:if-does-not-exist	:preserve-whitespace	:rename-and-delete

" Standard Lisp Variables
syn keyword lispVar	*applyhook*	*modules*	*print-circle*	*print-pretty*	*read-suppress*
syn keyword lispVar	*debug-io*	*package*	*print-escape*	*print-radix*	*readtable*
syn keyword lispVar	*error-output*	*print-array*	*print-gensym*	*query-io*	*standard-input*
syn keyword lispVar	*evalhook*	*print-base*	*print-length*	*random-state*	*terminal-io*
syn keyword lispVar	*features*	*print-case*	*print-level*	*read-base*	*trace-output*
syn keyword lispVar	*load-verbose*

syn keyword lispVar	*break-on-warnings*	*macroexpand-hook*	*standard-output*
syn keyword lispVar	*default-pathname-defaults*	*read-default-float-format*

" Strings
syn region	lispString	start=+"+	skip=+\\"+ end=+"+

" Shared with Xlisp, Declarations, Macros, Functions
syn keyword	lispDeclaration	defmacro defsetf deftype defun dotimes flet labels let locally macrolet
syn keyword	lispDeclaration	do* do-all-symbols do-external-symbols do-symbols multiple-value-bind

syn match	lispNumber	"[0-9]\+"

syn match	lispSpecial	oneline	"\*[a-zA-Z_][a-zA-Z_0-9-]*\*"
syn match	lispSpecial	oneline	!#|[^()'`,"; \t]\+|#!
syn match	lispSpecial	oneline	!#x[0-9a-fA-F]\+!
syn match	lispSpecial	oneline	!#o[0-7]\+!
syn match	lispSpecial	oneline	!#b[01]\+!
syn match	lispSpecial	oneline	!#\\[ -\~]!
syn match	lispSpecial	oneline	!#[':][^()'`,"; \t]\+!
syn match	lispSpecial	oneline	!#([^()'`,"; \t]\+)!

syn match	lispConcat	"\s\.\s"
syntax match	lispParenError	")"

" Comments
syn match	lispComment	";.*$"

" synchronization
syn sync lines=100

if !exists("did_lisp_syntax_inits")
  let did_lisp_syntax_inits= 1
  hi link lispAtomNmbr	lispNumber
  hi link lispAtomMark	lispMark

  hi link lispAtom	Identifier
  hi link lispAtomBarSymbol	Special
  hi link lispBarSymbol	Special
  hi link lispComment	Comment
  hi link lispConcat	Statement
  hi link lispDeclaration	Statement
  hi link lispFunc	Statement
  hi link lispKey		Type
  hi link lispMark	Delimiter
  hi link lispNumber	Number
  hi link lispParenError	Error
  hi link lispSpecial	Type
  hi link lispString	String
  hi link lispVar	Statement
  endif

let b:current_syntax = "lisp"

" vim: ts=18
