" Vim syntax file
" Language:	Maple V   (I only have release 1, so newer stuff is missing -- sorry!)
" Maintainer:	Dr Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	December 3, 1997

" Remove any old syntax stuff hanging around
syn clear

" parenthesis sanity checker
syn region mvZone	matchgroup=Delimiter start="(" matchgroup=Delimiter end=")" transparent contains=ALLBUT,mvError,mvBraceError,mvCurlyError
syn region mvZone	matchgroup=Delimiter start="{" matchgroup=Delimiter end="}" transparent contains=ALLBUT,mvError,mvBraceError,mvParenError
syn region mvZone	matchgroup=Delimiter start="\[" matchgroup=Delimiter end="]" transparent contains=ALLBUT,mvError,mvCurlyError,mvParenError
syn match  mvError	"[)\]}]"
syn match  mvBraceError	"[)}]"	contained
syn match  mvCurlyError	"[)\]]"	contained
syn match  mvParenError	"[\]}]"	contained
syn match  mvComma	"[,;:]"

" Maple V Packages, circa Release 1
syn keyword mvPackage	combinat	geometry	liesymm	np	orthopoly	powseries	simplex	student
syn keyword mvPackage	difforms	grobner	linalg	numtheory	plots	projgeom	stats	totorder
syn keyword mvPackage	geom3d	group	logic

" Language Support
syn keyword mvTodo	contained	TODO
syn region  mvString	start=+`+ skip=+``+ end=+`+	keepend	contains=mvTodo
syn region  mvDelayEval	start=+'+ end=+'+	keepend contains=ALLBUT,mvError,mvBraceError,mvCurlyError,mvParenError
syn match   mvVarAssign	"[a-zA-Z_][a-zA-Z_0-9]*[ \t]*:=" contains=mvAssign
syn match   mvAssign	":="	contained

syn keyword mvBool	true	false
syn keyword mvCond	elif	else	fi	if	then

syn keyword mvOper	and not or
syn match   mvOper	"<>\|[<>]=\|[<>]\|="

syn keyword mvRepeat	by	for	in	to
syn keyword mvRepeat	do	from	od	while

syn keyword mvSpecial	NULL
syn match   mvSpecial	"\[\]\|{}"

syn keyword mvStatement	Order	error	next	read	save
syn keyword mvStatement	break	fail	options	readlib	stop
syn keyword mvStatement	display	local	point	readstat	with
syn keyword mvStatement	done	mod	proc	remember	writeto
syn keyword mvStatement	end	mods	quit	return

" Builtin Constants
syn keyword mvConstant Catalan E Pi gamma

" Number handling
syn match mvNumber "\<[0-9][0-9.]*\>"

" Comments:
syn keyword mvDebug	contained	DEBUG
syn match mvComment "#.*$"	contains=mvTodo,mvDebug

