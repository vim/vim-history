" Vim syntax file
" Language:	Vim 6.0au script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	September 04, 2001
" Version:	6.0au-01

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	DeleteFirst	compiler	ij[ump]	p[rint]	so[urce]
syn keyword vimCommand contained	N[ext]	conf[irm]	il[ist]	prompt	sp[lit]
syn keyword vimCommand contained	Nread	con[tinue]	imapc[lear]	promptf[ind]	spr[evious]
syn keyword vimCommand contained	Nw	cope[n]	inorea[bbrev]	promptr[epl]	sr[ewind]
syn keyword vimCommand contained	P[rint]	co[py]	is[earch]	ps[earch]	sta[g]
syn keyword vimCommand contained	X	cp[revious]	isp[lit]	ptN[ext]	star[tinsert]
syn keyword vimCommand contained	ab[breviate]	cq[uit]	iuna[bbrev]	pta[g]	stj[ump]
syn keyword vimCommand contained	abc[lear]	cr[ewind]	iu[nmap]	ptf[irst]	st[op]
syn keyword vimCommand contained	abo[veleft]	cuna[bbrev]	iunme	ptj[ump]	sts[elect]
syn keyword vimCommand contained	aboveright	cu[nmap]	iunmenu	ptl[ast]	sun[hide]
syn keyword vimCommand contained	al[l]	cunme	j[oin]	ptn[ext]	sus[pend]
syn keyword vimCommand contained	arga[dd]	cunmenu	ju[mps]	ptp[revious]	sv[iew]
syn keyword vimCommand contained	argd[elete]	cw[indow]	k	ptr[ewind]	syncbind
syn keyword vimCommand contained	argdo	delc[ommand]	lan[guage]	pts[elect]	t
syn keyword vimCommand contained	arge[dit]	d[elete]	la[st]	pu[t]	tN[ext]
syn keyword vimCommand contained	argg[lobal]	delf[unction]	lc[d]	pw[d]	ta[g]
syn keyword vimCommand contained	argl[ocal]	diffg[et]	lch[dir]	pyf[ile]	tags
syn keyword vimCommand contained	ar[gs]	diffpatch	le[ft]	py[thon]	tc[l]
syn keyword vimCommand contained	argu[ment]	diffpu[t]	lefta[bove]	qa[ll]	tcld[o]
syn keyword vimCommand contained	as[cii]	diffsplit	leftbelow	q[uit]	tclf[ile]
syn keyword vimCommand contained	aun	diffthis	l[ist]	quita[ll]	te[aroff]
syn keyword vimCommand contained	aunmenu	dig[raphs]	lm[ap]	r[ead]	tf[irst]
syn keyword vimCommand contained	bN[ext]	di[splay]	lmapc[lear]	rec[over]	tj[ump]
syn keyword vimCommand contained	bad[d]	dj[ump]	lno[remap]	redi[r]	tl[ast]
syn keyword vimCommand contained	ba[ll]	dl[ist]	lo[adview]	red[o]	tm[enu]
syn keyword vimCommand contained	bd[elete]	dr[op]	ls	redr[aw]	tn[ext]
syn keyword vimCommand contained	belowleft	ds[earch]	lu[nmap]	reg[isters]	to[pleft]
syn keyword vimCommand contained	bel[owright]	dsp[lit]	mak[e]	res[ize]	tp[revious]
syn keyword vimCommand contained	bf[irst	echoe[rr]	ma[rk]	ret[ab]	tr[ewind]
syn keyword vimCommand contained	bl[ast]	echom[sg]	marks	retu[rn]	ts[elect]
syn keyword vimCommand contained	bm[odified]	echon	mat[ch]	rew[ind]	tu[nmenu]
syn keyword vimCommand contained	bn[ext]	e[dit]	menu-disable	ri[ght]	una[bbreviate]
syn keyword vimCommand contained	bo[tright]	el[se]	menu-enable	rightabove	u[ndo]
syn keyword vimCommand contained	bp[revious]	elsei[f]	menu-<script>	rightb[elow]	unh[ide]
syn keyword vimCommand contained	brea[k]	em[eenu]	menu-script	rub[y]	unm[ap]
syn keyword vimCommand contained	breaka[dd]	emenu	menu-<silent>	rubyd[o]	unme
syn keyword vimCommand contained	breakd[el]	emenu*	menu-silent	rubyf[ile]	unmenu
syn keyword vimCommand contained	breakl[ist]	endf[unction]	menut[ranslate]	ru[ntime]	verb[ose]
syn keyword vimCommand contained	br[ewind]	en[dif]	mk[exrc]	rv[iminfo]	ve[rsion]
syn keyword vimCommand contained	bro[wse]	endw[hile]	mks[ession]	sN[ext]	vert[ical]
syn keyword vimCommand contained	bufdo	ene[w]	mkvie[w]	sal[l]	v[global]
syn keyword vimCommand contained	b[uffer]	ex	mkv[imrc]	sa[rgument]	vie[w]
syn keyword vimCommand contained	buffers	exi[t]	mod[e]	sav[eas]	vi[sual]
syn keyword vimCommand contained	bun[load]	f[ile]	m[ove]	sbN[ext]	vmapc[lear]
syn keyword vimCommand contained	bw[ipeout]	files	new	sba[ll]	vne[w]
syn keyword vimCommand contained	cN[ext]	filetype	n[ext]	sbf[irst]	vs[plit]
syn keyword vimCommand contained	ca[bbrev]	fin[d]	nmapc[lear]	sbl[ast]	vu[nmap]
syn keyword vimCommand contained	cabc[lear]	fini[sh]	noh[lsearch]	sbm[odified]	vunme
syn keyword vimCommand contained	cal[l]	fir[st]	norea[bbrev]	sbn[ext]	vunmenu
syn keyword vimCommand contained	cc	fix[del]	norm[al]	sbp[revious]	wN[ext]
syn keyword vimCommand contained	ccl[ose]	fo[ld]	nu[mber]	sbr[ewind]	wa[ll]
syn keyword vimCommand contained	cd	foldc[lose]	nun[map]	sb[uffer]	wh[ile]
syn keyword vimCommand contained	ce[nter]	folddoc[losed]	nunme	scripte[ncoding]	win
syn keyword vimCommand contained	cf[ile]	foldd[oopen]	nunmenu	scrip[tnames]	winc[md]
syn keyword vimCommand contained	cfir[st]	foldo[pen]	omapc[lear]	se[t]	windo
syn keyword vimCommand contained	c[hange]	fu[nction]	on[ly]	setf[iletype]	winp[os]
syn keyword vimCommand contained	chd[ir]	g[lobal]	o[pen]	setg[lobal]	winpos*
syn keyword vimCommand contained	che[ckpath]	go[to]	opt[ions]	setl[ocal]	win[size]
syn keyword vimCommand contained	checkt[ime]	gr[ep]	ou[nmap]	sf[ind]	wn[ext]
syn keyword vimCommand contained	cla[st]	grepa[dd]	ounme	sfir[st	wp[revous]
syn keyword vimCommand contained	cl[ist]	ha[hardcopy]	ounmenu	sh[ell]	wq
syn keyword vimCommand contained	clo[se]	hardcopy	pc[lose]	sign	wqa[ll]
syn keyword vimCommand contained	cmapc[lear]	h[elp]	ped[it]	sil[ent]	w[rite]
syn keyword vimCommand contained	cnew[er]	helpf[ind]	pe[rl]	si[malt]	ws[verb]
syn keyword vimCommand contained	cn[ext]	helpt[ags]	perld[o]	sla[st]	wv[iminfo]
syn keyword vimCommand contained	cnf[ile]	hid[e]	po[p]	sl[eep]	xa[ll]
syn keyword vimCommand contained	cnorea[bbrev]	his[tory]	pop[up]	sm[agic]	x[it]
syn keyword vimCommand contained	col[der]	ia[bbrev]	pp[op]	sn[ext]	y[ank]
syn keyword vimCommand contained	colo[rscheme]	iabc[lear]	pre[serve]	sni[ff]	z[+-^.=]
syn keyword vimCommand contained	comc[lear]	if	prev[ious]	sno[magic]
syn match   vimCommand contained	"\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	ea	imak	pdev	swb
syn keyword vimOption contained	ai	ead	imc	pex	swf
syn keyword vimOption contained	akm	eadirection	imcmdline	pexpr	switchbuf
syn keyword vimOption contained	al	eb	imi	pfn	sws
syn keyword vimOption contained	aleph	ed	iminsert	pheader	sxq
syn keyword vimOption contained	allowrevins	edcompatible	ims	pm	syn
syn keyword vimOption contained	altkeymap	ef	imsearch	popt	syntax
syn keyword vimOption contained	ar	efm	inc	previewheight	ta
syn keyword vimOption contained	ari	ei	include	previewwindow	tabstop
syn keyword vimOption contained	autoindent	ek	includeexpr	printdevice	tag
syn keyword vimOption contained	autoread	enc	incsearch	printexpr	tagbsearch
syn keyword vimOption contained	autowrite	encoding	inde	printfont	taglength
syn keyword vimOption contained	autowriteall	endofline	indentexpr	printheader	tagrelative
syn keyword vimOption contained	aw	eol	indentkeys	printoptions	tags
syn keyword vimOption contained	awa	ep	indk	pt	tagstack
syn keyword vimOption contained	background	equalalways	inex	pvh	tb
syn keyword vimOption contained	backspace	equalprg	inf	pvw	tbi
syn keyword vimOption contained	backup	errorbells	infercase	readonly	tbs
syn keyword vimOption contained	backupcopy	errorfile	insertmode	remap	tenc
syn keyword vimOption contained	backupdir	errorformat	is	report	term
syn keyword vimOption contained	backupext	esckeys	isf	restorescreen	termencoding
syn keyword vimOption contained	backupskip	et	isfname	revins	terse
syn keyword vimOption contained	balloondelay	eventignore	isi	ri	textauto
syn keyword vimOption contained	ballooneval	ex	isident	rightleft	textmode
syn keyword vimOption contained	bdir	expandtab	isk	rl	textwidth
syn keyword vimOption contained	bdlay	exrc	iskeyword	ro	tf
syn keyword vimOption contained	beval	fcl	isp	rs	tgst
syn keyword vimOption contained	bex	fcs	isprint	rtp	thesaurus
syn keyword vimOption contained	bg	fdc	joinspaces	ru	tildeop
syn keyword vimOption contained	bh	fde	js	ruf	timeout
syn keyword vimOption contained	bin	fdi	key	ruler	timeoutlen
syn keyword vimOption contained	binary	fdl	keymap	rulerformat	title
syn keyword vimOption contained	biosk	fdls	keymodel	runtimepath	titlelen
syn keyword vimOption contained	bioskey	fdm	keywordprg	sb	titleold
syn keyword vimOption contained	bk	fdn	km	sbo	titlestring
syn keyword vimOption contained	bkc	fdo	kmp	sbr	tl
syn keyword vimOption contained	bl	fdt	kp	sc	tm
syn keyword vimOption contained	bomb	fen	langmap	scb	to
syn keyword vimOption contained	breakat	fenc	langmenu	scr	toolbar
syn keyword vimOption contained	brk	fencs	laststatus	scroll	top
syn keyword vimOption contained	browsedir	ff	lazyredraw	scrollbind	tr
syn keyword vimOption contained	bs	ffs	lbr	scrolljump	ts
syn keyword vimOption contained	bsdir	fileencoding	lcs	scrolloff	tsl
syn keyword vimOption contained	bsk	fileencodings	linebreak	scrollopt	tsr
syn keyword vimOption contained	bt	fileformat	lines	scs	ttimeout
syn keyword vimOption contained	bufhidden	fileformats	linespace	sect	ttimeoutlen
syn keyword vimOption contained	buflisted	filetype	lisp	sections	ttm
syn keyword vimOption contained	buftype	fillchars	lispwords	secure	tty
syn keyword vimOption contained	cb	fk	list	sel	ttybuiltin
syn keyword vimOption contained	ccv	fkmap	listchars	selection	ttyfast
syn keyword vimOption contained	cd	fml	lm	selectmode	ttym
syn keyword vimOption contained	cdpath	fmr	lmap	sessionoptions	ttymouse
syn keyword vimOption contained	cedit	fo	loadplugins	sft	ttyscroll
syn keyword vimOption contained	cf	foldclose	lpl	sh	ttytype
syn keyword vimOption contained	ch	foldcolumn	ls	shcf	tw
syn keyword vimOption contained	charconvert	foldenable	lsp	shell	tx
syn keyword vimOption contained	cin	foldexpr	lw	shellcmdflag	uc
syn keyword vimOption contained	cindent	foldignore	lz	shellpipe	ul
syn keyword vimOption contained	cink	foldlevel	ma	shellquote	undolevels
syn keyword vimOption contained	cinkeys	foldlevelstart	magic	shellredir	updatecount
syn keyword vimOption contained	cino	foldmarker	makeef	shellslash	updatetime
syn keyword vimOption contained	cinoptions	foldmethod	makeprg	shelltype	ut
syn keyword vimOption contained	cinw	foldminlines	mat	shellxquote	vb
syn keyword vimOption contained	cinwords	foldnestmax	matchpairs	shiftround	vbs
syn keyword vimOption contained	clipboard	foldopen	matchtime	shiftwidth	vdir
syn keyword vimOption contained	cmdheight	foldtext	maxfuncdepth	shm	ve
syn keyword vimOption contained	cmdwinheight	formatoptions	maxmapdepth	shortmess	verbose
syn keyword vimOption contained	cms	formatprg	maxmem	shortname	vi
syn keyword vimOption contained	co	fp	maxmemtot	showbreak	viewdir
syn keyword vimOption contained	columns	ft	mef	showcmd	viewoptions
syn keyword vimOption contained	com	gcr	menuitems	showfulltag	viminfo
syn keyword vimOption contained	comments	gd	mfd	showmatch	virtualedit
syn keyword vimOption contained	commentstring	gdefault	mh	showmode	visualbell
syn keyword vimOption contained	compatible	gfm	mis	shq	vop
syn keyword vimOption contained	complete	gfn	ml	si	wa
syn keyword vimOption contained	confirm	gfs	mls	sidescroll	wak
syn keyword vimOption contained	consk	gfw	mm	sidescrolloff	warn
syn keyword vimOption contained	conskey	ghr	mmd	siso	wb
syn keyword vimOption contained	cp	go	mmt	sj	wc
syn keyword vimOption contained	cpo	gp	mod	slm	wcm
syn keyword vimOption contained	cpoptions	grepformat	modeline	sm	wd
syn keyword vimOption contained	cpt	grepprg	modelines	smartcase	weirdinvert
syn keyword vimOption contained	cscopepathcomp	guicursor	modifiable	smartindent	wh
syn keyword vimOption contained	cscopeprg	guifont	modified	smarttab	whichwrap
syn keyword vimOption contained	cscopetag	guifontset	more	smd	wig
syn keyword vimOption contained	cscopetagorder	guifontwide	mouse	sn	wildchar
syn keyword vimOption contained	cscopeverbose	guiheadroom	mousef	so	wildcharm
syn keyword vimOption contained	cspc	guioptions	mousefocus	softtabstop	wildignore
syn keyword vimOption contained	csprg	guipty	mousehide	sol	wildmenu
syn keyword vimOption contained	cst	helpfile	mousem	sp	wildmode
syn keyword vimOption contained	csto	helpheight	mousemodel	splitbelow	wim
syn keyword vimOption contained	csverb	hf	mouses	splitright	winaltkeys
syn keyword vimOption contained	cwh	hh	mouseshape	spr	winheight
syn keyword vimOption contained	debug	hi	mouset	sr	winminheight
syn keyword vimOption contained	deco	hid	mousetime	srr	winminwidth
syn keyword vimOption contained	def	hidden	mp	ss	winwidth
syn keyword vimOption contained	define	highlight	mps	ssl	wiv
syn keyword vimOption contained	delcombine	history	nf	ssop	wiw
syn keyword vimOption contained	dex	hk	nrformats	st	wm
syn keyword vimOption contained	dg	hkmap	nu	sta	wmh
syn keyword vimOption contained	dict	hkmapp	number	startofline	wmnu
syn keyword vimOption contained	dictionary	hkp	oft	statusline	wmw
syn keyword vimOption contained	diff	hl	osfiletype	stl	wrap
syn keyword vimOption contained	diffexpr	hls	pa	sts	wrapmargin
syn keyword vimOption contained	diffopt	hlsearch	para	su	wrapscan
syn keyword vimOption contained	digraph	ic	paragraphs	sua	write
syn keyword vimOption contained	dip	icon	paste	suffixes	writeany
syn keyword vimOption contained	dir	iconstring	pastetoggle	suffixesadd	writebackup
syn keyword vimOption contained	directory	ignorecase	patchexpr	sw	writedelay
syn keyword vimOption contained	display	im	patchmode	swapfile	ws
syn keyword vimOption contained	dy	imactivatekey	path	swapsync	ww

" These are the turn-off setting variants
syn keyword vimOption contained	loadplugins	noea	noinfercase	noro	notagbsearch
syn keyword vimOption contained	noai	noeb	noinsertmode	nors	notagrelative
syn keyword vimOption contained	noakm	noed	nois	noru	notagstack
syn keyword vimOption contained	noallowrevins	noedcompatible	nojoinspaces	noruler	notbi
syn keyword vimOption contained	noaltkeymap	noek	nojs	nosb	notbs
syn keyword vimOption contained	noar	noendofline	nolazyredraw	nosc	noterse
syn keyword vimOption contained	noari	noeol	nolbr	noscb	notextauto
syn keyword vimOption contained	noautoindent	noequalalways	nolinebreak	noscrollbind	notextmode
syn keyword vimOption contained	noautoread	noerrorbells	nolisp	noscs	notf
syn keyword vimOption contained	noautowrite	noesckeys	nolist	nosecure	notgst
syn keyword vimOption contained	noautowriteall	noet	noloadplugins	nosft	notildeop
syn keyword vimOption contained	noaw	noex	nolpl	noshellslash	notimeout
syn keyword vimOption contained	noawa	noexpandtab	nolz	noshiftround	notitle
syn keyword vimOption contained	nobackup	noexrc	noma	noshortname	noto
syn keyword vimOption contained	noballooneval	nofen	nomagic	noshowcmd	notop
syn keyword vimOption contained	nobeval	nofk	nomh	noshowfulltag	notr
syn keyword vimOption contained	nobin	nofkmap	noml	noshowmatch	nottimeout
syn keyword vimOption contained	nobinary	nofoldenable	nomod	noshowmode	nottybuiltin
syn keyword vimOption contained	nobiosk	nogd	nomodeline	nosi	nottyfast
syn keyword vimOption contained	nobioskey	nogdefault	nomodifiable	nosm	notx
syn keyword vimOption contained	nobk	noguipty	nomodified	nosmartcase	novb
syn keyword vimOption contained	nobomb	nohid	nomore	nosmartindent	novisualbell
syn keyword vimOption contained	nocf	nohidden	nomousef	nosmarttab	nowa
syn keyword vimOption contained	nocin	nohk	nomousefocus	nosmd	nowarn
syn keyword vimOption contained	nocindent	nohkmap	nomousehide	nosn	nowb
syn keyword vimOption contained	nocompatible	nohkmapp	nonu	nosol	noweirdinvert
syn keyword vimOption contained	noconfirm	nohkp	nonumber	nosplitbelow	nowildmenu
syn keyword vimOption contained	noconsk	nohls	nopaste	nosplitright	nowiv
syn keyword vimOption contained	noconskey	nohlsearch	nopreviewwindow	nospr	nowmnu
syn keyword vimOption contained	nocp	noic	nopvw	nosr	nowrap
syn keyword vimOption contained	nocscopetag	noicon	noreadonly	nossl	nowrapscan
syn keyword vimOption contained	nocscopeverbose	noignorecase	noremap	nosta	nowrite
syn keyword vimOption contained	nocst	noim	norestorescreen	nostartofline	nowriteany
syn keyword vimOption contained	nocsverb	noimc	norevins	noswapfile	nowritebackup
syn keyword vimOption contained	nodg	noimcmdline	nori	noswf	nows
syn keyword vimOption contained	nodiff	noincsearch	norightleft	nota	pvw
syn keyword vimOption contained	nodigraph	noinf	norl

" termcap codes (which can also be set)
syn keyword vimOption contained	t_AB	t_IS	t_KI	t_cm	t_kN	t_nd
syn keyword vimOption contained	t_AF	t_K1	t_KJ	t_cs	t_kP	t_op
syn keyword vimOption contained	t_AL	t_K3	t_KK	t_da	t_kb	t_se
syn keyword vimOption contained	t_CS	t_K4	t_KL	t_db	t_kd	t_so
syn keyword vimOption contained	t_CV	t_K5	t_RI	t_dl	t_ke	t_sr
syn keyword vimOption contained	t_Co	t_K6	t_RV	t_fs	t_kh	t_te
syn keyword vimOption contained	t_DL	t_K7	t_Sb	t_k1	t_kl	t_ti
syn keyword vimOption contained	t_F1	t_K8	t_Sf	t_k2	t_kr	t_ts
syn keyword vimOption contained	t_F2	t_K9	t_WP	t_k3	t_ks	t_ue
syn keyword vimOption contained	t_F3	t_KA	t_WS	t_k4	t_ku	t_us
syn keyword vimOption contained	t_F4	t_KB	t_ZH	t_k5	t_le	t_ut
syn keyword vimOption contained	t_F5	t_KC	t_ZR	t_k6	t_mb	t_vb
syn keyword vimOption contained	t_F6	t_KD	t_al	t_k7	t_md	t_ve
syn keyword vimOption contained	t_F7	t_KE	t_bc	t_k8	t_me	t_vi
syn keyword vimOption contained	t_F8	t_KF	t_cd	t_k9	t_mr	t_vs
syn keyword vimOption contained	t_F9	t_KG	t_ce	t_kD	t_ms	t_xs
syn keyword vimOption contained	t_IE	t_KH	t_cl	t_kI
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
syn keyword vimAutoEvent contained	BufAdd	BufUnload	E200	FileReadPre	RemoteReply
syn keyword vimAutoEvent contained	BufCreate	BufWinEnter	E201	FileType	StdinReadPost
syn keyword vimAutoEvent contained	BufDelete	BufWinLeave	E203	FileWriteCmd	StdinReadPre
syn keyword vimAutoEvent contained	BufEnter	BufWipeout	E204	FileWritePost	Syntax
syn keyword vimAutoEvent contained	BufFilePost	BufWrite	EncodingChanged	FileWritePre	TermChanged
syn keyword vimAutoEvent contained	BufFilePre	BufWriteCmd	FileAppendCmd	FilterReadPost	TermResponse
syn keyword vimAutoEvent contained	BufHidden	BufWritePost	FileAppendPost	FilterReadPre	User
syn keyword vimAutoEvent contained	BufLeave	BufWritePre	FileAppendPre	FilterWritePost	UserGettingBored
syn keyword vimAutoEvent contained	BufNew	Cmd-event	FileChangedRO	FilterWritePre	VimEnter
syn keyword vimAutoEvent contained	BufNewFile	CmdwinEnter	FileChangedShell	FocusGained	VimLeave
syn keyword vimAutoEvent contained	BufRead	CmdwinLeave	FileEncoding	FocusLost	VimLeavePre
syn keyword vimAutoEvent contained	BufReadCmd	CursorHold	FileReadCmd	FuncUndefined	WinEnter
syn keyword vimAutoEvent contained	BufReadPost	E135	FileReadPost	GUIEnter	WinLeave
syn keyword vimAutoEvent contained	BufReadPre	E143

" Highlight commonly used Groupnames
syn keyword vimGroup contained	Comment	Identifier	Keyword	Type	Delimiter
syn keyword vimGroup contained	Constant	Function	Exception	StorageClass	SpecialComment
syn keyword vimGroup contained	String	Statement	PreProc	Structure	Debug
syn keyword vimGroup contained	Character	Conditional	Include	Typedef	Ignore
syn keyword vimGroup contained	Number	Repeat	Define	Special	Error
syn keyword vimGroup contained	Boolean	Label	Macro	SpecialChar	Todo
syn keyword vimGroup contained	Float	Operator	PreCondit	Tag

" Default highlighting groups
syn keyword vimHLGroup contained	Cursor	Directory	Menu	Scrollbar	Tooltip
syn keyword vimHLGroup contained	CursorIM	ErrorMsg	ModeMsg	Search	VertSplit
syn keyword vimHLGroup contained	DiffAdd	FoldColumn	MoreMsg	SpecialKey	Visual
syn keyword vimHLGroup contained	DiffChange	Folded	NonText	StatusLine	VisualNOS
syn keyword vimHLGroup contained	DiffDelete	IncSearch	Normal	StatusLineNC	WarningMsg
syn keyword vimHLGroup contained	DiffText	LineNr	Question	Title	WildMenu
syn case match

" Function Names
syn keyword vimFuncName contained	MyCounter	exists	globpath	mapcheck	strlen
syn keyword vimFuncName contained	append	expand	has	match	strpart
syn keyword vimFuncName contained	argc	expandpath	hasmapto	matchend	strridx
syn keyword vimFuncName contained	argidx	filereadable	histadd	matchstr	strtrans
syn keyword vimFuncName contained	argv	filewritable	histdel	mode	submatch
syn keyword vimFuncName contained	browse	fnamemodify	histget	nextnonblank	substitute
syn keyword vimFuncName contained	bufexists	foldclosed	histnr	nr2char	synID
syn keyword vimFuncName contained	buflisted	foldclosedend	hlID	prevnonblank	synIDattr
syn keyword vimFuncName contained	bufloaded	foldlevel	hlexists	remote_expr	synIDtrans
syn keyword vimFuncName contained	bufname	foldtext	hostname	remote_peek	system
syn keyword vimFuncName contained	bufnr	foreground	iconv	remote_read	tempname
syn keyword vimFuncName contained	bufwinnr	function	indent	remote_send	tolower
syn keyword vimFuncName contained	byte2line	getbufvar	input	rename	toupper
syn keyword vimFuncName contained	char2nr	getchar	inputdialog	resolve	type
syn keyword vimFuncName contained	cindent	getcharmod	inputsecret	search	virtcol
syn keyword vimFuncName contained	col	getcwd	isdirectory	searchpair	visualmode
syn keyword vimFuncName contained	confirm	getfsize	libcall	server2client	winbufnr
syn keyword vimFuncName contained	cscope_connection	getftime	libcallnr	serverlist	wincol
syn keyword vimFuncName contained	delete	getline	line	setbufvar	winheight
syn keyword vimFuncName contained	did_filetype	getwinposx	line2byte	setline	winline
syn keyword vimFuncName contained	escape	getwinposy	lispindent	setwinvar	winnr
syn keyword vimFuncName contained	eventhandler	getwinvar	localtime	strftime	winwidth
syn keyword vimFuncName contained	executable	glob	maparg	stridx

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep	"[:|]\+"	skipwhite nextgroup=vimAddress,vimAutoCmd,vimMark,vimFilter,vimUserCmd,vimSet,vimLet,vimCommand,vimSyntax,vimExtCmd
syn match vimIsCommand	"\<\a\+\>"	contains=vimCommand
syn match vimVar		"\<[bwglsav]:\K\k*\>"
syn match vimVar contained	"\<\K\k*\>"
syn match vimFunc		"\I\i*\s*("	contains=vimFuncName,vimCommand

" Insertions And Appends: insert append
" =======================
syn region vimInsert	matchgroup=vimCommand start="^[: \t]*a\%[ppend]$"	matchgroup=vimCommand end="^\.$""
syn region vimInsert	matchgroup=vimCommand start="^[: \t]*i\%[nsert]$"	matchgroup=vimCommand end="^\.$""

" Behave!
" =======
syn match   vimBehave	"\<be\%[have]\>" skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained	mswin	xterm
syn match   vimBehaveError contained	"[^ ]\+"

" Filetypes
" =========
syn match   vimFiletype	"\<filet\%[ype]\(\s\+\I\i*\)*\(|\|$\)"	skipwhite contains=vimFTCmd,vimFTOption,vimFTError
syn match   vimFTError  contained	"\I\i*"
syn keyword vimFTCmd    contained	filet[ype]
syn keyword vimFTOption contained	on	off	indent	plugin

" Augroup : vimAugroupError removed because long augroups caused sync'ing problems.
" ======= : Trade-off: Increasing synclines with slower editing vs augroup END error checking.
syn cluster vimAugroupList	contains=vimIsCommand,vimFunction,vimFunctionError,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar
syn region  vimAugroup	start="\<aug\%[roup]\>\s\+\K\k*" end="\<aug\%[roup]\>\s\+[eE][nN][dD]\>"	contains=vimAugroupKey,vimAutoCmd,@vimAugroupList keepend
syn match   vimAugroupError	"\<aug\%[roup]\>\s\+[eE][nN][dD]\>"
syn keyword vimAugroupKey contained	aug[roup]

" Functions : Tag is provided for those who wish to highlight tagged functions
" =========
syn cluster vimFuncList	contains=vimFuncKey,Tag,vimFuncSID
syn cluster vimFuncBodyList	contains=vimIsCommand,vimFunction,vimFunctionError,vimFuncBody,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar
syn match   vimFunctionError	"\<fu\%[nction]!\=\s\+\U.\{-}("me=e-1	contains=vimFuncKey,vimFuncBlank nextgroup=vimFuncBody
syn match   vimFunction	"\<fu\%[nction]!\=\s\+\(<[sS][iI][dD]>\|s:\|\u\)\w*("me=e-1	contains=@vimFuncList nextgroup=vimFuncBody
syn region  vimFuncBody  contained	start=")"	end="\<endf"	contains=@vimFuncBodyList
syn match   vimFuncVar   contained	"a:\(\I\i*\|\d\+\)"
syn match   vimFuncSID   contained	"<[sS][iI][dD]>\|\<s:"
syn keyword vimFuncKey   contained	fu[nction]
syn match   vimFuncBlank contained	"\s\+"

syn keyword vimPattern  contained	start	skip	end

" Operators
syn match vimOper	"||\|&&\|!=\|>=\|<=\|=\~\|!\~\|>\|<\|+\|-\|=\|\." skipwhite nextgroup=vimString,vimSpecFile

" Special Filenames, Modifiers, Extension Removal
syn match vimSpecFile	"<c\(word\|WORD\)>"	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"<\([acs]file\|amatch\|abuf\)>"	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"\s%[ \t:]"ms=s+1,me=e-1	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"\s%$"ms=s+1		nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"\s%<"ms=s+1,me=e-1	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile	"#\d\+\|[#%]<\>"		nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFileMod	"\(:[phtre]\)\+"		contained

" User-Specified Commands
syn cluster vimUserCmdList	contains=vimAddress,vimSyntax,vimHighlight,vimAutoCmd,vimCmplxRepeat,vimComment,vimCtrlChar,vimEscapeBrace,vimFilter,vimFunc,vimFunction,vimIsCommand,vimMark,vimNotation,vimNumber,vimOper,vimRegion,vimRegister,vimLet,vimSet,vimSetEqual,vimSetString,vimSpecFile,vimString,vimSubst,vimSubstRep,vimSubstRange,vimSynLine
syn keyword vimUserCommand	contained	com[mand]
syn match   vimUserCmd	"\<com\%[mand]!\=\>.*$"		contains=vimUserAttrb,vimUserCommand,@vimUserCmdList
syn match   vimUserAttrb	contained	"-n\%[args]=[01*?+]"	contains=vimUserAttrbKey,vimOper
syn match   vimUserAttrb	contained	"-com\%[plete]=\(augroup\|buffer\|command\|dir\|event\|file\|help\|highlight\|menu\|option\|tag\|var\)"	contains=vimUserAttrbKey,vimUserAttrbCmplt,vimOper
syn match   vimUserAttrb	contained	"-ra\%[nge]\(=%\|=\d\+\)\="	contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-cou\%[nt]=\d\+"		contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-b\%[ang]"		contains=vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-re\%[gister]"		contains=vimOper,vimUserAttrbKey
syn keyword vimUserAttrbKey	contained	b[ang]	cou[nt]	ra[nge]
syn keyword vimUserAttrbKey	contained	com[plete]	n[args]	re[gister]
syn keyword vimUserAttrbCmplt	contained	augroup	dir	help	menu	tag
syn keyword vimUserAttrbCmplt	contained	buffer	event	highlight	option	var
syn keyword vimUserAttrbCmplt	contained	command	file

" Numbers
" =======
syn match vimNumber	"\<\d\+\([lL]\|\.\d\+\)\="
syn match vimNumber	"-\d\+\([lL]\|\.\d\+\)\="
syn match vimNumber	"\<0[xX]\x\+"
syn match vimNumber	"#\x\{6}"

" Errors
" ======
syn match  vimElseIfErr	"\<else\s\+if\>"

" Lower Priority Comments: after some vim commands...
" =======================
syn match  vimComment	excludenl +\s"[^\-:.%#=*].*$+lc=1	contains=@vimCommentGroup,vimCommentString
syn match  vimComment	+\<endif\s\+".*$+lc=5	contains=@vimCommentGroup,vimCommentString
syn match  vimComment	+\<else\s\+".*$+lc=4	contains=@vimCommentGroup,vimCommentString
syn region vimCommentString	contained oneline start='\S\s\+"'ms=s+1	end='"'

" Environment Variables
" =====================
syn match vimEnvvar	"\$\I\i*"
syn match vimEnvvar	"\${\I\i*}"

" Try to catch strings, if nothing else matches (therefore it must precede the others!)
"  vimEscapeBrace handles ["]  []"] (ie. "s don't terminate string inside [])
syn region vimEscapeBrace	oneline contained transparent	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1
syn match  vimPatSepErr	contained	"\\)"
syn match  vimPatSep	contained	"\\|"
syn region vimPatSepZone	contained transparent matchgroup=vimPatSep start="\\%\=(" skip="\\\\" end="\\)"	contains=@vimStringGroup
syn region vimPatRegion	contained transparent matchgroup=vimPatSep start="\\z\=(" end="\\)"		contains=@vimSubstList oneline
syn match  vimNotPatSep	contained	"\\\\"
syn cluster vimStringGroup	contains=vimEscapeBrace,vimPatSep,vimNotPatSep,vimPatSepErr,vimPatSepZone
syn region vimString	oneline keepend	start=+[^:a-zA-Z>!\\]"+lc=1 skip=+\\\\\|\\"+ end=+"+		contains=@vimStringGroup
syn region vimString	oneline keepend	start=+[^:a-zA-Z>!\\]'+lc=1 skip=+\\\\\|\\'+ end=+'+		contains=@vimStringGroup
syn region vimString	oneline	start=+=!+lc=1	skip=+\\\\\|\\!+ end=+!+		contains=@vimStringGroup
syn region vimString	oneline	start="=+"lc=1	skip="\\\\\|\\+" end="+"		contains=@vimStringGroup
syn region vimString	oneline	start="[^\\]+\s*[^a-zA-Z0-9. \t]"lc=1 skip="\\\\\|\\+" end="+"	contains=@vimStringGroup
syn region vimString	oneline	start="\s/\s*\A"lc=1 skip="\\\\\|\\+" end="/"		contains=@vimStringGroup
syn match  vimString	contained	+"[^"]*\\$+	skipnl nextgroup=vimStringCont
syn match  vimStringCont	contained	+\(\\\\\|.\)\{-}[^\\]"+

