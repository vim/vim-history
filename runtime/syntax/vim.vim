" Vim syntax file
" Language:	Vim 6.1 script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	February 25, 2002
" Version:	6.1a-01

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	DeleteFirst	comc[lear]	iabc[lear]	ptN[ext]	so[urce]
syn keyword vimCommand contained	N[ext]	compiler	if	pta[g]	sp[lit]
syn keyword vimCommand contained	Nread	conf[irm]	ij[ump]	ptf[irst]	spr[evious]
syn keyword vimCommand contained	Nw	con[tinue]	il[ist]	ptj[ump]	sr[ewind]
syn keyword vimCommand contained	P[rint]	cope[n]	imapc[lear]	ptl[ast]	sta[g]
syn keyword vimCommand contained	X	co[py]	inorea[bbrev]	ptn[ext]	star[tinsert]
syn keyword vimCommand contained	ab[breviate]	cp[revious]	is[earch]	ptp[revious]	stj[ump]
syn keyword vimCommand contained	abc[lear]	cq[uit]	isp[lit]	ptr[ewind]	st[op]
syn keyword vimCommand contained	abo[veleft]	cr[ewind]	iuna[bbrev]	pts[elect]	sts[elect]
syn keyword vimCommand contained	al[l]	cuna[bbrev]	iu[nmap]	pu[t]	sun[hide]
syn keyword vimCommand contained	arga[dd]	cu[nmap]	j[oin]	pw[d]	sus[pend]
syn keyword vimCommand contained	argd[elete]	cw[indow]	ju[mps]	pyf[ile]	sv[iew]
syn keyword vimCommand contained	argdo	delc[ommand]	k	py[thon]	syncbind
syn keyword vimCommand contained	arge[dit]	d[elete]	lan[guage]	qa[ll]	t
syn keyword vimCommand contained	argg[lobal]	delf[unction]	la[st]	q[uit]	tN[ext]
syn keyword vimCommand contained	argl[ocal]	diffg[et]	lc[d]	quita[ll]	ta[g]
syn keyword vimCommand contained	ar[gs]	diffpatch	lch[dir]	r[ead]	tags
syn keyword vimCommand contained	argu[ment]	diffpu[t]	le[ft]	rec[over]	tc[l]
syn keyword vimCommand contained	as[cii]	diffsplit	lefta[bove]	redi[r]	tcld[o]
syn keyword vimCommand contained	bN[ext]	diffthis	l[ist]	red[o]	tclf[ile]
syn keyword vimCommand contained	bad[d]	dig[raphs]	lm[ap]	redr[aw]	te[aroff]
syn keyword vimCommand contained	ba[ll]	di[splay]	lmapc[lear]	reg[isters]	tf[irst]
syn keyword vimCommand contained	bd[elete]	dj[ump]	lno[remap]	res[ize]	tj[ump]
syn keyword vimCommand contained	bel[owright]	dl[ist]	lo[adview]	ret[ab]	tl[ast]
syn keyword vimCommand contained	bf[irst	dr[op]	ls	retu[rn]	tm[enu]
syn keyword vimCommand contained	bl[ast]	ds[earch]	lu[nmap]	rew[ind]	tn[ext]
syn keyword vimCommand contained	bm[odified]	dsp[lit]	mak[e]	ri[ght]	to[pleft]
syn keyword vimCommand contained	bn[ext]	echoe[rr]	ma[rk]	rightb[elow]	tp[revious]
syn keyword vimCommand contained	bo[tright]	echom[sg]	marks	rub[y]	tr[ewind]
syn keyword vimCommand contained	bp[revious]	echon	mat[ch]	rubyd[o]	ts[elect]
syn keyword vimCommand contained	brea[k]	e[dit]	menut[ranslate]	rubyf[ile]	tu[nmenu]
syn keyword vimCommand contained	breaka[dd]	el[se]	mk[exrc]	ru[ntime]	una[bbreviate]
syn keyword vimCommand contained	breakd[el]	elsei[f]	mks[ession]	rv[iminfo]	u[ndo]
syn keyword vimCommand contained	breakl[ist]	em[enu]	mkvie[w]	sN[ext]	unh[ide]
syn keyword vimCommand contained	br[ewind]	emenu*	mkv[imrc]	sal[l]	unm[ap]
syn keyword vimCommand contained	bro[wse]	endf[unction]	mod[e]	sa[rgument]	verb[ose]
syn keyword vimCommand contained	bufdo	en[dif]	m[ove]	sav[eas]	ve[rsion]
syn keyword vimCommand contained	b[uffer]	endw[hile]	new	sbN[ext]	vert[ical]
syn keyword vimCommand contained	buffers	ene[w]	n[ext]	sba[ll]	v[global]
syn keyword vimCommand contained	bun[load]	ex	nmapc[lear]	sbf[irst]	vie[w]
syn keyword vimCommand contained	bw[ipeout]	exi[t]	noh[lsearch]	sbl[ast]	vi[sual]
syn keyword vimCommand contained	cN[ext]	f[ile]	norea[bbrev]	sbm[odified]	vmapc[lear]
syn keyword vimCommand contained	ca[bbrev]	files	norm[al]	sbn[ext]	vne[w]
syn keyword vimCommand contained	cabc[lear]	filetype	nu[mber]	sbp[revious]	vs[plit]
syn keyword vimCommand contained	cal[l]	fin[d]	nun[map]	sbr[ewind]	vu[nmap]
syn keyword vimCommand contained	cc	fini[sh]	omapc[lear]	sb[uffer]	wN[ext]
syn keyword vimCommand contained	ccl[ose]	fir[st]	on[ly]	scripte[ncoding]	wa[ll]
syn keyword vimCommand contained	cd	fix[del]	o[pen]	scrip[tnames]	wh[ile]
syn keyword vimCommand contained	ce[nter]	fo[ld]	opt[ions]	se[t]	win
syn keyword vimCommand contained	cf[ile]	foldc[lose]	ou[nmap]	setf[iletype]	winc[md]
syn keyword vimCommand contained	cfir[st]	folddoc[losed]	pc[lose]	setg[lobal]	windo
syn keyword vimCommand contained	c[hange]	foldd[oopen]	ped[it]	setl[ocal]	winp[os]
syn keyword vimCommand contained	chd[ir]	foldo[pen]	pe[rl]	sf[ind]	winpos*
syn keyword vimCommand contained	che[ckpath]	fu[nction]	perld[o]	sfir[st	win[size]
syn keyword vimCommand contained	checkt[ime]	g[lobal]	po[p]	sh[ell]	wn[ext]
syn keyword vimCommand contained	cla[st]	go[to]	pop[up]	sign	wp[revous]
syn keyword vimCommand contained	cl[ist]	gr[ep]	pp[op]	sil[ent]	wq
syn keyword vimCommand contained	clo[se]	grepa[dd]	pre[serve]	si[malt]	wqa[ll]
syn keyword vimCommand contained	cmapc[lear]	ha[rdcopy]	prev[ious]	sla[st]	w[rite]
syn keyword vimCommand contained	cnew[er]	h[elp]	p[rint]	sl[eep]	ws[verb]
syn keyword vimCommand contained	cn[ext]	helpf[ind]	prompt	sm[agic]	wv[iminfo]
syn keyword vimCommand contained	cnf[ile]	helpt[ags]	promptf[ind]	sn[ext]	xa[ll]
syn keyword vimCommand contained	cnorea[bbrev]	hid[e]	promptr[epl]	sni[ff]	x[it]
syn keyword vimCommand contained	col[der]	his[tory]	ps[earch]	sno[magic]	y[ank]
syn keyword vimCommand contained	colo[rscheme]	ia[bbrev]
syn match   vimCommand contained	"\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	ead	imcmdline	pdev	swb
syn keyword vimOption contained	ai	eadirection	imd	pex	swf
syn keyword vimOption contained	akm	eb	imdisable	pexpr	switchbuf
syn keyword vimOption contained	al	ed	imi	pfn	sws
syn keyword vimOption contained	aleph	edcompatible	iminsert	pheader	sxq
syn keyword vimOption contained	allowrevins	ef	ims	pm	syn
syn keyword vimOption contained	altkeymap	efm	imsearch	popt	syntax
syn keyword vimOption contained	ar	ei	inc	previewheight	ta
syn keyword vimOption contained	ari	ek	include	previewwindow	tabstop
syn keyword vimOption contained	autoindent	enc	includeexpr	printdevice	tag
syn keyword vimOption contained	autoread	encoding	incsearch	printexpr	tagbsearch
syn keyword vimOption contained	autowrite	endofline	inde	printfont	taglength
syn keyword vimOption contained	autowriteall	eol	indentexpr	printheader	tagrelative
syn keyword vimOption contained	aw	ep	indentkeys	printoptions	tags
syn keyword vimOption contained	awa	equalalways	indk	pt	tagstack
syn keyword vimOption contained	background	equalprg	inex	pvh	tb
syn keyword vimOption contained	backspace	errorbells	inf	pvw	tbi
syn keyword vimOption contained	backup	errorfile	infercase	readonly	tbs
syn keyword vimOption contained	backupcopy	errorformat	insertmode	remap	tenc
syn keyword vimOption contained	backupdir	esckeys	is	report	term
syn keyword vimOption contained	backupext	et	isf	restorescreen	termencoding
syn keyword vimOption contained	backupskip	eventignore	isfname	revins	terse
syn keyword vimOption contained	balloondelay	ex	isi	ri	textauto
syn keyword vimOption contained	ballooneval	expandtab	isident	rightleft	textmode
syn keyword vimOption contained	bdir	exrc	isk	rl	textwidth
syn keyword vimOption contained	bdlay	fcl	iskeyword	ro	tf
syn keyword vimOption contained	beval	fcs	isp	rs	tgst
syn keyword vimOption contained	bex	fdc	isprint	rtp	thesaurus
syn keyword vimOption contained	bg	fde	joinspaces	ru	tildeop
syn keyword vimOption contained	bh	fdi	js	ruf	timeout
syn keyword vimOption contained	bin	fdl	key	ruler	timeoutlen
syn keyword vimOption contained	binary	fdls	keymap	rulerformat	title
syn keyword vimOption contained	biosk	fdm	keymodel	runtimepath	titlelen
syn keyword vimOption contained	bioskey	fdn	keywordprg	sb	titleold
syn keyword vimOption contained	bk	fdo	km	sbo	titlestring
syn keyword vimOption contained	bkc	fdt	kmp	sbr	tl
syn keyword vimOption contained	bl	fen	kp	sc	tm
syn keyword vimOption contained	bomb	fenc	langmap	scb	to
syn keyword vimOption contained	breakat	fencs	langmenu	scr	toolbar
syn keyword vimOption contained	brk	ff	laststatus	scroll	top
syn keyword vimOption contained	browsedir	ffs	lazyredraw	scrollbind	tr
syn keyword vimOption contained	bs	fileencoding	lbr	scrolljump	ts
syn keyword vimOption contained	bsdir	fileencodings	lcs	scrolloff	tsl
syn keyword vimOption contained	bsk	fileformat	linebreak	scrollopt	tsr
syn keyword vimOption contained	bt	fileformats	lines	scs	ttimeout
syn keyword vimOption contained	bufhidden	filetype	linespace	sect	ttimeoutlen
syn keyword vimOption contained	buflisted	fillchars	lisp	sections	ttm
syn keyword vimOption contained	buftype	fk	lispwords	secure	tty
syn keyword vimOption contained	cb	fkmap	list	sel	ttybuiltin
syn keyword vimOption contained	ccv	fml	listchars	selection	ttyfast
syn keyword vimOption contained	cd	fmr	lm	selectmode	ttym
syn keyword vimOption contained	cdpath	fo	lmap	sessionoptions	ttymouse
syn keyword vimOption contained	cedit	foldclose	loadplugins	sft	ttyscroll
syn keyword vimOption contained	cf	foldcolumn	lpl	sh	ttytype
syn keyword vimOption contained	ch	foldenable	ls	shcf	tw
syn keyword vimOption contained	charconvert	foldexpr	lsp	shell	tx
syn keyword vimOption contained	cin	foldignore	lw	shellcmdflag	uc
syn keyword vimOption contained	cindent	foldlevel	lz	shellpipe	ul
syn keyword vimOption contained	cink	foldlevelstart	ma	shellquote	undolevels
syn keyword vimOption contained	cinkeys	foldmarker	magic	shellredir	updatecount
syn keyword vimOption contained	cino	foldmethod	makeef	shellslash	updatetime
syn keyword vimOption contained	cinoptions	foldminlines	makeprg	shelltype	ut
syn keyword vimOption contained	cinw	foldnestmax	mat	shellxquote	vb
syn keyword vimOption contained	cinwords	foldopen	matchpairs	shiftround	vbs
syn keyword vimOption contained	clipboard	foldtext	matchtime	shiftwidth	vdir
syn keyword vimOption contained	cmdheight	formatoptions	maxfuncdepth	shm	ve
syn keyword vimOption contained	cmdwinheight	formatprg	maxmapdepth	shortmess	verbose
syn keyword vimOption contained	cms	fp	maxmem	shortname	vi
syn keyword vimOption contained	co	ft	maxmemtot	showbreak	viewdir
syn keyword vimOption contained	columns	gcr	mef	showcmd	viewoptions
syn keyword vimOption contained	com	gd	menuitems	showfulltag	viminfo
syn keyword vimOption contained	comments	gdefault	mfd	showmatch	virtualedit
syn keyword vimOption contained	commentstring	gfm	mh	showmode	visualbell
syn keyword vimOption contained	compatible	gfn	mis	shq	vop
syn keyword vimOption contained	complete	gfs	ml	si	wa
syn keyword vimOption contained	confirm	gfw	mls	sidescroll	wak
syn keyword vimOption contained	consk	ghr	mm	sidescrolloff	warn
syn keyword vimOption contained	conskey	go	mmd	siso	wb
syn keyword vimOption contained	cp	gp	mmt	sj	wc
syn keyword vimOption contained	cpo	grepformat	mod	slm	wcm
syn keyword vimOption contained	cpoptions	grepprg	modeline	sm	wd
syn keyword vimOption contained	cpt	guicursor	modelines	smartcase	weirdinvert
syn keyword vimOption contained	cscopepathcomp	guifont	modifiable	smartindent	wh
syn keyword vimOption contained	cscopeprg	guifontset	modified	smarttab	whichwrap
syn keyword vimOption contained	cscopetag	guifontwide	more	smd	wig
syn keyword vimOption contained	cscopetagorder	guiheadroom	mouse	sn	wildchar
syn keyword vimOption contained	cscopeverbose	guioptions	mousef	so	wildcharm
syn keyword vimOption contained	cspc	guipty	mousefocus	softtabstop	wildignore
syn keyword vimOption contained	csprg	helpfile	mousehide	sol	wildmenu
syn keyword vimOption contained	cst	helpheight	mousem	sp	wildmode
syn keyword vimOption contained	csto	hf	mousemodel	splitbelow	wim
syn keyword vimOption contained	csverb	hh	mouses	splitright	winaltkeys
syn keyword vimOption contained	cwh	hi	mouseshape	spr	winheight
syn keyword vimOption contained	debug	hid	mouset	sr	winminheight
syn keyword vimOption contained	deco	hidden	mousetime	srr	winminwidth
syn keyword vimOption contained	def	highlight	mp	ss	winwidth
syn keyword vimOption contained	define	history	mps	ssl	wiv
syn keyword vimOption contained	delcombine	hk	nf	ssop	wiw
syn keyword vimOption contained	dex	hkmap	nrformats	st	wm
syn keyword vimOption contained	dg	hkmapp	nu	sta	wmh
syn keyword vimOption contained	dict	hkp	number	startofline	wmnu
syn keyword vimOption contained	dictionary	hl	oft	statusline	wmw
syn keyword vimOption contained	diff	hls	osfiletype	stl	wrap
syn keyword vimOption contained	diffexpr	hlsearch	pa	sts	wrapmargin
syn keyword vimOption contained	diffopt	ic	para	su	wrapscan
syn keyword vimOption contained	digraph	icon	paragraphs	sua	write
syn keyword vimOption contained	dip	iconstring	paste	suffixes	writeany
syn keyword vimOption contained	dir	ignorecase	pastetoggle	suffixesadd	writebackup
syn keyword vimOption contained	directory	im	patchexpr	sw	writedelay
syn keyword vimOption contained	display	imactivatekey	patchmode	swapfile	ws
syn keyword vimOption contained	dy	imak	path	swapsync	ww
syn keyword vimOption contained	ea	imc

" These are the turn-off setting variants
syn keyword vimOption contained	loadplugins	nodisable	noincsearch	norightleft	nota
syn keyword vimOption contained	noai	noea	noinf	norl	notagbsearch
syn keyword vimOption contained	noakm	noeb	noinfercase	noro	notagrelative
syn keyword vimOption contained	noallowrevins	noed	noinsertmode	nors	notagstack
syn keyword vimOption contained	noaltkeymap	noedcompatible	nois	noru	notbi
syn keyword vimOption contained	noar	noek	nojoinspaces	noruler	notbs
syn keyword vimOption contained	noari	noendofline	nojs	nosb	noterse
syn keyword vimOption contained	noautoindent	noeol	nolazyredraw	nosc	notextauto
syn keyword vimOption contained	noautoread	noequalalways	nolbr	noscb	notextmode
syn keyword vimOption contained	noautowrite	noerrorbells	nolinebreak	noscrollbind	notf
syn keyword vimOption contained	noautowriteall	noesckeys	nolisp	noscs	notgst
syn keyword vimOption contained	noaw	noet	nolist	nosecure	notildeop
syn keyword vimOption contained	noawa	noex	noloadplugins	nosft	notimeout
syn keyword vimOption contained	nobackup	noexpandtab	nolpl	noshellslash	notitle
syn keyword vimOption contained	noballooneval	noexrc	nolz	noshiftround	noto
syn keyword vimOption contained	nobeval	nofen	noma	noshortname	notop
syn keyword vimOption contained	nobin	nofk	nomagic	noshowcmd	notr
syn keyword vimOption contained	nobinary	nofkmap	nomh	noshowfulltag	nottimeout
syn keyword vimOption contained	nobiosk	nofoldenable	noml	noshowmatch	nottybuiltin
syn keyword vimOption contained	nobioskey	nogd	nomod	noshowmode	nottyfast
syn keyword vimOption contained	nobk	nogdefault	nomodeline	nosi	notx
syn keyword vimOption contained	nobomb	noguipty	nomodifiable	nosm	novb
syn keyword vimOption contained	nocf	nohid	nomodified	nosmartcase	novisualbell
syn keyword vimOption contained	nocin	nohidden	nomore	nosmartindent	nowa
syn keyword vimOption contained	nocindent	nohk	nomousef	nosmarttab	nowarn
syn keyword vimOption contained	nocompatible	nohkmap	nomousefocus	nosmd	nowb
syn keyword vimOption contained	noconfirm	nohkmapp	nomousehide	nosn	noweirdinvert
syn keyword vimOption contained	noconsk	nohkp	nonu	nosol	nowildmenu
syn keyword vimOption contained	noconskey	nohls	nonumber	nosplitbelow	nowiv
syn keyword vimOption contained	nocp	nohlsearch	nopaste	nosplitright	nowmnu
syn keyword vimOption contained	nocscopetag	noic	nopreviewwindow	nospr	nowrap
syn keyword vimOption contained	nocscopeverbose	noicon	nopvw	nosr	nowrapscan
syn keyword vimOption contained	nocst	noignorecase	noreadonly	nossl	nowrite
syn keyword vimOption contained	nocsverb	noim	noremap	nosta	nowriteany
syn keyword vimOption contained	nodg	noimc	norestorescreen	nostartofline	nowritebackup
syn keyword vimOption contained	nodiff	noimcmdline	norevins	noswapfile	nows
syn keyword vimOption contained	nodigraph	noimd	nori	noswf	pvw

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
syn keyword vimGroup contained	Character	Conditional	Include	Typedef	Underlined
syn keyword vimGroup contained	Number	Repeat	Define	Special	Ignore
syn keyword vimGroup contained	Boolean	Label	Macro	SpecialChar	Error
syn keyword vimGroup contained	Float	Operator	PreCondit	Tag	Todo

" Default highlighting groups
syn keyword vimHLGroup contained	Cursor	Directory	Menu	Scrollbar	Tooltip
syn keyword vimHLGroup contained	CursorIM	ErrorMsg	ModeMsg	Search	VertSplit
syn keyword vimHLGroup contained	DiffAdd	FoldColumn	MoreMsg	SpecialKey	Visual
syn keyword vimHLGroup contained	DiffChange	Folded	NonText	StatusLine	VisualNOS
syn keyword vimHLGroup contained	DiffDelete	IncSearch	Normal	StatusLineNC	WarningMsg
syn keyword vimHLGroup contained	DiffText	LineNr	Question	Title	WildMenu
syn case match

" Function Names
syn keyword vimFuncName contained	MyCounter	executable	globpath	mapcheck	stridx
syn keyword vimFuncName contained	append	exists	has	match	strlen
syn keyword vimFuncName contained	argc	expand	hasmapto	matchend	strpart
syn keyword vimFuncName contained	argidx	filereadable	histadd	matchstr	strridx
syn keyword vimFuncName contained	argv	filewritable	histdel	mode	strtrans
syn keyword vimFuncName contained	browse	fnamemodify	histget	nextnonblank	submatch
syn keyword vimFuncName contained	bufexists	foldclosed	histnr	nr2char	substitute
syn keyword vimFuncName contained	buflisted	foldclosedend	hlID	prevnonblank	synID
syn keyword vimFuncName contained	bufloaded	foldlevel	hlexists	remote_expr	synIDattr
syn keyword vimFuncName contained	bufname	foldtext	hostname	remote_foreground	synIDtrans
syn keyword vimFuncName contained	bufnr	foreground	iconv	remote_peek	system
syn keyword vimFuncName contained	bufwinnr	function	indent	remote_read	tempname
syn keyword vimFuncName contained	byte2line	getbufvar	input	remote_send	tolower
syn keyword vimFuncName contained	char2nr	getchar	inputdialog	rename	toupper
syn keyword vimFuncName contained	cindent	getcharmod	inputsecret	resolve	type
syn keyword vimFuncName contained	col	getcwd	isdirectory	search	virtcol
syn keyword vimFuncName contained	confirm	getfsize	libcall	searchpair	visualmode
syn keyword vimFuncName contained	cscope_connection	getftime	libcallnr	server2client	winbufnr
syn keyword vimFuncName contained	cursor	getline	line	serverlist	wincol
syn keyword vimFuncName contained	delete	getwinposx	line2byte	setbufvar	winheight
syn keyword vimFuncName contained	did_filetype	getwinposy	lispindent	setline	winline
syn keyword vimFuncName contained	escape	getwinvar	localtime	setwinvar	winnr
syn keyword vimFuncName contained	eventhandler	glob	maparg	strftime	winwidth

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
syn match   vimUserAttrb	contained	"-com\%[plete]="		contains=vimUserAttrbKey,vimOper nextgroup=vimUserAttrbCmplt,vimUserCmdError
syn match   vimUserAttrb	contained	"-ra\%[nge]\(=%\|=\d\+\)\="	contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-cou\%[nt]=\d\+"		contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-bang\=\>"		contains=vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-bar\>"		contains=vimOper,vimUserAttrbKey
syn match   vimUserAttrb	contained	"-re\%[gister]\>"		contains=vimOper,vimUserAttrbKey
syn match   vimUserCmdError	contained	"\S\+\>"
syn keyword vimUserAttrbKey	contained	bar	ban[g]	cou[nt]	ra[nge]
syn keyword vimUserAttrbKey	contained	com[plete]	n[args]	re[gister]
syn keyword vimUserAttrbCmplt	contained	augroup	environment	function	mapping	tag
syn keyword vimUserAttrbCmplt	contained	buffer	event	help	menu	tag_listfiles
syn keyword vimUserAttrbCmplt	contained	command	expression	highlight	option	var
syn keyword vimUserAttrbCmplt	contained	dir	file

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
syn match   vimSubst		"\(:\+\s*\|^\s*\||\s*\)\<s\%[ubstitute][:[:alpha:]]\@!" nextgroup=vimSubstPat
syn match   vimSubst1       contained	"s\%[ubstitute]\>"		nextgroup=vimSubstPat
syn region  vimSubstPat     contained	matchgroup=vimSubstDelim start="\z([^a-zA-Z( \t[\]&]\)"rs=s+1 skip="\\\\\|\\\z1" end="\z1"re=e-1,me=e-1	 contains=@vimSubstList	nextgroup=vimSubstRep4	oneline
syn region  vimSubstRep4    contained	matchgroup=vimSubstDelim start="\z(.\)" skip="\\\\\|\\\z1" end="\z1" matchgroup=vimNotation end="<[cC][rR]>" contains=@vimSubstRepList	nextgroup=vimSubstFlagErr
syn region  vimCollection   contained transparent	start="\\\@<!\[" skip="\\\[" end="\]"	contains=vimCollClass
syn match   vimCollClassErr contained	"\[:.\{-\}:\]"
syn match   vimCollClass    contained transparent	"\[:\(alnum\|alpha\|blank\|cntrl\|digit\|graph\|lower\|print\|punct\|space\|upper\|xdigit\|return\|tab\|escape\|backspace\):\]"
syn match   vimSubstSubstr  contained	"\\z\=\d"
syn match   vimSubstTwoBS   contained	"\\\\"
syn match   vimSubstFlagErr contained	"[^< \t\r]\+" contains=vimSubstFlags
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
syn match   vimMapMod    contained	"\c<\(buffer\|\(local\)\=leader\|plug\|script\|sid\|unique\|silent\)\+>" skipwhite contains=vimMapModKey,vimMapModErr nextgroup=@vimMapGroup
syn case ignore
syn keyword vimMapModKey contained	buffer	leader	localleader	plug	script	sid	silent	unique
syn case match

" Menus
" =====
syn keyword vimCommand	am[enu]	cnoreme[nu]	me[nu]	noreme[nu]	vme[nu]	skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand	an[oremenu]	ime[nu]	nme[nu]	ome[nu]	vnoreme[nu]	skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand	cme[nu]	inoreme[nu]	nnoreme[nu]	onoreme[nu]		skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand	aun[menu]	iunme[nu]	ounme[nu]	unme[nu]	vunme[nu]	skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand	cunme[nu]	nunme[nu]				skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn match   vimMenuName	"[^ \t\\<]\+"		contained nextgroup=vimMenuNameMore,vimMenuMap
syn match   vimMenuPriority	"\d\+\(\.\d\+\)*"		contained skipwhite nextgroup=vimMenuName
syn match   vimMenuNameMore	"\\\s\|<[tT][aA][bB]>\|\\\."	contained nextgroup=vimMenuName,vimMenuNameMore contains=vimNotation
syn match   vimMenuSilent	"<[sS][iI][lL][eE][nN][tT]>"	contained skipwhite nextgroup=vimMenuName,vimMenuPriority
syn match   vimMenuMap	"\t"		contained skipwhite nextgroup=@vimMapGroup

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam]-\)\{0,4}x\=\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|k\=del\%[ete]\|bs\|backspace\|tab\|esc\|right\|left\|help\|undo\|insert\|ins\|k\=home\|k\=end\|kplus\|kminus\|kdivide\|kmultiply\|kenter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\)\=\(drag\|release\)\=>"	contains=vimBracket
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
syn match   vimSynContains	contained	"\<contain\(s\|edin\)="	nextgroup=vimGroupList
syn match   vimSynKeyContainedin	contained	"\<containedin="	nextgroup=vimGroupList
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
syn cluster vimSynKeyGroup	contains=vimSynNextgroup,vimSynKeyOpt,vimSynKeyContainedin
syn keyword vimSynType	contained	keyword		skipwhite nextgroup=vimSynKeyRegion
syn region  vimSynKeyRegion	contained keepend	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" matchgroup=vimSep end="|\|$" contains=@vimSynKeyGroup
syn match   vimSynKeyOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: match
syn cluster vimSynMtchGroup	contains=vimMtchComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn keyword vimSynType	contained	match	skipwhite nextgroup=vimSynMatchRegion
syn region  vimSynMatchRegion	contained keepend	matchgroup=vimGroupName start="\k\+" matchgroup=vimSep end="|\|$" contains=@vimSynMtchGroup
syn match   vimSynMtchOpt	contained	"\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|extend\|skipnl\|fold\)\>"

