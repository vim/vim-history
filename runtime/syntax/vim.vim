" Vim syntax file
" Language:	Vim 6.0r script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	January 02, 2001
" Version:	6.0r-01

" Remove old syntax
syn clear

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	N[ext]	comc[lear]	iuna[bbrev]	promptr[epl]	sta[g]
syn keyword vimCommand contained	Nread	conf[irm]	iu[nmap]	ps[earch]	star[tinsert]
syn keyword vimCommand contained	Nw	con[tinue]	iunme	ptN[ext]	stj[ump]
syn keyword vimCommand contained	P[rint]	co[py]	iunmenu	pta[g]	st[op]
syn keyword vimCommand contained	X	cp[revious]	j[oin]	ptf[irst]	sts[elect]
syn keyword vimCommand contained	ab[breviate]	cq[uit]	ju[mps]	ptj[ump]	sun[hide]
syn keyword vimCommand contained	abc[lear]	cr[ewind]	k	ptl[ast]	sus[pend]
syn keyword vimCommand contained	al[l]	cuna[bbrev]	lan[guage]	ptn[ext]	sv[iew]
syn keyword vimCommand contained	am	cu[nmap]	la[st]	ptp[revious]	syncbind
syn keyword vimCommand contained	amenu	cunme	lc[d]	ptr[ewind]	t
syn keyword vimCommand contained	an	cunmenu	lch[dir]	pts[elect]	tN[ext]
syn keyword vimCommand contained	anoremenu	cw[indow]	le[ft]	pu[t]	ta[g]
syn keyword vimCommand contained	a[ppend]	delc[ommand]	l[ist]	pw[d]	tags
syn keyword vimCommand contained	arga[dd]	d[elete]	ls	pyf[ile]	tc[l]
syn keyword vimCommand contained	argd[elete]	delf[unction]	mak[e]	py[thon]	tcld[o]
syn keyword vimCommand contained	argdo	dig[raphs]	ma[rk]	qa[ll]	tclf[ile]
syn keyword vimCommand contained	argg[lobal]	di[splay]	marks	q[uit]	te[aroff]
syn keyword vimCommand contained	argl[ocal]	dj[ump]	me	quita[ll]	tf[irst]
syn keyword vimCommand contained	ar[gs]	dl[ist]	menu	r[ead]	tj[ump]
syn keyword vimCommand contained	argu[ment]	dr[op]	menu-disable	rec[over]	tl[ast]
syn keyword vimCommand contained	as[cii]	ds[earch]	menu-enable	redi[r]	tm[enu]
syn keyword vimCommand contained	aun	dsp[lit]	menu-<script>	red[o]	tn[ext]
syn keyword vimCommand contained	aunmenu	echoe[rr]	menut[rans]	reg[isters]	to[pleft]
syn keyword vimCommand contained	bN[ext]	echom[sg]	mk[exrc]	res[ize]	tp[revious]
syn keyword vimCommand contained	bad[d]	echon	mks[ession]	ret[ab]	tr[ewind]
syn keyword vimCommand contained	ba[ll]	e[dit]	mkvie[w]	retu[rn]	ts[elect]
syn keyword vimCommand contained	bd[elete]	el[se]	mkv[imrc]	rew[ind]	tu[nmenu]
syn keyword vimCommand contained	bf[irst	elsei[f]	mod[e]	ri[ght]	una[bbreviate]
syn keyword vimCommand contained	bl[ast]	em[eenu]	m[ove]	rub[y]	u[ndo]
syn keyword vimCommand contained	bm[odified]	emenu	new	rubyd[o]	unh[ide]
syn keyword vimCommand contained	bn[ext]	endf[unction]	n[ext]	rubyf[ile]	unm[ap]
syn keyword vimCommand contained	bo[tright]	en[dif]	nmapc[lear]	ru[ntime]	unme
syn keyword vimCommand contained	bp[revious]	endw[hile]	nme	rv[iminfo]	unmenu
syn keyword vimCommand contained	brea[k]	ene[w]	nmenu	sN[ext]	uns[ign]
syn keyword vimCommand contained	breaka[dd]	ex	nnoreme	sal[l]	verb[ose]
syn keyword vimCommand contained	breakd[el]	exi[t]	nnoremenu	sa[rgument]	ve[rsion]
syn keyword vimCommand contained	breakl[ist]	f[ile]	noh[lsearch]	sbN[ext]	vert[ical]
syn keyword vimCommand contained	br[ewind]	files	norea[bbrev]	sba[ll]	v[global]
syn keyword vimCommand contained	bro[wse]	filetype	noreme	sbf[irst]	vie[w]
syn keyword vimCommand contained	bufdo	fin[d]	noremenu	sbl[ast]	vi[sual]
syn keyword vimCommand contained	b[uffer]	fini[sh]	norm[al]	sbm[odified]	vmapc[lear]
syn keyword vimCommand contained	buffers	fir[st]	nu[mber]	sbn[ext]	vme
syn keyword vimCommand contained	bun[load]	fix[del]	nun[map]	sbp[revious]	vmenu
syn keyword vimCommand contained	cN[ext]	fo[ld]	nunme	sbr[ewind]	vne[w]
syn keyword vimCommand contained	ca[bbrev]	foldo[pen]	nunmenu	sb[uffer]	vnoreme
syn keyword vimCommand contained	cabc[lear]	fu[nction]	omapc[lear]	scrip[tnames]	vnoremenu
syn keyword vimCommand contained	cal[l]	g[lobal]	ome	se[t]	vs[plit]
syn keyword vimCommand contained	cc	go[to]	omenu	setf[iletype]	vu[nmap]
syn keyword vimCommand contained	cd	gr[ep]	on[ly]	setg[lobal]	vunme
syn keyword vimCommand contained	ce[nter]	h[elp]	onoreme	setl[ocal]	vunmenu
syn keyword vimCommand contained	cf[ile]	helpf[ind]	onoremenu	sf[ind]	wN[ext]
syn keyword vimCommand contained	cfir[st]	helpt[ags]	o[pen]	sfir[st	wa[ll]
syn keyword vimCommand contained	c[hange]	hid[e]	opt[ions]	sh[ell]	wh[ile]
syn keyword vimCommand contained	chd[ir]	his[tory]	ou[nmap]	sig[n]	win
syn keyword vimCommand contained	che[ckpath]	ia[bbrev]	ounme	signs	windo
syn keyword vimCommand contained	cla[st]	iabc[lear]	ounmenu	sil[ent]	winp[os]
syn keyword vimCommand contained	cl[ist]	if	pc[lose]	si[malt]	win[size]
syn keyword vimCommand contained	clo[se]	ij[ump]	ped[it]	sla[st]	wn[ext]
syn keyword vimCommand contained	cmapc[lear]	il[ist]	pe[rl]	sl[eep]	wp[revous]
syn keyword vimCommand contained	cme	imapc[lear]	perld[o]	sm[agic]	wq
syn keyword vimCommand contained	cmenu	ime	po[p]	sn[ext]	wqa[ll]
syn keyword vimCommand contained	cnew[er]	imenu	pop[up]	sni[ff]	w[rite]
syn keyword vimCommand contained	cn[ext]	inorea[bbrev]	pp[op]	sno[magic]	ws[verb]
syn keyword vimCommand contained	cnf[ile]	inoreme	pre[serve]	so[urce]	wv[iminfo]
syn keyword vimCommand contained	cnorea[bbrev]	inoremenu	prev[ious]	sp[lit]	xa[ll]
syn keyword vimCommand contained	cnoreme	i[nsert]	p[rint]	spr[evious]	x[it]
syn keyword vimCommand contained	cnoremenu	is[earch]	promptf[ind]	sr[ewind]	y[ank]
syn keyword vimCommand contained	col[der]	isp[lit]
syn match   vimCommand contained	"\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	ed	indentexpr	pvh	sxq
syn keyword vimOption contained	ai	edcompatible	indentkeys	pvw	syn
syn keyword vimOption contained	akm	ef	indk	readonly	syntax
syn keyword vimOption contained	al	efm	inex	remap	ta
syn keyword vimOption contained	aleph	ei	inf	report	tabstop
syn keyword vimOption contained	allowrevins	ek	infercase	restorescreen	tag
syn keyword vimOption contained	altkeymap	endofline	insertmode	revins	tagbsearch
syn keyword vimOption contained	ar	eol	is	ri	taglength
syn keyword vimOption contained	ari	ep	isf	rightleft	tagrelative
syn keyword vimOption contained	autoindent	equalalways	isfname	rl	tags
syn keyword vimOption contained	autoread	equalprg	isi	ro	tagstack
syn keyword vimOption contained	autowrite	errorbells	isident	rs	tb
syn keyword vimOption contained	autowriteall	errorfile	isk	rtp	tbi
syn keyword vimOption contained	aw	errorformat	iskeyword	ru	tbs
syn keyword vimOption contained	awa	esckeys	isp	ruf	term
syn keyword vimOption contained	background	et	isprint	ruler	terse
syn keyword vimOption contained	backspace	eventignore	joinspaces	rulerformat	textauto
syn keyword vimOption contained	backup	ex	js	runtimepath	textmode
syn keyword vimOption contained	backupcopy	expandtab	key	sb	textwidth
syn keyword vimOption contained	backupdir	exrc	keymap	sbo	tf
syn keyword vimOption contained	backupext	fcc	keymodel	sbr	tgst
syn keyword vimOption contained	backupskip	fccs	keywordprg	sc	thesaurus
syn keyword vimOption contained	balloondelay	fcs	km	scb	tildeop
syn keyword vimOption contained	ballooneval	fdc	kmp	scr	timeout
syn keyword vimOption contained	bdir	fde	kp	scroll	timeoutlen
syn keyword vimOption contained	bdlay	fdi	langmap	scrollbind	title
syn keyword vimOption contained	beval	fdl	langmenu	scrolljump	titlelen
syn keyword vimOption contained	bex	fdm	laststatus	scrolloff	titleold
syn keyword vimOption contained	bg	fdt	lazyredraw	scrollopt	titlestring
syn keyword vimOption contained	bh	fe	lbr	scs	tl
syn keyword vimOption contained	bin	fen	lcs	sect	tm
syn keyword vimOption contained	binary	ff	linebreak	sections	to
syn keyword vimOption contained	biosk	ffs	lines	secure	toolbar
syn keyword vimOption contained	bioskey	filecharcode	linespace	sel	top
syn keyword vimOption contained	bk	filecharcodes	lisp	selection	tr
syn keyword vimOption contained	bkc	fileencoding	list	selectmode	ts
syn keyword vimOption contained	bomb	fileformat	listchars	sessionoptions	tsl
syn keyword vimOption contained	breakat	fileformats	lmap	sft	tsr
syn keyword vimOption contained	brk	filetype	lmenu	sh	ttimeout
syn keyword vimOption contained	browsedir	fillchars	loadplugins	shcf	ttimeoutlen
syn keyword vimOption contained	bs	fk	lpl	shell	ttm
syn keyword vimOption contained	bsdir	fkmap	ls	shellcmdflag	ttybuiltin
syn keyword vimOption contained	bsk	fmr	lsp	shellpipe	ttyfast
syn keyword vimOption contained	bt	fo	lz	shellquote	ttym
syn keyword vimOption contained	bufhidden	foldcolumn	ma	shellredir	ttymouse
syn keyword vimOption contained	buftype	foldenable	magic	shellslash	ttyscroll
syn keyword vimOption contained	cb	foldexpr	makeef	shelltype	ttytype
syn keyword vimOption contained	cc	foldignore	makeprg	shellxquote	tw
syn keyword vimOption contained	ccv	foldlevel	mat	shiftround	tx
syn keyword vimOption contained	cd	foldmarker	matchpairs	shiftwidth	uc
syn keyword vimOption contained	cdpath	foldmethod	matchtime	shm	ul
syn keyword vimOption contained	cf	foldtext	maxfuncdepth	shortmess	undolevels
syn keyword vimOption contained	ch	formatoptions	maxmapdepth	shortname	updatecount
syn keyword vimOption contained	charcode	formatprg	maxmem	showbreak	updatetime
syn keyword vimOption contained	charconvert	fp	maxmemtot	showcmd	ut
syn keyword vimOption contained	cin	ft	mef	showfulltag	vb
syn keyword vimOption contained	cindent	gcr	menuitems	showmatch	vbs
syn keyword vimOption contained	cink	gd	mfd	showmode	ve
syn keyword vimOption contained	cinkeys	gdefault	mh	shq	verbose
syn keyword vimOption contained	cino	gfm	mis	si	vi
syn keyword vimOption contained	cinoptions	gfn	ml	sidescroll	viminfo
syn keyword vimOption contained	cinw	gfs	mls	sidescrolloff	virtualedit
syn keyword vimOption contained	cinwords	gfw	mm	siso	visualbell
syn keyword vimOption contained	clipboard	ghr	mmd	sj	wa
syn keyword vimOption contained	cmdheight	go	mmt	slm	wak
syn keyword vimOption contained	co	gp	mod	sm	warn
syn keyword vimOption contained	columns	grepformat	modeline	smartcase	wb
syn keyword vimOption contained	com	grepprg	modelines	smartindent	wc
syn keyword vimOption contained	comments	guicursor	modifiable	smarttab	wcm
syn keyword vimOption contained	compatible	guifont	modified	smd	wd
syn keyword vimOption contained	complete	guifontset	more	sn	weirdinvert
syn keyword vimOption contained	confirm	guifontwide	mouse	so	wh
syn keyword vimOption contained	consk	guiheadroom	mousef	softtabstop	whichwrap
syn keyword vimOption contained	conskey	guioptions	mousefocus	sol	wig
syn keyword vimOption contained	cp	guipty	mousehide	sp	wildchar
syn keyword vimOption contained	cpo	helpfile	mousem	splitbelow	wildcharm
syn keyword vimOption contained	cpoptions	helpheight	mousemodel	splitright	wildignore
syn keyword vimOption contained	cpt	hf	mouses	spr	wildmenu
syn keyword vimOption contained	cscopeprg	hh	mouseshape	sr	wildmode
syn keyword vimOption contained	cscopetag	hi	mouset	srr	wim
syn keyword vimOption contained	cscopetagorder	hid	mousetime	ss	winaltkeys
syn keyword vimOption contained	cscopeverbose	hidden	mp	ssl	winheight
syn keyword vimOption contained	csprg	highlight	mps	ssop	winminheight
syn keyword vimOption contained	cst	history	nf	st	winminwidth
syn keyword vimOption contained	csto	hk	nrformats	sta	winwidth
syn keyword vimOption contained	csverb	hkmap	nu	startofline	wiv
syn keyword vimOption contained	debug	hkmapp	number	statusline	wiw
syn keyword vimOption contained	def	hkp	oft	stl	wm
syn keyword vimOption contained	define	hl	osfiletype	sts	wmh
syn keyword vimOption contained	dg	hls	pa	su	wmnu
syn keyword vimOption contained	dict	hlsearch	para	sua	wmw
syn keyword vimOption contained	dictionary	ic	paragraphs	suffixes	wrap
syn keyword vimOption contained	digraph	icon	paste	suffixesadd	wrapmargin
syn keyword vimOption contained	dir	iconstring	pastetoggle	sw	wrapscan
syn keyword vimOption contained	directory	ignorecase	patchmode	swapfile	write
syn keyword vimOption contained	display	im	path	swapsync	writeany
syn keyword vimOption contained	dy	inc	pm	swb	writebackup
syn keyword vimOption contained	ea	include	previewheight	swf	writedelay
syn keyword vimOption contained	ead	includeexpr	previewwindow	switchbuf	ws
syn keyword vimOption contained	eadirection	incsearch	pt	sws	ww
syn keyword vimOption contained	eb	inde

" These are the turn-off setting variants
syn keyword vimOption contained	loadplugins	noeb	noinsertmode	noro	notagbsearch
syn keyword vimOption contained	noai	noed	nois	nors	notagrelative
syn keyword vimOption contained	noakm	noedcompatible	nojoinspaces	noru	notagstack
syn keyword vimOption contained	noallowrevins	noek	nojs	noruler	notbi
syn keyword vimOption contained	noaltkeymap	noendofline	nolazyredraw	nosb	notbs
syn keyword vimOption contained	noar	noeol	nolbr	nosc	noterse
syn keyword vimOption contained	noari	noequalalways	nolinebreak	noscb	notextauto
syn keyword vimOption contained	noautoindent	noerrorbells	nolisp	noscrollbind	notextmode
syn keyword vimOption contained	noautoread	noesckeys	nolist	noscs	notf
syn keyword vimOption contained	noautowrite	noet	noloadplugins	nosecure	notgst
syn keyword vimOption contained	noautowriteall	noex	nolpl	nosft	notildeop
syn keyword vimOption contained	noaw	noexpandtab	nolz	noshellslash	notimeout
syn keyword vimOption contained	noawa	noexrc	noma	noshiftround	notitle
syn keyword vimOption contained	nobackup	nofdc	nomagic	noshortname	noto
syn keyword vimOption contained	noballooneval	nofen	nomh	noshowcmd	notop
syn keyword vimOption contained	nobeval	nofk	noml	noshowfulltag	notr
syn keyword vimOption contained	nobin	nofkmap	nomod	noshowmatch	nottimeout
syn keyword vimOption contained	nobinary	nofoldcolumn	nomodeline	noshowmode	nottybuiltin
syn keyword vimOption contained	nobiosk	nofoldenable	nomodifiable	nosi	nottyfast
syn keyword vimOption contained	nobioskey	nogd	nomodified	nosm	notx
syn keyword vimOption contained	nobk	nogdefault	nomore	nosmartcase	novb
syn keyword vimOption contained	nobomb	noguipty	nomousef	nosmartindent	novisualbell
syn keyword vimOption contained	nocf	nohid	nomousefocus	nosmarttab	nowa
syn keyword vimOption contained	nocin	nohidden	nomousehide	nosmd	nowarn
syn keyword vimOption contained	nocindent	nohk	nonu	nosn	nowb
syn keyword vimOption contained	nocompatible	nohkmap	nonumber	nosol	noweirdinvert
syn keyword vimOption contained	noconfirm	nohkmapp	nopaste	nosplitbelow	nowildmenu
syn keyword vimOption contained	noconsk	nohkp	nopreviewwindow	nosplitright	nowiv
syn keyword vimOption contained	noconskey	nohls	nopvw	nospr	nowmnu
syn keyword vimOption contained	nocp	nohlsearch	noreadonly	nosr	nowrap
syn keyword vimOption contained	nocscopetag	noic	noremap	nossl	nowrapscan
syn keyword vimOption contained	nocscopeverbose	noicon	norestorescreen	nosta	nowrite
syn keyword vimOption contained	nocst	noignorecase	norevins	nostartofline	nowriteany
syn keyword vimOption contained	nocsverb	noim	nori	noswapfile	nowritebackup
syn keyword vimOption contained	nodg	noincsearch	norightleft	noswf	nows
syn keyword vimOption contained	nodigraph	noinf	norl	nota	pvw
syn keyword vimOption contained	noea	noinfercase

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
syn keyword vimAutoEvent contained	BufDelete	BufUnload	FileChangedRO	FilterReadPre	TermChanged
syn keyword vimAutoEvent contained	BufEnter	BufWrite	FileChangedShell	FilterWritePost	TermResponse
syn keyword vimAutoEvent contained	BufFilePost	BufWriteCmd	FileEncoding	FilterWritePre	User
syn keyword vimAutoEvent contained	BufFilePre	BufWritePost	FileReadCmd	FocusGained	UserGettingBored
syn keyword vimAutoEvent contained	BufHidden	BufWritePre	FileReadPost	FocusLost	VimEnter
syn keyword vimAutoEvent contained	BufLeave	CharCode	FileReadPre	FuncUndefined	VimLeave
syn keyword vimAutoEvent contained	BufNewFile	Cmd-event	FileType	GUIEnter	VimLeavePre
syn keyword vimAutoEvent contained	BufRead	CursorHold	FileWriteCmd	StdinReadPost	WinEnter
syn keyword vimAutoEvent contained	BufReadCmd	FileAppendCmd	FileWritePost	StdinReadPre	WinLeave
syn keyword vimAutoEvent contained	BufReadPost	FileAppendPost	FileWritePre

" Highlight commonly used Groupnames
syn keyword vimGroup contained	Comment	Identifier	Keyword	Type	Delimiter
syn keyword vimGroup contained	Constant	Function	Exception	StorageClass	SpecialComment
syn keyword vimGroup contained	String	Statement	PreProc	Structure	Debug
syn keyword vimGroup contained	Character	Conditional	Include	Typedef	Ignore
syn keyword vimGroup contained	Number	Repeat	Define	Special	Error
syn keyword vimGroup contained	Boolean	Label	Macro	SpecialChar	Todo
syn keyword vimGroup contained	Float	Operator	PreCondit	Tag

" Default highlighting groups
syn keyword vimHLGroup contained	Cursor	Folded	MoreMsg	Search	Visual
syn keyword vimHLGroup contained	CursorIM	IncSearch	NonText	SpecialKey	VisualNOS
syn keyword vimHLGroup contained	Directory	LineNr	Normal	StatusLine	WarningMsg
syn keyword vimHLGroup contained	ErrorMsg	Menu	Question	StatusLineNC	WildMenu
syn keyword vimHLGroup contained	FillColumn	ModeMsg	Scrollbar	Title
syn case match

" Function Names
syn keyword vimFuncName contained	MyCounter	exists	has	maparg	strridx
syn keyword vimFuncName contained	append	expand	hasmapto	mapcheck	strtrans
syn keyword vimFuncName contained	argc	filereadable	histadd	match	submatch
syn keyword vimFuncName contained	argv	filewritable	histdel	matchend	substitute
syn keyword vimFuncName contained	browse	fnamemodify	histget	matchstr	synID
syn keyword vimFuncName contained	bufexists	foldclosed	histnr	mode	synIDattr
syn keyword vimFuncName contained	bufloaded	foldlevel	hlID	nextnonblank	synIDtrans
syn keyword vimFuncName contained	bufname	foldtext	hlexists	nr2char	system
syn keyword vimFuncName contained	bufnr	function	hostname	prevnonblank	tempname
syn keyword vimFuncName contained	bufwinnr	getbufvar	indent	rename	tolower
syn keyword vimFuncName contained	byte2line	getcwd	input	search	toupper
syn keyword vimFuncName contained	char2nr	getfsize	inputsecret	setbufvar	type
syn keyword vimFuncName contained	col	getftime	isdirectory	setline	virtcol
syn keyword vimFuncName contained	confirm	getline	libcall	setwinvar	visualmode
syn keyword vimFuncName contained	cscope_connection	getwinposx	libcallnr	strftime	winbufnr
syn keyword vimFuncName contained	delete	getwinposy	line	stridx	winheight
syn keyword vimFuncName contained	did_filetype	getwinvar	line2byte	strlen	winnr
syn keyword vimFuncName contained	escape	glob	localtime	strpart	winwidth
syn keyword vimFuncName contained	executable

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep	"[:|]\+"	skipwhite nextgroup=vimAddress,vimAutoCmd,vimMark,vimFilter,vimUserCmd,vimSet,vimLet,vimCommand,vimSyntax,vimExtCmd
syn match vimIsCommand	"\<\a\+\>"	contains=vimCommand
syn match vimVar		"\<[bwglsav]:\K\k*\>"
syn match vimFunc		"\I\i*\s*("	contains=vimFuncName,vimCommand

" Behave!
" =======
syn match   vimBehave	"\<be\(h\(a\(ve\=\)\=\)\=\)\=\>" contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained	mswin	xterm
syn match   vimBehaveError contained	"[^ ]\+"

" Filetypes
" =========
syn match   vimFiletype	"\<filet\(y\(pe\=\)\=\)\=\s\+\(I\i*\)*"	skipwhite contains=vimFTCmd,vimFTOption,vimFTError
syn match   vimFTError  contained	"\I\i*"
syn keyword vimFTCmd    contained	filet[ype]
syn keyword vimFTOption contained	on	off	indent	plugin

" Augroup : vimAugroupError removed because long augroups caused sync'ing problems.
" ======= : Trade-off: Increasing synclines with slower editing vs augroup END error checking.
syn cluster vimAugroupList	contains=vimIsCommand,vimFunction,vimFunctionError,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar
syn region  vimAugroup	start="\<aug\(r\(o\(up\=\)\=\)\=\)\=\>\s\+\K\k*" end="\<aug\(r\(o\(up\=\)\=\)\=\)\=\>\s\+[eE][nN][dD]\>"	contains=vimAugroupKey,vimAutoCmd,@vimAugroupList keepend
syn match   vimAugroupError	"\<aug\(r\(o\(up\=\)\=\)\=\)\=\>\s\+[eE][nN][dD]\>"
syn keyword vimAugroupKey contained	aug[roup]

" Functions : Tag is provided for those who wish to highlight tagged functions
" =========
syn cluster vimFuncList	contains=vimFuncKey,Tag,vimFuncSID
syn cluster vimFuncBodyList	contains=vimIsCommand,vimFunction,vimFunctionError,vimFuncBody,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar
syn match   vimFunctionError	"\<fu\(n\(c\(t\(i\(on\=\)\=\)\=\)\=\)\=\)\=!\=\s\+\U.\{-}("me=e-1	contains=vimFuncKey,vimFuncBlank nextgroup=vimFuncBody
syn match   vimFunction	"\<fu\(n\(c\(t\(i\(on\=\)\=\)\=\)\=\)\=\)\=!\=\s\+\(<[sS][iI][dD]>\|s:\|\u\)\w*("me=e-1	contains=@vimFuncList nextgroup=vimFuncBody
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
syn match   vimUserCmd	"\<com\(m\(a\(nd\=\)\=\)\=\)\=!\=\>.*$"		contains=vimUserAttrb,@vimUserCmdList
syn match   vimUserAttrb	contained	"-n\(a\(r\(gs\=\)\=\)\=\)\==[01*?+]"	contains=vimUserAttrbKey,vimOper
syn match   vimUserAttrb	contained	"-com\(p\(l\(e\(te\=\)\=\)\=\)\=\)\==\(augroup\|buffer\|command\|dir\|event\|file\|help\|highlight\|menu\|option\|tag\|var\)"	contains=vimUserAttrbKey,vimUserAttrbCmplt,vimOper
syn match   vimUserAttrb	contained	"-ra\(n\(ge\=\)\=\)\=\(=%\|=\d\+\)\="	contains=vimNumber,vimOper,vimUserAttrbKey
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
syn match  vimComment	excludenl +\s"[^\-:.%#=*].*$+lc=1	contains=@vimCommentGroup,vimCommentString
syn match  vimComment	+\<endif\s\+".*$+lc=5	contains=@vimCommentGroup,vimCommentString
syn match  vimComment	+\<else\s\+".*$+lc=4	contains=@vimCommentGroup,vimCommentString
syn region vimCommentString	contained oneline start='\S\s\+"'ms=s+1	end='"'

" Environment Variables
" =====================
syn match vimEnvvar	"\$\I\i*"
syn match vimEnvvar	"\${\I\i*}"

" Try to catch strings, if nothing else matches (therefore it must precede the others!)
"  vmEscapeBrace handles ["]  []"] (ie. stays as string)
syn region vimEscapeBrace	oneline contained transparent	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1
syn match  vimPatSep	contained	"\\|"
syn match  vimPatSepErr	contained	"\\)"
syn region vimPatRegion	contained	matchgroup=vimPatSep start="\\z\=(" end="\\)"	contains=@vimSubstList oneline
syn match  vimNotPatSep	contained	"\\\\"
syn region vimString	oneline	start=+[^:a-zA-Z>!\\]"+lc=1 skip=+\\\\\|\\"+ end=+"+	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region vimString	oneline	start=+[^:a-zA-Z>!\\]'+lc=1 skip=+\\\\\|\\'+ end=+'+	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region vimString	oneline	start=+=!+lc=1	skip=+\\\\\|\\!+ end=+!+		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region vimString	oneline	start="=+"lc=1	skip="\\\\\|\\+" end="+"		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region vimString	oneline	start="[^\\]+\s*[^a-zA-Z0-9. \t]"lc=1 skip="\\\\\|\\+" end="+"	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region vimString	oneline	start="\s/\s*\A"lc=1 skip="\\\\\|\\+" end="/"		contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn match  vimString	contained	+"[^"]*\\$+	skipnl nextgroup=vimStringCont
syn match  vimStringCont	contained	+\(\\\\\|.\)\{-}[^\\]"+

" Substitutions
" =============
syn cluster vimSubstList	contains=vimPatSep,vimPatRegion,vimPatSepErr,vimSubstTwoBS,vimSubstRange,vimNotation
syn cluster vimSubstRepList	contains=vimSubstSubstr,vimSubstTwoBS,vimNotation
syn cluster vimSubstList	add=vimCollection
syn match   vimSubst	"\(:\+\s*\|^\s*\||\s*\)\<s\(u\(b\(s\(t\(i\(t\(u\(te\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\>"	nextgroup=vimSubstPat
syn match   vimSubst1    contained	"s\(u\(b\(s\(t\(i\(t\(u\(te\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\>"		nextgroup=vimSubstPat
syn region  vimSubstPat  contained	matchgroup=vimSubstDelim start="\z([^a-zA-Z \t[\]&]\)"rs=s+1 skip="\\\\\|\\\z1" end="\z1"re=e-1,me=e-1	 contains=@vimSubstList	nextgroup=vimSubstRep4	oneline
syn region  vimSubstRep4 contained	matchgroup=vimSubstDelim start="\z(.\)" skip="\\\\\|\\\z1" end="\z1" matchgroup=vimNotation end="<[cC][rR]>" contains=@vimSubstRepList	nextgroup=vimSubstFlagErr
syn region  vimCollection contained	start="\[" skip="\\\[" end="\]"	contains=vimCollClass
syn match   vimCollClassErr contained "\[:.\{-\}:\]"
syn match   vimCollClass contained	"\[:\(alnum\|alpha\|blank\|cntrl\|digit\|graph\|lower\|print\|punct\|space\|upper\|xdigit\|return\|tab\|escape\|backspace\):\]"
syn match   vimSubstSubstr contained	"\\z\=\d"
syn match   vimSubstTwoBS contained	"\\\\"
syn match   vimSubstFlagErr contained	"[^< \t]\+" contains=vimSubstFlags
syn match   vimSubstFlags contained	"[&cegiIpr]\+"

" Marks, Registers, Addresses, Filters
syn match  vimMark	"[!,:]'[a-zA-Z0-9]"lc=1
syn match  vimMark	"'[a-zA-Z0-9][,!]"me=e-1
syn match  vimMark	"'[<>][,!]"me=e-1
syn match  vimMark	"\<norm\s'[a-zA-Z0-9]"lc=5
syn match  vimMark	"\<normal\s'[a-zA-Z0-9]"lc=7
syn match  vimPlainMark contained	"'[a-zA-Z0-9]"

syn match  vimRegister	'[^(,;.]"[a-zA-Z0-9\-:.%#*=][^a-zA-Z_"]'lc=1,me=e-1
syn match  vimRegister	'\<norm\s\+"[a-zA-Z0-9]'lc=5
syn match  vimRegister	'\<normal\s\+"[a-zA-Z0-9]'lc=7
syn match  vimPlainRegister contained	'"[a-zA-Z0-9\-:.%#*=]'

syn match  vimAddress	",[.$]"lc=1	skipwhite nextgroup=vimSubst1
syn match  vimAddress	"%\a"me=e-1	skipwhite nextgroup=vimString,vimSubst1

syn match  vimFilter contained	"^!.\{-}\(|\|$\)"	contains=vimSpecFile
syn match  vimFilter contained	"\A!.\{-}\(|\|$\)"ms=s+1	contains=vimSpecFile

" Complex repeats (:h complex-repeat)
syn match  vimCmplxRepeat		'[^a-zA-Z_/\\]q[0-9a-zA-Z"]'lc=1
syn match  vimCmplxRepeat		'@[0-9a-z".=@:]'

" Set command and associated set-options (vimOptions) with comment
syn region vimSet		matchgroup=vimCommand start="\<set\>" end="|"me=e-1 end="$" matchgroup=vimNotation end="<[cC][rR]>" keepend contains=vimSetEqual,vimOption,vimErrSetting,vimComment,vimSetString
syn region vimSetEqual contained	start="="	skip="\\\\\|\\\s" end="[| \t]\|$"me=e-1 contains=vimCtrlChar,vimSetSep,vimNotation
syn region vimSetString contained	start=+="+hs=s+1	skip=+\\\\\|\\"+  end=+"+	contains=vimCtrlChar
syn match  vimSetSep contained	"[,:]"

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
syn region  vimEcho	oneline excludenl matchgroup=vimCommand start="\<ec\(ho\=\)\=\>" skip="\(\\\\\)*\\|" end="$\||" contains=vimFuncName,vimString,vimOper,varVar
syn region  vimExecute	oneline excludenl matchgroup=vimCommand start="\<exe\(c\(u\(te\=\)\=\)\=\)\=\>" skip="\(\\\\\)*\\|" end="$\||\|<[cC][rR]>" contains=vimIsCommand,vimString,vimOper,vimVar,vimNotation
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
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam]-\)\{0,4}\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|del\(ete\)\=\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k \=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
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
syn region  vimSynMatchRegion	contained oneline matchgroup=vimGroupName start="\k\+" matchgroup=vimSep end="$\||" contains=vimComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn match   vimSynMtchOpt	contained	"\<\(contained\|display\|excludenl\|fold\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: off and on
syn keyword vimSynType	contained	off	on

