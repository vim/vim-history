" Language:	Vim 5.2 script
" Maintainer:	Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	August 28, 1998

" Remove old syntax
syn clear

syn keyword vimTodo contained	TODO

" regular vim commands
syn keyword vimCommand contained	I	cr[ewind]	ma[rk]	sbl[ast]	w[rite]
syn keyword vimCommand contained	N[ext]	cu[nmap]	mak[e]	sbm[odified]	wa[ll]
syn keyword vimCommand contained	Next	cuna[bbrev]	map	sbn[ext]	wh[ile]
syn keyword vimCommand contained	P[rint]	d[elete]	mapc[lear]	sbp[revious]	wn[ext]
syn keyword vimCommand contained	a[ppend]	delc[ommand]	marks	sbr[ewind]	wp[revous]
syn keyword vimCommand contained	ab[breviate]	delf[unction]	mk[exrc]	se[t]	wq
syn keyword vimCommand contained	abc[lear]	di[splay]	mks[ession]	set	wqa[ll]
syn keyword vimCommand contained	al[l]	dig[raphs]	mkv[imrc]	sh[ell]	wv[iminfo]
syn keyword vimCommand contained	ar[gs]	display	mod[e]	si[malt]	x[it]
syn keyword vimCommand contained	argu[ment]	dj[ump]	n[ext]	sl[eep]	xa[ll]
syn keyword vimCommand contained	as[cii]	dl[ist]	new	sla[st]	y[ank]
syn keyword vimCommand contained	au[tocmd]	do[autocmd]	nm[ap]	sm[agic]	z[+-^.=]
syn keyword vimCommand contained	aug[roup]	doautoa[ll]	nmap	sn[ext]	am
syn keyword vimCommand contained	autocmd	ds[earch]	nmapc[lear]	sno[magic]	amenu
syn keyword vimCommand contained	bN[ext]	dsp[lit]	nn[oremap]	so[urce]	an
syn keyword vimCommand contained	b[uffer]	e[dit]	no[remap]	sp[lit]	anoremenu
syn keyword vimCommand contained	ba[ll]	ec[ho]	noh[lsearch]	spr[evious]	aun
syn keyword vimCommand contained	bad[d]	echoh[l]	norea[bbrev]	sr[ewind]	aunmenu
syn keyword vimCommand contained	bd[elete]	echon	norm[al]	st[op]	be
syn keyword vimCommand contained	bdelete	el[se]	normal	sta[g]	behave
syn keyword vimCommand contained	be[have]	elsei[f]	nu[mber]	stj[ump]	cme
syn keyword vimCommand contained	bl[ast]	en[dif]	nun[map]	sts[elect]	cmenu
syn keyword vimCommand contained	bm[odified]	endf[unction]	o[pen]	sun[hide]	cnoreme
syn keyword vimCommand contained	bn[ext]	endw[hile]	om[ap]	sus[pend]	cnoremenu
syn keyword vimCommand contained	bp[revious]	ex	omap	sv[iew]	cunme
syn keyword vimCommand contained	br[ewind]	exe[cute]	omapc[lear]	sy[ntax]	cunmenu
syn keyword vimCommand contained	brea[k]	exi[t]	on[ly]	syntax	ime
syn keyword vimCommand contained	bro[wse]	f[ile]	ono[remap]	t	imenu
syn keyword vimCommand contained	browse	files	ou[nmap]	tN[ext]	inoreme
syn keyword vimCommand contained	buffers	fix[del]	p[rint]	ta[g]	inoremenu
syn keyword vimCommand contained	bun[load]	fu[nction]	pe[rl]	tags	iunme
syn keyword vimCommand contained	bunload	g[lobal]	perld[o]	tc[l]	iunmenu
syn keyword vimCommand contained	cN[ext]	global	po[p]	tcld[o]	me
syn keyword vimCommand contained	c[hange]	gr[ep]	pre[serve]	tclf[ile]	menu
syn keyword vimCommand contained	ca[bbrev]	h[elp]	prev[ious]	te[aroff]	nme
syn keyword vimCommand contained	cabc[lear]	hi[ghlight]	promptf[ind]	the	nmenu
syn keyword vimCommand contained	cal[l]	hid[e]	promptr[epl]	tj[ump]	nnoreme
syn keyword vimCommand contained	cc	i[nsert]	pu[t]	tl[ast]	nnoremenu
syn keyword vimCommand contained	cd	ia[bbrev]	pw[d]	tm[enu]	noreme
syn keyword vimCommand contained	ce[nter]	iabc[lear]	py[thon]	tn[ext]	noremenu
syn keyword vimCommand contained	cf[ile]	if	pyf[ile]	tp[revious]	nunme
syn keyword vimCommand contained	chd[ir]	ij[ump]	q[uit]	tr[ewind]	nunmenu
syn keyword vimCommand contained	che[ckpath]	il[ist]	qa[ll]	ts[elect]	ome
syn keyword vimCommand contained	cl[ist]	im[ap]	r[ead]	tu[nmenu]	omenu
syn keyword vimCommand contained	cla[st]	imap	rec[over]	tunmenu	onoreme
syn keyword vimCommand contained	clo[se]	imapc[lear]	red[o]	u[ndo]	onoremenu
syn keyword vimCommand contained	cm[ap]	ino[remap]	redi[r]	una[bbreviate]	ounme
syn keyword vimCommand contained	cmap	inorea[bbrev]	reg[isters]	unh[ide]	ounmenu
syn keyword vimCommand contained	cmapc[lear]	is[earch]	res[ize]	unl[et]	tm
syn keyword vimCommand contained	cn[ext]	isp[lit]	ret[ab]	unm[ap]	tmenu
syn keyword vimCommand contained	cnew[er]	iu[nmap]	retu[rn]	v[global]	tu
syn keyword vimCommand contained	cno[remap]	iuna[bbrev]	rew[ind]	ve[rsion]	tunmenu
syn keyword vimCommand contained	cnorea[bbrev]	j[oin]	ri[ght]	vi[sual]	unme
syn keyword vimCommand contained	co[py]	ju[mps]	rv[iminfo]	vie[w]	unmenu
syn keyword vimCommand contained	col[der]	k	sN[ext]	vm[ap]	vme
syn keyword vimCommand contained	com[mand]	l[ist]	s[ubstitute]	vmap	vmenu
syn keyword vimCommand contained	comc[lear]	la[st]	sa[rgument]	vmapc[lear]	vnoreme
syn keyword vimCommand contained	con[tinue]	le[ft]	sal[l]	vn[oremap]	vnoremenu
syn keyword vimCommand contained	conf[irm]	let	sbN[ext]	vu[nmap]	vunme
syn keyword vimCommand contained	cp[revious]	ls	sb[uffer]	wN[ext]	vunmenu
syn keyword vimCommand contained	cq[uit]	m[ove]	sba[ll]