" Substitutions
" =============
syn cluster vimSubstList	contains=vimPatSep,vimPatRegion,vimPatSepErr,vimSubstTwoBS,vimSubstRange,vimNotation
syn cluster vimSubstRepList	contains=vimSubstSubstr,vimSubstTwoBS,vimNotation
syn cluster vimSubstList	add=vimCollection
syn match   vimSubst		"\(:\+\s*\|^\s*\||\s*\)\<s\%[ubstitute]\>"	nextgroup=vimSubstPat
syn match   vimSubst1       contained	"s\%[ubstitute]\>"		nextgroup=vimSubstPat
syn region  vimSubstPat     contained	matchgroup=vimSubstDelim start="\z([^a-zA-Z \t[\]&]\)"rs=s+1 skip="\\\\\|\\\z1" end="\z1"re=e-1,me=e-1	 contains=@vimSubstList	nextgroup=vimSubstRep4	oneline
syn region  vimSubstRep4    contained	matchgroup=vimSubstDelim start="\z(.\)" skip="\\\\\|\\\z1" end="\z1" matchgroup=vimNotation end="<[cC][rR]>" contains=@vimSubstRepList	nextgroup=vimSubstFlagErr
syn region  vimCollection   contained transparent	start="\\\@<!\[" skip="\\\[" end="\]"	contains=vimCollClass
syn match   vimCollClassErr contained	"\[:.\{-\}:\]"
syn match   vimCollClass    contained transparent	"\[:\(alnum\|alpha\|blank\|cntrl\|digit\|graph\|lower\|print\|punct\|space\|upper\|xdigit\|return\|tab\|escape\|backspace\):\]"
syn match   vimSubstSubstr  contained	"\\z\=\d"
syn match   vimSubstTwoBS   contained	"\\\\"
syn match   vimSubstFlagErr contained	"[^< \t]\+" contains=vimSubstFlags
syn match   vimSubstFlags   contained	"[&cegiIpr]\+"

