" Vim syntax file
" Language:	Vim 5.5 script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	October 04, 1999
" Version:	5.5-03

" Remove old syntax
syn clear

syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO

" regular vim commands
syn keyword vimCommand contained	N[ext]	col[der]	is[earch]	perld[o]	star[tinsert]
syn keyword vimCommand contained	P[rint]	comc[lear]	isp[lit]	po[p]	stj[ump]
syn keyword vimCommand contained	X	con[tinue]	iu[nmap]	pp[op]	sts[elect]
syn keyword vimCommand contained	a[ppend]	conf[irm]	iuna[bbrev]	pre[serve]	sun[hide]
syn keyword vimCommand contained	ab[breviate]	cp[revious]	iunme	prev[ious]	sus[pend]
syn keyword vimCommand contained	abc[lear]	cq[uit]	iunmenu	promptf[ind]	sv[iew]
syn keyword vimCommand contained	al[l]	cr[ewind]	j[oin]	promptr[epl]	sy[ntax]
syn keyword vimCommand contained	am	cu[nmap]	ju[mps]	ptN[ext]	syncbind
syn keyword vimCommand contained	amenu	cuna[bbrev]	k	pta[g]	syntax
syn keyword vimCommand contained	an	cunme	l[ist]	ptj[ump]	t
syn keyword vimCommand contained	anoremenu	cunmenu	la[st]	ptl[ast]	tN[ext]
syn keyword vimCommand contained	ar[gs]	d[elete]	le[ft]	ptn[ext]	ta[g]
syn keyword vimCommand contained	argu[ment]	delc[ommand]	ls	ptp[revious]	tags
syn keyword vimCommand contained	as[cii]	delf[unction]	m[ove]	ptr[ewind]	tc[l]
syn keyword vimCommand contained	au[tocmd]	di[splay]	ma[rk]	pts[elect]	tcld[o]
syn keyword vimCommand contained	aug[roup]	dig[raphs]	mak[e]	pu[t]	tclf[ile]
syn keyword vimCommand contained	aun	display	map	pw[d]	te[aroff]
syn keyword vimCommand contained	aunmenu	dj[ump]	mapc[lear]	py[thon]	tj[ump]
syn keyword vimCommand contained	autocmd	dl[ist]	marks	pyf[ile]	tl[ast]
syn keyword vimCommand contained	bN[ext]	do[autocmd]	me	q[uit]	tm[enu]
syn keyword vimCommand contained	b[uffer]	doautoa[ll]	menu	qa[ll]	tn[ext]
syn keyword vimCommand contained	ba[ll]	ds[earch]	mk[exrc]	r[ead]	tp[revious]
syn keyword vimCommand contained	bad[d]	dsp[lit]	mks[ession]	rec[over]	tr[ewind]
syn keyword vimCommand contained	bd[elete]	e[dit]	mkv[imrc]	red[o]	ts[elect]
syn keyword vimCommand contained	be[have]	ec[ho]	mod[e]	redi[r]	tu[nmenu]
syn keyword vimCommand contained	bl[ast]	echoh[l]	n[ext]	reg[isters]	u[ndo]
syn keyword vimCommand contained	bm[odified]	echon	new	res[ize]	una[bbreviate]
syn keyword vimCommand contained	bn[ext]	el[se]	nm[ap]	ret[ab]	unh[ide]
syn keyword vimCommand contained	bp[revious]	eme	nmapc[lear]	retu[rn]	unl[et]
syn keyword vimCommand contained	br[ewind]	emenu	nme	rew[ind]	unm[ap]
syn keyword vimCommand contained	brea[k]	endf[unction]	nmenu	ri[ght]	unme
syn keyword vimCommand contained	bro[wse]	ex	nn[oremap]	rv[iminfo]	unmenu
syn keyword vimCommand contained	buffers	exe[cute]	nnoreme	sN[ext]	v[global]
syn keyword vimCommand contained	bun[load]	exi[t]	nnoremenu	s[ubstitute]	ve[rsion]
syn keyword vimCommand contained	cN[ext]	f[ile]	no[remap]	sa[rgument]	vi[sual]
syn keyword vimCommand contained	c[hange]	files	noh[lsearch]	sal[l]	vie[w]
syn keyword vimCommand contained	ca[bbrev]	fin[d]	norea[bbrev]	sbN[ext]	vm[ap]
syn keyword vimCommand contained	cabc[lear]	fix[del]	noreme	sb[uffer]	vmapc[lear]
syn keyword vimCommand contained	cal[l]	fu[nction]	noremenu	sba[ll]	vme
syn keyword vimCommand contained	cc	g[lobal]	norm[al]	sbl[ast]	vmenu
syn keyword vimCommand contained	cd	go[to]	nu[mber]	sbm[odified]	vn[oremap]
syn keyword vimCommand contained	ce[nter]	gr[ep]	nun[map]	sbn[ext]	vnoreme
syn keyword vimCommand contained	cf[ile]	h[elp]	nunme	sbp[revious]	vnoremenu
syn keyword vimCommand contained	chd[ir]	helpf[ind]	nunmenu	sbr[ewind]	vu[nmap]
syn keyword vimCommand contained	che[ckpath]	hi[ghlight]	o[pen]	se[t]	vunme
syn keyword vimCommand contained	cl[ist]	hid[e]	om[ap]	sf[ind]	vunmenu
syn keyword vimCommand contained	cla[st]	his[tory]	omapc[lear]	sh[ell]	wN[ext]
syn keyword vimCommand contained	clo[se]	i[nsert]	ome	si[malt]	w[rite]
syn keyword vimCommand contained	cm[ap]	ia[bbrev]	omenu	sl[eep]	wa[ll]
syn keyword vimCommand contained	cmapc[lear]	iabc[lear]	on[ly]	sla[st]	win[size]
syn keyword vimCommand contained	cme	ij[ump]	ono[remap]	sm[agic]	winp[os]
syn keyword vimCommand contained	cmenu	il[ist]	onoreme	sn[ext]	winsize
syn keyword vimCommand contained	cn[ext]	im[ap]	onoremenu	sni[ff]	wn[ext]
syn keyword vimCommand contained	cnew[er]	imapc[lear]	opt[ions]	sno[magic]	wp[revous]
syn keyword vimCommand contained	cnf[ile]	ime	ou[nmap]	so[urce]	wq
syn keyword vimCommand contained	cno[remap]	imenu	ounme	sp[lit]	wqa[ll]
syn keyword vimCommand contained	cnorea[bbrev]	ino[remap]	ounmenu	spr[evious]	wv[iminfo]
syn keyword vimCommand contained	cnoreme	inorea[bbrev]	p[rint]	sr[ewind]	x[it]
syn keyword vimCommand contained	cnoremenu	inoreme	pc[lose]	st[op]	xa[ll]
syn keyword vimCommand contained	co[py]	inoremenu	pe[rl]	sta[g]	y[ank]
syn match   vimCommand contained	"z[-+^.=]"

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep	"[:|]\+"	skipwhite nextgroup=vimIsCommand,vimAddress,vimAutoCmd,vimMark,vimFilter,vimUserCmd,vimSet,vimLet
syn match vimIsCommand	"\<\a\+\>"	contains=vimCommand,vimConditional,vimCondError

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	ai	equalalways	km	scb	tagstack
syn keyword vimOption contained	akm	equalprg	kp	scr	tb
syn keyword vimOption contained	al	errorbells	langmap	scroll	tbi
syn keyword vimOption contained	aleph	errorfile	laststatus	scrollbind	tbs
syn keyword vimOption contained	allowrevins	errorformat	lazyredraw	scrolljump	term
syn keyword vimOption contained	altkeymap	esckeys	lbr	scrolloff	terse
syn keyword vimOption contained	ari	et	lcs	scrollopt	textauto
syn keyword vimOption contained	autoindent	eventignore	linebreak	scs	textmode
syn keyword vimOption contained	autowrite	ex	lines	sect	textwidth
syn keyword vimOption contained	aw	expandtab	lisp	sections	tf
syn keyword vimOption contained	background	exrc	list	secure	tgst
syn keyword vimOption contained	backspace	fe	listchars	sel	tildeop
syn keyword vimOption contained	backup	ff	lmap	selection	timeout
syn keyword vimOption contained	backupdir	ffs	ls	selectmode	timeoutlen
syn keyword vimOption contained	backupext	fileencoding	lz	sessionoptions	title
syn keyword vimOption contained	bdir	fileformat	magic	sft	titlelen
syn keyword vimOption contained	bex	fileformats	makeef	sh	titleold
syn keyword vimOption contained	bg	filetype	makeprg	shcf	titlestring
syn keyword vimOption contained	bin	fk	mat	shell	tl
syn keyword vimOption contained	binary	fkmap	matchpairs	shellcmdflag	tm
syn keyword vimOption contained	biosk	fo	matchtime	shellpipe	to
syn keyword vimOption contained	bioskey	formatoptions	maxfuncdepth	shellquote	toolbar
syn keyword vimOption contained	bk	formatprg	maxmapdepth	shellredir	top
syn keyword vimOption contained	breakat	fp	maxmem	shellslash	tr
syn keyword vimOption contained	brk	ft	maxmemtot	shelltype	ts
syn keyword vimOption contained	browsedir	gcr	mef	shellxquote	tsl
syn keyword vimOption contained	bs	gd	mfd	shiftround	ttimeout
syn keyword vimOption contained	bsdir	gdefault	mh	shiftwidth	ttimeoutlen
syn keyword vimOption contained	cb	gfm	ml	shm	ttm
syn keyword vimOption contained	cf	gfn	mls	shortmess	ttybuiltin
syn keyword vimOption contained	ch	gfs	mm	shortname	ttyfast
syn keyword vimOption contained	cin	ghr	mmd	showbreak	ttym
syn keyword vimOption contained	cindent	go	mmt	showcmd	ttymouse
syn keyword vimOption contained	cink	gp	mod	showfulltag	ttyscroll
syn keyword vimOption contained	cinkeys	grepformat	modeline	showmatch	ttytype
syn keyword vimOption contained	cino	grepprg	modelines	showmode	tw
syn keyword vimOption contained	cinoptions	guicursor	modified	shq	tx
syn keyword vimOption contained	cinw	guifont	more	si	uc
syn keyword vimOption contained	cinwords	guifontset	mouse	sidescroll	ul
syn keyword vimOption contained	clipboard	guiheadroom	mousef	sj	undolevels
syn keyword vimOption contained	cmdheight	guioptions	mousefocus	slm	updatecount
syn keyword vimOption contained	co	guipty	mousehide	sm	updatetime
syn keyword vimOption contained	columns	helpfile	mousem	smartcase	ut
syn keyword vimOption contained	com	helpheight	mousemodel	smartindent	vb
syn keyword vimOption contained	comments	hf	mouset	smarttab	vbs
syn keyword vimOption contained	compatible	hh	mousetime	smd	verbose
syn keyword vimOption contained	complete	hi	mp	sn	vi
syn keyword vimOption contained	confirm	hid	mps	so	viminfo
syn keyword vimOption contained	consk	hidden	nf	softtabstop	visualbell
syn keyword vimOption contained	conskey	highlight	nrformats	sol	wa
syn keyword vimOption contained	cp	history	nu	sp	wak
syn keyword vimOption contained	cpo	hk	number	splitbelow	warn
syn keyword vimOption contained	cpoptions	hkmap	oft	sr	wb
syn keyword vimOption contained	cpt	hkmapp	osfiletype	srr	wc
syn keyword vimOption contained	cscopeprg	hkp	pa	ss	wcm
syn keyword vimOption contained	cscopetag	hl	para	ssl	wd
syn keyword vimOption contained	cscopetagorder	hls	paragraphs	ssop	weirdinvert
syn keyword vimOption contained	cscopeverbose	hlsearch	paste	st	wh
syn keyword vimOption contained	csprg	ic	pastetoggle	sta	whichwrap
syn keyword vimOption contained	cst	icon	patchmode	startofline	wig
syn keyword vimOption contained	csto	iconstring	path	statusline	wildchar
syn keyword vimOption contained	csverb	ignorecase	pm	stl	wildcharm
syn keyword vimOption contained	def	im	previewheight	sts	wildignore
syn keyword vimOption contained	define	inc	pt	su	wildmenu
syn keyword vimOption contained	dg	include	pvh	suffixes	wildmode
syn keyword vimOption contained	dict	incsearch	readonly	sw	wim
syn keyword vimOption contained	dictionary	inf	remap	swapfile	winaltkeys
syn keyword vimOption contained	digraph	infercase	report	swapsync	winheight
syn keyword vimOption contained	dir	insertmode	restorescreen	swb	winminheight
syn keyword vimOption contained	directory	is	revins	swf	wiv
syn keyword vimOption contained	display	isf	ri	switchbuf	wm
syn keyword vimOption contained	dy	isfname	rightleft	sws	wmh
syn keyword vimOption contained	ea	isi	rl	sxq	wmnu
syn keyword vimOption contained	eb	isident	ro	syn	wrap
syn keyword vimOption contained	ed	isk	rs	syntax	wrapmargin
syn keyword vimOption contained	edcompatible	iskeyword	ru	ta	wrapscan
syn keyword vimOption contained	ef	isp	ruf	tabstop	write
syn keyword vimOption contained	efm	isprint	ruler	tag	writeany
syn keyword vimOption contained	ei	joinspaces	rulerformat	tagbsearch	writebackup
syn keyword vimOption contained	ek	js	sb	taglength	writedelay
syn keyword vimOption contained	endofline	key	sbo	tagrelative	ws
syn keyword vimOption contained	eol	keymodel	sbr	tags	ww
syn keyword vimOption contained	ep	keywordprg	sc