" All vimCommands are contained by vimIsCommands.
syn match vimIsCommand	"^\s*[a-zA-Z]\+"		transparent contains=vimCommand
syn match vimIsCommand	"[:|]\s*[a-zA-Z]\+"	transparent contains=vimCommand

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	ai	eol	isprint	ruler	tagrelative
syn keyword vimOption contained	akm	ep	joinspaces	sb	tags
syn keyword vimOption contained	al	equalalways	js	sbr	tbi
syn keyword vimOption contained	aleph	equalprg	keymodel	sc	tbs
syn keyword vimOption contained	allowrevins	errorbells	keywordprg	scr	term
syn keyword vimOption contained	altkeymap	errorfile	km	scroll	terse
syn keyword vimOption contained	ari	errorformat	kp	scrolljump	textauto
syn keyword vimOption contained	autoindent	esckeys	langmap	scrolloff	textmode
syn keyword vimOption contained	autowrite	et	laststatus	scs	textwidth
syn keyword vimOption contained	aw	eventignore	lazyredraw	sect	tf
syn keyword vimOption contained	background	ex	lbr	sections	tildeop
syn keyword vimOption contained	backspace	expandtab	lcs	secure	timeout
syn keyword vimOption contained	backup	exrc	linebreak	sel	timeoutlen
syn keyword vimOption contained	backupdir	fe	lines	selection	title
syn keyword vimOption contained	backupext	ff	lisp	selectmode	titlelen
syn keyword vimOption contained	bdir	ffs	list	sessionoptions	titlestring
syn keyword vimOption contained	bex	fileencoding	listchars	sft	tl
syn keyword vimOption contained	bg	fileformat	lmap	sh	tm
syn keyword vimOption contained	bin	fileformats	ls	shcf	to
syn keyword vimOption contained	binary	filetype	lz	shell	top
syn keyword vimOption contained	biosk	fk	magic	shellcmdflag	tr
syn keyword vimOption contained	bioskey	fkmap	makeef	shellpipe	ts
syn keyword vimOption contained	bk	fo	makeprg	shellquote	tsl
syn keyword vimOption contained	breakat	formatoptions	mat	shellredir	ttimeout
syn keyword vimOption contained	brk	formatprg	matchpairs	shelltype	ttimeoutlen
syn keyword vimOption contained	browsedir	fp	matchtime	shellxquote	ttm
syn keyword vimOption contained	bs	ft	maxfuncdepth	shiftround	ttybuiltin
syn keyword vimOption contained	bsdir	gcr	maxmapdepth	shiftwidth	ttyfast
syn keyword vimOption contained	cf	gd	maxmem	shm	ttym
syn keyword vimOption contained	ch	gdefault	maxmemtot	shortmess	ttymouse
syn keyword vimOption contained	cin	gfm	mef	shortname	ttyscroll
syn keyword vimOption contained	cindent	gfn	mfd	showbreak	ttytype
syn keyword vimOption contained	cink	go	mh	showcmd	tw
syn keyword vimOption contained	cinkeys	gp	ml	showfulltag	tx
syn keyword vimOption contained	cino	grepformat	mls	showmatch	uc
syn keyword vimOption contained	cinoptions	grepprg	mm	showmode	ul
syn keyword vimOption contained	cinw	guicursor	mmd	shq	undolevels
syn keyword vimOption contained	cinwords	guifont	mmt	si	updatecount
syn keyword vimOption contained	cmdheight	guioptions	mod	sidescroll	updatetime
syn keyword vimOption contained	co	guipty	modeline	sj	ut
syn keyword vimOption contained	columns	helpfile	modelines	slm	vb
syn keyword vimOption contained	com	helpheight	modified	sm	vbs
syn keyword vimOption contained	comments	hf	more	smartcase	verbose
syn keyword vimOption contained	compatible	hh	mouse	smartindent	vi
syn keyword vimOption contained	complete	hi	mousef	smarttab	viminfo
syn keyword vimOption contained	confirm	hid	mousefocus	smd	visualbell
syn keyword vimOption contained	cp	hidden	mousehide	sn	wa
syn keyword vimOption contained	cpo	highlight	mousem	so	wak
syn keyword vimOption contained	cpoptions	history	mousemodel	softtabstop	warn
syn keyword vimOption contained	cpt	hk	mouset	sol	wb
syn keyword vimOption contained	cscopeprg	hkmap	mousetime	sp	wc
syn keyword vimOption contained	cscopetag	hkmapp	mp	splitbelow	wd
syn keyword vimOption contained	cscopetagorder	hkp	mps	sr	weirdinvert
syn keyword vimOption contained	cscopeverbose	hl	nf	srr	wh
syn keyword vimOption contained	csprg	hls	nrformats	ss	whichwrap
syn keyword vimOption contained	cst	hlsearch	nu	ssop	wig
syn keyword vimOption contained	csto	ic	number	st	wildchar
syn keyword vimOption contained	csverb	icon	pa	sta	wildignore
syn keyword vimOption contained	def	iconstring	para	startofline	wildmode
syn keyword vimOption contained	define	ignorecase	paragraphs	sts	wim
syn keyword vimOption contained	dg	im	paste	su	winaltkeys
syn keyword vimOption contained	dict	inc	patchmode	suffixes	winheight
syn keyword vimOption contained	dictionary	include	path	sw	winminheight
syn keyword vimOption contained	digraph	incsearch	pm	swapfile	wiv
syn keyword vimOption contained	dir	inf	readonly	swapsync	wm
syn keyword vimOption contained	directory	infercase	remap	swf	wmh
syn keyword vimOption contained	ea	insertmode	report	sws	wrap
syn keyword vimOption contained	eb	is	restorescreen	sxq	wrapmargin
syn keyword vimOption contained	ed	isf	revins	syn	wrapscan
syn keyword vimOption contained	edcompatible	isfname	ri	syntax	writeany
syn keyword vimOption contained	ef	isi	rightleft	ta	writebackup
syn keyword vimOption contained	efm	isident	rl	tabstop	writedelay
syn keyword vimOption contained	ei	isk	ro	tag	ws
syn keyword vimOption contained	ek	iskeyword	rs	tagbsearch	ww
syn keyword vimOption contained	endofline	isp	ru	taglength