" Marks, Registers, Addresses, Filters
syn match  vimMark	"[!,:]'[a-zA-Z0-9]"lc=1
syn match  vimMark	"'[a-zA-Z0-9][,!]"me=e-1
syn match  vimMark	"'[<>][,!]"me=e-1
syn match  vimMark	"\<norm\s'[a-zA-Z0-9]"lc=5
syn match  vimMark	"\<normal\s'[a-zA-Z0-9]"lc=7
syn match  vimPlainMark contained	"'[a-zA-Z0-9]"

syn match  vimRegister	'[^(,;.]"[a-zA-Z0-9\-:.%#*+=][^a-zA-Z_"]'lc=1,me=e-1
syn match  vimRegister	'\<norm\s\+"[a-zA-Z0-9]'lc=5
syn match  vimRegister	'\<normal\s\+"[a-zA-Z0-9]'lc=7
syn match  vimPlainRegister contained	'"[a-zA-Z0-9\-:.%#*+=]'

syn match  vimAddress	",[.$]"lc=1	skipwhite nextgroup=vimSubst1
syn match  vimAddress	"%\a"me=e-1	skipwhite nextgroup=vimString,vimSubst1

syn match  vimFilter contained	"^!.\{-}\(|\|$\)"	contains=vimSpecFile
syn match  vimFilter contained	"\A!.\{-}\(|\|$\)"ms=s+1	contains=vimSpecFile

