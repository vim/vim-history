" Vim syntax file
" Language:	Vim 5.2 script
" Maintainer:	Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	August 11, 1998 (small changes by Bram Moolenaar August 23)

" Remove old syntax
syn clear

syn keyword vimTodo contained	TODO

" regular vim commands
syn keyword vimCommand contained	I	cu[nmap]	ma[rk]	sbl[ast]	w[rite]
syn keyword vimCommand contained	N[ext]	cuna[bbrev]	mak[e]	sbm[odified]	wa[ll]
syn keyword vimCommand contained	Next	d[elete]	map	sbn[ext]	wh[ile]
syn keyword vimCommand contained	P[rint]	delc[ommand]	mapc[lear]	sbp[revious]	wn[ext]
syn keyword vimCommand contained	a[ppend]	delf[unction]	marks	sbr[ewind]	wp[revous]
syn keyword vimCommand contained	ab[breviate]	di[splay]	mk[exrc]	se[t]	wq
syn keyword vimCommand contained	abc[lear]	dig[raphs]	mks[ession]	set	wqa[ll]
syn keyword vimCommand contained	al[l]	display	mkv[imrc]	sh[ell]	wv[iminfo]
syn keyword vimCommand contained	ar[gs]	dj[ump]	mod[e]	si[malt]	x[it]
syn keyword vimCommand contained	argu[ment]	dl[ist]	n[ext]	sl[eep]	xa[ll]
syn keyword vimCommand contained	as[cii]	do[autocmd]	new	sla[st]	y[ank]
syn keyword vimCommand contained	au[tocmd]	doautoa[ll]	nm[ap]	sm[agic]	z[+-^.=]
syn keyword vimCommand contained	aug[roup]	ds[earch]	nmap	sn[ext]	am
syn keyword vimCommand contained	autocmd	dsp[lit]	nmapc[lear]	sno[magic]	amenu
syn keyword vimCommand contained	bN[ext]	e[dit]	nn[oremap]	so[urce]	an
syn keyword vimCommand contained	b[uffer]	ec[ho]	no[remap]	sp[lit]	anoremenu
syn keyword vimCommand contained	ba[ll]	echoh[l]	norea[bbrev]	spr[evious]	aun
syn keyword vimCommand contained	bad[d]	echon	norm[al]	sr[ewind]	aunmenu
syn keyword vimCommand contained	bd[elete]	el[se]	normal	st[op]	be
syn keyword vimCommand contained	bdelete	elsei[f]	nu[mber]	sta[g]	behave
syn keyword vimCommand contained	be[have]	en[dif]	nun[map]	stj[ump]	cme
syn keyword vimCommand contained	bl[ast]	endf[unction]	o[pen]	sts[elect]	cmenu
syn keyword vimCommand contained	bm[odified]	endw[hile]	om[ap]	sun[hide]	cnoreme
syn keyword vimCommand contained	bn[ext]	ex	omap	sus[pend]	cnoremenu
syn keyword vimCommand contained	bp[revious]	exe[cute]	omapc[lear]	sv[iew]	cunme
syn keyword vimCommand contained	br[ewind]	exi[t]	on[ly]	sy[ntax]	cunmenu
syn keyword vimCommand contained	brea[k]	f[ile]	ono[remap]	syntax	ime
syn keyword vimCommand contained	bro[wse]	files	ou[nmap]	t	imenu
syn keyword vimCommand contained	buffers	fix[del]	p[rint]	tN[ext]	inoreme
syn keyword vimCommand contained	bun[load]	fu[nction]	pe[rl]	ta[g]	inoremenu
syn keyword vimCommand contained	bunload	g[lobal]	perld[o]	tags	iunme
syn keyword vimCommand contained	cN[ext]	global	po[p]	tc[l]	iunmenu
syn keyword vimCommand contained	c[hange]	gr[ep]	pre[serve]	tcld[o]	me
syn keyword vimCommand contained	ca[bbrev]	h[elp]	prev[ious]	tclf[ile]	menu
syn keyword vimCommand contained	cabc[lear]	hi[ghlight]	promptf[ind]	te[aroff]	nme
syn keyword vimCommand contained	cal[l]	hid[e]	promptr[epl]	the	nmenu
syn keyword vimCommand contained	cc	i[nsert]	pu[t]	tj[ump]	nnoreme
syn keyword vimCommand contained	cd	ia[bbrev]	pw[d]	tl[ast]	nnoremenu
syn keyword vimCommand contained	ce[nter]	iabc[lear]	py[thon]	tm[enu]	noreme
syn keyword vimCommand contained	cf[ile]	if	pyf[ile]	tn[ext]	noremenu
syn keyword vimCommand contained	chd[ir]	ij[ump]	q[uit]	tp[revious]	nunme
syn keyword vimCommand contained	che[ckpath]	il[ist]	qa[ll]	tr[ewind]	nunmenu
syn keyword vimCommand contained	cl[ist]	im[ap]	r[ead]	ts[elect]	ome
syn keyword vimCommand contained	cla[st]	imap	rec[over]	tu[nmenu]	omenu
syn keyword vimCommand contained	clo[se]	imapc[lear]	red[o]	tunmenu	onoreme
syn keyword vimCommand contained	cm[ap]	ino[remap]	redi[r]	u[ndo]	onoremenu
syn keyword vimCommand contained	cmap	inorea[bbrev]	reg[isters]	una[bbreviate]	ounme
syn keyword vimCommand contained	cmapc[lear]	is[earch]	res[ize]	unh[ide]	ounmenu
syn keyword vimCommand contained	cn[ext]	isp[lit]	ret[ab]	unl[et]	tm
syn keyword vimCommand contained	cnew[er]	iu[nmap]	retu[rn]	unm[ap]	tmenu
syn keyword vimCommand contained	cno[remap]	iuna[bbrev]	rew[ind]	v[global]	tu
syn keyword vimCommand contained	cnorea[bbrev]	j[oin]	ri[ght]	ve[rsion]	tunmenu
syn keyword vimCommand contained	co[py]	ju[mps]	rv[iminfo]	vi[sual]	unme
syn keyword vimCommand contained	col[der]	k	sN[ext]	vie[w]	unmenu
syn keyword vimCommand contained	com[mand]	l[ist]	s[ubstitute]	vm[ap]	vme
syn keyword vimCommand contained	comc[lear]	la[st]	sa[rgument]	vmap	vmenu
syn keyword vimCommand contained	con[tinue]	le[ft]	sal[l]	vmapc[lear]	vnoreme
syn keyword vimCommand contained	conf[irm]	let	sbN[ext]	vn[oremap]	vnoremenu
syn keyword vimCommand contained	cp[revious]	ls	sb[uffer]	vu[nmap]	vunme
syn keyword vimCommand contained	cq[uit]	m[ove]	sba[ll]	wN[ext]	vunmenu
syn keyword vimCommand contained	cr[ewind]	noh[lsearch]

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
syn keyword vimAutoEvent contained	BufDelete	BufReadPost	FileAppendPre	FilterReadPost	TermChanged
syn keyword vimAutoEvent contained	BufEnter	BufReadPre	FileChangedShell	FilterReadPre	User
syn keyword vimAutoEvent contained	BufFilePost	BufUnload	FileReadPost	FilterWritePost	VimEnter
syn keyword vimAutoEvent contained	BufFilePre	BufWrite	FileReadPre	FilterWritePre	VimLeave
syn keyword vimAutoEvent contained	BufLeave	BufWritePost	FileWritePost	StdinReadPost	WinEnter
syn keyword vimAutoEvent contained	BufNewFile	BufWritePre	FileWritePre	StdinReadPre	WinLeave
syn keyword vimAutoEvent contained	BufRead	FileAppendPost	VimLeavePre

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
" Highlight syntax stuff