" Syntax: off and on
syn keyword vimSynType	contained	enable	list	manual	off	on	reset

" Syntax: region
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn cluster vimSynRegGroup	contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained keepend	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="|\|$" contains=@vimSynRegGroup
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
syn region vimString	start="^\s*\\\z(['"]\)" skip='\\\\\|\\\z1' end="\z1" oneline keepend contains=@vimStringGroup,vimContinue
syn match  vimCommentTitleLeader	'"\s\+'ms=s+1		contained

" Scripts  : perl,ruby : Benoit Cerrina
" =======    python,tcl: Johannes Zellner

" [-- perl --]
if filereadable(expand("<sfile>:p:h")."/perl.vim")
  unlet! b:current_syntax
  syn include @vimPerlScript <sfile>:p:h/perl.vim
  syn region vimPerlRegion matchgroup=vimScriptDelim start=+pe\%[rl]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimPerlScript
  syn region vimPerlRegion matchgroup=vimScriptDelim start=+pe\%[rl]\s*<<\s*$+ end=+\.$+ contains=@vimPerlScript
endif

" [-- ruby --]
if filereadable(expand("<sfile>:p:h")."/ruby.vim")
  unlet! b:current_syntax
  syn include @vimRubyScript <sfile>:p:h/ruby.vim
  syn region vimRubyRegion matchgroup=vimScriptDelim start=+rub[y]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimRubyScript
  syn region vimRubyRegion matchgroup=vimScriptDelim start=+rub[y]\s*<<\s*$+ end=+\.$+ contains=@vimRubyScript