" Complex repeats (:h complex-repeat)
syn match  vimCmplxRepeat		'[^a-zA-Z_/\\]q[0-9a-zA-Z"]'lc=1
syn match  vimCmplxRepeat		'@[0-9a-z".=@:]'

" Set command and associated set-options (vimOptions) with comment
syn region vimSet		matchgroup=vimCommand start="\<setlocal\|set\>" end="|"me=e-1 end="$" matchgroup=vimNotation end="<[cC][rR]>" keepend contains=vimSetEqual,vimOption,vimErrSetting,vimComment,vimSetString
syn region vimSetEqual  contained	start="="	skip="\\\\\|\\\s" end="[| \t]\|$"me=e-1 contains=vimCtrlChar,vimSetSep,vimNotation
syn region vimSetString contained	start=+="+hs=s+1	skip=+\\\\\|\\"+  end=+"+	contains=vimCtrlChar
syn match  vimSetSep    contained	"[,:]"

" Let
" ===
syn keyword vimLet		let	unl[et]	skipwhite nextgroup=vimVar

" Autocmd
" =======
syn match   vimAutoEventList	contained	"\(!\s\+\)\=\(\a\+,\)*\a\+"	contains=vimAutoEvent nextgroup=vimAutoCmdSpace
syn match   vimAutoCmdSpace	contained	"\s\+"		nextgroup=vimAutoCmdSfxList
syn match   vimAutoCmdSfxList	contained	"\S*"
syn keyword vimAutoCmd		au[tocmd] do[autocmd] doautoa[ll]	skipwhite nextgroup=vimAutoEventList