" These are the turn-off setting variants
syn keyword vimOption contained	noai	noedcompatible	noinsertmode	nors	notagstack
syn keyword vimOption contained	noakm	noek	nois	noru	notbi
syn keyword vimOption contained	noallowrevins	noendofline	nojoinspaces	noruler	notbs
syn keyword vimOption contained	noaltkeymap	noeol	nojs	nosb	noterse
syn keyword vimOption contained	noari	noequalalways	nolazyredraw	nosc	notextauto
syn keyword vimOption contained	noautoindent	noerrorbells	nolbr	noscs	notextmode
syn keyword vimOption contained	noautowrite	noesckeys	nolinebreak	nosecure	notf
syn keyword vimOption contained	noaw	noet	nolisp	noshellslash	notgst
syn keyword vimOption contained	nobackup	noex	nolist	noshiftround	notildeop
syn keyword vimOption contained	nobin	noexpandtab	nolz	noshortname	notimeout
syn keyword vimOption contained	nobinary	noexrc	nomagic	noshowcmd	notitle
syn keyword vimOption contained	nobiosk	nofk	nomh	noshowmatch	noto
syn keyword vimOption contained	nobioskey	nofkmap	noml	noshowmode	notop
syn keyword vimOption contained	nobk	nogd	nomod	nosi	notr
syn keyword vimOption contained	nocf	nogdefault	nomodeline	nosm	nottimeout
syn keyword vimOption contained	nocin	noguipty	nomodified	nosmartcase	nottybuiltin
syn keyword vimOption contained	nocindent	nohid	nomore	nosmartindent	nottyfast
syn keyword vimOption contained	nocompatible	nohidden	nomousef	nosmarttab	notx
syn keyword vimOption contained	noconfirm	nohk	nomousefocus	nosmd	novb
syn keyword vimOption contained	noconsk	nohkmap	nomousehide	nosn	novisualbell
syn keyword vimOption contained	noconskey	nohkmapp	nonu	nosol	nowa
syn keyword vimOption contained	nocp	nohkp	nonumber	nosplitbelow	nowarn
syn keyword vimOption contained	nocscopetag	nohls	nopaste	nosr	nowb
syn keyword vimOption contained	nocscopeverbose	nohlsearch	noreadonly	nossl	noweirdinvert
syn keyword vimOption contained	nocst	noic	noremap	nosta	nowiv
syn keyword vimOption contained	nocsverb	noicon	norestorescreen	nostartofline	nowrap
syn keyword vimOption contained	nodg	noignorecase	norevins	noswapfile	nowrapscan
syn keyword vimOption contained	nodigraph	noim	nori	noswf	nowrite
syn keyword vimOption contained	noea	noincsearch	norightleft	nota	nowriteany
syn keyword vimOption contained	noeb	noinf	norl	notagbsearch	nowritebackup
syn keyword vimOption contained	noed	noinfercase	noro	notagrelative	nows