endif

" [-- python --]
if filereadable(expand("<sfile>:p:h")."/python.vim")
  unlet! b:current_syntax
  syn include @vimPythonScript <sfile>:p:h/python.vim
  syn region vimPythonRegion matchgroup=vimScriptDelim start=+py\%[thon]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimPythonScript
  syn region vimPythonRegion matchgroup=vimScriptDelim start=+py\%[thon]\s*<<\s*$+ end=+\.$+ contains=@vimPythonScript
endif

" [-- tcl --]
if filereadable(expand("<sfile>:p:h")."/tcl.vim")
  unlet! b:current_syntax
  syn include @vimTclScript <sfile>:p:h/tcl.vim
  syn region vimTclRegion matchgroup=vimScriptDelim start=+tc[l]\=\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimTclScript
  syn region vimTclRegion matchgroup=vimScriptDelim start=+tc[l]\=\s*<<\s*$+ end=+\.$+ contains=@vimTclScript
endif

" Synchronize (speed)
"============
if exists("b:vim_minlines")
 exe "syn sync minlines=".b:vim_minlines
endif
if exists("b:vim_maxlines")
 exe "syn sync maxlines=".b:vim_maxlines
else
 syn sync maxlines=60
endif
syn sync linecont	"^\s\+\\"
syn sync match vimAugroupSyncA	groupthere NONE	"\<aug\%[roup]\>\s\+[eE][nN][dD]"

" Highlighting Settings
" ====================

" The default highlighting.
hi def link vimAuHighlight	vimHighlight
hi def link vimSubst1	vimSubst
hi def link vimBehaveModel	vimBehave

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
hi def link vimSubstTwoBS	vimString
hi def link vimSubstFlagErr	vimError
hi def link vimSynCaseError	vimError
hi def link vimSynContains	vimSynOption
hi def link vimSynKeyContainedin	vimSynContains
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
hi def link vimScriptDelim	Comment
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
hi def link vimUserCmdError	Error

let b:current_syntax = "vim"

" vim: ts=18
