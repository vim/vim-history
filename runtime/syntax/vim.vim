" Vim syntax file
" Language:	Vim 6.0j script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	October 23, 2000
" Version:	6.0j-01

" Remove old syntax
syn clear

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	N[ext]	cu[nmap]	k	ptf[irst]	star[tinsert]
syn keyword vimCommand contained	P[rint]	cunme	lan[guage]	ptj[ump]	stj[ump]
syn keyword vimCommand contained	X	cunmenu	la[st]	ptl[ast]	st[op]
syn keyword vimCommand contained	ab[breviate]	cw[indow]	lc[d]	ptn[ext]	sts[elect]
syn keyword vimCommand contained	abc[lear]	delc[ommand]	lch[dir]	ptp[revious]	sun[hide]
syn keyword vimCommand contained	al[l]	d[elete]	le[ft]	ptr[ewind]	sus[pend]
syn keyword vimCommand contained	am	delf[unction]	l[ist]	pts[elect]	sv[iew]
syn keyword vimCommand contained	amenu	dig[raphs]	ls	pu[t]	syncbind
syn keyword vimCommand contained	an	di[splay]	mak[e]	pw[d]	t
syn keyword vimCommand contained	anoremenu	dj[ump]	ma[rk]	pyf[ile]	tN[ext]
syn keyword vimCommand contained	a[ppend]	dl[ist]	marks	py[thon]	ta[g]
syn keyword vimCommand contained	ar[gs]	dr[op]	me	qa[ll]	tags
syn keyword vimCommand contained	argu[ment]	ds[earch]	menu	q[uit]	tc[l]
syn keyword vimCommand contained	as[cii]	dsp[lit]	menu-disable	quita[ll]	tcld[o]
syn keyword vimCommand contained	aun	echon	menu-enable	r[ead]	tclf[ile]
syn keyword vimCommand contained	aunmenu	e[dit]	menu-<script>	rec[over]	te[aroff]
syn keyword vimCommand contained	bN[ext]	el[se]	menut[rans]	redi[r]	tf[irst]
syn keyword vimCommand contained	bad[d]	elsei[f]	mk[exrc]	red[o]	tj[ump]
syn keyword vimCommand contained	ba[ll]	em[eenu]	mks[ession]	reg[isters]	tl[ast]
syn keyword vimCommand contained	bd[elete]	emenu	mkv[imrc]	res[ize]	tm[enu]
syn keyword vimCommand contained	bf[irst	endf[unction]	mod[e]	ret[ab]	tn[ext]
syn keyword vimCommand contained	bl[ast]	en[dif]	m[ove]	retu[rn]	to[pleft]
syn keyword vimCommand contained	bm[odified]	endw[hile]	new	rew[ind]	tp[revious]
syn keyword vimCommand contained	bn[ext]	ene[w]	n[ext]	ri[ght]	tr[ewind]
syn keyword vimCommand contained	bo[tright]	ex	nmapc[lear]	rub[y]	ts[elect]
syn keyword vimCommand contained	bp[revious]	exi[t]	nme	rubyd[o]	tu[nmenu]
syn keyword vimCommand contained	brea[k]	f[ile]	nmenu	rubyf[ile]	una[bbreviate]
syn keyword vimCommand contained	br[ewind]	files	nnoreme	ru[ntime]	u[ndo]
syn keyword vimCommand contained	bro[wse]	filetype	nnoremenu	rv[iminfo]	unh[ide]
syn keyword vimCommand contained	b[uffer]	fin[d]	noh[lsearch]	sN[ext]	unm[ap]
syn keyword vimCommand contained	buffers	fini[sh]	norea[bbrev]	sal[l]	unme
syn keyword vimCommand contained	bun[load]	fir[st]	noreme	sa[rgument]	unmenu
syn keyword vimCommand contained	cN[ext]	fix[del]	noremenu	sbN[ext]	uns[ign]
syn keyword vimCommand contained	ca[bbrev]	fo[ld]	norm[al]	sba[ll]	ve[rsion]
syn keyword vimCommand contained	cabc[lear]	foldo[pen]	nu[mber]	sbf[irst]	vert[ical]
syn keyword vimCommand contained	cal[l]	fu[nction]	nun[map]	sbl[ast]	v[global]
syn keyword vimCommand contained	cc	g[lobal]	nunme	sbm[odified]	vie[w]
syn keyword vimCommand contained	cd	go[to]	nunmenu	sbn[ext]	vi[sual]
syn keyword vimCommand contained	ce[nter]	gr[ep]	omapc[lear]	sbp[revious]	vmapc[lear]
syn keyword vimCommand contained	cf[ile]	h[elp]	ome	sbr[ewind]	vme
syn keyword vimCommand contained	cfir[st]	helpf[ind]	omenu	sb[uffer]	vmenu
syn keyword vimCommand contained	c[hange]	helpt[ags]	on[ly]	scrip[tnames]	vne[w]
syn keyword vimCommand contained	chd[ir]	hid[e]	onoreme	se[t]	vnoreme
syn keyword vimCommand contained	che[ckpath]	his[tory]	onoremenu	setf[iletype]	vnoremenu
syn keyword vimCommand contained	cla[st]	ia[bbrev]	o[pen]	setg[lobal]	vs[plit]
syn keyword vimCommand contained	cl[ist]	iabc[lear]	opt[ions]	setl[ocal]	vu[nmap]
syn keyword vimCommand contained	clo[se]	if	ou[nmap]	sf[ind]	vunme
syn keyword vimCommand contained	cmapc[lear]	ij[ump]	ounme	sfir[st	vunmenu
syn keyword vimCommand contained	cme	il[ist]	ounmenu	sh[ell]	wN[ext]
syn keyword vimCommand contained	cmenu	imapc[lear]	pc[lose]	sig[n]	wa[ll]
syn keyword vimCommand contained	cnew[er]	ime	ped[it]	signs	wh[ile]
syn keyword vimCommand contained	cn[ext]	imenu	pe[rl]	sil[ent]	win
syn keyword vimCommand contained	cnf[ile]	inorea[bbrev]	perld[o]	si[malt]	winp[os]
syn keyword vimCommand contained	cnorea[bbrev]	inoreme	po[p]	sla[st]	win[size]
syn keyword vimCommand contained	cnoreme	inoremenu	pop[up]	sl[eep]	wn[ext]
syn keyword vimCommand contained	cnoremenu	i[nsert]	pp[op]	sm[agic]	wp[revous]
syn keyword vimCommand contained	col[der]	is[earch]	pre[serve]	sn[ext]	wq
syn keyword vimCommand contained	comc[lear]	isp[lit]	prev[ious]	sni[ff]	wqa[ll]
syn keyword vimCommand contained	conf[irm]	iuna[bbrev]	p[rint]	sno[magic]	w[rite]
syn keyword vimCommand contained	con[tinue]	iu[nmap]	promptf[ind]	so[urce]	ws[verb]
syn keyword vimCommand contained	co[py]	iunme	promptr[epl]	sp[lit]	wv[iminfo]
syn keyword vimCommand contained	cp[revious]	iunmenu	ps[earch]	spr[evious]	xa[ll]
syn keyword vimCommand contained	cq[uit]	j[oin]	ptN[ext]	sr[ewind]	x[it]
syn keyword vimCommand contained	cr[ewind]	ju[mps]	pta[g]	sta[g]	y[ank]
syn keyword vimCommand contained	cuna[bbrev]
syn match   vimCommand contained	"\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	ek	indk	report	syntax
syn keyword vimOption contained	ai	endofline	inex	restorescreen	ta
syn keyword vimOption contained	akm	eol	inf	revins	tabstop
syn keyword vimOption contained	al	ep	infercase	ri	tag
syn keyword vimOption contained	aleph	equalalways	insertmode	rightleft	tagbsearch
syn keyword vimOption contained	allowrevins	equalprg	is	rl	taglength
syn keyword vimOption contained	altkeymap	errorbells	isf	ro	tagrelative
syn keyword vimOption contained	ar	errorfile	isfname	rs	tags
syn keyword vimOption contained	ari	errorformat	isi	rtp	tagstack
syn keyword vimOption contained	autoindent	esckeys	isident	ru	tb
syn keyword vimOption contained	autoread	et	isk	ruf	tbi
syn keyword vimOption contained	autowrite	eventignore	iskeyword	ruler	tbs
syn keyword vimOption contained	aw	ex	isp	rulerformat	term
syn keyword vimOption contained	background	expandtab	isprint	runtimepath	terse
syn keyword vimOption contained	backspace	exrc	joinspaces	sb	textauto
syn keyword vimOption contained	backup	fcc	js	sbo	textmode
syn keyword vimOption contained	backupcopy	fccs	key	sbr	textwidth
syn keyword vimOption contained	backupdir	fcs	keymap	sc	tf
syn keyword vimOption contained	backupext	fdc	keymodel	scb	tgst
syn keyword vimOption contained	balloondelay	fde	keywordprg	scr	thesaurus
syn keyword vimOption contained	ballooneval	fdi	km	scroll	tildeop
syn keyword vimOption contained	bdir	fdl	kmp	scrollbind	timeout
syn keyword vimOption contained	bdlay	fdm	kp	scrolljump	timeoutlen
syn keyword vimOption contained	beval	fdt	langmap	scrolloff	title
syn keyword vimOption contained	bex	fe	langmenu	scrollopt	titlelen
syn keyword vimOption contained	bg	fen	laststatus	scs	titleold
syn keyword vimOption contained	bin	ff	lazyredraw	sect	titlestring
syn keyword vimOption contained	binary	ffs	lbr	sections	tl
syn keyword vimOption contained	biosk	filecharcode	lcs	secure	tm
syn keyword vimOption contained	bioskey	filecharcodes	linebreak	sel	to
syn keyword vimOption contained	bk	fileencoding	lines	selection	toolbar
syn keyword vimOption contained	bkc	fileformat	linespace	selectmode	top
syn keyword vimOption contained	breakat	fileformats	lisp	sessionoptions	tr
syn keyword vimOption contained	brk	filetype	list	sft	ts
syn keyword vimOption contained	browsedir	fillchars	listchars	sh	tsl
syn keyword vimOption contained	bs	fk	lmap	shcf	tsr
syn keyword vimOption contained	bsdir	fkmap	lmenu	shell	ttimeout
syn keyword vimOption contained	bt	fmr	loadplugins	shellcmdflag	ttimeoutlen
syn keyword vimOption contained	buftype	fo	lpl	shellpipe	ttm
syn keyword vimOption contained	cb	foldcolumn	ls	shellquote	ttybuiltin
syn keyword vimOption contained	cc	foldenable	lsp	shellredir	ttyfast
syn keyword vimOption contained	ccv	foldexpr	lz	shellslash	ttym
syn keyword vimOption contained	cd	foldignore	magic	shelltype	ttymouse
syn keyword vimOption contained	cdpath	foldlevel	makeef	shellxquote	ttyscroll
syn keyword vimOption contained	cf	foldmarker	makeprg	shiftround	ttytype
syn keyword vimOption contained	ch	foldmethod	mat	shiftwidth	tw
syn keyword vimOption contained	charcode	foldtext	matchpairs	shm	tx
syn keyword vimOption contained	charconvert	formatoptions	matchtime	shortmess	uc
syn keyword vimOption contained	cin	formatprg	maxfuncdepth	shortname	ul
syn keyword vimOption contained	cindent	fp	maxmapdepth	showbreak	undolevels
syn keyword vimOption contained	cink	ft	maxmem	showcmd	updatecount
syn keyword vimOption contained	cinkeys	gcr	maxmemtot	showfulltag	updatetime
syn keyword vimOption contained	cino	gd	mef	showmatch	ut
syn keyword vimOption contained	cinoptions	gdefault	menuitems	showmode	vb
syn keyword vimOption contained	cinw	gfm	mfd	shq	vbs
syn keyword vimOption contained	cinwords	gfn	mh	si	ve
syn keyword vimOption contained	clipboard	gfs	mis	sidescroll	verbose
syn keyword vimOption contained	cmdheight	gfw	ml	sidescrolloff	vi
syn keyword vimOption contained	co	ghr	mls	siso	viminfo
syn keyword vimOption contained	columns	go	mm	sj	virtualedit
syn keyword vimOption contained	com	gp	mmd	slm	visualbell
syn keyword vimOption contained	comments	grepformat	mmt	sm	wa
syn keyword vimOption contained	compatible	grepprg	mod	smartcase	wak
syn keyword vimOption contained	complete	guicursor	modeline	smartindent	warn
syn keyword vimOption contained	confirm	guifont	modelines	smarttab	wb
syn keyword vimOption contained	consk	guifontset	modified	smd	wc
syn keyword vimOption contained	conskey	guifontwide	more	sn	wcm
syn keyword vimOption contained	cp	guiheadroom	mouse	so	wd
syn keyword vimOption contained	cpo	guioptions	mousef	softtabstop	weirdinvert
syn keyword vimOption contained	cpoptions	guipty	mousefocus	sol	wh
syn keyword vimOption contained	cpt	helpfile	mousehide	sp	whichwrap
syn keyword vimOption contained	cscopeprg	helpheight	mousem	splitbelow	wig
syn keyword vimOption contained	cscopetag	hf	mousemodel	splitright	wildchar
syn keyword vimOption contained	cscopetagorder	hh	mouses	spr	wildcharm
syn keyword vimOption contained	cscopeverbose	hi	mouseshape	sr	wildignore
syn keyword vimOption contained	csprg	hid	mouset	srr	wildmenu
syn keyword vimOption contained	cst	hidden	mousetime	ss	wildmode
syn keyword vimOption contained	csto	highlight	mp	ssl	wim
syn keyword vimOption contained	csverb	history	mps	ssop	winaltkeys
syn keyword vimOption contained	def	hk	nf	st	winheight
syn keyword vimOption contained	define	hkmap	nrformats	sta	winminheight
syn keyword vimOption contained	dg	hkmapp	nu	startofline	winminwidth
syn keyword vimOption contained	dict	hkp	number	statusline	winwidth
syn keyword vimOption contained	dictionary	hl	oft	stl	wiv
syn keyword vimOption contained	digraph	hls	osfiletype	sts	wiw
syn keyword vimOption contained	dir	hlsearch	pa	su	wm
syn keyword vimOption contained	directory	ic	para	sua	wmh
syn keyword vimOption contained	display	icon	paragraphs	suffixes	wmnu
syn keyword vimOption contained	dy	iconstring	paste	suffixesadd	wmw
syn keyword vimOption contained	ea	ignorecase	pastetoggle	sw	wrap
syn keyword vimOption contained	ead	im	patchmode	swapfile	wrapmargin
syn keyword vimOption contained	eadirection	inc	path	swapsync	wrapscan
syn keyword vimOption contained	eb	include	pm	swb	write
syn keyword vimOption contained	ed	includeexpr	previewheight	swf	writeany
syn keyword vimOption contained	edcompatible	incsearch	pt	switchbuf	writebackup
syn keyword vimOption contained	ef	inde	pvh	sws	writedelay
syn keyword vimOption contained	efm	indentexpr	readonly	sxq	ws
syn keyword vimOption contained	ei	indentkeys	remap	syn	ww

" These are the turn-off setting variants
syn keyword vimOption contained	loadplugins	noed	noinfercase	noru	notagrelative
syn keyword vimOption contained	noai	noedcompatible	noinsertmode	noruler	notagstack
syn keyword vimOption contained	noakm	noek	nois	nosb	notbi
syn keyword vimOption contained	noallowrevins	noendofline	nojoinspaces	nosc	notbs
syn keyword vimOption contained	noaltkeymap	noeol	nojs	noscb	noterse
syn keyword vimOption contained	noar	noequalalways	nolazyredraw	noscrollbind	notextauto
syn keyword vimOption contained	noari	noerrorbells	nolbr	noscs	notextmode
syn keyword vimOption contained	noautoindent	noesckeys	nolinebreak	nosecure	notf
syn keyword vimOption contained	noautoread	noet	nolisp	nosft	notgst
syn keyword vimOption contained	noautowrite	noex	nolist	noshellslash	notildeop
syn keyword vimOption contained	noaw	noexpandtab	noloadplugins	noshiftround	notimeout
syn keyword vimOption contained	nobackup	noexrc	nolpl	noshortname	notitle
syn keyword vimOption contained	noballooneval	nofdc	nolz	noshowcmd	noto
syn keyword vimOption contained	nobeval	nofen	nomagic	noshowfulltag	notop
syn keyword vimOption contained	nobin	nofk	nomh	noshowmatch	notr
syn keyword vimOption contained	nobinary	nofkmap	noml	noshowmode	nottimeout
syn keyword vimOption contained	nobiosk	nofoldcolumn	nomod	nosi	nottybuiltin
syn keyword vimOption contained	nobioskey	nofoldenable	nomodeline	nosm	nottyfast
syn keyword vimOption contained	nobk	nogd	nomodified	nosmartcase	notx
syn keyword vimOption contained	nocf	nogdefault	nomore	nosmartindent	novb
syn keyword vimOption contained	nocin	noguipty	nomousef	nosmarttab	novisualbell
syn keyword vimOption contained	nocindent	nohid	nomousefocus	nosmd	nowa
syn keyword vimOption contained	nocompatible	nohidden	nomousehide	nosn	nowarn
syn keyword vimOption contained	noconfirm	nohk	nonu	nosol	nowb
syn keyword vimOption contained	noconsk	nohkmap	nonumber	nosplitbelow	noweirdinvert
syn keyword vimOption contained	noconskey	nohkmapp	nopaste	nosplitright	nowildmenu
syn keyword vimOption contained	nocp	nohkp	noreadonly	nospr	nowiv
syn keyword vimOption contained	nocscopetag	nohls	noremap	nosr	nowmnu
syn keyword vimOption contained	nocscopeverbose	nohlsearch	norestorescreen	nossl	nowrap
syn keyword vimOption contained	nocst	noic	norevins	nosta	nowrapscan
syn keyword vimOption contained	nocsverb	noicon	nori	nostartofline	nowrite
syn keyword vimOption contained	nodg	noignorecase	norightleft	noswapfile	nowriteany
syn keyword vimOption contained	nodigraph	noim	norl	noswf	nowritebackup
syn keyword vimOption contained	noea	noincsearch	noro	nota	nows
syn keyword vimOption contained	noeb	noinf	nors	notagbsearch

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
syn keyword vimHLGroup contained	Cursor	Folded	MoreMsg	Search	Visual
syn keyword vimHLGroup contained	CursorIM	IncSearch	NonText	SpecialKey	VisualNOS
syn keyword vimHLGroup contained	Directory	LineNr	Normal	StatusLine	WarningMsg
syn keyword vimHLGroup contained	ErrorMsg	Menu	Question	StatusLineNC	WildMenu
syn keyword vimHLGroup contained	FillColumn	ModeMsg	Scrollbar	Title
syn case match

" Function Names
syn keyword vimFuncName contained	MyCounter	exists	hasmapto	mapcheck	strtrans
syn keyword vimFuncName contained	append	expand	histadd	match	submatch
syn keyword vimFuncName contained	argc	filereadable	histdel	matchend	substitute
syn keyword vimFuncName contained	argv	fnamemodify	histget	matchstr	synID
syn keyword vimFuncName contained	browse	foldclosed	histnr	mode	synIDattr
syn keyword vimFuncName contained	bufexists	foldlevel	hlID	nr2char	synIDtrans
syn keyword vimFuncName contained	bufloaded	function	hlexists	rename	system
syn keyword vimFuncName contained	bufname	getbufvar	hostname	search	tempname
syn keyword vimFuncName contained	bufnr	getcwd	indent	setbufvar	tolower
syn keyword vimFuncName contained	bufwinnr	getfsize	input	setline	toupper
syn keyword vimFuncName contained	byte2line	getftime	isdirectory	setwinvar	type
syn keyword vimFuncName contained	char2nr	getline	libcall	skipblank	virtcol
syn keyword vimFuncName contained	col	getwinposx	libcallnr	strftime	visualmode
syn keyword vimFuncName contained	confirm	getwinposy	line	stridx	winbufnr
syn keyword vimFuncName contained	cscope_connection	getwinvar	line2byte	strlen	winheight
syn keyword vimFuncName contained	delete	glob	localtime	strpart	winnr
syn keyword vimFuncName contained	did_filetype	has	maparg	strridx	winwidth
syn keyword vimFuncName contained	escape

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep	"[:|]\+"	skipwhite nextgroup=vimAddress,vimAutoCmd,vimMark,vimFilter,vimUserCmd,vimSet,vimLet,vimCommand,vimSyntax,vimExtCmd
syn match vimIsCommand	"\<\a\+\>"	contains=vimCommand
syn match vimVar		"\<[bwglsav]:\K\k*\>"
syn match vimFunc     	"\I\i*\s*("	contains=vimFuncName,vimCommand

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
syn cluster vimFuncList	contains=vimCommand,Tag
syn cluster vimFuncBodyList	contains=vimIsCommand,vimFunction,vimFunctionError,vimFuncBody,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar
syn match   vimFunction	"\<fu\(n\(c\(t\(t\(i\(on\=\)\=\)\=\)\=\)\=\)\=\)\=!\=\s\+\u\w*("me=e-1	contains=@vimFuncList nextgroup=vimFuncBody
syn match   vimFunctionError	"\<fu\(n\(c\(t\(t\(i\(on\=\)\=\)\=\)\=\)\=\)\=\)\=!\=\s\+\U.\{-}("me=e-1	contains=vimCommand   nextgroup=vimFuncBody
syn region  vimFuncBody contained	start=")"	end="\<endf"	contains=@vimFuncBodyList
syn match   vimFuncVar  contained	"a:\(\I\i*\|\d\+\)"

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
syn region vimString	oneline	start="[^\\]+\s*[^a-zA-Z0-9.]"lc=1 skip="\\\\\|\\+" end="+"	contains=vimEscapeBrace,vimPatSep,vimNotPatSep
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
syn cluster vimMapGroup	contains=vimMapBang,vimMapLhs
syn keyword vimMap	cm[ap]	map	om[ap]	skipwhite nextgroup=@vimMapGroup
syn keyword vimMap	cno[remap]	nm[ap]	ono[remap]	skipwhite nextgroup=@vimMapGroup
syn keyword vimMap	im[ap]	nn[oremap]	vm[ap]	skipwhite nextgroup=@vimMapGroup
syn keyword vimMap	ino[remap]	no[remap]	vn[oremap]	skipwhite nextgroup=@vimMapGroup
syn match   vimMapLhs contained	"\S\+"	contains=vimNotation,vimCtrlChar
syn match   vimMapBang contained	"!"	skipwhite nextgroup=vimMapLhs

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
syn match   vimSynKeyOpt	contained	"\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

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
syn match   vimSynReg	contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="	nextgroup=vimGroup,vimHLGroup
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn region  vimSynRegPat	contained oneline	start="\z([[:punct:]]\)"  skip="\\\\\|\\\z1"  end="\z1"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
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
syn region vimHiLink	contained oneline matchgroup=vimCommand start="link" end="$"	contains=vimHiGroup,vimGroup,vimHLGroup

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam]-\)\{0,4}\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|del\(ete\)\=\|bs\|backspace\|tab\|esc\|right\|left\|Help\|Undo\|Insert\|Ins\|k\=Home\|k \=End\|kPlus\|kMinus\|kDivide\|kMultiply\|kEnter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation	"\(\\\|<lt>\)\=<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\|drag\|release\)>" contains=vimBracket
syn match vimNotation	"\(\\\|<lt>\)\=<\(bslash\|space\|bar\|nop\|nul\|lt\)>"		contains=vimBracket
syn match vimNotation	'\(\\\|<lt>\)\=<C-R>[0-9a-z"%#:.\-=]'he=e-1			contains=vimBracket
syn match vimNotation	'\(\\\|<lt>\)\=<\(line[12]\|count\|bang\|reg\|args\|lt\|[qf]-args\)>'	contains=vimBracket
syn match vimBracket contained	"[\\<>]"
syn case match

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
if !exists("did_vim_syntax_inits")
  let did_vim_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later
  hi link vimAuHighlight	vimHighlight
  hi link vimSubst1	vimSubst

  hi link vimAddress	vimMark
  hi link vimAugroupKey	vimCommand
