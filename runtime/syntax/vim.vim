" Vim syntax file
" Language:	Vim 6.0t script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	January 31, 2001
" Version:	6.0t-02

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	N[ext]	col[der]	iuna[bbrev]	promptf[ind]	sr[ewind]
syn keyword vimCommand contained	Nread	comc[lear]	iu[nmap]	promptr[epl]	sta[g]
syn keyword vimCommand contained	Nw	conf[irm]	iunme	ps[earch]	stj[ump]
syn keyword vimCommand contained	P[rint]	con[tinue]	iunmenu	ptN[ext]	st[op]
syn keyword vimCommand contained	X	co[py]	j[oin]	pta[g]	sts[elect]
syn keyword vimCommand contained	ab[breviate]	cp[revious]	ju[mps]	ptf[irst]	sun[hide]
syn keyword vimCommand contained	abc[lear]	cq[uit]	k	ptj[ump]	sus[pend]
syn keyword vimCommand contained	al[l]	cr[ewind]	lan[guage]	ptl[ast]	sv[iew]
syn keyword vimCommand contained	am	cuna[bbrev]	la[st]	ptn[ext]	syncbind
syn keyword vimCommand contained	amenu	cu[nmap]	lc[d]	ptp[revious]	t
syn keyword vimCommand contained	an	cunme	lch[dir]	ptr[ewind]	tN[ext]
syn keyword vimCommand contained	anoremenu	cunmenu	le[ft]	pts[elect]	ta[g]
syn keyword vimCommand contained	arga[dd]	cw[indow]	l[ist]	pu[t]	tags
syn keyword vimCommand contained	argd[elete]	delc[ommand]	lo[adview]	pw[d]	tc[l]
syn keyword vimCommand contained	argdo	d[elete]	ls	pyf[ile]	tcld[o]
syn keyword vimCommand contained	arge[dit]	delf[unction]	mak[e]	py[thon]	tclf[ile]
syn keyword vimCommand contained	argg[lobal]	dig[raphs]	ma[rk]	qa[ll]	te[aroff]
syn keyword vimCommand contained	argl[ocal]	di[splay]	marks	q[uit]	tf[irst]
syn keyword vimCommand contained	ar[gs]	dj[ump]	me	quita[ll]	tj[ump]
syn keyword vimCommand contained	argu[ment]	dl[ist]	menu	r[ead]	tl[ast]
syn keyword vimCommand contained	as[cii]	dr[op]	menu-disable	rec[over]	tm[enu]
syn keyword vimCommand contained	aun	ds[earch]	menu-enable	redi[r]	tn[ext]
syn keyword vimCommand contained	aunmenu	dsp[lit]	menu-<script>	red[o]	to[pleft]
syn keyword vimCommand contained	bN[ext]	echoe[rr]	menut[rans]	reg[isters]	tp[revious]
syn keyword vimCommand contained	bad[d]	echom[sg]	mk[exrc]	res[ize]	tr[ewind]
syn keyword vimCommand contained	ba[ll]	echon	mks[ession]	ret[ab]	ts[elect]
syn keyword vimCommand contained	bd[elete]	e[dit]	mkvie[w]	retu[rn]	tu[nmenu]
syn keyword vimCommand contained	bf[irst	el[se]	mkv[imrc]	rew[ind]	una[bbreviate]
syn keyword vimCommand contained	bl[ast]	elsei[f]	mod[e]	ri[ght]	u[ndo]
syn keyword vimCommand contained	bm[odified]	em[eenu]	m[ove]	rub[y]	unh[ide]
syn keyword vimCommand contained	bn[ext]	emenu	new	rubyd[o]	unm[ap]
syn keyword vimCommand contained	bo[tright]	endf[unction]	n[ext]	rubyf[ile]	unme
syn keyword vimCommand contained	bp[revious]	en[dif]	nmapc[lear]	ru[ntime]	unmenu
syn keyword vimCommand contained	brea[k]	endw[hile]	nme	rv[iminfo]	uns[ign]
syn keyword vimCommand contained	breaka[dd]	ene[w]	nmenu	sN[ext]	verb[ose]
syn keyword vimCommand contained	breakd[el]	ex	nnoreme	sal[l]	ve[rsion]
syn keyword vimCommand contained	breakl[ist]	exi[t]	nnoremenu	sa[rgument]	vert[ical]
syn keyword vimCommand contained	br[ewind]	f[ile]	noh[lsearch]	sbN[ext]	v[global]
syn keyword vimCommand contained	bro[wse]	files	norea[bbrev]	sba[ll]	vie[w]
syn keyword vimCommand contained	bufdo	filetype	noreme	sbf[irst]	vi[sual]
syn keyword vimCommand contained	b[uffer]	fin[d]	noremenu	sbl[ast]	vmapc[lear]
syn keyword vimCommand contained	buffers	fini[sh]	norm[al]	sbm[odified]	vme
syn keyword vimCommand contained	bun[load]	fir[st]	nu[mber]	sbn[ext]	vmenu
syn keyword vimCommand contained	bw[ipeout]	fix[del]	nun[map]	sbp[revious]	vne[w]
syn keyword vimCommand contained	cN[ext]	fo[ld]	nunme	sbr[ewind]	vnoreme
syn keyword vimCommand contained	ca[bbrev]	foldo[pen]	nunmenu	sb[uffer]	vnoremenu
syn keyword vimCommand contained	cabc[lear]	fu[nction]	omapc[lear]	scrip[tnames]	vs[plit]
syn keyword vimCommand contained	cal[l]	g[lobal]	ome	se[t]	vu[nmap]
syn keyword vimCommand contained	cc	go[to]	omenu	setf[iletype]	vunme
syn keyword vimCommand contained	cd	gr[ep]	on[ly]	setg[lobal]	vunmenu
syn keyword vimCommand contained	ce[nter]	h[elp]	onoreme	setl[ocal]	wN[ext]
syn keyword vimCommand contained	cf[ile]	helpf[ind]	onoremenu	sf[ind]	wa[ll]
syn keyword vimCommand contained	cfir[st]	helpt[ags]	o[pen]	sfir[st	wh[ile]
syn keyword vimCommand contained	c[hange]	hid[e]	opt[ions]	sh[ell]	win
syn keyword vimCommand contained	chd[ir]	his[tory]	ou[nmap]	sig[n]	windo
syn keyword vimCommand contained	che[ckpath]	ia[bbrev]	ounme	signs	winp[os]
syn keyword vimCommand contained	cla[st]	iabc[lear]	ounmenu	sil[ent]	win[size]
syn keyword vimCommand contained	cl[ist]	if	pc[lose]	si[malt]	wn[ext]
syn keyword vimCommand contained	clo[se]	ij[ump]	ped[it]	sla[st]	wp[revous]
syn keyword vimCommand contained	cmapc[lear]	il[ist]	pe[rl]	sl[eep]	wq
syn keyword vimCommand contained	cme	imapc[lear]	perld[o]	sm[agic]	wqa[ll]
syn keyword vimCommand contained	cmenu	ime	po[p]	sn[ext]	w[rite]
syn keyword vimCommand contained	cnew[er]	imenu	pop[up]	sni[ff]	ws[verb]
syn keyword vimCommand contained	cn[ext]	inorea[bbrev]	pp[op]	sno[magic]	wv[iminfo]
syn keyword vimCommand contained	cnf[ile]	inoreme	pre[serve]	so[urce]	xa[ll]
syn keyword vimCommand contained	cnorea[bbrev]	inoremenu	prev[ious]	sp[lit]	x[it]
syn keyword vimCommand contained	cnoreme	is[earch]	p[rint]	spr[evious]	y[ank]
syn keyword vimCommand contained	cnoremenu	isp[lit]
syn match   vimCommand contained	"\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	ead	includeexpr	pvh	syntax
syn keyword vimOption contained	ai	eadirection	incsearch	pvw	ta
syn keyword vimOption contained	akm	eb	inde	readonly	tabstop
syn keyword vimOption contained	al	ed	indentexpr	remap	tag
syn keyword vimOption contained	aleph	edcompatible	indentkeys	report	tagbsearch
syn keyword vimOption contained	allowrevins	ef	indk	restorescreen	taglength
syn keyword vimOption contained	altkeymap	efm	inex	revins	tagrelative
syn keyword vimOption contained	ar	ei	inf	ri	tags
syn keyword vimOption contained	ari	ek	infercase	rightleft	tagstack
syn keyword vimOption contained	autoindent	endofline	insertmode	rl	tb
syn keyword vimOption contained	autoread	eol	is	ro	tbi
syn keyword vimOption contained	autowrite	ep	isf	rs	tbs
syn keyword vimOption contained	autowriteall	equalalways	isfname	rtp	term
syn keyword vimOption contained	aw	equalprg	isi	ru	terse
syn keyword vimOption contained	awa	errorbells	isident	ruf	textauto
syn keyword vimOption contained	background	errorfile	isk	ruler	textmode
syn keyword vimOption contained	backspace	errorformat	iskeyword	rulerformat	textwidth
syn keyword vimOption contained	backup	esckeys	isp	runtimepath	tf
syn keyword vimOption contained	backupcopy	et	isprint	sb	tgst
syn keyword vimOption contained	backupdir	eventignore	joinspaces	sbo	thesaurus
syn keyword vimOption contained	backupext	ex	js	sbr	tildeop
syn keyword vimOption contained	backupskip	expandtab	key	sc	timeout
syn keyword vimOption contained	balloondelay	exrc	keymap	scb	timeoutlen
syn keyword vimOption contained	ballooneval	fcc	keymodel	scr	title
syn keyword vimOption contained	bdir	fccs	keywordprg	scroll	titlelen
syn keyword vimOption contained	bdlay	fcs	km	scrollbind	titleold
syn keyword vimOption contained	beval	fdc	kmp	scrolljump	titlestring
syn keyword vimOption contained	bex	fde	kp	scrolloff	tl
syn keyword vimOption contained	bg	fdi	langmap	scrollopt	tm
syn keyword vimOption contained	bh	fdl	langmenu	scs	to
syn keyword vimOption contained	bin	fdm	laststatus	sect	toolbar
syn keyword vimOption contained	binary	fdn	lazyredraw	sections	top
syn keyword vimOption contained	biosk	fdt	lbr	secure	tr
syn keyword vimOption contained	bioskey	fe	lcs	sel	ts
syn keyword vimOption contained	bk	fen	linebreak	selection	tsl
syn keyword vimOption contained	bkc	ff	lines	selectmode	tsr
syn keyword vimOption contained	bomb	ffs	linespace	sessionoptions	ttimeout
syn keyword vimOption contained	breakat	filecharcode	lisp	sft	ttimeoutlen
syn keyword vimOption contained	brk	filecharcodes	list	sh	ttm
syn keyword vimOption contained	browsedir	fileencoding	listchars	shcf	ttybuiltin
syn keyword vimOption contained	bs	fileformat	lmap	shell	ttyfast
syn keyword vimOption contained	bsdir	fileformats	lmenu	shellcmdflag	ttym
syn keyword vimOption contained	bsk	filetype	loadplugins	shellpipe	ttymouse
syn keyword vimOption contained	bst	fillchars	lpl	shellquote	ttyscroll
syn keyword vimOption contained	bt	fk	ls	shellredir	ttytype
syn keyword vimOption contained	bufhidden	fkmap	lsp	shellslash	tw
syn keyword vimOption contained	bufsecret	fmr	lz	shelltype	tx
syn keyword vimOption contained	buftype	fo	ma	shellxquote	uc
syn keyword vimOption contained	cb	foldcolumn	magic	shiftround	ul
syn keyword vimOption contained	cc	foldenable	makeef	shiftwidth	undolevels
syn keyword vimOption contained	ccv	foldexpr	makeprg	shm	updatecount
syn keyword vimOption contained	cd	foldignore	mat	shortmess	updatetime
syn keyword vimOption contained	cdpath	foldlevel	matchpairs	shortname	ut
syn keyword vimOption contained	cedit	foldmarker	matchtime	showbreak	vb
syn keyword vimOption contained	cf	foldmethod	maxfuncdepth	showcmd	vbs
syn keyword vimOption contained	ch	foldnestmax	maxmapdepth	showfulltag	vdir
syn keyword vimOption contained	charcode	foldtext	maxmem	showmatch	ve
syn keyword vimOption contained	charconvert	formatoptions	maxmemtot	showmode	verbose
syn keyword vimOption contained	cin	formatprg	mef	shq	vi
syn keyword vimOption contained	cindent	fp	menuitems	si	viewdir
syn keyword vimOption contained	cink	ft	mfd	sidescroll	viewoptions
syn keyword vimOption contained	cinkeys	gcr	mh	sidescrolloff	viminfo
syn keyword vimOption contained	cino	gd	mis	siso	virtualedit
syn keyword vimOption contained	cinoptions	gdefault	ml	sj	visualbell
syn keyword vimOption contained	cinw	gfm	mls	slm	vop
syn keyword vimOption contained	cinwords	gfn	mm	sm	wa
syn keyword vimOption contained	clipboard	gfs	mmd	smartcase	wak
syn keyword vimOption contained	cmdheight	gfw	mmt	smartindent	warn
syn keyword vimOption contained	cmdwinheight	ghr	mod	smarttab	wb
syn keyword vimOption contained	co	go	modeline	smd	wc
syn keyword vimOption contained	columns	gp	modelines	sn	wcm
syn keyword vimOption contained	com	grepformat	modifiable	so	wd
syn keyword vimOption contained	comments	grepprg	modified	softtabstop	weirdinvert
syn keyword vimOption contained	compatible	guicursor	more	sol	wh
syn keyword vimOption contained	complete	guifont	mouse	sp	whichwrap
syn keyword vimOption contained	confirm	guifontset	mousef	splitbelow	wig
syn keyword vimOption contained	consk	guifontwide	mousefocus	splitright	wildchar
syn keyword vimOption contained	conskey	guiheadroom	mousehide	spr	wildcharm
syn keyword vimOption contained	cp	guioptions	mousem	sr	wildignore
syn keyword vimOption contained	cpo	guipty	mousemodel	srr	wildmenu
syn keyword vimOption contained	cpoptions	helpfile	mouses	ss	wildmode
syn keyword vimOption contained	cpt	helpheight	mouseshape	ssl	wim
syn keyword vimOption contained	cscopeprg	hf	mouset	ssop	winaltkeys
syn keyword vimOption contained	cscopetag	hh	mousetime	st	winheight
syn keyword vimOption contained	cscopetagorder	hi	mp	sta	winminheight
syn keyword vimOption contained	cscopeverbose	hid	mps	startofline	winminwidth
syn keyword vimOption contained	csprg	hidden	nf	statusline	winwidth
syn keyword vimOption contained	cst	highlight	nrformats	stl	wiv
syn keyword vimOption contained	csto	history	nu	sts	wiw
syn keyword vimOption contained	csverb	hk	number	su	wm
syn keyword vimOption contained	cwh	hkmap	oft	sua	wmh
syn keyword vimOption contained	debug	hkmapp	osfiletype	suffixes	wmnu
syn keyword vimOption contained	def	hkp	pa	suffixesadd	wmw
syn keyword vimOption contained	define	hl	para	sw	wrap
syn keyword vimOption contained	dg	hls	paragraphs	swapfile	wrapmargin
syn keyword vimOption contained	dict	hlsearch	paste	swapsync	wrapscan
syn keyword vimOption contained	dictionary	ic	pastetoggle	swb	write
syn keyword vimOption contained	digraph	icon	patchmode	swf	writeany
syn keyword vimOption contained	dir	iconstring	path	switchbuf	writebackup
syn keyword vimOption contained	directory	ignorecase	pm	sws	writedelay
syn keyword vimOption contained	display	im	previewheight	sxq	ws
syn keyword vimOption contained	dy	inc	previewwindow	syn	ww
syn keyword vimOption contained	ea	include	pt

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
syn keyword vimOption contained	nodigraph	noinfercase	norl	nota	pvw

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
syn keyword vimAutoEvent contained	BufCreate	BufReadPre	CmdwinLeave	FileWriteCmd	StdinReadPre
syn keyword vimAutoEvent contained	BufDelete	BufSecret	CursorHold	FileWritePost	Syntax
syn keyword vimAutoEvent contained	BufEnter	BufUnload	FileAppendCmd	FileWritePre	TermChanged
syn keyword vimAutoEvent contained	BufFilePost	BufWinLeave	FileAppendPost	FilterReadPost	TermResponse
syn keyword vimAutoEvent contained	BufFilePre	BufWipeout	FileAppendPre	FilterReadPre	User
syn keyword vimAutoEvent contained	BufHidden	BufWrite	FileChangedRO	FilterWritePost	UserGettingBored
syn keyword vimAutoEvent contained	BufLeave	BufWriteCmd	FileChangedShell	FilterWritePre	VimEnter
syn keyword vimAutoEvent contained	BufNewFile	BufWritePost	FileEncoding	FocusGained	VimLeave
syn keyword vimAutoEvent contained	BufRead	BufWritePre	FileReadCmd	FocusLost	VimLeavePre
syn keyword vimAutoEvent contained	BufReadAfter	CharCode	FileReadPost	FuncUndefined	WinEnter
syn keyword vimAutoEvent contained	BufReadCmd	Cmd-event	FileReadPre	GUIEnter	WinLeave
syn keyword vimAutoEvent contained	BufReadPost	CmdwinEnter	FileType	StdinReadPost

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
syn keyword vimFuncName contained	MyCounter	exists	getwinvar	localtime	strpart
syn keyword vimFuncName contained	append	expand	glob	maparg	strridx
syn keyword vimFuncName contained	argc	filereadable	has	mapcheck	strtrans
syn keyword vimFuncName contained	argv	filewritable	hasmapto	match	submatch
syn keyword vimFuncName contained	browse	fnamemodify	histadd	matchend	substitute
syn keyword vimFuncName contained	bufexists	foldclosed	histdel	matchstr	synID
syn keyword vimFuncName contained	bufloaded	foldclosedend	histget	mode	synIDattr
syn keyword vimFuncName contained	bufname	foldlevel	histnr	nextnonblank	synIDtrans
syn keyword vimFuncName contained	bufnr	foldtext	hlID	nr2char	system
syn keyword vimFuncName contained	bufwinnr	function	hlexists	prevnonblank	tempname
syn keyword vimFuncName contained	byte2line	getbufvar	hostname	rename	tolower
syn keyword vimFuncName contained	char2nr	getchar	indent	search	toupper
syn keyword vimFuncName contained	col	getcharmod	input	searchpair	type
syn keyword vimFuncName contained	confirm	getcwd	inputsecret	setbufvar	virtcol
syn keyword vimFuncName contained	cscope_connection	getfsize	isdirectory	setline	visualmode
syn keyword vimFuncName contained	delete	getftime	libcall	setwinvar	winbufnr
syn keyword vimFuncName contained	did_filetype	getline	libcallnr	strftime	winheight
syn keyword vimFuncName contained	escape	getwinposx	line	stridx	winnr
syn keyword vimFuncName contained	executable	getwinposy	line2byte	strlen	winwidth

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep	"[:|]\+"	skipwhite nextgroup=vimAddress,vimAutoCmd,vimMark,vimFilter,vimUserCmd,vimSet,vimLet,vimCommand,vimSyntax,vimExtCmd
syn match vimIsCommand	"\<\a\+\>"	contains=vimCommand
syn match vimVar		"\<[bwglsav]:\K\k*\>"
syn match vimVar contained	"\<\K\k*\>"
syn match vimFunc		"\I\i*\s*("	contains=vimFuncName,vimCommand

" Insertions And Appends: insert append startinsert
" =======================
syn region vimInsert	matchgroup=vimCommand start="^:*\s*a\%[ppend]$"	matchgroup=vimCommand end="^\.$""
syn region vimInsert	matchgroup=vimCommand start="^:*\s*i\%[nsert]$"	matchgroup=vimCommand end="^\.$""
syn region vimInsert	matchgroup=vimCommand start="^:*\s*star\%[tinsert]$"	matchgroup=vimCommand end="^\.$""

" Behave!
" =======
syn match   vimBehave	"\<be\%[have]\>" contains=vimCommand skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained	mswin	xterm
syn match   vimBehaveError contained	"[^ ]\+"

" Filetypes
" =========
syn match   vimFiletype	"\<filet\%[ype]\s\+\(\I\i*\)*"	skipwhite contains=vimFTCmd,vimFTOption,vimFTError
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
syn match   vimUserCmd	"\<com\%[mand]!\=\>.*$"		contains=vimUserAttrb,@vimUserCmdList
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
syn match   vimSubst1      contained	"s\%[ubstitute]\>"		nextgroup=vimSubstPat
syn region  vimSubstPat    contained	matchgroup=vimSubstDelim start="\z([^a-zA-Z \t[\]&]\)"rs=s+1 skip="\\\\\|\\\z1" end="\z1"re=e-1,me=e-1	 contains=@vimSubstList	nextgroup=vimSubstRep4	oneline
syn region  vimSubstRep4   contained	matchgroup=vimSubstDelim start="\z(.\)" skip="\\\\\|\\\z1" end="\z1" matchgroup=vimNotation end="<[cC][rR]>" contains=@vimSubstRepList	nextgroup=vimSubstFlagErr
syn region  vimCollection  contained	start="\[" skip="\\\[" end="\]"	contains=vimCollClass
syn match   vimCollClassErr contained	"\[:.\{-\}:\]"
syn match   vimCollClass   contained	"\[:\(alnum\|alpha\|blank\|cntrl\|digit\|graph\|lower\|print\|punct\|space\|upper\|xdigit\|return\|tab\|escape\|backspace\):\]"
syn match   vimSubstSubstr contained	"\\z\=\d"
syn match   vimSubstTwoBS  contained	"\\\\"
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
syn region  vimSynMatchRegion	contained oneline matchgroup=vimGroupName start="\k\+" matchgroup=vimSep end="$\||" contains=vimComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn match   vimSynMtchOpt	contained	"\<\(contained\|display\|excludenl\|fold\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: off and on
syn keyword vimSynType	contained	off	on

" Syntax: region
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn keyword vimSynType	contained	region	skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	contained	matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="$\||" contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn match   vimSynRegOpt	contained	"\<\(contained\|display\|excludenl\|fold\|transparent\|skipempty\|skipwhite\|skipnl\|oneline\|keepend\)\>"
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
syn match   vimHighlight		"\<hi\%[ghlight]\>" skipwhite nextgroup=vimHiLink,vimHiClear,vimHiKeyList,vimComment

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
hi def link vimUserCmd	vimCommand

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