" Basic Library Functions
syn keyword mvLibrary $	@	@@	BesselI	BesselJ	BesselK
syn keyword mvLibrary BesselY	Beta	Ci	Content	D	Det
syn keyword mvLibrary Diff	DistDeg	Divide	ERROR	Ei	Eigenvals
syn keyword mvLibrary Eval	Expand	Factor	Factors	FresnelC	FresnelS
syn keyword mvLibrary GAMMA	Gcd	Gcdex	Hermite	Int	Interp
syn keyword mvLibrary Irreduc	Limit	MeijerG	Normal	Nullspace	Power
syn keyword mvLibrary Powmod	Prem	Primitive	Primpart	Product	Psi
syn keyword mvLibrary Quo	RETURN	Randpoly	Rem	Resultant	RootOf
syn keyword mvLibrary Roots	Si	Smith	Sprem	Sqrfree	Sum
syn keyword mvLibrary Svd	Zeta	abs	addressof	alias	allvalues
syn keyword mvLibrary anames	appendto	arccos	arccosh	arccot	arccoth
syn keyword mvLibrary arccsc	arccsch	arcsec	arcsech	arcsin	arcsinh
syn keyword mvLibrary arctan	arctanh	array	assemble	assign	assigned
syn keyword mvLibrary asympt	bernoulli	binomial	cat	chebyshev	chrem
syn keyword mvLibrary coeff	coeffs	collect	combine	compoly	content
syn keyword mvLibrary convert	copy	cos	cosh	cot	coth
syn keyword mvLibrary csc	csch	define	degree	denom	diff
syn keyword mvLibrary dilog	disassemble	discrim	divide	dsolve	entries
syn keyword mvLibrary eqn	erf	euler	eval	evala	evalb
syn keyword mvLibrary evalc	evalf	hypergeom	evalhf	evalm	evaln
syn keyword mvLibrary example	exp	expand	factor	factorial	fnormal
syn keyword mvLibrary fortran	frac	frontend	fsolve	galois	gc
syn keyword mvLibrary gcd	gcdex	genpoly	has	hastype	help
syn keyword mvLibrary icontent	if	ifactor	igcd	igcdex	ilcm
syn keyword mvLibrary indets	indices	int	series	interface	interp
syn keyword mvLibrary intersect	iquo	irem	irreduc	isolve	isprime
syn keyword mvLibrary isqrt	ithprime	laplace	latex	lcm	lcoeff
syn keyword mvLibrary ldegree	leadterm	length	lexorder	lhs	limit
syn keyword mvLibrary ln	log	lprint	macro	map	match
syn keyword mvLibrary max	maxnorm	mellin	member	min	minus
syn keyword mvLibrary modp	modp1	mods	msolve	nextprime	nops
syn keyword mvLibrary norm	normal	numer	op	order	plot
syn keyword mvLibrary plot3d	plotsetup	pointto	prem	prevprime	primpart
syn keyword mvLibrary print	product	quo	radsimp	rand	randpoly
syn keyword mvLibrary readlib	readstat	rem	resultant	rhs	roots
syn keyword mvLibrary round	rsolve	sec	sech	select	seq
syn keyword mvLibrary series	sign	signum	simplify	sin	sinh
syn keyword mvLibrary solve	sort	sprem	sqrt	subs	subsop
syn keyword mvLibrary substring	sum	system	table	tan	tanh
syn keyword mvLibrary taylor	tcoeff	testeq	time	trace	traperror
syn keyword mvLibrary trunc	type	unames	unapply	union	userinfo
syn keyword mvLibrary whattype	with	words	writeto	zip	ztrans


" --  PACKAGES  -------------------------------------------------------
" There are a lot of packages; these are some of the standard ones.
" If you have a slow terminal, you may well wish to comment the
" lines between -- PACKAGES -- lines out.

" Package: combinat
syn keyword mvPkgFunc bell	binomial	cartprod	character
syn keyword mvPkgFunc Chi	combine	composition	decodepart
syn keyword mvPkgFunc encodepart	fibonacci	firstpart	inttovec
syn keyword mvPkgFunc lastpart	multinomial	nextpart	numbcomb
syn keyword mvPkgFunc numbcomp	numbpart	numbperm	partition
syn keyword mvPkgFunc permute	powerset	prevpart	randcomb
syn keyword mvPkgFunc randpart	randperm	stirling1	stirling2
syn keyword mvPkgFunc subsets	vectoint

" Package: difforms
syn keyword mvPkgFunc &^	d	defform	formpart
syn keyword mvPkgFunc mixpar	parity	scalarpart	simpform
syn keyword mvPkgFunc wdegree

" Package: geom3d
syn keyword mvPkgFunc angle	area	are_collinear	are_concurrent
syn keyword mvPkgFunc are_parallel	are_perpendicular	are_tangent	center
syn keyword mvPkgFunc centroid	coordinates	coplanar	distance
syn keyword mvPkgFunc inter	midpoint	onsegment	on_plane
syn keyword mvPkgFunc on_sphere	parallel	perpendicular	powerps
syn keyword mvPkgFunc projection	radius	rad_plane	reflect
syn keyword mvPkgFunc sphere	symmetric	tangent	tetrahedron
syn keyword mvPkgFunc triangle3d	volume

