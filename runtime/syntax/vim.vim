" Vim syntax file
" Language:	Vim 6.0c script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	July 24, 2000
" Version:	6.0c-01

" Remove old syntax
syn clear

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	N[ext]	cq[uit]	iunmenu	prev[ious]	stj[ump]
syn keyword vimCommand contained	P[rint]	cr[ewind]	j[oin]	promptf[ind]	sts[elect]
syn keyword vimCommand contained	X	cu[nmap]	ju[mps]	promptr[epl]	sun[hide]
syn keyword vimCommand contained	a[ppend]	cuna[bbrev]	k	ptN[ext]	sus[pend]
syn keyword vimCommand contained	ab[breviate]	cunme	l[ist]	pta[g]	sv[iew]
syn keyword vimCommand contained	abc[lear]	cunmenu	la[st]	ptj[ump]	syncbind
syn keyword vimCommand contained	al[l]	cw[indow]	lan[guage]	ptl[ast]	syntax
syn keyword vimCommand contained	am	d[elete]	lc[d]	ptn[ext]	t
syn keyword vimCommand contained	amenu	delc[ommand]	lch[dir]	ptp[revious]	tN[ext]
syn keyword vimCommand contained	an	delf[unction]	le[ft]	ptr[ewind]	ta[g]
syn keyword vimCommand contained	anoremenu	dig[raphs]	ls	pts[elect]	tags
syn keyword vimCommand contained	ar[gs]	dj[ump]	m[ove]	pu[t]	tc[l]
syn keyword vimCommand contained	argu[ment]	dl[ist]	ma[rk]	pw[d]	tcld[o]
syn keyword vimCommand contained	as[cii]	do[autocmd]	mak[e]	py[thon]	tclf[ile]
syn keyword vimCommand contained	au[tocmd]	doautoa[ll]	map	pyf[ile]	te[aroff]
syn keyword vimCommand contained	aug[roup]	ds[earch]	mapc[lear]	q[uit]	tj[ump]
syn keyword vimCommand contained	aun	dsp[lit]	marks	qa[ll]	tl[ast]
syn keyword vimCommand contained	aunmenu	e[dit]	me	quita[ll]	tm[enu]
syn keyword vimCommand contained	autocmd	ec[ho]	menu	r[ead]	tn[ext]
syn keyword vimCommand contained	bN[ext]	echoh[l]	menu-disable	rec[over]	to[pleft]
syn keyword vimCommand contained	b[uffer]	echon	menu-enable	red[o]	tp[revious]
syn keyword vimCommand contained	ba[ll]	el[se]	menut[rans]	redi[r]	tr[ewind]
syn keyword vimCommand contained	bad[d]	elsei[f]	mk[exrc]	reg[isters]	ts[elect]
syn keyword vimCommand contained	bd[elete]	eme	mks[ession]	res[ize]	tu[nmenu]
syn keyword vimCommand contained	be[have]	emenu	mkv[imrc]	ret[ab]	u[ndo]
syn keyword vimCommand contained	bl[ast]	en[dif]	mod[e]	retu[rn]	una[bbreviate]
syn keyword vimCommand contained	bm[odified]	endf[unction]	n[ext]	rew[ind]	unh[ide]
syn keyword vimCommand contained	bn[ext]	endw[hile]	new	ri[ght]	unl[et]
syn keyword vimCommand contained	bo[tright]	ene[w]	nm[ap]	ru[ntime]	unm[ap]
syn keyword vimCommand contained	bp[revious]	ex	nmapc[lear]	rv[iminfo]	unme
syn keyword vimCommand contained	br[ewind]	exe[cute]	nme	s	unmenu
syn keyword vimCommand contained	brea[k]	exi[t]	nmenu	sN[ext]	uns[ign]
syn keyword vimCommand contained	bro[wse]	f[ile]	nn[oremap]	s[ubstitute]	v[global]
syn keyword vimCommand contained	buffers	files	nnoreme	sa[rgument]	ve[rsion]
syn keyword vimCommand contained	bun[load]	fin[d]	nnoremenu	sal[l]	vert[ical]
syn keyword vimCommand contained	cN[ext]	fini[sh]	no[remap]	sbN[ext]	vi[sual]
syn keyword vimCommand contained	c[hange]	fix[del]	noh[lsearch]	sb[uffer]	vie[w]
syn keyword vimCommand contained	ca[bbrev]	fo[ld]	norea[bbrev]	sba[ll]	vm[ap]
syn keyword vimCommand contained	cabc[lear]	foldo[pen]	noreme	sbl[ast]	vmapc[lear]
syn keyword vimCommand contained	cal[l]	fu[nction]	noremenu	sbm[odified]	vme
syn keyword vimCommand contained	cc	g[lobal]	norm[al]	sbn[ext]	vmenu
syn keyword vimCommand contained	cd	go[to]	nu[mber]	sbp[revious]	vn[oremap]
syn keyword vimCommand contained	ce[nter]	gr[ep]	nun[map]	sbr[ewind]	vne[w]
syn keyword vimCommand contained	cf[ile]	h[elp]	nunme	se[t]	vnoreme
syn keyword vimCommand contained	chd[ir]	helpf[ind]	nunmenu	setg[lobal]	vnoremenu
syn keyword vimCommand contained	che[ckpath]	hid[e]	o[pen]	setl[ocal]	vs[plit]
syn keyword vimCommand contained	cl[ist]	his[tory]	om[ap]	sf[ind]	vu[nmap]
syn keyword vimCommand contained	cla[st]	i[nsert]	omapc[lear]	sh[ell]	vunme
syn keyword vimCommand contained	clo[se]	ia[bbrev]	ome	si[malt]	vunmenu
syn keyword vimCommand contained	cm[ap]	iabc[lear]	omenu	sig[n]	wN[ext]
syn keyword vimCommand contained	cmapc[lear]	if	on[ly]	signs	w[rite]
syn keyword vimCommand contained	cme	ij[ump]	ono[remap]	sil[ent]	wa[ll]
syn keyword vimCommand contained	cmenu	il[ist]	onoreme	sl[eep]	wh[ile]
syn keyword vimCommand contained	cn[ext]	im[ap]	onoremenu	sla[st]	win[size]
syn keyword vimCommand contained	cnew[er]	imapc[lear]	opt[ions]	sm[agic]	winp[os]
syn keyword vimCommand contained	cnf[ile]	ime	ou[nmap]	sn[ext]	winsize
syn keyword vimCommand contained	cno[remap]	imenu	ounme	sni[ff]	wn[ext]
syn keyword vimCommand contained	cnorea[bbrev]	ino[remap]	ounmenu	sno[magic]	wp[revous]
syn keyword vimCommand contained	cnoreme	inorea[bbrev]	p[rint]	so[urce]	wq
syn keyword vimCommand contained	cnoremenu	inoreme	pc[lose]	sp[lit]	wqa[ll]
syn keyword vimCommand contained	co[py]	inoremenu	pe[rl]	spr[evious]	ws[verb]
syn keyword vimCommand contained	col[der]	is[earch]	perld[o]	sr[ewind]	wv[iminfo]
syn keyword vimCommand contained	comc[lear]	isp[lit]	po[p]	st[op]	x[it]
syn keyword vimCommand contained	con[tinue]	iu[nmap]	pp[op]	sta[g]	xa[ll]
syn keyword vimCommand contained	conf[irm]	iuna[bbrev]	pre[serve]	star[tinsert]	y[ank]
syn keyword vimCommand contained	cp[revious]	iunme
syn match   vimCommand contained	"z[-+^.=]"
syn match   vimCommand contained	"di\(s\(p\(l\(a\(y\)\=\)\=\)\=\)\=\)\="

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep	"[:|]\+"	skipwhite nextgroup=vimAddress,vimAutoCmd,vimMark,vimFilter,vimUserCmd,vimSet,vimLet,vimCommand,vimSyntax
syn match vimIsCommand	"\<\a\+\>"	contains=vimCommand

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	ek	infercase	ro	tagbsearch
syn keyword vimOption contained	ai	endofline	insertmode	rs	taglength
syn keyword vimOption contained	akm	eol	is	rtp	tagrelative
syn keyword vimOption contained	al	ep	isf	ru	tags
syn keyword vimOption contained	aleph	equalalways	isfname	ruf	tagstack
syn keyword vimOption contained	allowrevins	equalprg	isi	ruler	tb
syn keyword vimOption contained	altkeymap	errorbells	isident	rulerformat	tbi
syn keyword vimOption contained	ar	errorfile	isk	runtimepath	tbs
syn keyword vimOption contained	ari	errorformat	iskeyword	sb	term
syn keyword vimOption contained	autoindent	esckeys	isp	sbo	terse
syn keyword vimOption contained	autoread	et	isprint	sbr	textauto
syn keyword vimOption contained	autowrite	eventignore	joinspaces	sc	textmode
syn keyword vimOption contained	aw	ex	js	scb	textwidth
syn keyword vimOption contained	background	expandtab	key	scr	tf
syn keyword vimOption contained	backspace	exrc	keymodel	scroll	tgst
syn keyword vimOption contained	backup	fcc	keywordprg	scrollbind	thesaurus
syn keyword vimOption contained	backupdir	fccs	km	scrolljump	tildeop
syn keyword vimOption contained	backupext	fcs	kp	scrolloff	timeout
syn keyword vimOption contained	balloondelay	fdc	langmap	scrollopt	timeoutlen
syn keyword vimOption contained	ballooneval	fde	laststatus	scs	title
syn keyword vimOption contained	bdir	fdi	lazyredraw	sect	titlelen
syn keyword vimOption contained	bdlay	fdl	lbr	sections	titleold
syn keyword vimOption contained	beval	fdm	lcs	secure	titlestring
syn keyword vimOption contained	bex	fdt	linebreak	sel	tl
syn keyword vimOption contained	bg	fe	lines	selection	tm
syn keyword vimOption contained	bin	fen	linespace	selectmode	to
syn keyword vimOption contained	binary	ff	lisp	sessionoptions	toolbar
syn keyword vimOption contained	biosk	ffs	list	sft	top
syn keyword vimOption contained	bioskey	filecharcode	listchars	sh	tr
syn keyword vimOption contained	bk	filecharcodes	lmap	shcf	ts
syn keyword vimOption contained	breakat	fileencoding	ls	shell	tsl
syn keyword vimOption contained	brk	fileformat	lsp	shellcmdflag	tsr
syn keyword vimOption contained	browsedir	fileformats	lz	shellpipe	ttimeout
syn keyword vimOption contained	bs	filetype	magic	shellquote	ttimeoutlen
syn keyword vimOption contained	bsdir	fillchars	makeef	shellredir	ttm
syn keyword vimOption contained	bt	fk	makeprg	shellslash	ttybuiltin
syn keyword vimOption contained	buftype	fkmap	mat	shelltype	ttyfast
syn keyword vimOption contained	cb	fmr	matchpairs	shellxquote	ttym
syn keyword vimOption contained	cc	fo	matchtime	shiftround	ttymouse
syn keyword vimOption contained	ccv	foldcolumn	maxfuncdepth	shiftwidth	ttyscroll
syn keyword vimOption contained	cd	foldenable	maxmapdepth	shm	ttytype
syn keyword vimOption contained	cdpath	foldexpr	maxmem	shortmess	tw
syn keyword vimOption contained	cf	foldignore	maxmemtot	shortname	tx
syn keyword vimOption contained	ch	foldlevel	mef	showbreak	uc
syn keyword vimOption contained	charcode	foldmarker	menuitems	showcmd	ul
syn keyword vimOption contained	charconvert	foldmethod	mfd	showfulltag	undolevels
syn keyword vimOption contained	cin	foldtext	mh	showmatch	updatecount
syn keyword vimOption contained	cindent	formatoptions	mis	showmode	updatetime
syn keyword vimOption contained	cink	formatprg	ml	shq	ut
syn keyword vimOption contained	cinkeys	fp	mls	si	vb
syn keyword vimOption contained	cino	ft	mm	sidescroll	vbs
syn keyword vimOption contained	cinoptions	gcr	mmd	sj	ve
syn keyword vimOption contained	cinw	gd	mmt	slm	verbose
syn keyword vimOption contained	cinwords	gdefault	mod	sm	vi
syn keyword vimOption contained	clipboard	gfm	modeline	smartcase	viminfo
syn keyword vimOption contained	cmdheight	gfn	modelines	smartindent	virtualedit
syn keyword vimOption contained	co	gfs	modified	smarttab	visualbell
syn keyword vimOption contained	columns	ghr	more	smd	wa
syn keyword vimOption contained	com	go	mouse	sn	wak
syn keyword vimOption contained	comments	gp	mousef	so	warn
syn keyword vimOption contained	compatible	grepformat	mousefocus	softtabstop	wb
syn keyword vimOption contained	complete	grepprg	mousehide	sol	wc
syn keyword vimOption contained	confirm	guicursor	mousem	sp	wcm
syn keyword vimOption contained	consk	guifont	mousemodel	splitbelow	wd
syn keyword vimOption contained	conskey	guifontset	mouses	splitright	weirdinvert
syn keyword vimOption contained	cp	guiheadroom	mouseshape	spr	wh
syn keyword vimOption contained	cpo	guioptions	mouset	sr	whichwrap
syn keyword vimOption contained	cpoptions	guipty	mousetime	srr	wig
syn keyword vimOption contained	cpt	helpfile	mp	ss	wildchar
syn keyword vimOption contained	cscopeprg	helpheight	mps	ssl	wildcharm
syn keyword vimOption contained	cscopetag	hf	nf	ssop	wildignore
syn keyword vimOption contained	cscopetagorder	hh	nrformats	st	wildmenu
syn keyword vimOption contained	cscopeverbose	hi	nu	sta	wildmode
syn keyword vimOption contained	csprg	hid	number	startofline	wim
syn keyword vimOption contained	cst	hidden	oft	statusline	winaltkeys
syn keyword vimOption contained	csto	highlight	osfiletype	stl	winheight
syn keyword vimOption contained	csverb	history	pa	sts	winminheight
syn keyword vimOption contained	def	hk	para	su	winminwidth
syn keyword vimOption contained	define	hkmap	paragraphs	sua	winwidth
syn keyword vimOption contained	dg	hkmapp	paste	suffixes	wiv
syn keyword vimOption contained	dict	hkp	pastetoggle	suffixesadd	wiw
syn keyword vimOption contained	dictionary	hl	patchmode	sw	wm
syn keyword vimOption contained	digraph	hls	path	swapfile	wmh
syn keyword vimOption contained	dir	hlsearch	pm	swapsync	wmnu
syn keyword vimOption contained	directory	ic	previewheight	swb	wmw
syn keyword vimOption contained	dy	icon	pt	swf	wrap
syn keyword vimOption contained	ea	iconstring	pvh	switchbuf	wrapmargin
syn keyword vimOption contained	ead	ignorecase	readonly	sws	wrapscan
syn keyword vimOption contained	eadirection	im	remap	sxq	write
syn keyword vimOption contained	eb	inc	report	syn	writeany
syn keyword vimOption contained	ed	include	restorescreen	syntax	writebackup
syn keyword vimOption contained	edcompatible	includeexpr	revins	ta	writedelay
syn keyword vimOption contained	ef	incsearch	ri	tabstop	ws
syn keyword vimOption contained	efm	inex	rightleft	tag	ww
syn keyword vimOption contained	ei	inf	rl
syn match vimOption contained	"display"