" termcap codes (which can also be set)
syn keyword vimOption contained	t_AB	t_IE	t_WS	t_k1	t_kd	t_op
syn keyword vimOption contained	t_AF	t_IS	t_ZH	t_k2	t_ke	t_se
syn keyword vimOption contained	t_AL	t_K1	t_ZR	t_k3	t_kh	t_so
syn keyword vimOption contained	t_CS	t_K3	t_al	t_k4	t_kl	t_sr
syn keyword vimOption contained	t_Co	t_K4	t_bc	t_k5	t_kr	t_te
syn keyword vimOption contained	t_DL	t_K5	t_cd	t_k6	t_ks	t_ti
syn keyword vimOption contained	t_F1	t_K6	t_ce	t_k7	t_ku	t_ts
syn keyword vimOption contained	t_F2	t_K7	t_cl	t_k8	t_le	t_ue
syn keyword vimOption contained	t_F3	t_K9	t_cm	t_k9	t_mb	t_us
syn keyword vimOption contained	t_F4	t_KA	t_cs	t_kD	t_md	t_vb
syn keyword vimOption contained	t_F5	t_RI	t_da	t_kI	t_me	t_ve
syn keyword vimOption contained	t_F6	t_RV	t_db	t_kN	t_mr	t_vi
syn keyword vimOption contained	t_F7	t_Sb	t_dl	t_kP	t_ms	t_vs
syn keyword vimOption contained	t_F8	t_Sf	t_fs	t_kb	t_nd	t_xs
syn keyword vimOption contained	t_F9	t_WP
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
syn case ignore
syn keyword vimAutoEvent contained	BufCreate	BufReadPost	FileAppendPre	FilterReadPre	Syntax
syn keyword vimAutoEvent contained	BufDelete	BufReadPre	FileChangedShell	FilterWritePost	TermChanged
syn keyword vimAutoEvent contained	BufEnter	BufUnload	FileReadPost	FilterWritePre	User
syn keyword vimAutoEvent contained	BufFilePost	BufWrite	FileReadPre	FocusGained	VimEnter
syn keyword vimAutoEvent contained	BufFilePre	BufWritePost	FileType	FocusLost	VimLeave
syn keyword vimAutoEvent contained	BufHidden	BufWritePre	FileWritePost	GUIEnter	VimLeavePre
syn keyword vimAutoEvent contained	BufLeave	CursorHold	FileWritePre	StdinReadPost	WinEnter
syn keyword vimAutoEvent contained	BufNewFile	FileAppendPost	FilterReadPost	StdinReadPre	WinLeave
syn keyword vimAutoEvent contained	BufRead