" Functions
syn match  vimFunction	"function\s\+[A-Z][a-z]*("	contains=vimCommand	nextgroup=vimFuncBody
syn match  vimFunctionError	"function\s\+[a-z].*("	contains=vimCommand	nextgroup=vimFuncBody
syn region vimFuncBody	start=")"	end="\<endf"	contains=vimIsCommand,vimGroup,vimHLGroup,vimFunction,vimFunctionError,vimFuncBody,vimSpecial,vimSpecFile,vimConditional,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimIsCommand,vimCtrlChar,vimFuncVar
syn match  vimFuncVar contained	"a:\(\I\i*\|[0-9]\+\)"

syn keyword vimPattern contained	start	skip	end

syn keyword vimSpecial	ALL	ccomment	matchgroup	skipempty	sync
syn keyword vimSpecial	NONE	lines	minlines	skipnl	transparent

" Highlight syntax stuff -- the following vimSpecials aren't keywords because Vim's syntax checking
" disapproves
syn match vimSpecial contained	"\<ALLBUT\>"
syn match vimSpecial contained	"\<clear\>"
syn match vimSpecial contained	"\<contained\>"
syn match vimSpecial contained	"\<oneline\>"
syn match vimSpecial contained	"\<contains\>"
syn match vimSpecial contained	"\<grouphere\>"
syn match vimSpecial contained	"\<groupthere\>"
syn match vimSpecial contained	"\<keepend\>"
syn match vimSpecial contained	"\<maxlines\>"
syn match vimSpecial contained	"\<nextgroup\>"
syn match vimSpecial contained	"\<skipempty\>"
syn match vimSpecial contained	"\<skipnl\>"
syn match vimSpecial contained	"\<skipwhite\>"
syn match vimSpecial contained	"\<transparent\>"