" Syntax: region
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained oneline matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(contained\|display\|excludenl\|fold\|transparent\|skipempty\|skipwhite\|skipnl\|oneline\|keepend\)\>"
syn match   vimSynReg	contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="	nextgroup=vimGroup,vimHLGroup
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn region  vimSynRegPat	contained oneline	start="\z([[:punct:]]\)"  skip="\\\\\|\\\z1"  end="\z1"  contains=@vimSynRegPatGroup skipwhite nextgroup=vimSynPatMod,vimSynReg
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\="
syn match   vimSynPatMod	contained	"\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\=," nextgroup=vimSynPatMod
syn match   vimSynPatMod	contained	"lc=\d\+"
syn match   vimSynPatMod	contained	"lc=\d\+," nextgroup=vimSynPatMod
syn region  vimSynPatRange	contained oneline start="\["	skip="\\\\\|\\]"   end="]"
syn match   vimSynNotPatRange	contained	"\\\\\|\\\["

" Syntax: sync
" ============
syn keyword vimSynType	contained	sync	skipwhite	nextgroup=vimSyncC,vimSyncLines,vimSyncMatch,vimSyncError,vimSyncLinecont,vimSyncRegion
syn match   vimSyncError	contained	"\i\+"
syn keyword vimSyncC	contained	ccomment	clear
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
syn region vimHiLink	contained oneline matchgroup=vimCommand start="\(def\s\+\)\=link" end="$"	contains=vimHiGroup,vimGroup,vimHLGroup