" Highlight commonly used Groupnames
syn keyword vimGroup contained	Comment	Identifier	Keyword	Type	Delimiter
syn keyword vimGroup contained	Constant	Function	Exception	StorageClass	SpecialComment
syn keyword vimGroup contained	String	Statement	PreProc	Structure	Debug
syn keyword vimGroup contained	Character	Conditional	Include	Typedef	Ignore
syn keyword vimGroup contained	Number	Repeat	Define	Special	Error
syn keyword vimGroup contained	Boolean	Label	Macro	SpecialChar	Todo
syn keyword vimGroup contained	Float	Operator	PreCondit	Tag

" Default highlighting groups
syn keyword vimHLGroup contained	Cursor	Menu	Normal	SpecialKey	Visual
syn keyword vimHLGroup contained	Directory	ModeMsg	Question	StatusLine	VisualNOS
syn keyword vimHLGroup contained	ErrorMsg	MoreMsg	Scrollbar	StatusLineNC	WarningMsg
syn keyword vimHLGroup contained	IncSearch	NonText	Search	Title	WildMenu
syn keyword vimHLGroup contained	LineNr
syn case match

" Function Names
syn keyword vimFuncName contained	append	delete	has	localtime	strtrans
syn keyword vimFuncName contained	argc	did_filetype	histadd	maparg	substitute
syn keyword vimFuncName contained	argv	escape	histdel	mapcheck	synID
syn keyword vimFuncName contained	browse	exists	histget	match	synIDattr
syn keyword vimFuncName contained	bufexists	expand	histnr	matchend	synIDtrans
syn keyword vimFuncName contained	bufloaded	filereadable	hlID	matchstr	system
syn keyword vimFuncName contained	bufname	fnamemodify	hlexists	nr2char	tempname
syn keyword vimFuncName contained	bufnr	getcwd	hostname	rename	virtcol
syn keyword vimFuncName contained	bufwinnr	getftime	input	setline	visualmode
syn keyword vimFuncName contained	byte2line	getline	isdirectory	strftime	winbufnr
syn keyword vimFuncName contained	char2nr	getwinposx	libcall	strlen	winheight
syn keyword vimFuncName contained	col	getwinposy	line	strpart	winnr
syn keyword vimFuncName contained	confirm	glob	line2byte
syn match   vimFunc     "\I\i*\s*("	contains=vimFuncName

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" Behave!
" =======
syn match   vimBehave	"\<\(behave\|behav\|beha\|beh\|be\)" contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained	mswin	xterm
syn match   vimBehaveError contained	"[^ ]\+"