" Package: geometry
syn keyword mvPkgFunc altitude	Appolonius	area	are_collinear
syn keyword mvPkgFunc are_concurrent	are_harmonic	are_orthogonal	are_parallel
syn keyword mvPkgFunc are_perpendicular	are_similar	are_tangent	bisector
syn keyword mvPkgFunc center	centroid	circumcircle	conic
syn keyword mvPkgFunc convexhull	coordinates	detailf	diameter
syn keyword mvPkgFunc distance	ellipse	Eulercircle	Eulerline
syn keyword mvPkgFunc excircle	find_angle	Gergonnepoint	harmonic
syn keyword mvPkgFunc incircle	inter	inversion	is_equilateral
syn keyword mvPkgFunc is_right	make_square	median	midpoint
syn keyword mvPkgFunc Nagelpoint	onsegment	on_circle	on_line
syn keyword mvPkgFunc orthocenter	parallel	perpendicular	perpen_bisector
syn keyword mvPkgFunc polar_point	pole_line	powerpc	projection
syn keyword mvPkgFunc radius	rad_axis	rad_center	randpoint
syn keyword mvPkgFunc reflect	rotate	sides	similitude
syn keyword mvPkgFunc Simsonline	square	symmetric	tangent
syn keyword mvPkgFunc tangentpc

" Package: grobner
syn keyword mvPkgFunc finduni	finite	gbasis	gsolve
syn keyword mvPkgFunc leadmon	normalf	solvable	spoly
syn keyword mvPktOption plex tdeg

" Package: group
syn keyword mvPkgFunc centralizer	cosets	cosrep	groupmember
syn keyword mvPkgFunc grouporder	inter	invperm	isnormal
syn keyword mvPkgFunc mulperms	normalizer	permrep	pres

" Package: liesymm
syn keyword mvPkgFunc &mod	&^	annul	close
syn keyword mvPkgFunc d	determine	getcoeff	getform
syn keyword mvPkgFunc hasclosure	hook	Lie	Lrank
syn keyword mvPkgFunc makeforms	mixpar	setup	value
syn keyword mvPkgFunc wcollect	wdegree	wedgeset	wsubs

" Package: linalg
syn keyword mvPkgFunc add	addcol	addrow	adj	adjoint	angle
syn keyword mvPkgFunc augment	backsub	band	basis	bezout	BlockDiagonal
syn keyword mvPkgFunc charmat	charpoly	col	coldim	colspace	colspan
syn keyword mvPkgFunc companion	concat	cond	copyinto	crossprod	curl
syn keyword mvPkgFunc definite	delcols	delrows	det	diag	diverge
syn keyword mvPkgFunc dotprod	eigenvals	eigenvects	equal	exponential	extend
syn keyword mvPkgFunc ffgausselim	fibonacci	frobenius	gausselim	gaussjord	genmatrix
syn keyword mvPkgFunc grad	GramSchmidt	hadamard	hermite	hessian	hilbert
syn keyword mvPkgFunc htranspose	ihermite	indexfunc	innerprod	intbasis	inverse
syn keyword mvPkgFunc ismith	iszero	jacobian	jordan	JordanBlock	kernel
syn keyword mvPkgFunc laplacian	leastsqrs	linsolve	matrix	minor	minpoly
syn keyword mvPkgFunc mulcol	mulrow	multiply	norm	nullspace	orthog
syn keyword mvPkgFunc permanent	pivot	potential	randmatrix	range	rank
syn keyword mvPkgFunc row	rowdim	rowspace	rowspan	rref	scalarmul
syn keyword mvPkgFunc singularvals	smith	stack	submatrix	subvector	sumbasis
syn keyword mvPkgFunc swapcol	swaprow	sylvester	toeplitz	trace	transpose
syn keyword mvPkgFunc vandermonde	vecpotent	vectdim	vector

" Package: logic
syn keyword mvPkgFunc bequal	bsimp	canon	frominert
syn keyword mvPkgFunc MOD2	toinert	distrib	dual
syn keyword mvPkgFunc environ	randbool	satisfy	tautology

" Package: np
syn keyword mvPkgFunc conj	D	eqns	suball
syn keyword mvPkgFunc V	V_D	X	X_D
syn keyword mvPkgFunc X_V	Y	Y_D	Y_V
syn keyword mvPkgFunc Y_X

