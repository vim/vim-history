" Vim syntax file
" Language:	Vim 6.0ab script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	April 20, 2001
" Version:	6.0ab-02

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	N[ext]	con[tinue]	is[earch]	pre[serve]	spr[evious]
syn keyword vimCommand contained	Nread	co[py]	isp[lit]	prev[ious]	sr[ewind]
syn keyword vimCommand contained	Nw	cp[revious]	iuna[bbrev]	p[rint]	sta[g]
syn keyword vimCommand contained	P[rint]	cq[uit]	iu[nmap]	promptf[ind]	star[tinsert]
syn keyword vimCommand contained	X	cr[ewind]	iunme	promptr[epl]	stj[ump]
syn keyword vimCommand contained	ab[breviate]	cuna[bbrev]	iunmenu	ps[earch]	st[op]
syn keyword vimCommand contained	abc[lear]	cu[nmap]	j[oin]	ptN[ext]	sts[elect]
syn keyword vimCommand contained	al[l]	cunme	ju[mps]	pta[g]	sun[hide]
syn keyword vimCommand contained	am	cunmenu	k	ptf[irst]	sus[pend]
syn keyword vimCommand contained	amenu	cw[indow]	lan[guage]	ptj[ump]	sv[iew]
syn keyword vimCommand contained	an	delc[ommand]	la[st]	ptl[ast]	syncbind
syn keyword vimCommand contained	anoremenu	d[elete]	lc[d]	ptn[ext]	t
syn keyword vimCommand contained	arga[dd]	delf[unction]	lch[dir]	ptp[revious]	tN[ext]
syn keyword vimCommand contained	argd[elete]	diffg[et]	le[ft]	ptr[ewind]	ta[g]
syn keyword vimCommand contained	argdo	diffpatch	l[ist]	pts[elect]	tags
syn keyword vimCommand contained	arge[dit]	diffpu[t]	lm[ap]	pu[t]	tc[l]
syn keyword vimCommand contained	argg[lobal]	diffsplit	lmapc[lear]	pw[d]	tcld[o]
syn keyword vimCommand contained	argl[ocal]	dig[raphs]	lno[remap]	pyf[ile]	tclf[ile]
syn keyword vimCommand contained	ar[gs]	di[splay]	lo[adview]	py[thon]	te[aroff]
syn keyword vimCommand contained	argu[ment]	dj[ump]	ls	qa[ll]	tf[irst]
syn keyword vimCommand contained	as[cii]	dl[ist]	lu[nmap]	q[uit]	tj[ump]
syn keyword vimCommand contained	aun	dr[op]	mak[e]	quita[ll]	tl[ast]
syn keyword vimCommand contained	aunmenu	ds[earch]	ma[rk]	r[ead]	tm[enu]
syn keyword vimCommand contained	bN[ext]	dsp[lit]	marks	rec[over]	tn[ext]
syn keyword vimCommand contained	bad[d]	echoe[rr]	mat[ch]	redi[r]	to[pleft]
syn keyword vimCommand contained	ba[ll]	echom[sg]	me	red[o]	tp[revious]
syn keyword vimCommand contained	bd[elete]	echon	menu	reg[isters]	tr[ewind]
syn keyword vimCommand contained	bf[irst	e[dit]	menu-disable	res[ize]	ts[elect]
syn keyword vimCommand contained	bl[ast]	el[se]	menu-enable	ret[ab]	tu[nmenu]
syn keyword vimCommand contained	bm[odified]	elsei[f]	menu-<script>	retu[rn]	una[bbreviate]
syn keyword vimCommand contained	bn[ext]	em[eenu]	menut[rans]	rew[ind]	u[ndo]
syn keyword vimCommand contained	bo[tright]	emenu	mk[exrc]	ri[ght]	unh[ide]
syn keyword vimCommand contained	bp[revious]	endf[unction]	mks[ession]	rub[y]	unm[ap]
syn keyword vimCommand contained	brea[k]	en[dif]	mkvie[w]	rubyd[o]	unme
syn keyword vimCommand contained	breaka[dd]	endw[hile]	mkv[imrc]	rubyf[ile]	unmenu
syn keyword vimCommand contained	breakd[el]	ene[w]	mod[e]	ru[ntime]	uns[ign]
syn keyword vimCommand contained	breakl[ist]	ex	m[ove]	rv[iminfo]	verb[ose]
syn keyword vimCommand contained	br[ewind]	exi[t]	new	sN[ext]	ve[rsion]
syn keyword vimCommand contained	bro[wse]	f[ile]	n[ext]	sal[l]	vert[ical]
syn keyword vimCommand contained	bufdo	files	nmapc[lear]	sa[rgument]	v[global]
syn keyword vimCommand contained	b[uffer]	filetype	nme	sav[eas]	vie[w]
syn keyword vimCommand contained	buffers	fin[d]	nmenu	sbN[ext]	vi[sual]
syn keyword vimCommand contained	bun[load]	fini[sh]	nnoreme	sba[ll]	vmapc[lear]
syn keyword vimCommand contained	bw[ipeout]	fir[st]	nnoremenu	sbf[irst]	vme
syn keyword vimCommand contained	cN[ext]	fix[del]	noh[lsearch]	sbl[ast]	vmenu
syn keyword vimCommand contained	ca[bbrev]	fo[ld]	norea[bbrev]	sbm[odified]	vne[w]
syn keyword vimCommand contained	cabc[lear]	foldc[lose]	noreme	sbn[ext]	vnoreme
syn keyword vimCommand contained	cal[l]	folddoc[losed]	noremenu	sbp[revious]	vnoremenu
syn keyword vimCommand contained	cc	foldd[oopen]	norm[al]	sbr[ewind]	vs[plit]
syn keyword vimCommand contained	cd	foldo[pen]	nu[mber]	sb[uffer]	vu[nmap]
syn keyword vimCommand contained	ce[nter]	fu[nction]	nun[map]	scripte[ncoding]	vunme
syn keyword vimCommand contained	cf[ile]	g[lobal]	nunme	scrip[tnames]	vunmenu
syn keyword vimCommand contained	cfir[st]	go[to]	nunmenu	se[t]	wN[ext]
syn keyword vimCommand contained	c[hange]	gr[ep]	omapc[lear]	setf[iletype]	wa[ll]
syn keyword vimCommand contained	chd[ir]	grepa[dd]	ome	setg[lobal]	wh[ile]
syn keyword vimCommand contained	che[ckpath]	h[elp]	omenu	setl[ocal]	win
syn keyword vimCommand contained	cla[st]	helpf[ind]	on[ly]	sf[ind]	winc[md]
syn keyword vimCommand contained	cl[ist]	helpt[ags]	onoreme	sfir[st	windo
syn keyword vimCommand contained	clo[se]	hid[e]	onoremenu	sh[ell]	winp[os]
syn keyword vimCommand contained	cmapc[lear]	his[tory]	o[pen]	sig[n]	win[size]
syn keyword vimCommand contained	cme	ia[bbrev]	opt[ions]	signs	wn[ext]
syn keyword vimCommand contained	cmenu	iabc[lear]	ou[nmap]	sil[ent]	wp[revous]
syn keyword vimCommand contained	cnew[er]	if	ounme	si[malt]	wq
syn keyword vimCommand contained	cn[ext]	ij[ump]	ounmenu	sla[st]	wqa[ll]
syn keyword vimCommand contained	cnf[ile]	il[ist]	pc[lose]	sl[eep]	w[rite]
syn keyword vimCommand contained	cnorea[bbrev]	imapc[lear]	ped[it]	sm[agic]	ws[verb]
syn keyword vimCommand contained	cnoreme	ime	pe[rl]	sn[ext]	wv[iminfo]
syn keyword vimCommand contained	cnoremenu	imenu	perld[o]	sni[ff]	xa[ll]
syn keyword vimCommand contained	col[der]	inorea[bbrev]	po[p]	sno[magic]	x[it]
syn keyword vimCommand contained	comc[lear]	inoreme	pop[up]	so[urce]	y[ank]
syn keyword vimCommand contained	conf[irm]	inoremenu	pp[op]	sp[lit]
syn match   vimCommand contained	"\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	dir	hls	patchmode	sxq
syn keyword vimOption contained	ai	directory	hlsearch	path	syn
syn keyword vimOption contained	akm	display	ic	pex	syntax
syn keyword vimOption contained	al	dy	icon	pm	ta
syn keyword vimOption contained	aleph	ea	iconstring	previewheight	tabstop
syn keyword vimOption contained	allowrevins	ead	ignorecase	previewwindow	tag
syn keyword vimOption contained	altkeymap	eadirection	im	pt	tagbsearch
syn keyword vimOption contained	ar	eb	inc	pvh	taglength
syn keyword vimOption contained	ari	ed	include	pvw	tagrelative
syn keyword vimOption contained	autoindent	edcompatible	includeexpr	readonly	tags
syn keyword vimOption contained	autoread	ef	incsearch	remap	tagstack
syn keyword vimOption contained	autowrite	efm	inde	report	tb
syn keyword vimOption contained	autowriteall	ei	indentexpr	restorescreen	tbi
syn keyword vimOption contained	aw	ek	indentkeys	revins	tbs
syn keyword vimOption contained	awa	enc	indk	ri	tenc
syn keyword vimOption contained	background	encoding	inex	rightleft	term
syn keyword vimOption contained	backspace	endofline	inf	rl	termencoding
syn keyword vimOption contained	backup	eol	infercase	ro	terse
syn keyword vimOption contained	backupcopy	ep	insertmode	rs	textauto
syn keyword vimOption contained	backupdir	equalalways	is	rtp	textmode
syn keyword vimOption contained	backupext	equalprg	isf	ru	textwidth
syn keyword vimOption contained	backupskip	errorbells	isfname	ruf	tf
syn keyword vimOption contained	balloondelay	errorfile	isi	ruler	tgst
syn keyword vimOption contained	ballooneval	errorformat	isident	rulerformat	thesaurus
syn keyword vimOption contained	bdir	esckeys	isk	runtimepath	tildeop
syn keyword vimOption contained	bdlay	et	iskeyword	sb	timeout
syn keyword vimOption contained	beval	eventignore	isp	sbo	timeoutlen
syn keyword vimOption contained	bex	ex	isprint	sbr	title
syn keyword vimOption contained	bg	expandtab	joinspaces	sc	titlelen
syn keyword vimOption contained	bh	exrc	js	scb	titleold
syn keyword vimOption contained	bin	fcl	key	scr	titlestring
syn keyword vimOption contained	binary	fcs	keymap	scroll	tl
syn keyword vimOption contained	biosk	fdc	keymodel	scrollbind	tm
syn keyword vimOption contained	bioskey	fde	keywordprg	scrolljump	to
syn keyword vimOption contained	bk	fdi	km	scrolloff	toolbar
syn keyword vimOption contained	bkc	fdl	kmp	scrollopt	top
syn keyword vimOption contained	bl	fdls	kp	scs	tr
syn keyword vimOption contained	bomb	fdm	langmap	sect	ts
syn keyword vimOption contained	breakat	fdn	langmenu	sections	tsl
syn keyword vimOption contained	brk	fdo	laststatus	secure	tsr
syn keyword vimOption contained	browsedir	fdt	lazyredraw	sel	ttimeout
syn keyword vimOption contained	bs	fen	lbr	selection	ttimeoutlen
syn keyword vimOption contained	bsdir	fenc	lcs	selectmode	ttm
syn keyword vimOption contained	bsk	fencs	linebreak	sessionoptions	tty
syn keyword vimOption contained	bt	ff	lines	sft	ttybuiltin
syn keyword vimOption contained	bufhidden	ffs	linespace	sh	ttyfast
syn keyword vimOption contained	buflisted	fileencoding	lisp	shcf	ttym
syn keyword vimOption contained	buftype	fileencodings	list	shell	ttymouse
syn keyword vimOption contained	cb	fileformat	listchars	shellcmdflag	ttyscroll
syn keyword vimOption contained	ccv	fileformats	lmap	shellpipe	ttytype
syn keyword vimOption contained	cd	filetype	lmenu	shellquote	tw
syn keyword vimOption contained	cdpath	fillchars	loadplugins	shellredir	tx
syn keyword vimOption contained	cedit	fk	lpl	shellslash	uc
syn keyword vimOption contained	cf	fkmap	ls	shelltype	ul
syn keyword vimOption contained	ch	fml	lsp	shellxquote	undolevels
syn keyword vimOption contained	charconvert	fmr	lz	shiftround	updatecount
syn keyword vimOption contained	cin	fo	ma	shiftwidth	updatetime
syn keyword vimOption contained	cindent	foldclose	magic	shm	ut
syn keyword vimOption contained	cink	foldcolumn	makeef	shortmess	vb
syn keyword vimOption contained	cinkeys	foldenable	makeprg	shortname	vbs
syn keyword vimOption contained	cino	foldexpr	mat	showbreak	vdir
syn keyword vimOption contained	cinoptions	foldignore	matchpairs	showcmd	ve
syn keyword vimOption contained	cinw	foldlevel	matchtime	showfulltag	verbose
syn keyword vimOption contained	cinwords	foldlevelstart	maxfuncdepth	showmatch	vi
syn keyword vimOption contained	clipboard	foldmarker	maxmapdepth	showmode	viewdir
syn keyword vimOption contained	cmdheight	foldmethod	maxmem	shq	viewoptions
syn keyword vimOption contained	cmdwinheight	foldminlines	maxmemtot	si	viminfo
syn keyword vimOption contained	cms	foldnestmax	mef	sidescroll	virtualedit
syn keyword vimOption contained	co	foldopen	menuitems	sidescrolloff	visualbell
syn keyword vimOption contained	columns	foldtext	mfd	siso	vop
syn keyword vimOption contained	com	formatoptions	mh	sj	wa
syn keyword vimOption contained	comments	formatprg	mis	slm	wak
syn keyword vimOption contained	commentstring	fp	ml	sm	warn
syn keyword vimOption contained	compatible	ft	mls	smartcase	wb
syn keyword vimOption contained	complete	gcr	mm	smartindent	wc
syn keyword vimOption contained	confirm	gd	mmd	smarttab	wcm
syn keyword vimOption contained	consk	gdefault	mmt	smd	wd
syn keyword vimOption contained	conskey	gfm	mod	sn	weirdinvert
syn keyword vimOption contained	cp	gfn	modeline	so	wh
syn keyword vimOption contained	cpo	gfs	modelines	softtabstop	whichwrap
syn keyword vimOption contained	cpoptions	gfw	modifiable	sol	wig
syn keyword vimOption contained	cpt	ghr	modified	sp	wildchar
syn keyword vimOption contained	cscopepathcomp	go	more	splitbelow	wildcharm
syn keyword vimOption contained	cscopeprg	gp	mouse	splitright	wildignore
syn keyword vimOption contained	cscopetag	grepformat	mousef	spr	wildmenu
syn keyword vimOption contained	cscopetagorder	grepprg	mousefocus	sr	wildmode
syn keyword vimOption contained	cscopeverbose	guicursor	mousehide	srr	wim
syn keyword vimOption contained	cspc	guifont	mousem	ss	winaltkeys
syn keyword vimOption contained	csprg	guifontset	mousemodel	ssl	winheight
syn keyword vimOption contained	cst	guifontwide	mouses	ssop	winminheight
syn keyword vimOption contained	csto	guiheadroom	mouseshape	st	winminwidth
syn keyword vimOption contained	csverb	guioptions	mouset	sta	winwidth
syn keyword vimOption contained	cwh	guipty	mousetime	startofline	wiv
syn keyword vimOption contained	debug	helpfile	mp	statusline	wiw
syn keyword vimOption contained	deco	helpheight	mps	stl	wm
syn keyword vimOption contained	def	hf	nf	sts	wmh
syn keyword vimOption contained	define	hh	nrformats	su	wmnu
syn keyword vimOption contained	delcombine	hi	nu	sua	wmw
syn keyword vimOption contained	dex	hid	number	suffixes	wrap
syn keyword vimOption contained	dg	hidden	oft	suffixesadd	wrapmargin
syn keyword vimOption contained	dict	highlight	osfiletype	sw	wrapscan
syn keyword vimOption contained	dictionary	history	pa	swapfile	write
syn keyword vimOption contained	diff	hk	para	swapsync	writeany
syn keyword vimOption contained	diffexpr	hkmap	paragraphs	swb	writebackup
syn keyword vimOption contained	diffopt	hkmapp	paste	swf	writedelay
syn keyword vimOption contained	digraph	hkp	pastetoggle	switchbuf	ws
syn keyword vimOption contained	dip	hl	patchexpr	sws	ww

" These are the turn-off setting variants
syn keyword vimOption contained	loadplugins	noea	noinsertmode	noro	notagbsearch
syn keyword vimOption contained	noai	noeb	nois	nors	notagrelative
syn keyword vimOption contained	noakm	noed	nojoinspaces	noru	notagstack
syn keyword vimOption contained	noallowrevins	noedcompatible	nojs	noruler	notbi
syn keyword vimOption contained	noaltkeymap	noek	nolazyredraw	nosb	notbs
syn keyword vimOption contained	noar	noendofline	nolbr	nosc	noterse
syn keyword vimOption contained	noari	noeol	nolinebreak	noscb	notextauto
syn keyword vimOption contained	noautoindent	noequalalways	nolisp	noscrollbind	notextmode
syn keyword vimOption contained	noautoread	noerrorbells	nolist	noscs	notf
syn keyword vimOption contained	noautowrite	noesckeys	noloadplugins	nosecure	notgst
syn keyword vimOption contained	noautowriteall	noet	nolpl	nosft	notildeop
syn keyword vimOption contained	noaw	noex	nolz	noshellslash	notimeout
syn keyword vimOption contained	noawa	noexpandtab	noma	noshiftround	notitle
syn keyword vimOption contained	nobackup	noexrc	nomagic	noshortname	noto
syn keyword vimOption contained	noballooneval	nofen	nomh	noshowcmd	notop
syn keyword vimOption contained	nobeval	nofk	noml	noshowfulltag	notr
syn keyword vimOption contained	nobin	nofkmap	nomod	noshowmatch	nottimeout
syn keyword vimOption contained	nobinary	nofoldenable	nomodeline	noshowmode	nottybuiltin
syn keyword vimOption contained	nobiosk	nogd	nomodifiable	nosi	nottyfast
syn keyword vimOption contained	nobioskey	nogdefault	nomodified	nosm	notx
syn keyword vimOption contained	nobk	noguipty	nomore	nosmartcase	novb
syn keyword vimOption contained	nobomb	nohid	nomousef	nosmartindent	novisualbell
syn keyword vimOption contained	nocf	nohidden	nomousefocus	nosmarttab	nowa
syn keyword vimOption contained	nocin	nohk	nomousehide	nosmd	nowarn
syn keyword vimOption contained	nocindent	nohkmap	nonu	nosn	nowb
syn keyword vimOption contained	nocompatible	nohkmapp	nonumber	nosol	noweirdinvert
syn keyword vimOption contained	noconfirm	nohkp	nopaste	nosplitbelow	nowildmenu
syn keyword vimOption contained	noconsk	nohls	nopreviewwindow	nosplitright	nowiv
syn keyword vimOption contained	noconskey	nohlsearch	nopvw	nospr	nowmnu
syn keyword vimOption contained	nocp	noic	noreadonly	nosr	nowrap
syn keyword vimOption contained	nocscopetag	noicon	noremap	nossl	nowrapscan
syn keyword vimOption contained	nocscopeverbose	noignorecase	norestorescreen	nosta	nowrite
syn keyword vimOption contained	nocst	noim	norevins	nostartofline	nowriteany
syn keyword vimOption contained	nocsverb	noincsearch	nori	noswapfile	nowritebackup
syn keyword vimOption contained	nodg	noinf	norightleft	noswf	nows
syn keyword vimOption contained	nodiff	noinfercase	norl	nota	pvw
syn keyword vimOption contained	nodigraph

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
syn keyword vimAutoEvent contained	BufAdd	BufReadPost	CmdwinLeave	FileType	StdinReadPost
syn keyword vimAutoEvent contained	BufCreate	BufReadPre	CursorHold	FileWriteCmd	StdinReadPre
syn keyword vimAutoEvent contained	BufDelete	BufUnload	EncodingChanged	FileWritePost	Syntax
syn keyword vimAutoEvent contained	BufEnter	BufWinEnter	FileAppendCmd	FileWritePre	TermChanged
syn keyword vimAutoEvent contained	BufFilePost	BufWinLeave	FileAppendPost	FilterReadPost	TermResponse
syn keyword vimAutoEvent contained	BufFilePre	BufWipeout	FileAppendPre	FilterReadPre	User
syn keyword vimAutoEvent contained	BufHidden	BufWrite	FileChangedRO	FilterWritePost	UserGettingBored
syn keyword vimAutoEvent contained	BufLeave	BufWriteCmd	FileChangedShell	FilterWritePre	VimEnter
syn keyword vimAutoEvent contained	BufNew	BufWritePost	FileEncoding	FocusGained	VimLeave
syn keyword vimAutoEvent contained	BufNewFile	BufWritePre	FileReadCmd	FocusLost	VimLeavePre
syn keyword vimAutoEvent contained	BufRead	Cmd-event	FileReadPost	FuncUndefined	WinEnter
syn keyword vimAutoEvent contained	BufReadCmd	CmdwinEnter	FileReadPre	GUIEnter	WinLeave

" Highlight commonly used Groupnames
syn keyword vimGroup contained	Comment	Identifier	Keyword	Type	Delimiter
syn keyword vimGroup contained	Constant	Function	Exception	StorageClass	SpecialComment
syn keyword vimGroup contained	String	Statement	PreProc	Structure	Debug
syn keyword vimGroup contained	Character	Conditional	Include	Typedef	Ignore
syn keyword vimGroup contained	Number	Repeat	Define	Special	Error
syn keyword vimGroup contained	Boolean	Label	Macro	SpecialChar	Todo
syn keyword vimGroup contained	Float	Operator	PreCondit	Tag

" Default highlighting groups
syn keyword vimHLGroup contained	Cursor	Directory	Menu	Scrollbar	VertSplit
syn keyword vimHLGroup contained	CursorIM	ErrorMsg	ModeMsg	Search	Visual
syn keyword vimHLGroup contained	DiffAdd	FoldColumn	MoreMsg	SpecialKey	VisualNOS
syn keyword vimHLGroup contained	DiffChange	Folded	NonText	StatusLine	WarningMsg
syn keyword vimHLGroup contained	DiffDelete	IncSearch	Normal	StatusLineNC	WildMenu
syn keyword vimHLGroup contained	DiffText	LineNr	Question	Title
syn case match

" Function Names
syn keyword vimFuncName contained	MyCounter	executable	getwinvar	line2byte	strpart
syn keyword vimFuncName contained	append	exists	glob	localtime	strridx
syn keyword vimFuncName contained	argc	expand	globpath	maparg	strtrans
syn keyword vimFuncName contained	argv	filereadable	has	mapcheck	submatch
syn keyword vimFuncName contained	browse	filewritable	hasmapto	match	substitute
syn keyword vimFuncName contained	bufexists	fnamemodify	histadd	matchend	synID
syn keyword vimFuncName contained	buflisted	foldclosed	histdel	matchstr	synIDattr
syn keyword vimFuncName contained	bufloaded	foldclosedend	histget	mode	synIDtrans
syn keyword vimFuncName contained	bufname	foldlevel	histnr	nextnonblank	system
syn keyword vimFuncName contained	bufnr	foldtext	hlID	nr2char	tempname
syn keyword vimFuncName contained	bufwinnr	function	hlexists	prevnonblank	tolower
syn keyword vimFuncName contained	byte2line	getbufvar	hostname	rename	toupper
syn keyword vimFuncName contained	char2nr	getchar	iconv	search	type
syn keyword vimFuncName contained	col	getcharmod	indent	searchpair	virtcol
syn keyword vimFuncName contained	confirm	getcwd	input	setbufvar	visualmode
syn keyword vimFuncName contained	cscope_connection	getfsize	inputsecret	setline	winbufnr
syn keyword vimFuncName contained	delete	getftime	isdirectory	setwinvar	winheight
syn keyword vimFuncName contained	did_filetype	getline	libcall	strftime	winnr
syn keyword vimFuncName contained	escape	getwinposx	libcallnr	stridx	winwidth
syn keyword vimFuncName contained	eventhandler	getwinposy	line	strlen

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
syn match   vimBehave	"\<be\%[have]\>" contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
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
syn match vimNumber	"\<\d\+.\d\+"
syn match vimNumber	"\<\d\+L\="
syn match vimNumber	"-\d\+.\d\+"
syn match vimNumber	"-\d\+L\="
syn match vimNumber	"[[;:]\d\+"lc=1
syn match vimNumber	"0[xX]\x\+"
syn match vimNumber	"#\x\+"

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
syn match   vimMapMod    contained	"<\([lL]eader\|[pP]lug\|[sS]cript\|[sS][iI][dD]\|[uU]nique\)\+>"	skipwhite contains=vimMapModKey,vimMapModErr nextgroup=@vimMapGroup
syn case ignore
syn keyword vimMapModKey contained	leader	plug	script	sid	unique
syn case match

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam]-\)\{0,4}\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|del\%[ete]\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k \=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
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
syn region  vimSynMatchRegion	contained oneline matchgroup=vimGroupName start="\k\+" matchgroup=vimSep end="|\|$" contains=vimMtchComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn match   vimSynMtchOpt	contained	"\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|extend\|skipnl\|fold\)\>"
syn match   vimMtchComment	contained	'"[^"]\+$'

" Syntax: off and on
syn keyword vimSynType	contained	off	on

" Syntax: region
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|keepend\|oneline\|extend\|skipnl\|fold\)\>"
syn match   vimSynReg	contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="	nextgroup=vimGroup,vimHLGroup
syn region  vimSynRegPat	contained	start="\z([[:punct:]]\)"  skip="\\\\\|\\\z1"  end="\z1"  contains=@vimSynRegPatGroup skipwhite nextgroup=vimSynPatMod,vimSynReg
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\="
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\=," nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"lc=\d\+"
syn match   vimSynPatMod	contained	"lc=\d\+," nextgroup=vimSynPatMod
syn region  vimSynPatRange	contained	start="\["	skip="\\\\\|\\]"   end="]"
syn match   vimSynNotPatRange	contained	"\\\\\|\\\["

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

let b:current_syntax = "vim"

" vim: ts=18