" Highlight Terminal Stuff
syn keyword vimTermSet  contained	background	cterm	ctermfg	font	gui	guifg
syn keyword vimTermSet  contained	bg	ctermbg	fg	foreground	guibg	term	start
syn match   vimTermSet  contained	"stop"

" Special Filenames and Modifiers
syn match vimSpecFile	"<\(cword\|cWORD\|[cas]file\)>"	nextgroup=vimSpecFileMod
syn match vimSpecFile	"#\d\|[#%]"		nextgroup=vimSpecFileMod
syn match vimSpecFileMod	"\(:[phtre]\)\+"		contained

" Conditionals
syn match vimConditional	"^\s*\(continue\|con\)\>"
syn match vimConditional	"^\s*\(endif\|en\)\>"
syn match vimConditional	"^\s*\(endwhile\|endw\)\>"
syn match vimConditional	"^\s*\(while\|wh\)\>"
syn match vimConditional	"^\s*break\=\>"
syn match vimConditional	"^\s*else\>"
syn match vimConditional	"^\s*elseif\=\>"
syn match vimConditional	"^\s*if\>"

" Operators
syn match vimOper	"||\|&&\|!=\|>=\|<=\|=\~\|!\~\|>\|<\|+\|-\|=\|\." skipwhite nextgroup=vimString

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
syn region	vimEscapeBrace	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1	oneline contained transparent
syn match	vimPatSep	"\\[|()]"hs=e-1	contained
syn match	vimNotPatSep	"\\\\"	contained
syn region	vimString	start=+[^:a-zA-Z>!\\]"+lc=1 skip=+\\\\\|\\"+ end=+"+ oneline contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	start=+[^:a-zA-Z>!\\]'+lc=1 skip=+\\\\\|\\'+ end=+'+ oneline contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	start=+=!+lc=1	skip=+\\\\\|\\!+ end=+!+	oneline contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	start="=+"lc=1	skip="\\\\\|\\+" end="+"	oneline contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	start="[^\\]+\s*[^a-zA-Z0-9.]"lc=1 skip="\\\\\|\\+" end="+"	oneline contains=vimEscapeBrace,vimPatSep,vimNotPatSep
syn region	vimString	start="\s/\s*[^a-zA-Z]"lc=1 skip="\\\\\|\\+" end="/"	oneline contains=vimEscapeBrace,vimPatSep,vimNotPatSep