" These are the turn-off setting variants
syn keyword vimOption contained	noai	noequalalways	nojoinspaces	noru	notbs
syn keyword vimOption contained	noakm	noerrorbells	nojs	noruler	noterse
syn keyword vimOption contained	noalkeymap	noesckeys	nolbr	nosb	notextauto
syn keyword vimOption contained	noallowrevins	noet	nolinebreak	nosc	notextmode
syn keyword vimOption contained	noari	noex	nolisp	noscs	notf
syn keyword vimOption contained	noautoindent	noexpandtab	nolist	nosecure	notildeop
syn keyword vimOption contained	noautowrite	noexrc	nomagic	noshiftround	notimeout
syn keyword vimOption contained	noaw	nofk	nomh	noshortname	notitle
syn keyword vimOption contained	nobackup	nofkmap	noml	noshowcmd	noto
syn keyword vimOption contained	nobin	nogd	nomod	noshowmatch	notop
syn keyword vimOption contained	nobinary	nogdefault	nomodeline	noshowmode	notr
syn keyword vimOption contained	nobiosk	noguipty	nomodified	nosi	nottimeout
syn keyword vimOption contained	nobioskey	nohid	nomore	nosm	nottybuiltin
syn keyword vimOption contained	nobk	nohidden	nomousef	nosmartcase	nottyfast
syn keyword vimOption contained	nocf	nohk	nomousefocus	nosmartindent	notx
syn keyword vimOption contained	nocin	nohkmap	nomousehide	nosmarttab	novb
syn keyword vimOption contained	nocindent	nohkmapp	nonu	nosmd	novisualbell
syn keyword vimOption contained	nocompatible	nohkp	nonumber	nosn	nowa
syn keyword vimOption contained	noconfirm	nohls	nopaste	nosol	nowarn
syn keyword vimOption contained	nocp	nohlsearch	noreadonly	nosplitbelow	nowb
syn keyword vimOption contained	nodg	noic	noremap	nosr	noweirdinvert
syn keyword vimOption contained	nodigraph	noicon	norestorescreen	nosta	nowiv
syn keyword vimOption contained	noea	noignorecase	norevins	nostartofline	nowrap
syn keyword vimOption contained	noeb	noim	nori	noswapfile	nowrapscan
syn keyword vimOption contained	noed	noincsearch	norightleft	nota	nowriteany
syn keyword vimOption contained	noedcompatible	noinf	norl	notagbsearch	nowritebackup
syn keyword vimOption contained	noek	noinfercase	noro	notagrelative	nows
syn keyword vimOption contained	noendofline	noinsertmode	nors	notbi	nowsf
syn keyword vimOption contained	noeol	nois

" termcap codes (which can also be set)
syn keyword vimOption contained	t_AB	t_RI	t_da	t_k9	t_ks	t_so
syn keyword vimOption contained	t_AF	t_Sb	t_db	t_kD	t_ku	t_sr
syn keyword vimOption contained	t_AL	t_Sf	t_dl	t_kI	t_le	t_te
syn keyword vimOption contained	t_CS	t_ZH	t_k1	t_kN	t_mb	t_ti
syn keyword vimOption contained	t_Co	t_ZR	t_k2	t_kP	t_md	t_ue
syn keyword vimOption contained	t_DL	t_al	t_k3	t_kb	t_me	t_us
syn keyword vimOption contained	t_F1	t_bc	t_k4	t_kd	t_mr	t_vb
syn keyword vimOption contained	t_F2	t_cd	t_k5	t_ke	t_ms	t_ve
syn keyword vimOption contained	t_K1	t_ce	t_k6	t_kh	t_nd	t_vi
syn keyword vimOption contained	t_K3	t_cl	t_k7	t_kl	t_op	t_vs
syn keyword vimOption contained	t_K4	t_cm	t_k8	t_kr	t_se	t_xs
syn keyword vimOption contained	t_K5	t_cs
syn match   vimOption contained	"t_#4"
syn match   vimOption contained	"t_%1"
syn match   vimOption contained	"t_%i"
syn match   vimOption contained	"t_&8"
syn match   vimOption contained	"t_@7"
syn match   vimOption contained	"t_k;"