" These are the turn-off setting variants
syn keyword vimOption contained	noai	noed	nojoinspaces	nosc	notagstack
syn keyword vimOption contained	noakm	noedcompatible	nojs	noscb	notbi
syn keyword vimOption contained	noallowrevins	noek	nolazyredraw	noscrollbind	notbs
syn keyword vimOption contained	noaltkeymap	noendofline	nolbr	noscs	noterse
syn keyword vimOption contained	noar	noeol	nolinebreak	nosecure	notextauto
syn keyword vimOption contained	noari	noequalalways	nolisp	nosft	notextmode
syn keyword vimOption contained	noautoindent	noerrorbells	nolist	noshellslash	notf
syn keyword vimOption contained	noautoread	noesckeys	nolz	noshiftround	notgst
syn keyword vimOption contained	noautowrite	noet	nomagic	noshortname	notildeop
syn keyword vimOption contained	noaw	noex	nomh	noshowcmd	notimeout
syn keyword vimOption contained	nobackup	noexpandtab	noml	noshowfulltag	notitle
syn keyword vimOption contained	noballooneval	noexrc	nomod	noshowmatch	noto
syn keyword vimOption contained	nobeval	nofk	nomodeline	noshowmode	notop
syn keyword vimOption contained	nobin	nofkmap	nomodified	nosi	notr
syn keyword vimOption contained	nobinary	nogd	nomore	nosm	nottimeout
syn keyword vimOption contained	nobiosk	nogdefault	nomousef	nosmartcase	nottybuiltin
syn keyword vimOption contained	nobioskey	noguipty	nomousefocus	nosmartindent	nottyfast
syn keyword vimOption contained	nobk	nohid	nomousehide	nosmarttab	notx
syn keyword vimOption contained	nocf	nohidden	nonu	nosmd	novb
syn keyword vimOption contained	nocin	nohk	nonumber	nosn	novisualbell
syn keyword vimOption contained	nocindent	nohkmap	nopaste	nosol	nowa
syn keyword vimOption contained	nocompatible	nohkmapp	noreadonly	nosplitbelow	nowarn
syn keyword vimOption contained	noconfirm	nohkp	noremap	nosplitright	nowb
syn keyword vimOption contained	noconsk	nohls	norestorescreen	nospr	noweirdinvert
syn keyword vimOption contained	noconskey	nohlsearch	norevins	nosr	nowildmenu
syn keyword vimOption contained	nocp	noic	nori	nossl	nowiv
syn keyword vimOption contained	nocscopetag	noicon	norightleft	nosta	nowmnu
syn keyword vimOption contained	nocscopeverbose	noignorecase	norl	nostartofline	nowrap
syn keyword vimOption contained	nocst	noim	noro	noswapfile	nowrapscan
syn keyword vimOption contained	nocsverb	noincsearch	nors	noswf	nowrite
syn keyword vimOption contained	nodg	noinf	noru	nota	nowriteany
syn keyword vimOption contained	nodigraph	noinfercase	noruler	notagbsearch	nowritebackup
syn keyword vimOption contained	noea	noinsertmode	nosb	notagrelative	nows
syn keyword vimOption contained	noeb	nois