" Echo and Execute -- prefer strings!
" ================
syn region  vimEcho	oneline excludenl matchgroup=vimCommand start="\<ec\%[ho]\>" skip="\(\\\\\)*\\|" end="$\||" contains=vimFuncName,vimString,vimOper,varVar
syn region  vimExecute	oneline excludenl matchgroup=vimCommand start="\<exe\%[cute]\>" skip="\(\\\\\)*\\|" end="$\||\|<[cC][rR]>" contains=vimIsCommand,vimString,vimOper,vimVar,vimNotation
syn match   vimEchoHL	"echohl\="	skipwhite nextgroup=vimGroup,vimHLGroup,vimEchoHLNone
syn case ignore
syn keyword vimEchoHLNone	none
syn case match

" Maps
" ====
syn cluster vimMapGroup	contains=vimMapBang,vimMapLhs,vimMapMod
syn keyword vimMap	cm[ap]	map	om[ap]	skipwhite nextgroup=@vimMapGroup
syn keyword vimMap	cno[remap]	nm[ap]	ono[remap]	skipwhite nextgroup=@vimMapGroup
syn keyword vimMap	im[ap]	nn[oremap]	vm[ap]	skipwhite nextgroup=@vimMapGroup
syn keyword vimMap	ino[remap]	no[remap]	vn[oremap]	skipwhite nextgroup=@vimMapGroup
syn match   vimMapLhs    contained	"\S\+"	contains=vimNotation,vimCtrlChar
syn match   vimMapBang   contained	"!"	skipwhite nextgroup=vimMapLhs
syn match   vimMapMod    contained	"<\([lL]eader\|[pP]lug\|[sS]cript\|[sS][iI][dD]\|[uU]nique\|[sS]ilent\)\+>" skipwhite contains=vimMapModKey,vimMapModErr nextgroup=@vimMapGroup
syn case ignore
syn keyword vimMapModKey contained	leader	plug	script	sid	unique	silent
syn case match