" Substitutions
syn region	vimSubst	start=":\=s/.\{-}" skip="\(\\\\\)*\\/" end="/"	oneline contains=vimPatSep,vimSubstTwoBS,vimSubstRange,vimNotation nextgroup=vimSubstEnd
syn region	vimSubstEnd	start="."lc=1	skip="\(\\\\\)*\\/"   end="/"	oneline contained contains=vimSubstPat,vimSubstTwoBS,vimNotation
syn region	vimSubstRange	start="\["	skip="\(\\\\\)*\\]"   end="]"	oneline contained
syn match	vimSubstPat	"\\\d"			contained
syn match	vimSubstTwoBS	"\\\\"			contained

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
syn match	vimAddress	"[:|][%.]"lc=1
syn match	vimAddress	"<Bar>%"lc=5

syn match	vimFilter	":\s*!"ms=e
syn match	vimFilter	"[^a-zA-Z]!"lc=1

" Complex repeats (:h complex-repeat)
syn match vimCmplxRepeat	'[^a-zA-Z_/\\]q[0-9a-zA-Z"]'lc=1
syn match vimCmplxRepeat	'@[0-9a-z".=@:]'

" High Priority Comments: beginning of line with optional spaces then "
syn match vimComment	+^\s*".*$+	contains=vimTodo

" Set command and associated set-options (vimOptions) with comment
syn region vimSet matchgroup=vimCommand start="\s*set\>" end="|\|$"	keepend contains=vimSetEqual,vimOption,vimErrSetting,vimComment,vimSetString
syn region vimSetEqual  contained	start="="	skip="\(\\\\\)*\\\s" end="[| \t]\|$"me=e-1 contains=vimCtrlChar
syn region vimSetString contained	start=+="+hs=s+1	skip=+\(\\\\\)\\"+   end=+"+	contains=vimCtrlChar

" Highlight modifiers
syn keyword VimHLMod contained NONE bold inverse italic reverse standout underline

" Autocmd
syn match  vimIsCommand2 "[ \t<Bar>][a-zA-Z_]\+"lc=1 transparent contained contains=vimCommand
syn keyword vimAutoCmdOpt	contained	nested
syn region vimAutoCmd matchgroup=vimAutoSet start=+^\s*\(autocmd\|au\|doautocmd\|doau\)\>+	end="$" keepend contains=vimAutoEvent,vimSet,vimIsCommand,vimIsCommand2,vimString,vimComment,vimSpecFile,vimNotation,vimFilter,vimAuSynLine,vimAuRegion,vimAutoCmdOpt
syn region vimAutoCmd matchgroup=vimAutoSet start=+^\s*\(doautoa\|doautoall\)\>+		end="$" keepend contains=vimAutoEvent,vimSet,vimIsCommand,vimIsCommand2,vimString,vimComment,vimSpecFile,vimNotation,vimFilter,vimAuSynLine,vimAuRegion,vimAutoCmdOpt

" Syntax regions (all oneliners)
syn keyword vimKeyword  link   match	contained
syn keyword vimCase     ignore match	contained
syn keyword vimSynGroup add remove	contained
syn match   vimSynGroup "contains"	contained
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(highlight\|hi\)\s"	end="$"	keepend contains=vimKeyword,vimSpecial,vimGroup,vimTermSet,vimComment,vimCtrlChar,VimRegion,vimNotation,vimNumber,vimHLMod,vimOption,vimHLGroup
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+case"	end="$"	keepend contains=vimCase,vimComment
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+clear"rs=e-5 end="$"	keepend contains=vimSpecial,vimComment
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+cluster"	end="$"	keepend contains=vimSynGroup,vimComment
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+group"	end="$"	keepend contains=vimSynGroup,vimComment
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+include"	end="$"	keepend contains=vimComment,vimNotation
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+match"	end="$"	keepend contains=vimKeyword,vimSpecial,vimString,vimComment
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+region"	end="$"	keepend contains=vimKeyword,vimSpecial,vimString,vimPattern,vimComment
syn region vimRegion  matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+sync"	end="$"	keepend contains=vimKeyword,vimSpecial,vimString,vimComment
syn region vimSynLine matchgroup=vimKeyword start="^\s*\(syntax\|syn\)\s\+keyword"	end="$"	keepend contains=vimKeyword,vimSpecial,vimComment