" termcap codes (which can also be set)
syn keyword vimOption contained	t_AB	t_K1	t_KJ	t_cm	t_kI	t_ms
syn keyword vimOption contained	t_AF	t_K3	t_KK	t_cs	t_kN	t_nd
syn keyword vimOption contained	t_AL	t_K4	t_KL	t_da	t_kP	t_op
syn keyword vimOption contained	t_CS	t_K5	t_RI	t_db	t_kb	t_se
syn keyword vimOption contained	t_Co	t_K6	t_RV	t_dl	t_kd	t_so
syn keyword vimOption contained	t_DL	t_K7	t_Sb	t_fs	t_ke	t_sr
syn keyword vimOption contained	t_F1	t_K8	t_Sf	t_k1	t_kh	t_te
syn keyword vimOption contained	t_F2	t_K9	t_WP	t_k2	t_kl	t_ti
syn keyword vimOption contained	t_F3	t_KA	t_WS	t_k3	t_kr	t_ts
syn keyword vimOption contained	t_F4	t_KB	t_ZH	t_k4	t_ks	t_ue
syn keyword vimOption contained	t_F5	t_KC	t_ZR	t_k5	t_ku	t_us
syn keyword vimOption contained	t_F6	t_KD	t_al	t_k6	t_le	t_vb
syn keyword vimOption contained	t_F7	t_KE	t_bc	t_k7	t_mb	t_ve
syn keyword vimOption contained	t_F8	t_KF	t_cd	t_k8	t_md	t_vi
syn keyword vimOption contained	t_F9	t_KG	t_ce	t_k9	t_me	t_vs
syn keyword vimOption contained	t_IE	t_KH	t_cl	t_kD	t_mr	t_xs
syn keyword vimOption contained	t_IS	t_KI
syn match   vimOption contained	"t_#2"
syn match   vimOption contained	"t_#4"
syn match   vimOption contained	"t_%1"
syn match   vimOption contained	"t_%i"
syn match   vimOption contained	"t_&8"
syn match   vimOption contained	"t_*7"
syn match   vimOption contained	"t_@7"
syn match   vimOption contained	"t_k;"