" Menus
" =====
syn keyword vimCommand	am[enu]	cnoreme[nu]	me[nu]	noreme[nu]	vme[nu]	skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand	an[oremenu]	ime[nu]	nme[nu]	ome[nu]	vnoreme[nu]	skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand	cme[nu]	inoreme[nu]	nnoreme[nu]	onoreme[nu]		skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn match   vimMenuName	"[^ \t\\<]\+"		contained nextgroup=vimMenuNameMore
syn match   vimMenuPriority	"\d\+\(\.\d\+\)*"		contained skipwhite nextgroup=vimMenuName
syn match   vimMenuNameMore	"\\\s\|<[tT][aA][bB]>\|\\\."	contained nextgroup=vimMenuName,vimMenuNameMore contains=vimNotation
syn match   vimMenuSilent	"<[sS][iI][lL][eE][nN][tT]>"	contained skipwhite nextgroup=vimMenuName,vimMenuPriority

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam]-\)\{0,4}\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|del\%[ete]\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k\=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\|drag\|release\)>" contains=vimBracket
syn match vimNotation	"\(\\\|<lt>\)\=<\(bslash\|plug\|sid\|space\|bar\|nop\|nul\|lt\)>"		contains=vimBracket
syn match vimNotation	'\(\\\|<lt>\)\=<C-R>[0-9a-z"%#:.\-=]'he=e-1			contains=vimBracket
syn match vimNotation	'\(\\\|<lt>\)\=<\(line[12]\|count\|bang\|reg\|args\|lt\|[qf]-args\)>'	contains=vimBracket
syn match vimBracket contained	"[\\<>]"
syn case match