" Filetypes
" =========
syn match   vimFiletype	"filet\(ype\|yp\|y\)\=\s\+\I\i*"	skipwhite contains=vimFTCmd,vimFTOption,vimFTError
syn match   vimFTError  contained	"\I\i*"
syn keyword vimFTCmd    contained	filet[ype]
syn keyword vimFTOption contained	on	off

" Functions
" =========
syn cluster vimFuncList	contains=vimCommand
syn cluster vimFuncBodyList	contains=vimIsCommand,vimFunction,vimFunctionError,vimFuncBody,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimIsCommand,vimCtrlChar,vimFuncVar
syn match   vimFunction	"\<\(function\|functio\|functi\|funct\|func\|fun\|fu\)!\=\s\+\u\w*("me=e-1	contains=@vimFuncList nextgroup=vimFuncBody
syn match   vimFunctionError	"\<\(function\|functio\|functi\|funct\|func\|fun\|fu\)!\=\s\+\U.\{-}("me=e-1	contains=vimCommand   nextgroup=vimFuncBody
syn region  vimFuncBody contained	start=")"	end="\<endf"	contains=@vimFuncBodyList
syn match   vimFuncVar  contained	"a:\(\I\i*\|\d\+\)"

syn keyword vimPattern  contained	start	skip	end

" Special Filenames, Modifiers, Extension Removal
syn match vimSpecFile	"<c\(word\|WORD\)>"	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"<\([acs]file\|amatch\|abuf\)>"	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"\s%[ \t:]"ms=s+1,me=e-1	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"\s%$"ms=s+1		nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"\s%<"ms=s+1,me=e-1	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"#\d\+\|[#%]<\>"		nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFileMod	"\(:[phtre]\)\+"		contained

" Operators
syn match vimOper	"||\|&&\|!=\|>=\|<=\|=\~\|!\~\|>\|<\|+\|-\|=\|\." skipwhite nextgroup=vimString,vimSpecFile

" Conditionals
syn keyword vimElseif   contained	elsei[f]
syn match   vimCondError	"\<\(endwhile\|endwhil\|endwhi\|endif\|endwh\|endi\|endw\|end\|en\)\>"
syn cluster vimTop	contains=vimAddress,vimAutoCmd,vimBehave,vimCmplxRepeat,vimComment,vimConditional,vimContinue,vimCtrlChar,vimEcho,vimElseif,vimEnvvar,vimFiletype,vimFilter,vimFunc,vimFunction,vimFunctionError,vimHighlight,vimIsCommand,vimLineComment,vimMark,vimNotation,vimNumber,vimOper,vimRegister,vimLet,vimSet,vimSpecFile,vimString,vimSubst,vimSyntax,vimUserCmd
syn region  vimConditional	matchgroup=vimCondHL start="\<if\>"		matchgroup=vimCondHL end="\<\(endif\|endi\|end\|en\)\>"		contains=@vimTop
syn region  vimConditional	matchgroup=vimCondHL start="\<\(while\|whil\|whi\|wh\)\>"	matchgroup=vimCondHL end="\<\(endwhile\|endwhil\|endwhi\|endwh\|endw\)\>"	contains=@vimTop

" User-Specified Commands
syn cluster vimUserCmdList	contains=vimAddress,vimSyntax,vimHighlight,vimAutoCmd,vimCmplxRepeat,vimComment,vimCtrlChar,vimEscapeBrace,vimFilter,vimFunc,vimFunction,vimIsCommand,vimIsCommand,vimMark,vimNotation,vimNumber,vimOper,vimRegion,vimRegister,vimLet,vimSet,vimSetEqual,vimSetString,vimSpecFile,vimString,vimSubst,vimSubstEnd,vimSubstRange,vimSynLine
syn region  vimUserCmd	oneline matchgroup=vimStatement start="\<\(command\|comman\|comma\|comm\|com\)" end="$"	contains=vimUserAttrb,@vimUserCmdList
syn match   vimUserAttrb 	contained	"-nargs=[01*?+]"		contains=vimUserAttrbKey,vimOper
syn match   vimUserAttrb 	contained	"-complete=\(augroup\|buffer\|command\|dir\|event\|file\|help\|highlight\|menu\|option\|tag\|var\)"	contains=vimUserAttrbKey,vimUserAttrbCmplt,vimOper
syn match   vimUserAttrb 	contained	"-range\(=%\|=\d\+\)\="	contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-count=\d\+"		contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-bang\|-register"	contains=vimOper,vimUserAttrbKey
syn keyword vimUserAttrbKey	contained	bang	count	range
syn keyword vimUserAttrbKey	contained	complete	nargs	register
syn keyword vimUserAttrbCmplt	contained	augroup	dir	help	menu	tag
syn keyword vimUserAttrbCmplt	contained	buffer	event	highlight	option	var
syn keyword vimUserAttrbCmplt	contained	command	file