" these settings don't actually cause errors in vim, but were supported by vi and don't do anything in vim
syn keyword vimErrSetting contained	hardtabs	w1200	w9600	wi	window
syn keyword vimErrSetting contained	ht	w300

" AutoBuf Events
syn keyword vimAutoEvent contained	BufDelete	BufReadPost	FileAppendPre	FilterReadPre	User
syn keyword vimAutoEvent contained	BufEnter	BufReadPre	FileChangedShell	FilterWritePost	VimEnter
syn keyword vimAutoEvent contained	BufFilePost	BufUnload	FileReadPost	FilterWritePre	VimLeave
syn keyword vimAutoEvent contained	BufFilePre	BufWrite	FileReadPre	StdinReadPost	VimLeavePre
syn keyword vimAutoEvent contained	BufLeave	BufWritePost	FileWritePost	StdinReadPre	WinEnter
syn keyword vimAutoEvent contained	BufNewFile	BufWritePre	FileWritePre	TermChanged	WinLeave
syn keyword vimAutoEvent contained	BufRead	FileAppendPost	FilterReadPost

" Highlight commonly used Groupnames
syn keyword vimGroup	Comment	Identifier	Keyword	Type	Delimiter
syn keyword vimGroup	Constant	Function	Exception	StorageClass	SpecialComment
syn keyword vimGroup	String	Statement	PreProc	Structure	Debug
syn keyword vimGroup	Character	Conditional	Include	Typedef	Ignore
syn keyword vimGroup	Number	Repeat	Define	Special	Error
syn keyword vimGroup	Boolean	Label	Macro	SpecialChar	Todo
syn keyword vimGroup	Float	Operator	PreCondit	Tag

" Default highlighting groups
syn keyword vimHLGroup	Cursor	LineNr	NonText	Search	Title
syn keyword vimHLGroup	Directory	Menu	Normal	SpecialKey	Visual
syn keyword vimHLGroup	ErrorMsg	ModeMsg	Question	StatusLine	WarningMsg
syn keyword vimHLGroup	IncSearch	MoreMsg	Scrollbar	StatusLineNC

" Function Names
syn keyword vimFuncName contained	argc	delete	has	matchend	synID
syn keyword vimFuncName contained	argv	escape	hlID	matchstr	synIDattr
syn keyword vimFuncName contained	browse	exists	hlexists	nr2char	synIDtran
syn keyword vimFuncName contained	bufexists	expand	hostname	setline	tempname
syn keyword vimFuncName contained	bufname	filereadable	input	strftime	virtcol
syn keyword vimFuncName contained	bufnr	fnamemodify	isdirectory	strlen	winbufnr
syn keyword vimFuncName contained	char2nr	getcwd	line	strpart	winheight
syn keyword vimFuncName contained	col	getline	match	substitute	winnr
syn keyword vimFuncName contained	confirm
syn match   vimFunc     contained	"\I\i*\s*("	contains=vimFuncName

"--- syntax above generated by mkvimvim ---
" Special Vim Highlighting

" Behave!
" =======
syn match   vimBehave	"^\s*\(behave\|behav\|beha\|beh\|be\)" contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn match   vimBehave	"[:|]\s*\(behave\|behav\|beha\|beh\|be\)"lc=1 contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained	mswin	xterm
syn match   vimBehaveError contained	"[^ ]\+"

" Functions
" =========
syn match  vimFunction	"\(function\|functio\|functi\|funct\|func\|fun\)\s\+[A-Z][a-z]*("	contains=vimCommand nextgroup=vimFuncBody
syn match  vimFunctionError	"\(function\|functio\|functi\|funct\|func\|fun\)\s\+[a-z].*("	contains=vimCommand nextgroup=vimFuncBody
syn region vimFuncBody contained	start=")"	end="\<endf"	contains=vimIsCommand,vimGroup,vimHLGroup,vimFunction,vimFunctionError,vimFuncBody,vimSpecFile,vimConditional,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimIsCommand,vimCtrlChar,vimFuncVar
syn match  vimFuncVar  contained	"a:\(\I\i*\|[0-9]\+\)"

syn keyword vimPattern contained	start	skip	end

" Special Filenames and Modifiers
syn match vimSpecFile	"<\(cword\|cWORD\|[cas]file\)>"	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"#\d\+\|[#%]<"		nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFileMod	"\(:[phtre]\)\+"		contained

" Conditionals
syn match vimConditional	"^\s*\(continue\|continu\|contin\|conti\|cont\|con\)\>"
syn match vimConditional	"^\s*\(endif\|endi\|end\|en\)\>"
syn match vimConditional	"^\s*\(endwhile\|endwhil\|endwhi\|endwh\|endw\)\>"
syn match vimConditional	"^\s*\(while\|whil\|whi\|wh\)\>"
syn match vimConditional	"^\s*break\=\>"
syn match vimConditional	"^\s*else\>"
syn match vimConditional	"^\s*elseif\=\>"
syn match vimConditional	"^\s*if\>"