" these settings don't actually cause errors in vim, but were supported by vi and don't do anything in vim
syn keyword vimErrSetting contained	hardtabs	w1200	w9600	wi	window
syn keyword vimErrSetting contained	ht	w300

" AutoBuf Events
syn case ignore
syn keyword vimAutoEvent contained	BufCreate	BufReadPre	FileAppendPre	FilterReadPost	Syntax
syn keyword vimAutoEvent contained	BufDelete	BufUnload	FileChangedShell	FilterReadPre	TermChanged
syn keyword vimAutoEvent contained	BufEnter	BufWrite	FileEncoding	FilterWritePost	TermResponse
syn keyword vimAutoEvent contained	BufFilePost	BufWriteCmd	FileReadCmd	FilterWritePre	User
syn keyword vimAutoEvent contained	BufFilePre	BufWritePost	FileReadPost	FocusGained	UserGettingBored
syn keyword vimAutoEvent contained	BufHidden	BufWritePre	FileReadPre	FocusLost	VimEnter
syn keyword vimAutoEvent contained	BufLeave	CharCode	FileType	FuncUndefined	VimLeave
syn keyword vimAutoEvent contained	BufNewFile	Cmd-event	FileWriteCmd	GUIEnter	VimLeavePre
syn keyword vimAutoEvent contained	BufRead	CursorHold	FileWritePost	StdinReadPost	WinEnter
syn keyword vimAutoEvent contained	BufReadCmd	FileAppendCmd	FileWritePre	StdinReadPre	WinLeave
syn keyword vimAutoEvent contained	BufReadPost	FileAppendPost