" Numbers
" =======
syn match vimNumber	"\<\d\+.\d\+"
syn match vimNumber	"\<\d\+L\="
syn match vimNumber	"-\d\+.\d\+"
syn match vimNumber	"-\d\+L\="
syn match vimNumber	"[[;:]\d\+"lc=1
syn match vimNumber	"0[xX]\x\+"
syn match vimNumber	"#\x\+"

" Lower Priority Comments: after some vim commands...
" =======================
syn match  vimComment	+\s"[^\-:.%#=*].*$+lc=1	contains=vimTodo,vimCommentString
syn match  vimComment	+\<endif\s\+".*$+lc=5	contains=vimTodo,vimCommentString
syn match  vimComment	+\<else\s\+".*$+lc=4	contains=vimTodo,vimCommentString
syn region vimCommentString	contained oneline start='\S\s\+"'ms=s+1	end='"'

" Environment Variables
" =====================
syn match vimEnvvar	"\$\I\i*"
syn match vimEnvvar	"\${\I\i*}"

" Try to catch strings, if nothing else matches (therefore it must precede the others!)
"  vmEscapeBrace handles ["]  []"] (ie. stays as string)
syn region	vimEscapeBrace	oneline contained transparent	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1
syn match	vimPatSep	contained	"\\[|()]"
syn match	vimNotPatSep	contained	"\\\\"
syn region	vimString	oneline	start=+[^:a-zA-Z>!\\]"+lc=1 skip=+\\\\\|\\"+ end=+"+	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start=+[^:a-zA-Z>!\\]'+lc=1 skip=+\\\\\|\\'+ end=+'+	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start=+=!+lc=1	skip=+\\\\\|\\!+ end=+!+		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start="=+"lc=1	skip="\\\\\|\\+" end="+"		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start="[^\\]+\s*[^a-zA-Z0-9.]"lc=1 skip="\\\\\|\\+" end="+"	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	oneline	start="\s/\s*\A"lc=1 skip="\\\\\|\\+" end="/"		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn match        vimString	contained	+"[^"]*\\$+	skipnl nextgroup=vimStringCont
syn match	vimStringCont	contained	+\(\\\\\|.\)\{-}[^\\]"+

" Substitutions
" =============
syn cluster	vimSubstList	contains=vimPatSep,vimSubstTwoBS,vimSubstRange,vimNotation
syn cluster	vimSubstEndList	contains=vimSubstPat,vimSubstTwoBS,vimNotation
syn region	vimSubst	oneline	 start=":\=s/.\{-}" skip="\\\\\|\\/" end="/"	contains=@vimSubstList nextgroup=vimSubstEnd1
syn region	vimSubstEnd1	contained oneline start="."lc=1	   skip="\\\\\|\\/" end="/"	contains=@vimSubstEndList
syn region	vimSubst	oneline	 start=":\=s?.\{-}" skip="\\\\\|\\?" end="?"	contains=@vimSubstList nextgroup=vimSubstEnd2
syn region	vimSubstEnd2	contained oneline start="."lc=1	   skip="\\\\\|\\?" end="?"	contains=@vimSubstEndList
syn region	vimSubst	oneline	 start=":\=s@.\{-}" skip="\\\\\|\\@" end="@"	contains=@vimSubstList nextgroup=vimSubstEnd3
syn region	vimSubstEnd3	contained oneline start="."lc=1	   skip="\\\\\|\\@" end="@"	contains=@vimSubstEndList
syn region	vimSubstRange	contained oneline start="\["	   skip="\\\\\|\\]" end="]"
syn match	vimSubstPat	contained	"\\\d"
syn match	vimSubstTwoBS	contained	"\\\\"

" Marks, Registers, Addresses, Filters
syn match  vimMark		"[!,:]'[a-zA-Z0-9]"lc=1
syn match  vimMark		"'[a-zA-Z0-9][,!]"me=e-1
syn match  vimMark		"'[<>][,!]"me=e-1
syn match  vimMark		"\<norm\s'[a-zA-Z0-9]"lc=5
syn match  vimMark		"\<normal\s'[a-zA-Z0-9]"lc=7
syn match  vimPlainMark	contained	"'[a-zA-Z0-9]"

syn match  vimRegister		'[^(,;]"[a-zA-Z0-9\-:.%#*=][^a-zA-Z_"]'lc=1
syn match  vimRegister		'\<norm\s\+"[a-zA-Z0-9]'lc=5
syn match  vimRegister		'\<normal\s\+"[a-zA-Z0-9]'lc=7
syn match  vimPlainRegister	contained	'"[a-zA-Z0-9\-:.%#*=]'

syn match  vimAddress		",\."lc=1
syn match  vimAddress		"[%.]"

syn match  vimFilter	contained	"!"

" Complex repeats (:h complex-repeat)
syn match  vimCmplxRepeat		'[^a-zA-Z_/\\]q[0-9a-zA-Z"]'lc=1
syn match  vimCmplxRepeat		'@[0-9a-z".=@:]'

" Set command and associated set-options (vimOptions) with comment
syn region vimSet	matchgroup=vimCommand start="\<set\>" end="|"me=e-1 end="$" matchgroup=vimNotation end="<CR>" keepend contains=vimSetEqual,vimOption,vimErrSetting,vimComment,vimSetString
syn region vimSetEqual	contained	start="="	skip="\\\\\|\\\s" end="[| \t]\|$"me=e-1 contains=vimCtrlChar,vimSetSep,vimNotation
syn region vimSetString	contained	start=+="+hs=s+1	skip=+\\\\\|\\"+  end=+"+	contains=vimCtrlChar
syn match  vimSetSep	contained	"[,:]"

" Let
" ===
syn keyword vimLet		let	skipwhite nextgroup=vimLetVar
syn match   vimLetVar	contained	"\I\i*"

" Autocmd
" =======
syn match   vimAutoEventList	contained	"\(\a\+,\)*\a\+"	contains=vimAutoEvent nextgroup=vimAutoCmdSpace
syn match   vimAutoCmdSpace	contained	"\s\+"	nextgroup=vimAutoCmdSfxList
syn match   vimAutoCmdSfxList	contained	"\S*"
syn keyword vimAutoCmd		au[tocmd] do[autocmd]	skipwhite nextgroup=vimAutoEventList

" Echo -- prefer strings!
syn region  vimEcho	oneline	start="\<\(echo\|ech\|ec\)\>"   skip="\(\\\\\)*\\|" end="$\||" contains=vimCommand,vimString,vimOper

" Syntax
"=======
syn match   vimGroupList	contained	"@\=[^ \t,]*"	contains=vimGroupSpecial,vimPatSep
syn match   vimGroupList	contained	"@\=[^ \t,]*,"	nextgroup=vimGroupList contains=vimGroupSpecial,vimPatSep
syn keyword vimGroupSpecial	contained	ALL	ALLBUT
syn match   vimSynError	contained	"\i\+"
syn match   vimSynError	contained	"\i\+="	nextgroup=vimGroupList
syn match   vimSynContains	contained	"contains="	nextgroup=vimGroupList
syn match   vimSynNextgroup	contained	"nextgroup="	nextgroup=vimGroupList

syn match   vimSyntax	"\<\(syntax\|synta\|synt\|syn\|sy\)\>"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment
syn match   vimAuSyntax	contained	"\s+\(syntax\|synta\|synt\|syn\|sy\)"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment

" Syntax: case
syn keyword vimSynType		contained	case	skipwhite nextgroup=vimSynCase,vimSynCaseError
syn match   vimSynCaseError	contained	"\i\+"
syn keyword vimSynCase		contained	ignore	match

" Syntax: clear
syn keyword vimSynType	contained	clear	skipwhite nextgroup=vimGroupList

" Syntax: cluster
syn keyword vimSynType		contained	cluster		skipwhite nextgroup=vimClusterName
syn region  vimClusterName	contained	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimGroupAdd,vimGroupRem,vimSynContains,vimSynError
syn match   vimGroupAdd	contained	"add="		nextgroup=vimGroupList
syn match   vimGroupRem	contained	"remove="	nextgroup=vimGroupList

" Syntax: include
syn keyword vimSynType	contained	include		skipwhite nextgroup=vimGroupList

" Syntax: keyword
syn keyword vimSynType	contained	keyword		skipwhite nextgroup=vimSynKeyRegion
syn region  vimSynKeyRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynNextgroup,vimSynKeyOpt
syn match   vimSynKeyOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: match
syn keyword vimSynType	contained	match	skipwhite nextgroup=vimSynMatchRegion
syn region  vimSynMatchRegion	contained oneline matchgroup=vimGroupName start="\k\+" end="$" contains=vimComment,vimSynContains,vimSynError,vimSynKeyOpt,vimSynNextgroup,vimSynRegPat

" Syntax: off
syn keyword vimSynType	contained	off

" Syntax: region
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\|oneline\|keepend\)\>"
syn match   vimSynReg		contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="	nextgroup=vimGroup,vimHLGroup
syn region  vimSynRegPat	contained oneline	start="!"  skip="\\\\\|\\!"  end="!"  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="%"  skip="\\\\\|\\-"  end="%"  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="'"  skip="\\\\\|\\'"  end="'"  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="+"  skip="\\\\\|\\+"  end="+"  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start="@"  skip="\\\\\|\\@"  end="@"  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start='"'  skip='\\\\\|\\"'  end='"'  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start='/'  skip='\\\\\|\\/'  end='/'  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start=','  skip='\\\\\|\\,'  end=','  contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn region  vimSynRegPat	contained oneline	start='\$' skip='\\\\\|\\\$' end='\$' contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\="
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\=," nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"lc=\d\+"
syn match   vimSynPatMod	contained	"lc=\d\+," nextgroup=vimSynPatMod
syn region  vimSynPatRange	contained oneline start="\["	skip="\\\\\|\\]"   end="]"
syn match   vimSynNotPatRange	contained	"\\\\\|\\\["