syn match vimConditional	"[:|]\s*\(continue\|continu\|contin\|conti\|cont\|con\)\>"lc=1
syn match vimConditional	"[:|]\s*\(endif\|endi\|end\|en\)\>"lc=1
syn match vimConditional	"[:|]\s*\(endwhile\|endwhil\|endwhi\|endwh\|endw\)\>"lc=1
syn match vimConditional	"[:|]\s*\(while\|whil\|whi\|wh\)\>"lc=1
syn match vimConditional	"[:|]\s*break\=\>"lc=1
syn match vimConditional	"[:|]\s*else\>"lc=1
syn match vimConditional	"[:|]\s*elseif\=\>"lc=1
syn match vimConditional	"[:|]\s*if\>"lc=1

" Operators
syn match vimOper	"[^,:|]||\|&&\|!=\|>=\|<=\|=\~\|!\~\|>\|<\|+\|-\|=\|\."lc=1 skipwhite nextgroup=vimString

" User-Specified Commands
syn cluster vimUserCmdList	contains=vimAddress,vimSyntax,vimHighlight,vimAutoCmd,vimCmplxRepeat,vimComment,vimConditional,vimCtrlChar,vimEscapeBrace,vimFilter,vimFunc,vimFunction,vimGroup,vimHLGroup,vimIsCommand,vimIsCommand,vimMark,vimNotation,vimNumber,vimOper,vimRegion,vimRegister,vimSet,vimSetEqual,vimSetString,vimSpecFile,vimString,vimSubst,vimSubstEnd,vimSubstRange,vimSynLine
syn region  vimUserCmd	transparent oneline matchgroup=vimStatement start="[:|]\s*\(command\|comman\|comma\|comm\|com\)"hs=s+1 end="$"	contains=vimUserAttrb,@vimUserCmdList
syn region  vimUserCmd	transparent oneline matchgroup=vimStatement start="^\s*\(command\|comman\|comma\|comm\|com\)" end="$"		contains=vimUserAttrb,@vimUserCmdList
syn match   vimUserAttrb	contained	"-nargs=[01*?+]"		contains=vimUserAttrbKey,vimOper
syn match   vimUserAttrb	contained transparent  "-complete=\(augroup\|buffer\|command\|dir\|event\|file\|help\|highlight\|menu\|option\|tag\|var\)"	contains=vimUserAttrbCmplt,vimOper
syn match   vimUserAttrb	contained	"-range\(=%\|=[0-9]\+\)\="	contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-count=[0-9]\+"		contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-bang\|-register"	contains=vimOper,vimUserAttrbKey
syn keyword vimUserAttrbKey	contained	bang	count	range
syn keyword vimUserAttrbKey	contained	complete	nargs	register
syn keyword vimUserAttrbCmplt	contained	augroup	dir	help	menu	tag
syn keyword vimUserAttrbCmplt	contained	buffer	event	highlight	option	var
syn keyword vimUserAttrbCmplt	contained	command	file

" Numbers
syn match vimNumber	"\<\d\+.\d\+"
syn match vimNumber	"\<\d\+L\="
syn match vimNumber	"-\d\+.\d\+"
syn match vimNumber	"-\d\+L\="
syn match vimNumber	"[[;:]\d\+"lc=1
syn match vimNumber	"0[xX][0-9a-fA-F]\+"
syn match vimNumber	"#[0-9a-fA-F]\+"

" Lower Priority Comments: after some vim commands...
syn match vimComment	+\s"[^\-:.%#=*].*$+lc=1	contains=vimTodo
syn match vimComment	+\<endif\s\+".*$+lc=5	contains=vimTodo
syn match vimComment	+\<else\s\+".*$+lc=4	contains=vimTodo
syn match vimComment	+^\s*".*$+		contains=vimTodo

" Try to catch strings, if nothing else matches (therefore it must precede the others!)
"  vmEscapeBrace handles ["]  []"] (ie. stays as string)
syn region	vimEscapeBrace	oneline contained transparent	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1
syn match	vimPatSep	contained	"\\[|()]"
syn match	vimNotPatSep	contained	"\\\\"
syn region	vimString	oneline	start=+[^:a-zA-Z>!\\]"+lc=1 skip=+\(\\\\\)*\\"+ end=+"+	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start=+[^:a-zA-Z>!\\]'+lc=1 skip=+\(\\\\\)*\\'+ end=+'+	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start=+=!+lc=1	skip=+\(\\\\\)*\\!+ end=+!+		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start="=+"lc=1	skip="\(\\\\\)*\\+" end="+"		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start="[^\\]+\s*[^a-zA-Z0-9.]"lc=1 skip="\(\\\\\)*\\+" end="+"	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start="\s/\s*[^a-zA-Z]"lc=1 skip="\(\\\\\)*\\+" end="/"	contains=vimEscapeBrace,vimPatSep,vimNotPatSep

" Substitutions
syn region	vimSubst	oneline	 start=":\=s/.\{-}" skip="\(\\\\\)*\\/" end="/"	contains=vimPatSep,vimSubstTwoBS,vimSubstRange,vimNotation nextgroup=vimSubstEnd
syn region	vimSubstEnd	contained oneline start="."lc=1	   skip="\(\\\\\)*\\/" end="/"	contains=vimSubstPat,vimSubstTwoBS,vimNotation
syn region	vimSubst	oneline	 start=":\=s?.\{-}" skip="\(\\\\\)*\\?" end="?"	contains=vimPatSep,vimSubstTwoBS,vimSubstRange,vimNotation nextgroup=vimSubstEnd
syn region	vimSubstEnd	contained oneline start="."lc=1	   skip="\(\\\\\)*\\?" end="?"	contains=vimSubstPat,vimSubstTwoBS,vimNotation
syn region	vimSubstRange	contained oneline start="\["	   skip="\(\\\\\)*\\]" end="]"
syn match	vimSubstPat	contained	"\\\d"
syn match	vimSubstTwoBS	contained	"\\\\"