" Highlight commonly used Groupnames
syn keyword vimGroup contained	Comment	Identifier	Keyword	Type	Delimiter
syn keyword vimGroup contained	Constant	Function	Exception	StorageClass	SpecialComment
syn keyword vimGroup contained	String	Statement	PreProc	Structure	Debug
syn keyword vimGroup contained	Character	Conditional	Include	Typedef	Ignore
syn keyword vimGroup contained	Number	Repeat	Define	Special	Error
syn keyword vimGroup contained	Boolean	Label	Macro	SpecialChar	Todo
syn keyword vimGroup contained	Float	Operator	PreCondit	Tag

" Default highlighting groups
syn keyword vimHLGroup contained	Cursor	IncSearch	NonText	SpecialKey	Visual
syn keyword vimHLGroup contained	Directory	LineNr	Normal	StatusLine	VisualNOS
syn keyword vimHLGroup contained	ErrorMsg	Menu	Question	StatusLineNC	WarningMsg
syn keyword vimHLGroup contained	FillColumn	ModeMsg	Scrollbar	Title	WildMenu
syn keyword vimHLGroup contained	Folded	MoreMsg	Search
syn case match

" Function Names
syn keyword vimFuncName contained	append	did_filetype	histadd	localtime	strpart
syn keyword vimFuncName contained	argc	escape	histdel	maparg	strtrans
syn keyword vimFuncName contained	argv	exists	histget	mapcheck	submatch
syn keyword vimFuncName contained	browse	expand	histnr	match	substitute
syn keyword vimFuncName contained	bufexists	filereadable	hlID	matchend	synID
syn keyword vimFuncName contained	bufloaded	fnamemodify	hlexists	matchstr	synIDattr
syn keyword vimFuncName contained	bufname	getbufvar	hostname	nr2char	synIDtrans
syn keyword vimFuncName contained	bufnr	getcwd	indent	rename	system
syn keyword vimFuncName contained	bufwinnr	getftime	input	search	tempname
syn keyword vimFuncName contained	byte2line	getline	isdirectory	setbufvar	virtcol
syn keyword vimFuncName contained	char2nr	getwinposx	libcall	setline	visualmode
syn keyword vimFuncName contained	col	getwinposy	libcallnr	setwinvar	winbufnr
syn keyword vimFuncName contained	confirm	getwinvar	line	strftime	winheight
syn keyword vimFuncName contained	cscope_connection	glob	line2byte	strlen	winnr
syn keyword vimFuncName contained	delete	has
syn match   vimFunc     "\I\i*\s*("	contains=vimFuncName,vimCommand

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" Behave!
" =======
syn match   vimBehave	"\<be\(h\(a\(ve\=\)\=\)\=\)\=\>" contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained	mswin	xterm
syn match   vimBehaveError contained	"[^ ]\+"