" Syntax: sync
" ============
syn keyword vimSynType	contained	sync	skipwhite nextgroup=vimSyncC,vimSyncLines,vimSyncMatch,vimSyncError,vimSyncLinecont
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
syn match vimIsCommand	"<Bar>\s*\a\+"	transparent contains=vimCommand,vimNotation

" Highlighting
" ============
syn match   vimHighlight		"\<\(highlight\|highligh\|highlig\|highli\|highl\|high\|hig\|hi\)\>"	skipwhite nextgroup=vimHiLink,vimHiClear,vimHiKeyList,vimComment

syn match   vimHiGroup	contained	"\i\+"
syn case ignore
syn keyword vimHiAttrib	contained	none bold inverse italic reverse standout underline
syn keyword vimFgBgAttrib	contained	none bg background fg foreground
syn case match
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
syn cluster vimHiCluster contains=vimHiGroup,vimHiTerm,vimHiCTerm,vimHiStartStop,vimHiCtermFgBg,vimHiGui,vimHiGuiFont,vimHiGuiFgBg,vimHiKeyError
syn region vimHiKeyList	contained oneline start="\i\+" skip="\\\\\|\\|" end="$\||"	contains=@vimHiCluster
syn match  vimHiKeyError	contained	"\i\+="he=e-1
syn match  vimHiTerm	contained	"[tT][eE][rR][mM]="he=e-1			nextgroup=vimHiAttribList
syn match  vimHiStartStop	contained	"\([sS][tT][aA][rR][tT]\|[sS][tT][oO][pP]\)="he=e-1	nextgroup=vimHiTermcap,vimOption
syn match  vimHiCTerm	contained	"[cC][tT][eE][rR][mM]="he=e-1			nextgroup=vimHiAttribList
syn match  vimHiCtermFgBg	contained	"[cC][tT][eE][rR][mM][fFbB][gG]="he=e-1		nextgroup=vimNumber,vimHiCtermColor,vimFgBgAttrib,vimHiCtermError
syn match  vimHiGui	contained	"[gG][uU][iI]="he=e-1			nextgroup=vimHiAttribList
syn match  vimHiGuiFont	contained	"[fF][oO][nN][tT]="he=e-1			nextgroup=vimHiFontname
syn match  vimHiGuiFgBg	contained	"[gG][uU][iI][fFbB][gG]="he=e-1			nextgroup=vimHiGroup,vimHiGuiFontname,vimHiGuiRgb,vimFgBgAttrib
syn match  vimHiTermcap	contained	"\S\+"		contains=vimNotation