" Syntax
"=======
syn match   vimGroupList	contained	"@\=[^ \t,]*"	contains=vimGroupSpecial,vimPatSep
syn match   vimGroupList	contained	"@\=[^ \t,]*,"	nextgroup=vimGroupList contains=vimGroupSpecial,vimPatSep
syn keyword vimGroupSpecial	contained	ALL	ALLBUT
syn match   vimSynError	contained	"\i\+"
syn match   vimSynError	contained	"\i\+="	nextgroup=vimGroupList
syn match   vimSynContains	contained	"contains="	nextgroup=vimGroupList
syn match   vimSynNextgroup	contained	"nextgroup="	nextgroup=vimGroupList

syn match   vimSyntax	"\<sy\%[ntax]\>"		contains=vimCommand skipwhite nextgroup=vimSynType,vimComment
syn match   vimAuSyntax	contained	"\s+sy\%[ntax]"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment

" Syntax: case
syn keyword vimSynType	contained	case	skipwhite nextgroup=vimSynCase,vimSynCaseError
syn match   vimSynCaseError	contained	"\i\+"
syn keyword vimSynCase	contained	ignore	match

" Syntax: clear
syn keyword vimSynType	contained	clear	skipwhite nextgroup=vimGroupList

" Syntax: cluster
syn keyword vimSynType		contained	cluster		skipwhite nextgroup=vimClusterName
syn region  vimClusterName	contained	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" matchgroup=vimSep end="$\||" contains=vimGroupAdd,vimGroupRem,vimSynContains,vimSynError
syn match   vimGroupAdd	contained	"add="		nextgroup=vimGroupList
syn match   vimGroupRem	contained	"remove="	nextgroup=vimGroupList

" Syntax: include
syn keyword vimSynType	contained	include		skipwhite nextgroup=vimGroupList

" Syntax: keyword
syn keyword vimSynType	contained	keyword		skipwhite nextgroup=vimSynKeyRegion
syn region  vimSynKeyRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" matchgroup=vimSep end="$\||" contains=vimSynNextgroup,vimSynKeyOpt
syn match   vimSynKeyOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: match
syn keyword vimSynType	contained	match	skipwhite nextgroup=vimSynMatchRegion
syn region  vimSynMatchRegion	contained oneline keepend matchgroup=vimGroupName start="\k\+" matchgroup=vimSep end="|\|$" contains=vimMtchComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn match   vimSynMtchOpt	contained	"\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|extend\|skipnl\|fold\)\>"

" Syntax: off and on
syn keyword vimSynType	contained	off	on

" Syntax: region
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|keepend\|oneline\|extend\|skipnl\|fold\)\>"
syn match   vimSynReg	contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="	nextgroup=vimGroup,vimHLGroup
syn region  vimSynRegPat	contained extend	start="\z([[:punct:]]\)"  skip="\\\\\|\\\z1"  end="\z1"  contains=@vimSynRegPatGroup skipwhite nextgroup=vimSynPatMod,vimSynReg
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\="
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\=," nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"lc=\d\+"
syn match   vimSynPatMod	contained	"lc=\d\+," nextgroup=vimSynPatMod
syn region  vimSynPatRange	contained	start="\["	skip="\\\\\|\\]"   end="]"
syn match   vimSynNotPatRange	contained	"\\\\\|\\\["
syn match   vimMtchComment	contained	'"[^"]\+$'

" Syntax: sync
" ============
syn keyword vimSynType	contained	sync	skipwhite	nextgroup=vimSyncC,vimSyncLines,vimSyncMatch,vimSyncError,vimSyncLinecont,vimSyncRegion
syn match   vimSyncError	contained	"\i\+"
syn keyword vimSyncC	contained	ccomment	clear	fromstart
syn keyword vimSyncMatch	contained	match	skipwhite	nextgroup=vimSyncGroupName
syn keyword vimSyncRegion	contained	region	skipwhite	nextgroup=vimSynReg
syn keyword vimSyncLinecont	contained	linecont	skipwhite	nextgroup=vimSynRegPat
syn match   vimSyncLines	contained	"\(min\|max\)\=lines="	nextgroup=vimNumber
syn match   vimSyncGroupName	contained	"\k\+"	skipwhite	nextgroup=vimSyncKey
syn match   vimSyncKey	contained	"\<groupthere\|grouphere\>"	skipwhite nextgroup=vimSyncGroup
syn match   vimSyncGroup	contained	"\k\+"	skipwhite	nextgroup=vimSynRegPat,vimSyncNone
syn keyword vimSyncNone	contained	NONE

" Additional IsCommand, here by reasons of precedence
" ====================
syn match vimIsCommand	"<Bar>\s*\a\+"	transparent contains=vimCommand,vimNotation

" Highlighting
" ============
syn cluster vimHighlightCluster	contains=vimHiLink,vimHiClear,vimHiKeyList,vimComment
syn match   vimHighlight		"\<hi\%[ghlight]\>" skipwhite nextgroup=vimHiBang,@vimHighlightCluster
syn match   vimHiBang	contained	"!"	  skipwhite nextgroup=@vimHighlightCluster

syn match   vimHiGroup	contained	"\i\+"
syn case ignore
syn keyword vimHiAttrib	contained	none bold inverse italic reverse standout underline
syn keyword vimFgBgAttrib	contained	none bg background fg foreground
syn case match
syn match   vimHiAttribList	contained	"\i\+"	contains=vimHiAttrib
syn match   vimHiAttribList	contained	"\i\+,"he=e-1	contains=vimHiAttrib nextgroup=vimHiAttribList,vimHiAttrib
syn case ignore
syn keyword vimHiCtermColor	contained	black	darkgray	darkyellow	lightcyan	lightred
syn keyword vimHiCtermColor	contained	blue	darkgreen	gray	lightgray	magenta
syn keyword vimHiCtermColor	contained	brown	darkgrey	green	lightgreen	red
syn keyword vimHiCtermColor	contained	cyan	darkmagenta	grey	lightgrey	white
syn keyword vimHiCtermColor	contained	darkBlue	darkred	lightblue	lightmagenta	yellow
syn keyword vimHiCtermColor	contained	darkcyan
syn case match
syn match   vimHiFontname	contained	"[a-zA-Z\-*]\+"
syn match   vimHiGuiFontname	contained	"'[a-zA-Z\-* ]\+'"
syn match   vimHiGuiRgb	contained	"#\x\{6}"
syn match   vimHiCtermError	contained	"[^0-9]\i*"

" Highlighting: hi group key=arg ...
syn cluster vimHiCluster contains=vimHiGroup,vimHiTerm,vimHiCTerm,vimHiStartStop,vimHiCtermFgBg,vimHiGui,vimHiGuiFont,vimHiGuiFgBg,vimHiKeyError,vimNotation
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
syn region vimHiLink	contained oneline matchgroup=vimCommand start="\<\(def\s\+\)\=link\>\|\<def\>" end="$"	contains=vimHiGroup,vimGroup,vimHLGroup,vimNotation