syn region vimAuSynLine matchgroup=vimKeyword start="\s\(syntax\|syn\)\s\+keyword"lc=1 end="$"	keepend contains=vimKeyword,vimSpecial,vimComment contained
syn region vimAuRegion  matchgroup=vimKeyword start="\s\(syntax\|syn\)\s\+region"lc=1  end="$"	keepend contains=vimKeyword,vimSpecial,vimString,vimPattern,vimComment contained
syn region vimAuRegion  matchgroup=vimKeyword start="\s\(syntax\|syn\)\s\+match"lc=1	end="$"	keepend contains=vimKeyword,vimSpecial,vimString,vimComment contained
syn region vimAuRegion  matchgroup=vimKeyword start="\s\(syntax\|syn\)\s\+clear"rs=e-5 end="$"	keepend contains=vimSpecial,vimComment contained
syn region vimAuRegion  matchgroup=vimKeyword start="\s\(syntax\|syn\)\s\+case"lc=1 end="$"	keepend contains=vimCase,vimComment contained
syn region vimAuRegion  matchgroup=vimKeyword start="\s\(syntax\|syn\)\s\+sync"lc=1 end="$"	keepend contains=vimKeyword,vimSpecial,vimString,vimComment contained
syn region vimAuRegion  matchgroup=vimKeyword start="\s\(highlight\|hi\)\s"lc=1 end="$"	keepend contains=vimKeyword,vimSpecial,vimGroup,vimTermSet,vimComment,vimCtrlChar,VimRegion,vimNotation,vimNumber,vimHLMod,vimOption,vimHLGroup contained

" Angle-Bracket Notation (tnx to Michael Geddes)
syn case ignore
syn match vimNotation	"<\([scam]-\)\{0,4}\([a-z]\|f[1-9]\|f[1-9][0-9]\|tab\|cr\|return\|nl\|newline\|lf\|linefeed\|esc\|right\|left\|\(page\)\=\(\|down\|up\)\|home\|end\|del\|delete\|bs\|backspace\|space\|bar\|bslash\|help\|undo\|insert\|ins\|khome\|kend\|kpageup\|kpagedown\|kplus\|kminus\|kdivide\|kmultiply\|kenter\|lt\|mouse\|nul\)>"		contains=vimBracket
syn match vimNotation	"<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\|drag\|release\)>"		contains=vimBracket
syn match vimNotation	'<C-R>[0-9a-z"%#:.\-=]'he=e-1		contains=vimBracket
syn match vimBracket contained	"[<>]"
syn match vimBracket contained	"[<>]"
syn case match

" Additional IsCommand stuff, here by reasons of precedence
syn match vimIsCommand	"<Bar>\s*[a-zA-Z]\+"	transparent contains=vimCommand,vimNotation

" Control Characters
syn match vimCtrlChar	"[--]"

if !exists("did_vim_syntax_inits")
  let did_vim_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link vimAddress	vimMark
  hi link vimAutoCmdOpt	vimOption
  hi link vimAutoSet	vimCommand
  hi link vimConditional	vimStatement
  hi link vimFilter	vimOper
  hi link vimHLGroup	vimGroup
  hi link vimNotPatSep	vimString
  hi link vimPlainMark	vimMark
  hi link vimPlainRegister	vimRegister
  hi link vimSetString	vimString
  hi link vimSpecFileMod	vimSpecFile
  hi link vimSynGroup	vimSpecial
  hi link vimTerminal	vimGroup

  hi link vimAutoEvent	Type
  hi link vimBracket	Delimiter
  hi link vimCmplxRepeat	SpecialChar
  hi link vimCommand	Statement
  hi link vimComment	Comment
  hi link vimCtrlChar	SpecialChar
  hi link vimErrSetting	Error
  hi link vimFuncName	Function
  hi link vimFuncVar	Identifier
  hi link vimFunctionError	Error
  hi link vimGroup	Type
  hi link vimHLMod	PreProc
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
  hi link vimTermSet	StorageClass
  hi link vimTodo	Todo
endif

let b:current_syntax = "vim"

" vim: ts=17