" Highlight: clear
syn keyword vimHiClear	contained	clear		nextgroup=vimHiGroup

" Highlight: link
syn region vimHiLink	contained oneline matchgroup=vimCommand start="link" end="$"	contains=vimHiGroup,vimGroup,vimHLGroup

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation	"<\([scam]-\)\{0,4}\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|del\(ete\)\ =\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k \=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation	"<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\|drag\|release\)>" contains=vimBracket
syn match vimNotation	"<\(bslash\|space\|bar\|nop\|nul\|lt\)>"		contains=vimBracket
syn match vimNotation	'<C-R>[0-9a-z"%#:.\-=]'he=e-1			contains=vimBracket
syn match vimNotation	'<\(line[12]\|count\|bang\|reg\|args\|lt\|[qf]-args\)>'	contains=vimBracket
syn match vimBracket contained	"[<>]"
syn match vimBracket contained	"[<>]"
syn case match

" Control Characters
" ==================
syn match vimCtrlChar	"[--]"

" Beginners - Patterns that involve ^
" =========
syn match  vimLineComment	+^[ \t:]*".*$+		contains=vimTodo,vimCommentString,vimCommentTitle
syn match  vimCommentTitle	"\<\(\u\a*\s*\)\+:"	contained
syn match  vimContinue	"^\s*\\"

" Synchronization : now needed due to vimConditional and vimCondError
" ===============
syn sync minlines=150

" Highlighting Settings
" ====================
if !exists("did_vim_syntax_inits")
  let did_vim_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later
  hi link vimAuHighlight	vimHighlight

  hi link vimAddress	vimMark
  hi link vimAutoCmd	vimCommand
  hi link vimAutoCmdOpt	vimOption
  hi link vimAutoSet	vimCommand
  hi link vimBehaveError	vimError
  hi link vimCommentString	vimString
  hi link vimCondHL	vimCommand
  hi link vimCondError	vimError
  hi link vimElseif	vimCondHL
  hi link vimErrSetting	vimError
  hi link vimFgBgAttrib	vimHiAttrib
  hi link vimFTCmd	vimCommand
  hi link vimFTError	vimError
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
  hi link vimKeyCode	vimSpecFile
  hi link vimKeyCodeError	vimError
  hi link vimLet		vimCommand
  hi link vimLineComment	vimComment
  hi link vimNotPatSep	vimString
  hi link vimPlainMark	vimMark
  hi link vimPlainRegister	vimRegister
  hi link vimSetString	vimString
  hi link vimSpecFileMod	vimSpecFile
  hi link vimStringCont	vimString
  hi link vimSynCaseError	vimError
  hi link vimSynContains	vimSynOption
  hi link vimSynKeyOpt	vimSynOption
  hi link vimSynMtchGrp	vimSynOption
  hi link vimSynNextgroup	vimSynOption
  hi link vimSynNotPatRange	vimSynRegPat
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
  hi link vimCommentTitle	PreProc
  hi link vimContinue	Special
  hi link vimCtrlChar	SpecialChar
  hi link vimEnvvar	PreProc
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
  hi link vimSetSep	Statement
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