"  hi link vimAugroupError	vimError
  hi link vimAutoCmd	vimCommand
  hi link vimAutoCmdOpt	vimOption
  hi link vimAutoSet	vimCommand
  hi link vimBehaveError	vimError
  hi link vimCollClassErr	vimError
  hi link vimCommentString	vimString
  hi link vimCondHL	vimCommand
  hi link vimEchoHL	vimCommand
  hi link vimEchoHLNone	vimGroup
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
  hi link vimMap		vimCommand
  hi link vimMapBang	vimCommand
  hi link vimNotFunc	vimCommand
  hi link vimNotPatSep	vimString
  hi link vimPatSepErr	vimError
  hi link vimPlainMark	vimMark
  hi link vimPlainRegister	vimRegister
  hi link vimSetString	vimString
  hi link vimSpecFileMod	vimSpecFile
  hi link vimStringCont	vimString
  hi link vimSubst	vimCommand
  hi link vimSubstFlagErr	vimError
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
  hi link vimMark		Number
  hi link vimNotation	Special
  hi link vimNumber	Number
  hi link vimOper		Operator
  hi link vimOption	PreProc
  hi link vimPatSep	SpecialChar
  hi link vimPattern	Type
  hi link vimRegister	SpecialChar
  hi link vimSetSep	Statement
  hi link vimSpecFile	Identifier
  hi link vimSpecial	Type
  hi link vimStatement	Statement
  hi link vimString	String
  hi link vimSubstDelim	Delimiter
  hi link vimSubstFlags	Special
  hi link vimSubstSubstr	SpecialChar
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

" vim: ts=18