" Marks, Registers, Addresses, Filters
syn match	vimMark	"[!,:]'[a-zA-Z0-9]"lc=1
syn match	vimMark	"'[a-zA-Z0-9][,!]"me=e-1
syn match	vimMark	"'[<>][,!]"me=e-1
syn match	vimMark	"[^a-zA-Z]norm\s'[a-zA-Z0-9]"lc=6
syn match	vimMark	"[^a-zA-Z]normal\s'[a-zA-Z0-9]"lc=8
syn match	vimPlainMark	"'[a-zA-Z0-9]"		contained

syn match	vimRegister	'[^(,;]"[a-zA-Z0-9\-:.%#*=][^a-zA-Z_"]'lc=1
syn match	vimRegister	'[^a-zA-Z]norm\s"[a-zA-Z0-9]'lc=6
syn match	vimRegister	'[^a-zA-Z]normal\s"[a-zA-Z0-9]'lc=8
syn match	vimPlainRegister	'"[a-zA-Z0-9\-:.%#*=]'	contained

syn match	vimAddress	",\."lc=1
syn match   	vimAddress	"[:|][%.]"lc=1
syn match   	vimAddress	"<Bar>%"lc=5

syn match	vimFilter	":\s*!"ms=e
syn match	vimFilter	"[^a-zA-Z]!"lc=1

" Complex repeats (:h complex-repeat)
syn match vimCmplxRepeat	'[^a-zA-Z_/\\]q[0-9a-zA-Z"]'lc=1
syn match vimCmplxRepeat	'@[0-9a-z".=@:]'

" High Priority Comments: beginning of line with optional spaces then "
syn match vimComment	+^\s*".*$+	contains=vimTodo

" Set command and associated set-options (vimOptions) with comment
syn region vimSet		matchgroup=vimCommand start="\s*set\>" end="|\|$"	keepend contains=vimSetEqual,vimOption,vimErrSetting,vimComment,vimSetString
syn region vimSetEqual	contained	start="="	skip="\(\\\\\)*\\\s" end="[| \t]\|$"me=e-1 contains=vimCtrlChar
syn region vimSetString	contained	start=+="+hs=s+1	skip=+\(\\\\\)\\"+   end=+"+	contains=vimCtrlChar

" Autocmd
syn cluster vimAutoCmdCluster	contains=vimAutoCmdOpt,vimAutoEvent,vimComment,vimFilter,vimAuHighlight,vimIsCommand,vimIsCommand2,vimNotation,vimSet,vimSpecFile,vimString,vimAuSyntax
syn match   vimIsCommand2	contained transparent "[ \t<Bar>][a-zA-Z_]\+"lc=1  contains=vimCommand
syn keyword vimAutoCmdOpt	contained	nested
syn region  vimAutoCmd		matchgroup=vimAutoSet start=+^\s*\(autocmd\|au\|doautocmd\|doau\)\>+	end="$" keepend contains=@vimAutoCmdCluster
syn region  vimAutoCmd		matchgroup=vimAutoSet start=+^\s*\(doautoa\|doautoall\)\>+		end="$" keepend contains=@vimAutoCmdCluster

" Syntax
"=======
syn match   vimGroupList	contained	"@\=[^ \t,]*"	contains=vimGroupSpecial,vimPatSep
syn match   vimGroupList	contained	"@\=[^ \t,]*,"	nextgroup=vimGroupList contains=vimGroupSpecial,vimPatSep
syn keyword vimGroupSpecial	contained	ALL	ALLBUT
syn match   vimSynError	contained	"\i\+"
syn match   vimSynError	contained	"\i\+="	nextgroup=vimGroupList
syn match   vimSynContains	contained	"contains="	nextgroup=vimGroupList
syn match   vimSynNextgroup	contained	"nextgroup="	nextgroup=vimGroupList

syn match   vimSyntax	"^\s*\(syntax\|synta\|synt\|syn\|sy\)"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment
syn match   vimSyntax	"[:|]\s*\(syntax\|synta\|synt\|syn\|sy\)"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment
syn match   vimAuSyntax	contained	"\s+\(syntax\|synta\|synt\|syn\|sy\)"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment

" Syntax: case
syn keyword vimSynType		contained	case	skipwhite nextgroup=vimSynCase,vimSynCaseError
syn match   vimSynCaseError	contained	"\i\+"
syn keyword vimSynCase		contained	ignore	match

" Syntax: clear
syn keyword vimSynType		contained	clear	skipwhite nextgroup=vimGroupList

" Syntax: cluster
syn keyword vimSynType		contained	cluster		skipwhite nextgroup=vimClusterName
syn region  vimClusterName	contained	matchgroup=vimGroupName start="\k\+" skip="\(\\\\\)*\\|" end="$\||" contains=vimGroupAdd,vimGroupRem,vimSynContains,vimSynError
syn match   vimGroupAdd		contained	"add="		nextgroup=vimGroupList
syn match   vimGroupRem		contained	"remove="	nextgroup=vimGroupList

" Syntax: include
syn keyword vimSynType		contained	include		skipwhite nextgroup=vimGroupList