" Filetypes
" =========
syn match   vimFiletype	"filet\(y\(pe\=\)\=\)\=\s\+\I\i*"	skipwhite contains=vimFTCmd,vimFTOption,vimFTError
syn match   vimFTError  contained	"\I\i*"
syn keyword vimFTCmd    contained	filet[ype]
syn keyword vimFTOption contained	on	off

" Functions : Tag is provided for those who wish to highlight tagged functions
" =========
syn cluster vimFuncList	contains=vimCommand,Tag
syn cluster vimFuncBodyList	contains=vimIsCommand,vimFunction,vimFunctionError,vimFuncBody,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar
syn match   vimFunction	"\<fu\(n\(c\(t\(t\(i\(on\=\)\=\)\=\)\=\)\=\)\=\)\=!\=\s\+\u\w*("me=e-1	contains=@vimFuncList nextgroup=vimFuncBody
syn match   vimFunctionError	"\<fu\(n\(c\(t\(t\(i\(on\=\)\=\)\=\)\=\)\=\)\=\)\=!\=\s\+\U.\{-}("me=e-1	contains=vimCommand   nextgroup=vimFuncBody
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

" User-Specified Commands
syn cluster vimUserCmdList	contains=vimAddress,vimSyntax,vimHighlight,vimAutoCmd,vimCmplxRepeat,vimComment,vimCtrlChar,vimEscapeBrace,vimFilter,vimFunc,vimFunction,vimIsCommand,vimMark,vimNotation,vimNumber,vimOper,vimRegion,vimRegister,vimLet,vimSet,vimSetEqual,vimSetString,vimSpecFile,vimString,vimSubst,vimSubstEnd,vimSubstRange,vimSynLine
syn match   vimUserCmd	"\<com\(m\(a\(nd\=\)\=\)\=\)\=!\=\>.*$"		contains=vimUserAttrb,@vimUserCmdList
syn match   vimUserAttrb 	contained	"-n\(a\(r\(gs\=\)\=\)\=\)\==[01*?+]"	contains=vimUserAttrbKey,vimOper
syn match   vimUserAttrb 	contained	"-com\(p\(l\(e\(te\=\)\=\)\=\)\=\)\==\(augroup\|buffer\|command\|dir\|event\|file\|help\|highlight\|menu\|option\|tag\|var\)"	contains=vimUserAttrbKey,vimUserAttrbCmplt,vimOper
syn match   vimUserAttrb 	contained	"-ra\(n\(ge\=\)\=\)\=\(=%\|=\d\+\)\="	contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-cou\(nt\=\)\==\d\+"		contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-b\(a\(ng\=\)\=\)\="		contains=vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-re\(g\(i\(s\(t\(er\=\)\=\)\=\)\=\)\=\)\="	contains=vimOper,vimUserAttrbKey
syn keyword vimUserAttrbKey	contained	b[ang]	cou[nt]	ra[nge]
syn keyword vimUserAttrbKey	contained	com[plete]	n[args]	re[gister]
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
syn match  vimComment	+\s"[^\-:.%#=*].*$+lc=1	contains=@vimCommentGroup,vimCommentString
syn match  vimComment	+\<endif\s\+".*$+lc=5	contains=@vimCommentGroup,vimCommentString
syn match  vimComment	+\<else\s\+".*$+lc=4	contains=@vimCommentGroup,vimCommentString
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

