" Vim syntax file
" Language:	Vim 6.0f script
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	August 15, 2000
" Version:	6.0f-01

" Remove old syntax
syn clear

" vimTodo: contains common special-notices for comments
"          Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	COMBAK	NOT	RELEASED	TODO
syn cluster vimCommentGroup	contains=vimTodo

" regular vim commands
syn keyword vimCommand contained	N[ext]	cq[uit]	ju[mps]	ptf[irst]	stj[ump]
syn keyword vimCommand contained	P[rint]	cr[ewind]	k	ptj[ump]	sts[elect]
syn keyword vimCommand contained	X	cu[nmap]	l[ist]	ptl[ast]	sun[hide]
syn keyword vimCommand contained	a[ppend]	cuna[bbrev]	la[st]	ptn[ext]	sus[pend]
syn keyword vimCommand contained	ab[breviate]	cunme	lan[guage]	ptp[revious]	sv[iew]
syn keyword vimCommand contained	abc[lear]	cunmenu	lc[d]	ptr[ewind]	syncbind
syn keyword vimCommand contained	al[l]	cw[indow]	lch[dir]	pts[elect]	t
syn keyword vimCommand contained	am	d[elete]	le[ft]	pu[t]	tN[ext]
syn keyword vimCommand contained	amenu	delc[ommand]	ls	pw[d]	ta[g]
syn keyword vimCommand contained	an	delf[unction]	m[ove]	py[thon]	tags
syn keyword vimCommand contained	anoremenu	dig[raphs]	ma[rk]	pyf[ile]	tc[l]
syn keyword vimCommand contained	ar[gs]	dj[ump]	mak[e]	q[uit]	tcld[o]
syn keyword vimCommand contained	argu[ment]	dl[ist]	marks	qa[ll]	tclf[ile]
syn keyword vimCommand contained	as[cii]	ds[earch]	me	quita[ll]	te[aroff]
syn keyword vimCommand contained	aug[roup]	dsp[lit]	menu	r[ead]	tf[irst]
syn keyword vimCommand contained	aun	e[dit]	menu-disable	rec[over]	tj[ump]
syn keyword vimCommand contained	aunmenu	echon	menu-enable	red[o]	tl[ast]
syn keyword vimCommand contained	bN[ext]	el[se]	menut[rans]	redi[r]	tm[enu]
syn keyword vimCommand contained	b[uffer]	elsei[f]	mk[exrc]	reg[isters]	tn[ext]
syn keyword vimCommand contained	ba[ll]	eme[enu]	mks[ession]	res[ize]	to[pleft]
syn keyword vimCommand contained	bad[d]	emenu	mkv[imrc]	ret[ab]	tp[revious]
syn keyword vimCommand contained	bd[elete]	en[dif]	mod[e]	retu[rn]	tr[ewind]
syn keyword vimCommand contained	bf[irst	endf[unction]	n[ext]	rew[ind]	ts[elect]
syn keyword vimCommand contained	bl[ast]	endw[hile]	new	ri[ght]	tu[nmenu]
syn keyword vimCommand contained	bm[odified]	ene[w]	nmapc[lear]	ru[ntime]	u[ndo]
syn keyword vimCommand contained	bn[ext]	ex	nme	rv[iminfo]	una[bbreviate]
syn keyword vimCommand contained	bo[tright]	exi[t]	nmenu	sN[ext]	unh[ide]
syn keyword vimCommand contained	bp[revious]	f[ile]	nnoreme	sa[rgument]	unm[ap]
syn keyword vimCommand contained	br[ewind]	files	nnoremenu	sal[l]	unme
syn keyword vimCommand contained	brea[k]	fin[d]	noh[lsearch]	sbN[ext]	unmenu
syn keyword vimCommand contained	bro[wse]	fini[sh]	norea[bbrev]	sb[uffer]	uns[ign]
syn keyword vimCommand contained	buffers	fir[st]	noreme	sba[ll]	v[global]
syn keyword vimCommand contained	bun[load]	fix[del]	noremenu	sbf[irst]	ve[rsion]
syn keyword vimCommand contained	cN[ext]	fo[ld]	norm[al]	sbl[ast]	vert[ical]
syn keyword vimCommand contained	c[hange]	foldo[pen]	nu[mber]	sbm[odified]	vi[sual]
syn keyword vimCommand contained	ca[bbrev]	fu[nction]	nun[map]	sbn[ext]	vie[w]
syn keyword vimCommand contained	cabc[lear]	g[lobal]	nunme	sbp[revious]	vmapc[lear]
syn keyword vimCommand contained	cal[l]	go[to]	nunmenu	sbr[ewind]	vme
syn keyword vimCommand contained	cc	gr[ep]	o[pen]	se[t]	vmenu
syn keyword vimCommand contained	cd	h[elp]	omapc[lear]	setf[iletype]	vne[w]
syn keyword vimCommand contained	ce[nter]	helpf[ind]	ome	setg[lobal]	vnoreme
syn keyword vimCommand contained	cf[ile]	hid[e]	omenu	setl[ocal]	vnoremenu
syn keyword vimCommand contained	cfir[st]	his[tory]	on[ly]	sf[ind]	vs[plit]
syn keyword vimCommand contained	chd[ir]	i[nsert]	onoreme	sfir[st	vu[nmap]
syn keyword vimCommand contained	che[ckpath]	ia[bbrev]	onoremenu	sh[ell]	vunme
syn keyword vimCommand contained	cl[ist]	iabc[lear]	opt[ions]	si[malt]	vunmenu
syn keyword vimCommand contained	cla[st]	if	ou[nmap]	sig[n]	wN[ext]
syn keyword vimCommand contained	clo[se]	ij[ump]	ounme	signs	w[rite]
syn keyword vimCommand contained	cmapc[lear]	il[ist]	ounmenu	sil[ent]	wa[ll]
syn keyword vimCommand contained	cme	imapc[lear]	p[rint]	sl[eep]	wh[ile]
syn keyword vimCommand contained	cmenu	ime	pc[lose]	sla[st]	win[size]
syn keyword vimCommand contained	cn[ext]	imenu	pe[rl]	sm[agic]	winp[os]
syn keyword vimCommand contained	cnew[er]	inorea[bbrev]	perld[o]	sn[ext]	winsize
syn keyword vimCommand contained	cnf[ile]	inoreme	po[p]	sni[ff]	wn[ext]
syn keyword vimCommand contained	cnorea[bbrev]	inoremenu	pop[up]	sno[magic]	wp[revous]
syn keyword vimCommand contained	cnoreme	is[earch]	pp[op]	so[urce]	wq
syn keyword vimCommand contained	cnoremenu	isp[lit]	pre[serve]	sp[lit]	wqa[ll]
syn keyword vimCommand contained	co[py]	iu[nmap]	prev[ious]	spr[evious]	ws[verb]
syn keyword vimCommand contained	col[der]	iuna[bbrev]	promptf[ind]	sr[ewind]	wv[iminfo]
syn keyword vimCommand contained	comc[lear]	iunme	promptr[epl]	st[op]	x[it]
syn keyword vimCommand contained	con[tinue]	iunmenu	ptN[ext]	sta[g]	xa[ll]
syn keyword vimCommand contained	conf[irm]	j[oin]	pta[g]	star[tinsert]	y[ank]
syn keyword vimCommand contained	cp[revious]
syn match   vimCommand contained	"\<z[-+^.=]"
syn match   vimCommand contained	"\<di\(s\(p\(l\(a\(y\)\=\)\=\)\=\)\=\)\=\>"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	:	endofline	infercase	rightleft	tag
syn keyword vimOption contained	ai	eol	insertmode	rl	tagbsearch
syn keyword vimOption contained	akm	ep	is	ro	taglength
syn keyword vimOption contained	al	equalalways	isf	rs	tagrelative
syn keyword vimOption contained	aleph	equalprg	isfname	rtp	tags
syn keyword vimOption contained	allowrevins	errorbells	isi	ru	tagstack
syn keyword vimOption contained	altkeymap	errorfile	isident	ruf	tb
syn keyword vimOption contained	ar	errorformat	isk	ruler	tbi
syn keyword vimOption contained	ari	esckeys	iskeyword	rulerformat	tbs
syn keyword vimOption contained	autoindent	et	isp	runtimepath	term
syn keyword vimOption contained	autoread	eventignore	isprint	sb	terse
syn keyword vimOption contained	autowrite	ex	joinspaces	sbo	textauto
syn keyword vimOption contained	aw	expandtab	js	sbr	textmode
syn keyword vimOption contained	background	exrc	key	sc	textwidth
syn keyword vimOption contained	backspace	fcc	keymodel	scb	tf
syn keyword vimOption contained	backup	fccs	keywordprg	scr	tgst
syn keyword vimOption contained	backupdir	fcs	km	scroll	thesaurus
syn keyword vimOption contained	backupext	fdc	kp	scrollbind	tildeop
syn keyword vimOption contained	balloondelay	fde	langmap	scrolljump	timeout
syn keyword vimOption contained	ballooneval	fdi	laststatus	scrolloff	timeoutlen
syn keyword vimOption contained	bdir	fdl	lazyredraw	scrollopt	title
syn keyword vimOption contained	bdlay	fdm	lbr	scs	titlelen
syn keyword vimOption contained	beval	fdt	lcs	sect	titleold
syn keyword vimOption contained	bex	fe	linebreak	sections	titlestring
syn keyword vimOption contained	bg	fen	lines	secure	tl
syn keyword vimOption contained	bin	ff	linespace	sel	tm
syn keyword vimOption contained	binary	ffs	lisp	selection	to
syn keyword vimOption contained	biosk	filecharcode	list	selectmode	toolbar
syn keyword vimOption contained	bioskey	filecharcodes	listchars	sessionoptions	top
syn keyword vimOption contained	bk	fileencoding	lmap	sft	tr
syn keyword vimOption contained	breakat	fileformat	loadplugins	sh	ts
syn keyword vimOption contained	brk	fileformats	lpl	shcf	tsl
syn keyword vimOption contained	browsedir	filetype	ls	shell	tsr
syn keyword vimOption contained	bs	fillchars	lsp	shellcmdflag	ttimeout
syn keyword vimOption contained	bsdir	fk	lz	shellpipe	ttimeoutlen
syn keyword vimOption contained	bt	fkmap	magic	shellquote	ttm
syn keyword vimOption contained	buftype	fmr	makeef	shellredir	ttybuiltin
syn keyword vimOption contained	cb	fo	makeprg	shellslash	ttyfast
syn keyword vimOption contained	cc	foldcolumn	mat	shelltype	ttym
syn keyword vimOption contained	ccv	foldenable	matchpairs	shellxquote	ttymouse
syn keyword vimOption contained	cd	foldexpr	matchtime	shiftround	ttyscroll
syn keyword vimOption contained	cdpath	foldignore	maxfuncdepth	shiftwidth	ttytype
syn keyword vimOption contained	cf	foldlevel	maxmapdepth	shm	tw
syn keyword vimOption contained	ch	foldmarker	maxmem	shortmess	tx
syn keyword vimOption contained	charcode	foldmethod	maxmemtot	shortname	uc
syn keyword vimOption contained	charconvert	foldtext	mef	showbreak	ul
syn keyword vimOption contained	cin	formatoptions	menuitems	showcmd	undolevels
syn keyword vimOption contained	cindent	formatprg	mfd	showfulltag	updatecount
syn keyword vimOption contained	cink	fp	mh	showmatch	updatetime
syn keyword vimOption contained	cinkeys	ft	mis	showmode	ut
syn keyword vimOption contained	cino	gcr	ml	shq	vb
syn keyword vimOption contained	cinoptions	gd	mls	si	vbs
syn keyword vimOption contained	cinw	gdefault	mm	sidescroll	ve
syn keyword vimOption contained	cinwords	gfm	mmd	sj	verbose
syn keyword vimOption contained	clipboard	gfn	mmt	slm	vi
syn keyword vimOption contained	cmdheight	gfs	mod	sm	viminfo
syn keyword vimOption contained	co	gfw	modeline	smartcase	virtualedit
syn keyword vimOption contained	columns	ghr	modelines	smartindent	visualbell
syn keyword vimOption contained	com	go	modified	smarttab	wa
syn keyword vimOption contained	comments	gp	more	smd	wak
syn keyword vimOption contained	compatible	grepformat	mouse	sn	warn
syn keyword vimOption contained	complete	grepprg	mousef	so	wb
syn keyword vimOption contained	confirm	guicursor	mousefocus	softtabstop	wc
syn keyword vimOption contained	consk	guifont	mousehide	sol	wcm
syn keyword vimOption contained	conskey	guifontset	mousem	sp	wd
syn keyword vimOption contained	cp	guifontwide	mousemodel	splitbelow	weirdinvert
syn keyword vimOption contained	cpo	guiheadroom	mouses	splitright	wh
syn keyword vimOption contained	cpoptions	guioptions	mouseshape	spr	whichwrap
syn keyword vimOption contained	cpt	guipty	mouset	sr	wig
syn keyword vimOption contained	cscopeprg	helpfile	mousetime	srr	wildchar
syn keyword vimOption contained	cscopetag	helpheight	mp	ss	wildcharm
syn keyword vimOption contained	cscopetagorder	hf	mps	ssl	wildignore
syn keyword vimOption contained	cscopeverbose	hh	nf	ssop	wildmenu
syn keyword vimOption contained	csprg	hi	nrformats	st	wildmode
syn keyword vimOption contained	cst	hid	nu	sta	wim
syn keyword vimOption contained	csto	hidden	number	startofline	winaltkeys
syn keyword vimOption contained	csverb	highlight	oft	statusline	winheight
syn keyword vimOption contained	def	history	osfiletype	stl	winminheight
syn keyword vimOption contained	define	hk	pa	sts	winminwidth
syn keyword vimOption contained	dg	hkmap	para	su	winwidth
syn keyword vimOption contained	dict	hkmapp	paragraphs	sua	wiv
syn keyword vimOption contained	dictionary	hkp	paste	suffixes	wiw
syn keyword vimOption contained	digraph	hl	pastetoggle	suffixesadd	wm
syn keyword vimOption contained	dir	hls	patchmode	sw	wmh
syn keyword vimOption contained	directory	hlsearch	path	swapfile	wmnu
syn keyword vimOption contained	dy	ic	pm	swapsync	wmw
syn keyword vimOption contained	ea	icon	previewheight	swb	wrap
syn keyword vimOption contained	ead	iconstring	pt	swf	wrapmargin
syn keyword vimOption contained	eadirection	ignorecase	pvh	switchbuf	wrapscan
syn keyword vimOption contained	eb	im	readonly	sws	write
syn keyword vimOption contained	ed	inc	remap	sxq	writeany
syn keyword vimOption contained	edcompatible	include	report	syn	writebackup
syn keyword vimOption contained	ef	includeexpr	restorescreen	syntax	writedelay
syn keyword vimOption contained	efm	incsearch	revins	ta	ws
syn keyword vimOption contained	ei	inex	ri	tabstop	ww
syn keyword vimOption contained	ek	inf
syn match vimOption contained	"display"

" These are the turn-off setting variants
syn keyword vimOption contained	loadplugins	noeb	nois	noruler	notagrelative
syn keyword vimOption contained	noai	noed	nojoinspaces	nosb	notagstack
syn keyword vimOption contained	noakm	noedcompatible	nojs	nosc	notbi
syn keyword vimOption contained	noallowrevins	noek	nolazyredraw	noscb	notbs
syn keyword vimOption contained	noaltkeymap	noendofline	nolbr	noscrollbind	noterse
syn keyword vimOption contained	noar	noeol	nolinebreak	noscs	notextauto
syn keyword vimOption contained	noari	noequalalways	nolisp	nosecure	notextmode
syn keyword vimOption contained	noautoindent	noerrorbells	nolist	nosft	notf
syn keyword vimOption contained	noautoread	noesckeys	noloadplugins	noshellslash	notgst
syn keyword vimOption contained	noautowrite	noet	nolpl	noshiftround	notildeop
syn keyword vimOption contained	noaw	noex	nolz	noshortname	notimeout
syn keyword vimOption contained	nobackup	noexpandtab	nomagic	noshowcmd	notitle
syn keyword vimOption contained	noballooneval	noexrc	nomh	noshowfulltag	noto
syn keyword vimOption contained	nobeval	nofk	noml	noshowmatch	notop
syn keyword vimOption contained	nobin	nofkmap	nomod	noshowmode	notr
syn keyword vimOption contained	nobinary	nogd	nomodeline	nosi	nottimeout
syn keyword vimOption contained	nobiosk	nogdefault	nomodified	nosm	nottybuiltin
syn keyword vimOption contained	nobioskey	noguipty	nomore	nosmartcase	nottyfast
syn keyword vimOption contained	nobk	nohid	nomousef	nosmartindent	notx
syn keyword vimOption contained	nocf	nohidden	nomousefocus	nosmarttab	novb
syn keyword vimOption contained	nocin	nohk	nomousehide	nosmd	novisualbell
syn keyword vimOption contained	nocindent	nohkmap	nonu	nosn	nowa
syn keyword vimOption contained	nocompatible	nohkmapp	nonumber	nosol	nowarn
syn keyword vimOption contained	noconfirm	nohkp	nopaste	nosplitbelow	nowb
syn keyword vimOption contained	noconsk	nohls	noreadonly	nosplitright	noweirdinvert
syn keyword vimOption contained	noconskey	nohlsearch	noremap	nospr	nowildmenu
syn keyword vimOption contained	nocp	noic	norestorescreen	nosr	nowiv
syn keyword vimOption contained	nocscopetag	noicon	norevins	nossl	nowmnu
syn keyword vimOption contained	nocscopeverbose	noignorecase	nori	nosta	nowrap
syn keyword vimOption contained	nocst	noim	norightleft	nostartofline	nowrapscan
syn keyword vimOption contained	nocsverb	noincsearch	norl	noswapfile	nowrite
syn keyword vimOption contained	nodg	noinf	noro	noswf	nowriteany
syn keyword vimOption contained	nodigraph	noinfercase	nors	nota	nowritebackup
syn keyword vimOption contained	noea	noinsertmode	noru	notagbsearch	nows

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

"--- syntax above generated by mkvimvim ---
" History:
" v6.0e-01: contains \z()-based string handling
"           \(\)\) mismatch error catching
"            improved :s handling
"           sync maxlines now set to 100 (for somewhat better speed)

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
syn region	vimEscapeBrace	oneline contained transparent	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1
syn match	vimPatSep	contained	"\\|"
syn match	vimPatSepErr	contained	"\\)"
syn region	vimPatRegion	contained	matchgroup=vimPatSep start="\\z\=(" end="\\)"	contains=@vimSubstList
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
syn cluster	vimSubstList	contains=vimPatSep,vimPatRegion,vimPatSepErr,vimSubstTwoBS,vimSubstRange,vimNotation
syn cluster	vimSubstRepList	contains=vimSubstSubstr,vimSubstTwoBS,vimNotation
if version < 600
 syn region	vimSubst	oneline	 start=":*s/.\{-}" skip="\\\\\|\\/" end="/"	contains=@vimSubstList nextgroup=vimSubstRep1
 syn region	vimSubstRep1	contained oneline start="."lc=1	  skip="\\\\\|\\/" end="/"	contains=@vimSubstRepList
 syn region	vimSubst	oneline	 start=":*s?.\{-}" skip="\\\\\|\\?" end="?"	contains=@vimSubstList nextgroup=vimSubstRep2
 syn region	vimSubstRep2	contained oneline start="."lc=1	  skip="\\\\\|\\?" end="?"	contains=@vimSubstRepList
 syn region	vimSubst	oneline	 start=":*s@.\{-}" skip="\\\\\|\\@" end="@"	contains=@vimSubstList nextgroup=vimSubstRep3
 syn region	vimSubstRep3	contained oneline start="."lc=1	  skip="\\\\\|\\@" end="@"	contains=@vimSubstRepList
 syn region	vimSubstRange	contained oneline start="\["	  skip="\\\\\|\\]" end="]"
else
 syn match	vimSubst	"\(:\+\s*\|^\s*\||\s*\)\<s\(u\(b\(s\(t\(i\(t\(u\(te\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\>" nextgroup=vimSubstPat
 syn match	vimSubst1	contained "s\(u\(b\(s\(t\(i\(t\(u\(te\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\)\=\>" nextgroup=vimSubstPat
 syn region	vimSubstPat	contained matchgroup=vimSubstDelim start="\z([^a-zA-Z[\]&]\)"rs=s+1 skip="\\\\\|\\\z1" end="\z1"re=e-1,me=e-1 contains=@vimSubstList	nextgroup=vimSubstRep4
 syn region	vimSubstRep4	contained matchgroup=vimSubstDelim start="\z(.\)" skip="\\\\\|\\\z1" end="\z1" matchgroup=vimNotation end="<[cC][rR]>" contains=@vimSubstRepList	nextgroup=vimSubstFlagErr
 syn region	vimCollection	contained start="\[" skip="\\\[" end="\]"	contains=vimCollClass
 syn match	vimCollClassErr	contained "\[:.\{-\}:\]"
 syn match	vimCollClass	contained "\[:\(alnum\|alpha\|blank\|cntrl\|digit\|graph\|lower\|print\|punct\|space\|upper\|xdigit\|return\|tab\|escape\|backspace\):\]"
endif
syn match	vimSubstSubstr	contained	"\\z\=\d"
syn match	vimSubstTwoBS	contained	"\\\\"
syn match	vimSubstFlagErr	contained	"[^< \t]\+" contains=vimSubstFlags
syn match	vimSubstFlags	contained	"[&cegiIpr]\+"

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

syn match  vimAddress		",[.$]"lc=1	skipwhite nextgroup=vimSubst1
syn match  vimAddress		"%\a"me=e-1	skipwhite nextgroup=vimString,vimSubst1

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
syn region  vimExecute	oneline excludenl matchgroup=vimCommand start="\<exe\(c\(u\(te\=\)\=\)\=\)\=\>" skip="\(\\\\\)*\\|" end="$\||" contains=vimIsCommand,vimString,vimOper,vimVar
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
syn match   vimMapLhs	contained	"\S\+"		contains=vimNotation,vimCtrlChar
syn match   vimMapBang	contained	"!"		skipwhite nextgroup=vimMapLhs

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
syn match   vimSynReg	contained	"\(start\|skip\|end\)="he=e-1	nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	contained	"matchgroup="	nextgroup=vimGroup,vimHLGroup
syn cluster vimSynRegPatGroup	contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
if version < 600
 syn region  vimSynRegPat	contained oneline	start="!"  skip="\\\\\|\\!"  end="!"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start="%"  skip="\\\\\|\\-"  end="%"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start="'"  skip="\\\\\|\\'"  end="'"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start="+"  skip="\\\\\|\\+"  end="+"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start="@"  skip="\\\\\|\\@"  end="@"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start='"'  skip='\\\\\|\\"'  end='"'  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start='/'  skip='\\\\\|\\/'  end='/'  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start=','  skip='\\\\\|\\,'  end=','  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
 syn region  vimSynRegPat	contained oneline	start='\$' skip='\\\\\|\\\$' end='\$' contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
else
 syn region  vimSynRegPat	contained oneline	start="\z([[:punct:]]\)"  skip="\\\\\|\\\z1"  end="\z1"  contains=@vimSynRegPatGroup nextgroup=vimSynPatMod
endif
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
syn match  vimCommentTitle	'"\s*\u\a*\(\s\+\u\a*\)*:'ms=s+1	contained
syn match  vimContinue	"^\s*\\"

" Synchronize (speed)
"============
syn sync linecont	"^\s\+\\"
syn sync minlines=10	maxlines=100

" Highlighting Settings
" ====================
if !exists("did_vim_syntax_inits")
  let did_vim_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later
  hi link vimAuHighlight	vimHighlight
  hi link vimSubst1	vimSubst

  hi link vimAddress	vimMark
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

" vim: ts=17