" Syntax: keyword
syn keyword vimSynType		contained	keyword		skipwhite nextgroup=vimSynKeyRegion
syn region  vimSynKeyRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\(\\\\\)*\\|" end="$\||" contains=vimSynNextgroup,vimSynKeyOpt
syn match   vimSynKeyOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: match
syn keyword vimSynType		contained	match	skipwhite nextgroup=vimSynMatchRegion
syn region  vimSynMatchRegion	contained oneline keepend matchgroup=vimGroupName start="\k\+" end="$" contains=vimComment,vimSynContains,vimSynError,vimSynKeyOpt,vimSynNextgroup,vimSynRegPat

" Syntax: off
syn keyword vimSynType		contained	off

" Syntax: region
syn keyword vimSynType		contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\(\\\\\)*\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\|oneline\|keepend\)\>"
syn match   vimSynReg		contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="
syn region  vimSynRegPat	contained oneline	start="!" skip="\(\\\\\)*\\!" end="!" contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="%" skip="\(\\\\\)*\\-" end="%" contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="'" skip="\(\\\\\)*\\'" end="'" contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="+" skip="\(\\\\\)*\\+" end="+" contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="@" skip="\(\\\\\)*\\@" end="@" contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start='"' skip='\(\\\\\)*\\"' end='"' contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start='/' skip='\(\\\\\)*\\/' end='/' contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start=',' skip='\(\\\\\)*\\,' end=',' contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start='\$' skip='\(\\\\\)*\\\$' end='\$' contains=vimPatSep,vimNotPatSep,vimSynPatRange nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+][0-9]\+\)\="
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+][0-9]\+\)\=," nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"lc=[0-9]"
syn match   vimSynPatMod	contained	"lc=[0-9]," nextgroup=vimSynPatMod
syn region  vimSynPatRange	contained oneline start="\["	skip="\(\\\\\)*\\]"   end="]"

" Syntax: sync
syn keyword vimSynType		contained	sync	skipwhite nextgroup=vimSyncC,vimSyncLines,vimSyncMatch,vimSyncError,vimSyncLinecont
syn match   vimSyncError	contained	"\i\+"
syn keyword vimSyncC	contained	ccomment	clear
syn keyword vimSyncMatch	contained	match	skipwhite nextgroup=vimSyncGroupName
syn keyword vimSyncLinecont	contained	linecont	skipwhite nextgroup=vimSynRegPat
syn match   vimSyncLines	contained	"\(minlines\|maxlines\|lines\)=" nextgroup=vimNumber
syn match   vimSyncGroupName	contained	"\k\+" skipwhite nextgroup=vimSyncKey
syn match   vimSyncKey		contained	"\<groupthere\|grouphere\>"	skipwhite nextgroup=vimSyncGroup
syn match   vimSyncGroup	contained	"\k\+" skipwhite nextgroup=vimSynRegPat,vimSyncNone
syn keyword vimSyncNone		contained	NONE

" Additional IsCommand stuff, here by reasons of precedence
" ====================
syn match vimIsCommand	"<Bar>\s*[a-zA-Z]\+"	transparent contains=vimCommand,vimNotation

" Highlighting
" ============
syn match   vimHighlight	"^\s*\(highlight\|highligh\|highlig\|highli\|highl\|high\|hig\|hi\)"	skipwhite nextgroup=vimHiLink,vimHiClear,vimHiKeyList,vimComment
syn match   vimHighlight	"[:|]\s*\(highlight\|highligh\|highlig\|highli\|highl\|high\|hig\|hi\)"	skipwhite nextgroup=vimHiLink,vimHiClear,vimHiKeyList,vimComment
syn match   vimAuHighlight	contained	"\s+\(highlight\|highligh\|highlig\|highli\|highl\|high\|hig\|hi\)"	skipwhite nextgroup=vimHiLink,vimHiClear,vimHiKeyList,vimComment

syn match   vimHiGroup	contained	"\i\+"
syn keyword vimHiAttrib	contained	NONE bold inverse italic reverse standout underline
syn match   vimHiAttribList	contained	"\i\+"	contains=vimHiAttrib
syn match   vimHiAttribList	contained	"\i\+,"he=e-1	contains=vimHiAttrib nextgroup=vimHiAttribList,vimHiAttrib
syn case ignore
syn keyword vimHiCtermColor	contained	black	darkcyan	darkred	lightcyan	lightred
syn keyword vimHiCtermColor	contained	blue	darkgray	gray	lightgray	magenta
syn keyword vimHiCtermColor	contained	brown	darkgreen	green	lightgreen	red
syn keyword vimHiCtermColor	contained	cyan	darkgrey	grey	lightgrey	white
syn keyword vimHiCtermColor	contained	darkBlue	darkmagenta	lightblue	lightmagenta	yellow
syn case match
syn match   vimHiFontname	contained	"[a-zA-z\-*]\+"
syn match   vimHiGuiFontname	contained	"'[a-zA-z\-* ]\+'"
syn match   vimHiGuiRgb	contained	"#\x\{6}"
syn match   vimHiCtermError	contained	"[^0-9]\i*"