" Package: numtheory
syn keyword mvPkgFunc B	bernoulli	cfrac	cyclotomic
syn keyword mvPkgFunc divisors	E	euler	F
syn keyword mvPkgFunc factorset	fermat	GIgcd	ifactor
syn keyword mvPkgFunc imagunit	isolve	isprime	issqrfree
syn keyword mvPkgFunc ithprime	J	jacobi	L
syn keyword mvPkgFunc lambda	legendre	M	mcombine
syn keyword mvPkgFunc mersenne	mipolys	mlog	mobius
syn keyword mvPkgFunc mroot	msqrt	nextprime	nthpow
syn keyword mvPkgFunc order	phi	pprimroot	prevprime
syn keyword mvPkgFunc primroot	rootsunity	safeprime	sigma
syn keyword mvPkgFunc tau

" Package: orthopoly
syn keyword mvPkgFunc G	H	L	P	T	U

" Package: plots
syn keyword mvPkgFunc conformal	cylinderplot	display	display3d
syn keyword mvPkgFunc matrixplot	pointplot	polarplot	replot
syn keyword mvPkgFunc spacecurve	sparsematrixplot	sphereplot	tubeplot

" Package: powseries
syn keyword mvPkgFunc add	compose	evalpow	inverse
syn keyword mvPkgFunc multconst	multiply	negative	powcreate
syn keyword mvPkgFunc powdiff	powexp	powint	powlog
syn keyword mvPkgFunc powpoly	powsolve	quotient	reversion
syn keyword mvPkgFunc subtract	tpsform

" Package: projgeom
syn keyword mvPkgFunc collinear	concur	conjugate	ctangent
syn keyword mvPkgFunc fpconic	harmonic	inter	join
syn keyword mvPkgFunc lccutc	lccutr	lccutr2p	linemeet
syn keyword mvPkgFunc midpoint	onsegment	polarp	poleline
syn keyword mvPkgFunc ptangent	rtangent	tangentte	tharmonic

" Package: simplex
syn keyword mvPkgFunc basis	convexhull	cterm	dual
syn keyword mvPkgFunc feasible	maximize	minimize	pivot
syn keyword mvPkgFunc pivoteqn	pivotvar	ratio	setup
syn keyword mvPkgFunc standardize

" Package: stats
syn keyword mvPkgFunc addrecord	average	ChiSquare	correlation
syn keyword mvPkgFunc covariance	evalstat	Exponential	Fdist
syn keyword mvPkgFunc Ftest	getkey	linregress	median
syn keyword mvPkgFunc mode	multregress	N	projection
syn keyword mvPkgFunc putkey	Q	RandBeta	RandChiSquare
syn keyword mvPkgFunc RandExponential	RandFdist	RandGamma	RandNormal
syn keyword mvPkgFunc RandPoisson	RandStudentsT	RandUniform	regression
syn keyword mvPkgFunc removekey	Rsquared	sdev	serr
syn keyword mvPkgFunc statplot	StudentsT	Uniform	variance

" Package: student
syn keyword mvPkgFunc changevar	combine	completesquare	D
syn keyword mvPkgFunc distance	Int	intercept	intparts
syn keyword mvPkgFunc isolate	leftbox	leftsum	Limit
syn keyword mvPkgFunc makeproc	maximize	middlesum	midpoint
syn keyword mvPkgFunc minimize	powsubs	rightbox	rightsum
syn keyword mvPkgFunc showtangent	simpson	slope	Sum
syn keyword mvPkgFunc trapezoid	value

" Package: totorder
syn keyword mvPkgFunc assume is forget ordering
" --  PACKAGES  -------------------------------------------------------

if !exists("did_maplev_syntax_inits")
  let did_maplev_syntax_inits = 1

  hi link mvBraceError	mvError
  hi link mvCurlyError	mvError
  hi link mvDebug		mvTodo
  hi link mvParenError	mvError

  hi link mvAssign	Delimiter
  hi link mvBool		Boolean
  hi link mvComma		Delimiter
  hi link mvComment	Comment
  hi link mvCond		Conditional
  hi link mvConstant	Number
  hi link mvDelayEval	Label
  hi link mvError		Error
  hi link mvLibrary	Statement
  hi link mvNumber	Number
  hi link mvOper		Operator
  hi link mvPackage	Type
  hi link mvPkgFunc	Type
  hi link mvPktOption	Special
  hi link mvRepeat	Repeat
  hi link mvSpecial	Special
  hi link mvStatement	Statement
  hi link mvString	String
  hi link mvTodo		Todo
endif

let b:current_syntax = "maple"

" vim: ts=9