syn match  vimRegister		'[^(,;.]"[a-zA-Z0-9\-:.%#*=][^a-zA-Z_"]'lc=1,me=e-1
syn match  vimRegister		'\<norm\s\+"[a-zA-Z0-9]'lc=5
syn match  vimRegister		'\<normal\s\+"[a-zA-Z0-9]'lc=7
syn match  vimPlainRegister	contained	'"[a-zA-Z0-9\-:.%#*=]'

syn match  vimAddress		",\."lc=1
syn match  vimAddress		"[%.]" skipwhite	nextgroup=vimString

syn match  vimFilter	contained	"^!.\{-}\(|\|$\)"	contains=vimSpecFile
syn match  vimFilter	contained	"\A!.\{-}\(|\|$\)"ms=s+1	contains=vimSpecFile

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
syn match   vimAutoEventList	contained	"\(!\s\+\)\=\(\a\+,\)*\a\+"	contains=vimAutoEvent nextgroup=vimAutoCmdSpace
syn match   vimAutoCmdSpace	contained	"\s\+"		nextgroup=vimAutoCmdSfxList
syn match   vimAutoCmdSfxList	contained	"\S*"
syn keyword vimAutoCmd		au[tocmd] do[autocmd] doautoa[ll]	skipwhite nextgroup=vimAutoEventList

" Echo and Execute -- prefer strings!
syn region  vimEcho	oneline	start="\<ec\(ho\=\)\=\>"   skip="\(\\\\\)*\\|" end="$\||" contains=vimCommand,vimString,vimOper
syn region  vimEcho	oneline	start="\<exe\(c\(u\(te\=\)\=\)\=\)\=\>"   skip="\(\\\\\)*\\|" end="$\||" contains=vimCommand,vimString,vimOper

" Syntax
"=======
syn match   vimGroupList	contained	"@\=[^ \t,]*"	contains=vimGroupSpecial,vimPatSep
syn match   vimGroupList	contained	"@\=[^ \t,]*,"	nextgroup=vimGroupList contains=vimGroupSpecial,vimPatSep
syn keyword vimGroupSpecial	contained	ALL	ALLBUT
syn match   vimSynError	contained	"\i\+"
syn match   vimSynError	contained	"\i\+="	nextgroup=vimGroupList
syn match   vimSynContains	contained	"contains="	nextgroup=vimGroupList
syn match   vimSynNextgroup	contained	"nextgroup="	nextgroup=vimGroupList