" Highlighting: hi group key=arg ...
syn region vimHiKeyList	contained oneline start="\i\+" skip="\(\\\\\)*\\|" end="$\||"	contains=vimHiGroup,vimHiTerm,vimHiCTerm,vimHiStartStop,vimHiCtermFgBg,vimHiGui,vimHiGuiFont,vimHiGuiFgBg,vimHiKeyError
syn match  vimHiKeyError	contained	"\i\+="he=e-1
syn match  vimHiTerm	contained	"term="he=e-1		nextgroup=vimHiAttribList
syn match  vimHiStartStop	contained	"\(start\|stop\)="he=e-1	nextgroup=vimHiTermcap,vimOption
syn match  vimHiCTerm	contained	"cterm="he=e-1		nextgroup=vimHiAttribList
syn match  vimHiCtermFgBg	contained	"cterm[fb]g="he=e-1	nextgroup=vimNumber,vimHiCtermColor,vimHiCtermError
syn match  vimHiGui	contained	"gui="he=e-1		nextgroup=vimHiAttribList
syn match  vimHiGuiFont	contained	"font="he=e-1		nextgroup=vimHiFontname
syn match  vimHiGuiFgBg	contained	"gui[fb]g="he=e-1	nextgroup=vimHiGroup,vimHiGuiFontname,vimHiGuiRgb
syn match  vimHiTermcap	contained	"\S\+"		contains=vimNotation

" Highlight: clear
syn keyword vimHiClear	contained	clear		nextgroup=vimHiGroup

" Highlight: link
syn region vimHiLink	contained oneline matchgroup=vimCommand start="link" end="$"	contains=vimHiGroup,vimGroup

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation	"<\([scam]-\)\{0,4}\(f[0-9]\{1,2}\|\S\|cr\|lf\|linefeed\|return\|del\(ete\)\ =\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k \=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation	"<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\|drag\|release\)>" contains=vimBracket
syn match vimNotation	"<\(nul\|bar\|bslash\|lt\)>"	contains=vimBracket
syn match vimNotation	'<C-R>[0-9a-z"%#:.\-=]'he=e-1	contains=vimBracket
syn match vimBracket contained	"[<>]"
syn match vimBracket contained	"[<>]"
syn case match

" Control Characters
" ==================
syn match vimCtrlChar	"[--]"

" Highlighting Settings
" ====================
if !exists("did_vim_syntax_inits")
  let did_vim_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later
  hi link vimAuHighlight	vimHighlight

  hi link vimAddress	vimMark
  hi link vimAutoCmdOpt	vimOption
  hi link vimAutoSet	vimCommand
  hi link vimBehaveError	vimError
  hi link vimConditional	vimStatement
  hi link vimErrSetting	vimError
  hi link vimFilter	vimOper
  hi link vimFunctionError	vimError
  hi link vimGroupAdd	vimSynOption
  hi link vimGroupRem	vimSynOption
  hi link vimHLGroup	vimGroup
  hi link vimHiAttribList	vimError
  hi link vimHiCTerm	vimHiTerm
  hi link vimHiCtermError	vimError
  hi link vimHiCtermFgBg	vimHiTerm
  hi link vimHiGroup	vimGroupName
  hi link vimHiGui	vimHiTerm
  hi link vimHiGuiFgBg	vimHiTerm
  hi link vimHiGuiFont	vimHiTerm
  hi link vimHiGuiRgb	vimNumber
  hi link vimHiKeyError	vimError
  hi link vimHiStartStop	vimHiTerm
  hi link vimHighlight	vimCommand
  hi link vimNotPatSep	vimString
  hi link vimPlainMark	vimMark
  hi link vimPlainRegister	vimRegister
  hi link vimSetString	vimString
  hi link vimSpecFileMod	vimSpecFile
  hi link vimSynCaseError	vimError
  hi link vimSynContains	vimSynOption
  hi link vimSynKeyOpt	vimSynOption
  hi link vimSynMtchGrp	vimSynOption
  hi link vimSynNextgroup	vimSynOption
  hi link vimSynPatRange	vimString
  hi link vimSynRegOpt	vimSynOption
  hi link vimSynRegPat	vimString
  hi link vimSynType	vimSpecial
  hi link vimSyncGroup	vimGroupName
  hi link vimSyncGroupName	vimGroupName
  hi link vimUserAttrb	vimSpecial
  hi link vimUserAttrbCmplt	vimSpecial
  hi link vimUserAttrbKey	vimOption
  hi link vimUserCmd	vimCommand

  hi link vimAutoEvent	Type
  hi link vimBracket	Delimiter
  hi link vimCmplxRepeat	SpecialChar
  hi link vimCommand	Statement
  hi link vimComment	Comment
  hi link vimCtrlChar	SpecialChar
  hi link vimError	Error
  hi link vimFuncName	Function
  hi link vimFuncVar	Identifier
  hi link vimGroup	Type
  hi link vimGroupSpecial	Special
  hi link vimHLMod	PreProc
  hi link vimHiAttrib	PreProc
  hi link vimHiTerm	Type
  hi link vimKeyword	Statement
  hi link vimMark	Number
  hi link vimNotation	Special
  hi link vimNumber	Number
  hi link vimOper	Operator
  hi link vimOption	PreProc
  hi link vimPatSep	SpecialChar
  hi link vimPattern	Type
  hi link vimRegister	SpecialChar
  hi link vimSpecFile	Identifier
  hi link vimSpecial	Type
  hi link vimStatement	Statement
  hi link vimString	String
  hi link vimSubstPat	SpecialChar
  hi link vimSynCase	Type
  hi link vimSynCaseError	Error
  hi link vimSynError	Error
  hi link vimSynOption	Special
  hi link vimSynReg	Type
  hi link vimSyncC	Type
  hi link vimSyncError	Error
  hi link vimSyncKey	Type
  hi link vimSyncNone	Type
  hi link vimTodo	Todo
endif

let b:current_syntax = "vim"

" vim: ts=17