" Control Characters
" ==================
syn match vimCtrlChar	"[--]"

" Beginners - Patterns that involve ^
" =========
syn match  vimLineComment	+^[ \t:]*".*$+		contains=@vimCommentGroup,vimCommentString,vimCommentTitle
syn match  vimCommentTitle	'"\s*\u\a*\(\s\+\u\a*\)*:'hs=s+1	contained contains=vimCommentTitleLeader,vimTodo
syn match  vimContinue	"^\s*\\"
syn match  vimCommentTitleLeader	'"\s\+'ms=s+1		contained

" Scripts
" =======

" [-- python --]
syn include @vimPythonScript <sfile>:p:h/python.vim
syn region vimPythonRegion matchgroup=vimScriptDelim start=+py\%[thon]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimPythonScript
syn region vimPythonRegion matchgroup=vimScriptDelim start=+py\%[thon]\s*<<\s*$+ end=+\.$+ contains=@vimPythonScript

" [-- tcl --]
syn include @vimTclScript <sfile>:p:h/tcl.vim
syn region vimTclRegion matchgroup=vimScriptDelim start=+tc[l]\=\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimTclScript
syn region vimTclRegion matchgroup=vimScriptDelim start=+tc[l]\=\s*<<\s*$+ end=+\.$+ contains=@vimTclScript

" Synchronize (speed)
"============
syn sync linecont	"^\s\+\\"
syn sync minlines=10	maxlines=100
syn sync match vimAugroupSyncA	groupthere NONE	"\<aug\%[roup]\>\s\+[eE][nN][dD]"

" Highlighting Settings
" ====================

" The default highlighting.
hi def link vimAuHighlight	vimHighlight
hi def link vimSubst1	vimSubst

hi def link vimAddress	vimMark
hi def link vimAugroupKey	vimCommand
"  hi def link vimAugroupError	vimError
hi def link vimAutoCmd	vimCommand
hi def link vimAutoCmdOpt	vimOption
hi def link vimAutoSet	vimCommand
hi def link vimBehave	vimCommand
hi def link vimBehaveError	vimError
hi def link vimCollClassErr	vimError
hi def link vimCommentString	vimString
hi def link vimCondHL	vimCommand
hi def link vimEchoHL	vimCommand
hi def link vimEchoHLNone	vimGroup
hi def link vimElseif	vimCondHL
hi def link vimErrSetting	vimError
hi def link vimFgBgAttrib	vimHiAttrib
hi def link vimFTCmd	vimCommand
hi def link vimFTOption	vimSynType
hi def link vimFTError	vimError
hi def link vimFunctionError	vimError
hi def link vimFuncKey	vimCommand
hi def link vimGroupAdd	vimSynOption
hi def link vimGroupRem	vimSynOption
hi def link vimHLGroup	vimGroup
hi def link vimHiAttribList	vimError
hi def link vimHiCTerm	vimHiTerm
hi def link vimHiCtermError	vimError
hi def link vimHiCtermFgBg	vimHiTerm
hi def link vimHiGroup	vimGroupName
hi def link vimHiGui	vimHiTerm
hi def link vimHiGuiFgBg	vimHiTerm
hi def link vimHiGuiFont	vimHiTerm
hi def link vimHiGuiRgb	vimNumber
hi def link vimHiKeyError	vimError
hi def link vimHiStartStop	vimHiTerm
hi def link vimHighlight	vimCommand
hi def link vimInsert	vimString
hi def link vimKeyCode	vimSpecFile
hi def link vimKeyCodeError	vimError
hi def link vimLet	vimCommand
hi def link vimLineComment	vimComment
hi def link vimMap	vimCommand
hi def link vimMapMod	vimBracket
hi def link vimMapModErr	vimError
hi def link vimMapModKey	vimFuncSID
hi def link vimMapBang	vimCommand
hi def link vimMenuNameMore	vimMenuName
hi def link vimMtchComment	vimComment
hi def link vimNotFunc	vimCommand
hi def link vimNotPatSep	vimString
hi def link vimPatSepErr	vimPatSep
hi def link vimPlainMark	vimMark
hi def link vimPlainRegister	vimRegister
hi def link vimSetString	vimString
hi def link vimSpecFileMod	vimSpecFile
hi def link vimStringCont	vimString
hi def link vimSubst	vimCommand
hi def link vimSubstFlagErr	vimError
hi def link vimSynCaseError	vimError
hi def link vimSynContains	vimSynOption
hi def link vimSynKeyOpt	vimSynOption
hi def link vimSynMtchGrp	vimSynOption
hi def link vimSynMtchOpt	vimSynOption
hi def link vimSynNextgroup	vimSynOption
hi def link vimSynNotPatRange	vimSynRegPat
hi def link vimSynPatRange	vimString
hi def link vimSynRegOpt	vimSynOption
hi def link vimSynRegPat	vimString
hi def link vimSyntax	vimCommand
hi def link vimSynType	vimSpecial
hi def link vimSyncGroup	vimGroupName
hi def link vimSyncGroupName	vimGroupName
hi def link vimUserAttrb	vimSpecial
hi def link vimUserAttrbCmplt	vimSpecial
hi def link vimUserAttrbKey	vimOption
hi def link vimUserCommand	vimCommand

hi def link vimAutoEvent	Type
hi def link vimBracket	Delimiter
hi def link vimCmplxRepeat	SpecialChar
hi def link vimCommand	Statement
hi def link vimComment	Comment
hi def link vimCommentTitle	PreProc
hi def link vimContinue	Special
hi def link vimCtrlChar	SpecialChar
hi def link vimElseIfErr	Error
hi def link vimEnvvar	PreProc
hi def link vimError	Error
hi def link vimFuncName	Function
hi def link vimFuncSID	Special
hi def link vimFuncVar	Identifier
hi def link vimGroup	Type
hi def link vimGroupSpecial	Special
hi def link vimHLMod	PreProc
hi def link vimHiAttrib	PreProc
hi def link vimHiTerm	Type
hi def link vimKeyword	Statement
hi def link vimMark	Number
hi def link vimMenuName	PreProc
hi def link vimNotation	Special
hi def link vimNumber	Number
hi def link vimOper	Operator
hi def link vimOption	PreProc
hi def link vimPatSep	SpecialChar
hi def link vimPattern	Type
hi def link vimRegister	SpecialChar
hi def link vimSep	Delimiter
hi def link vimSetSep	Statement
hi def link vimSpecFile	Identifier
hi def link vimSpecial	Type
hi def link vimStatement	Statement
hi def link vimString	String
hi def link vimSubstDelim	Delimiter
hi def link vimSubstFlags	Special
hi def link vimSubstSubstr	SpecialChar
hi def link vimSynCase	Type
hi def link vimSynCaseError	Error
hi def link vimSynError	Error
hi def link vimSynOption	Special
hi def link vimSynReg	Type
hi def link vimSyncC	Type
hi def link vimSyncError	Error
hi def link vimSyncKey	Type
hi def link vimSyncNone	Type
hi def link vimTodo	Todo
hi def link vimScriptDelim	Comment

let b:current_syntax = "vim"

" vim: ts=18