syn match   vimSyntax	"\<sy\(n\(t\(ax\=\)\=\)\=\)\=\>"		contains=vimCommand skipwhite nextgroup=vimSynType,vimComment
syn match   vimAuSyntax	contained	"\s+sy\(n\(t\(ax\=\)\=\)\=\)\="	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment

" Syntax: case
syn keyword vimSynType	contained	case	skipwhite nextgroup=vimSynCase,vimSynCaseError
syn match   vimSynCaseError	contained	"\i\+"
syn keyword vimSynCase	contained	ignore	match

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
syn match   vimSynKeyOpt	contained	"\<\(contained\|display\|fold\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: match
syn keyword vimSynType	contained	match	skipwhite nextgroup=vimSynMatchRegion
syn region  vimSynMatchRegion	contained oneline matchgroup=vimGroupName start="\k\+" end="$" contains=vimComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn match   vimSynMtchOpt	contained	"\<\(contained\|display\|excludenl\|fold\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: off and on
syn keyword vimSynType	contained	off	on

" Syntax: region
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(contained\|display\|excludenl\|fold\|transparent\|skipempty\|skipwhite\|skipnl\|oneline\|keepend\)\>"
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
syn keyword vimSynType	contained	sync	skipwhite	nextgroup=vimSyncC,vimSyncLines,vimSyncMatch,vimSyncError,vimSyncLinecont
syn match   vimSyncError	contained	"\i\+"
syn keyword vimSyncC	contained	ccomment	clear
syn keyword vimSyncMatch	contained	match	skipwhite	nextgroup=vimSyncGroupName
syn keyword vimSyncLinecont	contained	linecont	skipwhite	nextgroup=vimSynRegPat
syn match   vimSyncLines	contained	"\(min\|max\)\=lines="	nextgroup=vimNumber
syn match   vimSyncGroupName	contained	"\k\+"	skipwhite	nextgroup=vimSyncKey
syn match   vimSyncKey	contained	"\<groupthere\|grouphere\>"	skipwhite nextgroup=vimSyncGroup
syn match   vimSyncGroup	contained	"\k\+"	skipwhite	nextgroup=vimSynRegPat,vimSyncNone
syn keyword vimSyncNone	contained	NONE

" Additional IsCommand stuff, here by reasons of precedence
" ====================
syn match vimIsCommand	"<Bar>\s*\a\+"	transparent contains=vimCommand,vimNotation

" Highlighting
" ============
syn match   vimHighlight		"\<hi\(g\(h\(l\(i\(g\(ht\=\)\=\)\=\)\=\)\=\)\=\)\=\>" skipwhite nextgroup=vimHiLink,vimHiClear,vimHiKeyList,vimComment

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
syn match   vimHiFontname	contained	"[a-zA-Z\-*]\+"
syn match   vimHiGuiFontname	contained	"'[a-zA-Z\-* ]\+'"
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
syn match vimNotation	"\\<\([scam]-\)\{0,4}\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|del\(ete\)\=\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k \=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation	"\\<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\|drag\|release\)>" contains=vimBracket
syn match vimNotation	"\\<\(bslash\|space\|bar\|nop\|nul\|lt\)>"		contains=vimBracket
syn match vimNotation	'\\<C-R>[0-9a-z"%#:.\-=]'he=e-1			contains=vimBracket
syn match vimNotation	'\\<\(line[12]\|count\|bang\|reg\|args\|lt\|[qf]-args\)>'	contains=vimBracket
syn match vimBracket contained	"[\\<>]"
syn case match

" Control Characters
" ==================
syn match vimCtrlChar	"[--]"

" Beginners - Patterns that involve ^
" =========
syn match  vimLineComment	+^[ \t:]*".*$+		contains=@vimCommentGroup,vimCommentString,vimCommentTitle
syn match  vimCommentTitle	'"\s*\u\a*\(\s\+\u\a*\)*:'ms=s+1	contained
syn match  vimContinue	"^\s*\\"

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
  hi link vimElseif	vimCondHL
  hi link vimErrSetting	vimError
  hi link vimFgBgAttrib	vimHiAttrib
  hi link vimFTCmd	vimCommand
  hi link vimFTOption	vimSynType
  hi link vimFTError	vimError
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
  hi link vimNotFunc	vimCommand
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
  hi link vimSynMtchOpt	vimSynOption
  hi link vimSynNextgroup	vimSynOption
  hi link vimSynNotPatRange	vimSynRegPat
  hi link vimSynPatRange	vimString
  hi link vimSynRegOpt	vimSynOption
  hi link vimSynRegPat	vimString
  hi link vimSyntax	vimCommand
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