" Control Characters
" ==================
syn match vimCtrlChar	"[--]"

" Beginners - Patterns that involve ^
" =========
syn match  vimLineComment	+^[ \t:]*".*$+		contains=@vimCommentGroup,vimCommentString,vimCommentTitle
syn match  vimCommentTitle	'"\s*\u\a*\(\s\+\u\a*\)*:'hs=s+1	contained contains=vimCommentTitleLeader
syn match  vimContinue	"^\s*\\"
syn match  vimCommentTitleLeader	'"\s\+'ms=s+1		contained

" Synchronize (speed)
"============
syn sync linecont	"^\s\+\\"
syn sync minlines=10	maxlines=100
syn sync match vimAugroupSyncA	groupthere NONE	"\<aug\(r\(o\(up\=\)\=\)\=\)\=\>\s\+[eE][nN][dD]"

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
hi def link vimKeyCode	vimSpecFile
hi def link vimKeyCodeError	vimError
hi def link vimLet	vimCommand
hi def link vimLineComment	vimComment
hi def link vimMap	vimCommand
hi def link vimMapMod	vimBracket
hi def link vimMapModErr	vimError
hi def link vimMapModKey	vimFuncSID
hi def link vimMapBang	vimCommand
hi def link vimNotFunc	vimCommand
hi def link vimNotPatSep	vimString
hi def link vimPatSepErr	vimError
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
hi def link vimUserCmd	vimCommand

hi def link vimAutoEvent	Type
hi def link vimBracket	Delimiter
hi def link vimCmplxRepeat	SpecialChar
hi def link vimCommand	Statement
hi def link vimComment	Comment
hi def link vimCommentTitle	PreProc
hi def link vimContinue	Special
hi def link vimCtrlChar	SpecialChar
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
