" Vim syntax file
" Language:	  Vim 6.2c script
" Maintainer:	  Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	  May 05, 2003
" Version:	  6.2c-01

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" prevent embedded script syntax highlighting when vim doesn't
" have perl/python/ruby/tcl support
if !exists("g:vimembedscript")
 let g:vimembedscript= 1
endif

" vimTodo: contains common special-notices for comments
"	   Use the vimCommentGroup cluster to add your own.
syn keyword vimTodo contained	    COMBAK	      NOT		RELEASED	  TODO		    WIP
syn cluster vimCommentGroup	    contains=vimTodo

" regular vim commands
syn keyword vimCommand contained    DeleteFirst	      colo[rscheme]	hid[e]		  ps[earch]	    sp[lit]
syn keyword vimCommand contained    I		      comc[lear]	his[tory]	  ptN[ext]	    spr[evious]
syn keyword vimCommand contained    N[ext]	      comp[iler]	ia[bbrev]	  pta[g]	    sr[ewind]
syn keyword vimCommand contained    Nread	      conf[irm]		iabc[lear]	  ptf[irst]	    sta[g]
syn keyword vimCommand contained    Nw		      con[tinue]	if		  ptj[ump]	    star[tinsert]
syn keyword vimCommand contained    P[rint]	      cope[n]		ij[ump]		  ptl[ast]	    stj[ump]
syn keyword vimCommand contained    X		      co[py]		il[ist]		  ptn[ext]	    st[op]
syn keyword vimCommand contained    ab[breviate]      cp[revious]	imapc[lear]	  ptp[revious]	    sts[elect]
syn keyword vimCommand contained    abc[lear]	      cq[uit]		inorea[bbrev]	  ptr[ewind]	    sun[hide]
syn keyword vimCommand contained    abo[veleft]	      cr[ewind]		is[earch]	  pts[elect]	    sus[pend]
syn keyword vimCommand contained    al[l]	      cuna[bbrev]	isp[lit]	  pu[t]		    sv[iew]
syn keyword vimCommand contained    arga[dd]	      cu[nmap]		iuna[bbrev]	  pw[d]		    syncbind
syn keyword vimCommand contained    argd[elete]	      cw[indow]		iu[nmap]	  pyf[ile]	    t
syn keyword vimCommand contained    argdo	      debugg[reedy]	j[oin]		  py[thon]	    tN[ext]
syn keyword vimCommand contained    arge[dit]	      delc[ommand]	ju[mps]		  qa[ll]	    ta[g]
syn keyword vimCommand contained    argg[lobal]	      d[elete]		k		  q[uit]	    tags
syn keyword vimCommand contained    argl[ocal]	      delf[unction]	lan[guage]	  quita[ll]	    tc[l]
syn keyword vimCommand contained    ar[gs]	      diffg[et]		la[st]		  r[ead]	    tcld[o]
syn keyword vimCommand contained    argu[ment]	      diffpatch		lc[d]		  rec[over]	    tclf[ile]
syn keyword vimCommand contained    as[cii]	      diffpu[t]		lch[dir]	  redi[r]	    te[aroff]
syn keyword vimCommand contained    bN[ext]	      diffsplit		le[ft]		  red[o]	    tf[irst]
syn keyword vimCommand contained    bad[d]	      diffthis		lefta[bove]	  redr[aw]	    the
syn keyword vimCommand contained    ba[ll]	      dig[raphs]	l[ist]		  reg[isters]	    th[row]
syn keyword vimCommand contained    bd[elete]	      di[splay]		lm[ap]		  res[ize]	    tj[ump]
syn keyword vimCommand contained    bel[owright]      dj[ump]		lmapc[lear]	  ret[ab]	    tl[ast]
syn keyword vimCommand contained    bf[irst]	      dl[ist]		lno[remap]	  retu[rn]	    tm[enu]
syn keyword vimCommand contained    bl[ast]	      dr[op]		lo[adview]	  rew[ind]	    tn[ext]
syn keyword vimCommand contained    bm[odified]	      ds[earch]		ls		  ri[ght]	    to[pleft]
syn keyword vimCommand contained    bn[ext]	      dsp[lit]		lu[nmap]	  rightb[elow]	    tp[revious]
syn keyword vimCommand contained    bo[tright]	      echoe[rr]		mak[e]		  rub[y]	    tr[ewind]
syn keyword vimCommand contained    bp[revious]	      echom[sg]		ma[rk]		  rubyd[o]	    try
syn keyword vimCommand contained    brea[k]	      echon		marks		  rubyf[ile]	    ts[elect]
syn keyword vimCommand contained    breaka[dd]	      e[dit]		mat[ch]		  ru[ntime]	    tu[nmenu]
syn keyword vimCommand contained    breakd[el]	      el[se]		menut[ranslate]	  rv[iminfo]	    una[bbreviate]
syn keyword vimCommand contained    breakl[ist]	      elsei[f]		mk[exrc]	  sN[ext]	    u[ndo]
syn keyword vimCommand contained    br[ewind]	      em[enu]		mks[ession]	  sal[l]	    unh[ide]
syn keyword vimCommand contained    bro[wse]	      emenu*		mkvie[w]	  sa[rgument]	    unm[ap]
syn keyword vimCommand contained    bufdo	      endf[unction]	mkv[imrc]	  sav[eas]	    up[date]
syn keyword vimCommand contained    b[uffer]	      en[dif]		mod[e]		  sbN[ext]	    verb[ose]
syn keyword vimCommand contained    buffers	      endt[ry]		m[ove]		  sba[ll]	    ve[rsion]
syn keyword vimCommand contained    bun[load]	      endw[hile]	new		  sbf[irst]	    vert[ical]
syn keyword vimCommand contained    bw[ipeout]	      ene[w]		n[ext]		  sbl[ast]	    v[global]
syn keyword vimCommand contained    cN[ext]	      ex		nmapc[lear]	  sbm[odified]	    vie[w]
syn keyword vimCommand contained    ca[bbrev]	      exi[t]		noh[lsearch]	  sbn[ext]	    vi[sual]
syn keyword vimCommand contained    cabc[lear]	      f[ile]		norea[bbrev]	  sbp[revious]	    vmapc[lear]
syn keyword vimCommand contained    cal[l]	      files		norm[al]	  sbr[ewind]	    vne[w]
syn keyword vimCommand contained    cat[ch]	      filetype		nu[mber]	  sb[uffer]	    vs[plit]
syn keyword vimCommand contained    cc		      fina[lly]		nun[map]	  scripte[ncoding]  vu[nmap]
syn keyword vimCommand contained    ccl[ose]	      fin[d]		omapc[lear]	  scrip[tnames]	    wN[ext]
syn keyword vimCommand contained    cd		      fini[sh]		on[ly]		  se[t]		    wa[ll]
syn keyword vimCommand contained    ce[nter]	      fir[st]		o[pen]		  setf[iletype]	    wh[ile]
syn keyword vimCommand contained    cf[ile]	      fix[del]		opt[ions]	  setg[lobal]	    winc[md]
syn keyword vimCommand contained    cfir[st]	      fo[ld]		ou[nmap]	  setl[ocal]	    windo
syn keyword vimCommand contained    cg[etfile]	      foldc[lose]	pc[lose]	  sf[ind]	    winp[os]
syn keyword vimCommand contained    c[hange]	      folddoc[losed]	ped[it]		  sfir[st	    winpos*
syn keyword vimCommand contained    chd[ir]	      foldd[oopen]	pe[rl]		  sh[ell]	    win[size]
syn keyword vimCommand contained    che[ckpath]	      foldo[pen]	perld[o]	  sign		    wn[ext]
syn keyword vimCommand contained    checkt[ime]	      fu[nction]	po[p]		  sil[ent]	    wp[revous]
syn keyword vimCommand contained    cla[st]	      g[lobal]		pop[up]		  si[malt]	    wq
syn keyword vimCommand contained    cl[ist]	      go[to]		pp[op]		  sla[st]	    wqa[ll]
syn keyword vimCommand contained    clo[se]	      gr[ep]		pre[serve]	  sl[eep]	    w[rite]
syn keyword vimCommand contained    cmapc[lear]	      grepa[dd]		prev[ious]	  sm[agic]	    ws[verb]
syn keyword vimCommand contained    cnew[er]	      ha[rdcopy]	p[rint]		  sn[ext]	    wv[iminfo]
syn keyword vimCommand contained    cn[ext]	      h[elp]		prompt		  sni[ff]	    xa[ll]
syn keyword vimCommand contained    cnf[ile]	      helpf[ind]	promptf[ind]	  sno[magic]	    x[it]
syn keyword vimCommand contained    cnorea[bbrev]     helpg[rep]	promptr[epl]	  so[urce]	    y[ank]
syn keyword vimCommand contained    col[der]	      helpt[ags]
syn match   vimCommand contained    "\<z[-+^.=]"

" vimOptions are caught only when contained in a vimSet
syn keyword vimOption contained	    :		      dip		imak		  pex		    switchbuf
syn keyword vimOption contained	    acd		      dir		imc		  pexpr		    sws
syn keyword vimOption contained	    ai		      directory		imcmdline	  pfn		    sxq
syn keyword vimOption contained	    akm		      display		imd		  pheader	    syn
syn keyword vimOption contained	    al		      dy		imdisable	  pm		    syntax
syn keyword vimOption contained	    aleph	      ea		imi		  popt		    ta
syn keyword vimOption contained	    allowrevins	      ead		iminsert	  previewheight	    tabstop
syn keyword vimOption contained	    altkeymap	      eadirection	ims		  previewwindow	    tag
syn keyword vimOption contained	    ambiwidth	      eb		imsearch	  printdevice	    tagbsearch
syn keyword vimOption contained	    ambw	      ed		inc		  printencoding	    taglength
syn keyword vimOption contained	    ar		      edcompatible	include		  printexpr	    tagrelative
syn keyword vimOption contained	    arab	      ef		includeexpr	  printfont	    tags
syn keyword vimOption contained	    arabic	      efm		incsearch	  printheader	    tagstack
syn keyword vimOption contained	    arabicshape	      ei		inde		  printoptions	    tb
syn keyword vimOption contained	    ari		      ek		indentexpr	  pt		    tbi
syn keyword vimOption contained	    arshape	      enc		indentkeys	  pvh		    tbidi
syn keyword vimOption contained	    autochdir	      encoding		indk		  pvw		    tbis
syn keyword vimOption contained	    autoindent	      endofline		inex		  readonly	    tbs
syn keyword vimOption contained	    autoread	      eol		inf		  remap		    tenc
syn keyword vimOption contained	    autowrite	      ep		infercase	  report	    term
syn keyword vimOption contained	    autowriteall      equalalways	insertmode	  restorescreen	    termbidi
syn keyword vimOption contained	    aw		      equalprg		is		  revins	    termencoding
syn keyword vimOption contained	    awa		      errorbells	isf		  ri		    terse
syn keyword vimOption contained	    background	      errorfile		isfname		  rightleft	    textauto
syn keyword vimOption contained	    backspace	      errorformat	isi		  rightleftcmd	    textmode
syn keyword vimOption contained	    backup	      esckeys		isident		  rl		    textwidth
syn keyword vimOption contained	    backupcopy	      et		isk		  rlc		    tf
syn keyword vimOption contained	    backupdir	      eventignore	iskeyword	  ro		    tgst
syn keyword vimOption contained	    backupext	      ex		isp		  rs		    thesaurus
syn keyword vimOption contained	    backupskip	      expandtab		isprint		  rtp		    tildeop
syn keyword vimOption contained	    balloondelay      exrc		joinspaces	  ru		    timeout
syn keyword vimOption contained	    ballooneval	      fcl		js		  ruf		    timeoutlen
syn keyword vimOption contained	    bdir	      fcs		key		  ruler		    title
syn keyword vimOption contained	    bdlay	      fdc		keymap		  rulerformat	    titlelen
syn keyword vimOption contained	    beval	      fde		keymodel	  runtimepath	    titleold
syn keyword vimOption contained	    bex		      fdi		keywordprg	  sb		    titlestring
syn keyword vimOption contained	    bg		      fdl		km		  sbo		    tl
syn keyword vimOption contained	    bh		      fdls		kmp		  sbr		    tm
syn keyword vimOption contained	    bin		      fdm		kp		  sc		    to
syn keyword vimOption contained	    binary	      fdn		langmap		  scb		    toolbar
syn keyword vimOption contained	    biosk	      fdo		langmenu	  scr		    toolbariconsize
syn keyword vimOption contained	    bioskey	      fdt		laststatus	  scroll	    top
syn keyword vimOption contained	    bk		      fen		lazyredraw	  scrollbind	    tr
syn keyword vimOption contained	    bkc		      fenc		lbr		  scrolljump	    ts
syn keyword vimOption contained	    bl		      fencs		lcs		  scrolloff	    tsl
syn keyword vimOption contained	    bomb	      ff		linebreak	  scrollopt	    tsr
syn keyword vimOption contained	    breakat	      ffs		lines		  scs		    ttimeout
syn keyword vimOption contained	    brk		      fileencoding	linespace	  sect		    ttimeoutlen
syn keyword vimOption contained	    browsedir	      fileencodings	lisp		  sections	    ttm
syn keyword vimOption contained	    bs		      fileformat	lispwords	  secure	    tty
syn keyword vimOption contained	    bsdir	      fileformats	list		  sel		    ttybuiltin
syn keyword vimOption contained	    bsk		      filetype		listchars	  selection	    ttyfast
syn keyword vimOption contained	    bt		      fillchars		lm		  selectmode	    ttym
syn keyword vimOption contained	    bufhidden	      fk		lmap		  sessionoptions    ttymouse
syn keyword vimOption contained	    buflisted	      fkmap		loadplugins	  sft		    ttyscroll
syn keyword vimOption contained	    buftype	      fml		lpl		  sh		    ttytype
syn keyword vimOption contained	    casemap	      fmr		ls		  shcf		    tw
syn keyword vimOption contained	    cb		      fo		lsp		  shell		    tx
syn keyword vimOption contained	    ccv		      foldclose		lw		  shellcmdflag	    uc
syn keyword vimOption contained	    cd		      foldcolumn	lz		  shellpipe	    ul
syn keyword vimOption contained	    cdpath	      foldenable	ma		  shellquote	    undolevels
syn keyword vimOption contained	    cedit	      foldexpr		magic		  shellredir	    updatecount
syn keyword vimOption contained	    cf		      foldignore	makeef		  shellslash	    updatetime
syn keyword vimOption contained	    ch		      foldlevel		makeprg		  shelltype	    ut
syn keyword vimOption contained	    charconvert	      foldlevelstart	mat		  shellxquote	    vb
syn keyword vimOption contained	    cin		      foldmarker	matchpairs	  shiftround	    vbs
syn keyword vimOption contained	    cindent	      foldmethod	matchtime	  shiftwidth	    vdir
syn keyword vimOption contained	    cink	      foldminlines	maxfuncdepth	  shm		    ve
syn keyword vimOption contained	    cinkeys	      foldnestmax	maxmapdepth	  shortmess	    verbose
syn keyword vimOption contained	    cino	      foldopen		maxmem		  shortname	    vi
syn keyword vimOption contained	    cinoptions	      foldtext		maxmemtot	  showbreak	    viewdir
syn keyword vimOption contained	    cinw	      formatoptions	mef		  showcmd	    viewoptions
syn keyword vimOption contained	    cinwords	      formatprg		menuitems	  showfulltag	    viminfo
syn keyword vimOption contained	    clipboard	      fp		mfd		  showmatch	    virtualedit
syn keyword vimOption contained	    cmdheight	      ft		mh		  showmode	    visualbell
syn keyword vimOption contained	    cmdwinheight      gcr		mis		  shq		    vop
syn keyword vimOption contained	    cmp		      gd		ml		  si		    wa
syn keyword vimOption contained	    cms		      gdefault		mls		  sidescroll	    wak
syn keyword vimOption contained	    co		      gfm		mm		  sidescrolloff	    warn
syn keyword vimOption contained	    columns	      gfn		mmd		  siso		    wb
syn keyword vimOption contained	    com		      gfs		mmt		  sj		    wc
syn keyword vimOption contained	    comments	      gfw		mod		  slm		    wcm
syn keyword vimOption contained	    commentstring     ghr		modeline	  sm		    wd
syn keyword vimOption contained	    compatible	      go		modelines	  smartcase	    weirdinvert
syn keyword vimOption contained	    complete	      gp		modifiable	  smartindent	    wfh
syn keyword vimOption contained	    confirm	      grepformat	modified	  smarttab	    wh
syn keyword vimOption contained	    consk	      grepprg		more		  smd		    whichwrap
syn keyword vimOption contained	    conskey	      guicursor		mouse		  sn		    wig
syn keyword vimOption contained	    cp		      guifont		mousef		  so		    wildchar
syn keyword vimOption contained	    cpo		      guifontset	mousefocus	  softtabstop	    wildcharm
syn keyword vimOption contained	    cpoptions	      guifontwide	mousehide	  sol		    wildignore
syn keyword vimOption contained	    cpt		      guiheadroom	mousem		  sp		    wildmenu
syn keyword vimOption contained	    cscopepathcomp    guioptions	mousemodel	  splitbelow	    wildmode
syn keyword vimOption contained	    cscopeprg	      guipty		mouses		  splitright	    wim
syn keyword vimOption contained	    cscopetag	      helpfile		mouseshape	  spr		    winaltkeys
syn keyword vimOption contained	    cscopetagorder    helpheight	mouset		  sr		    winfixheight
syn keyword vimOption contained	    cscopeverbose     hf		mousetime	  srr		    winheight
syn keyword vimOption contained	    cspc	      hh		mp		  ss		    winminheight
syn keyword vimOption contained	    csprg	      hi		mps		  ssl		    winminwidth
syn keyword vimOption contained	    cst		      hid		nf		  ssop		    winwidth
syn keyword vimOption contained	    csto	      hidden		nrformats	  st		    wiv
syn keyword vimOption contained	    csverb	      highlight		nu		  sta		    wiw
syn keyword vimOption contained	    cwh		      history		number		  startofline	    wm
syn keyword vimOption contained	    debug	      hk		oft		  statusline	    wmh
syn keyword vimOption contained	    deco	      hkmap		osfiletype	  stl		    wmnu
syn keyword vimOption contained	    def		      hkmapp		pa		  sts		    wmw
syn keyword vimOption contained	    define	      hkp		para		  su		    wrap
syn keyword vimOption contained	    delcombine	      hl		paragraphs	  sua		    wrapmargin
syn keyword vimOption contained	    dex		      hls		paste		  suffixes	    wrapscan
syn keyword vimOption contained	    dg		      hlsearch		pastetoggle	  suffixesadd	    write
syn keyword vimOption contained	    dict	      ic		patchexpr	  sw		    writeany
syn keyword vimOption contained	    dictionary	      icon		patchmode	  swapfile	    writebackup
syn keyword vimOption contained	    diff	      iconstring	path		  swapsync	    writedelay
syn keyword vimOption contained	    diffexpr	      ignorecase	pdev		  swb		    ws
syn keyword vimOption contained	    diffopt	      im		penc		  swf		    ww
syn keyword vimOption contained	    digraph	      imactivatekey

" These are the turn-off setting variants
syn keyword vimOption contained	    loadplugins	      nocsverb		noimcmdline	  norightleft	    nota
syn keyword vimOption contained	    noacd	      nodg		noimd		  norightleftcmd    notagbsearch
syn keyword vimOption contained	    noai	      nodiff		noincsearch	  norl		    notagrelative
syn keyword vimOption contained	    noakm	      nodigraph		noinf		  norlc		    notagstack
syn keyword vimOption contained	    noallowrevins     nodisable		noinfercase	  noro		    notbi
syn keyword vimOption contained	    noaltkeymap	      noea		noinsertmode	  nors		    notbidi
syn keyword vimOption contained	    noar	      noeb		nois		  noru		    notbs
syn keyword vimOption contained	    noarab	      noed		nojoinspaces	  noruler	    notermbidi
syn keyword vimOption contained	    noarabic	      noedcompatible	nojs		  nosb		    noterse
syn keyword vimOption contained	    noarabicshape     noek		nolazyredraw	  nosc		    notextauto
syn keyword vimOption contained	    noari	      noendofline	nolbr		  noscb		    notextmode
syn keyword vimOption contained	    noarshape	      noeol		nolinebreak	  noscrollbind	    notf
syn keyword vimOption contained	    noatuochdir	      noequalalways	nolisp		  noscs		    notgst
syn keyword vimOption contained	    noautoindent      noerrorbells	nolist		  nosecure	    notildeop
syn keyword vimOption contained	    noautoread	      noesckeys		noloadplugins	  nosft		    notimeout
syn keyword vimOption contained	    noautowrite	      noet		nolpl		  noshellslash	    notitle
syn keyword vimOption contained	    noautowriteall    noex		nolz		  noshiftround	    noto
syn keyword vimOption contained	    noaw	      noexpandtab	noma		  noshortname	    notop
syn keyword vimOption contained	    noawa	      noexrc		nomagic		  noshowcmd	    notr
syn keyword vimOption contained	    nobackup	      nofen		nomh		  noshowfulltag	    nottimeout
syn keyword vimOption contained	    noballooneval     nofk		noml		  noshowmatch	    nottybuiltin
syn keyword vimOption contained	    nobeval	      nofkmap		nomod		  noshowmode	    nottyfast
syn keyword vimOption contained	    nobin	      nofoldenable	nomodeline	  nosi		    notx
syn keyword vimOption contained	    nobinary	      nogd		nomodifiable	  nosm		    novb
syn keyword vimOption contained	    nobiosk	      nogdefault	nomodified	  nosmartcase	    novisualbell
syn keyword vimOption contained	    nobioskey	      noguipty		nomore		  nosmartindent	    nowa
syn keyword vimOption contained	    nobk	      nohid		nomousef	  nosmarttab	    nowarn
syn keyword vimOption contained	    nobomb	      nohidden		nomousefocus	  nosmd		    nowb
syn keyword vimOption contained	    nocf	      nohk		nomousehide	  nosn		    noweirdinvert
syn keyword vimOption contained	    nocin	      nohkmap		nonu		  nosol		    nowildmenu
syn keyword vimOption contained	    nocindent	      nohkmapp		nonumber	  nosplitbelow	    nowiv
syn keyword vimOption contained	    nocompatible      nohkp		nopaste		  nosplitright	    nowmnu
syn keyword vimOption contained	    noconfirm	      nohls		nopreviewwindow	  nospr		    nowrap
syn keyword vimOption contained	    noconsk	      nohlsearch	nopvw		  nosr		    nowrapscan
syn keyword vimOption contained	    noconskey	      noic		noreadonly	  nossl		    nowrite
syn keyword vimOption contained	    nocp	      noicon		noremap		  nosta		    nowriteany
syn keyword vimOption contained	    nocscopetag	      noignorecase	norestorescreen	  nostartofline	    nowritebackup
syn keyword vimOption contained	    nocscopeverbose   noim		norevins	  noswapfile	    nows
syn keyword vimOption contained	    nocst	      noimc		nori		  noswf

" termcap codes (which can also be set)
syn keyword vimOption contained	    t_AB	      t_IS		t_KI		  t_cm		    t_kI	      t_nd
syn keyword vimOption contained	    t_AF	      t_K1		t_KJ		  t_cs		    t_kN	      t_op
syn keyword vimOption contained	    t_AL	      t_K3		t_KK		  t_da		    t_kP	      t_se
syn keyword vimOption contained	    t_CS	      t_K4		t_KL		  t_db		    t_kb	      t_so
syn keyword vimOption contained	    t_CV	      t_K5		t_RI		  t_dl		    t_kd	      t_sr
syn keyword vimOption contained	    t_Co	      t_K6		t_RV		  t_fs		    t_ke	      t_te
syn keyword vimOption contained	    t_DL	      t_K7		t_Sb		  t_k1		    t_kh	      t_ti
syn keyword vimOption contained	    t_F1	      t_K8		t_Sf		  t_k2		    t_kl	      t_ts
syn keyword vimOption contained	    t_F2	      t_K9		t_WP		  t_k3		    t_kr	      t_ue
syn keyword vimOption contained	    t_F3	      t_KA		t_WS		  t_k4		    t_ks	      t_us
syn keyword vimOption contained	    t_F4	      t_KB		t_ZH		  t_k5		    t_ku	      t_ut
syn keyword vimOption contained	    t_F5	      t_KC		t_ZR		  t_k6		    t_le	      t_vb
syn keyword vimOption contained	    t_F6	      t_KD		t_al		  t_k7		    t_mb	      t_ve
syn keyword vimOption contained	    t_F7	      t_KE		t_bc		  t_k8		    t_md	      t_vi
syn keyword vimOption contained	    t_F8	      t_KF		t_cd		  t_k9		    t_me	      t_vs
syn keyword vimOption contained	    t_F9	      t_KG		t_ce		  t_kB		    t_mr	      t_xs
syn keyword vimOption contained	    t_IE	      t_KH		t_cl		  t_kD		    t_ms
syn match   vimOption contained	    "t_#2"
syn match   vimOption contained	    "t_#4"
syn match   vimOption contained	    "t_%1"
syn match   vimOption contained	    "t_%i"
syn match   vimOption contained	    "t_&8"
syn match   vimOption contained	    "t_*7"
syn match   vimOption contained	    "t_@7"
syn match   vimOption contained	    "t_k;"

" these settings don't actually cause errors in vim, but were supported by vi and don't do anything in vim
syn keyword vimErrSetting contained hardtabs	      w1200		w9600		  wi		    window
syn keyword vimErrSetting contained ht		      w300

" AutoBuf Events
syn case ignore
syn keyword vimAutoEvent contained  BufAdd	      BufUnload		E200		  FileReadPre	    RemoteReply
syn keyword vimAutoEvent contained  BufCreate	      BufWinEnter	E201		  FileType	    StdinReadPost
syn keyword vimAutoEvent contained  BufDelete	      BufWinLeave	E203		  FileWriteCmd	    StdinReadPre
syn keyword vimAutoEvent contained  BufEnter	      BufWipeout	E204		  FileWritePost	    Syntax
syn keyword vimAutoEvent contained  BufFilePost	      BufWrite		EncodingChanged	  FileWritePre	    TermChanged
syn keyword vimAutoEvent contained  BufFilePre	      BufWriteCmd	FileAppendCmd	  FilterReadPost    TermResponse
syn keyword vimAutoEvent contained  BufHidden	      BufWritePost	FileAppendPost	  FilterReadPre	    User
syn keyword vimAutoEvent contained  BufLeave	      BufWritePre	FileAppendPre	  FilterWritePost   UserGettingBored
syn keyword vimAutoEvent contained  BufNew	      Cmd-event		FileChangedRO	  FilterWritePre    VimEnter
syn keyword vimAutoEvent contained  BufNewFile	      CmdwinEnter	FileChangedShell  FocusGained	    VimLeave
syn keyword vimAutoEvent contained  BufRead	      CmdwinLeave	FileEncoding	  FocusLost	    VimLeavePre
syn keyword vimAutoEvent contained  BufReadCmd	      CursorHold	FileReadCmd	  FuncUndefined	    WinEnter
syn keyword vimAutoEvent contained  BufReadPost	      E135		FileReadPost	  GUIEnter	    WinLeave
syn keyword vimAutoEvent contained  BufReadPre	      E143

" Highlight commonly used Groupnames
syn keyword vimGroup contained	    Comment	      Identifier	Keyword		  Type		    Delimiter
syn keyword vimGroup contained	    Constant	      Function		Exception	  StorageClass	    SpecialComment
syn keyword vimGroup contained	    String	      Statement		PreProc		  Structure	    Debug
syn keyword vimGroup contained	    Character	      Conditional	Include		  Typedef	    Underlined
syn keyword vimGroup contained	    Number	      Repeat		Define		  Special	    Ignore
syn keyword vimGroup contained	    Boolean	      Label		Macro		  SpecialChar	    Error
syn keyword vimGroup contained	    Float	      Operator		PreCondit	  Tag		    Todo

" Default highlighting groups
syn keyword vimHLGroup contained    Cursor	      Directory		Menu		  Scrollbar	    Tooltip
syn keyword vimHLGroup contained    CursorIM	      ErrorMsg		ModeMsg		  Search	    VertSplit
syn keyword vimHLGroup contained    DiffAdd	      FoldColumn	MoreMsg		  SpecialKey	    Visual
syn keyword vimHLGroup contained    DiffChange	      Folded		NonText		  StatusLine	    VisualNOS
syn keyword vimHLGroup contained    DiffDelete	      IncSearch		Normal		  StatusLineNC	    WarningMsg
syn keyword vimHLGroup contained    DiffText	      LineNr		Question	  Title		    WildMenu
syn case match

" Function Names
syn keyword vimFuncName contained   MyCounter	      exists		globpath	  maparg	    strftime
syn keyword vimFuncName contained   append	      expand		has		  mapcheck	    stridx
syn keyword vimFuncName contained   argc	      filereadable	hasmapto	  match		    strlen
syn keyword vimFuncName contained   argidx	      filewritable	histadd		  matchend	    strpart
syn keyword vimFuncName contained   argv	      fnamemodify	histdel		  matchstr	    strridx
syn keyword vimFuncName contained   browse	      foldclosed	histget		  mode		    strtrans
syn keyword vimFuncName contained   bufexists	      foldclosedend	histnr		  nextnonblank	    submatch
syn keyword vimFuncName contained   buflisted	      foldlevel		hlID		  nr2char	    substitute
syn keyword vimFuncName contained   bufloaded	      foldtext		hlexists	  prevnonblank	    synID
syn keyword vimFuncName contained   bufname	      foreground	hostname	  remote_expr	    synIDattr
syn keyword vimFuncName contained   bufnr	      function		iconv		  remote_foreground synIDtrans
syn keyword vimFuncName contained   bufwinnr	      getbufvar		indent		  remote_peek	    system
syn keyword vimFuncName contained   byte2line	      getchar		input		  remote_read	    tempname
syn keyword vimFuncName contained   char2nr	      getcharmod	inputdialog	  remote_send	    tolower
syn keyword vimFuncName contained   cindent	      getcwd		inputrestore	  rename	    toupper
syn keyword vimFuncName contained   col		      getfsize		inputsave	  resolve	    type
syn keyword vimFuncName contained   confirm	      getftime		inputsecret	  search	    virtcol
syn keyword vimFuncName contained   cscope_connection getline		isdirectory	  searchpair	    visualmode
syn keyword vimFuncName contained   cursor	      getreg		libcall		  server2client	    winbufnr
syn keyword vimFuncName contained   delete	      getregtype	libcallnr	  serverlist	    wincol
syn keyword vimFuncName contained   did_filetype      getwinposx	line		  setbufvar	    winheight
syn keyword vimFuncName contained   escape	      getwinposy	line2byte	  setline	    winline
syn keyword vimFuncName contained   eventhandler      getwinvar		lispindent	  setreg	    winnr
syn keyword vimFuncName contained   executable	      glob		localtime	  setwinvar	    winwidth

"--- syntax above generated by mkvimvim ---

" Special Vim Highlighting

" All vimCommands are contained by vimIsCommands.
syn match vimCmdSep		    "[:|]\+"	      skipwhite nextgroup=vimAddress,vimAutoCmd,vimCommand,vimExtCmd,vimFilter,vimLet,vimMap,vimMark,vimSet,vimSyntax,vimUserCmd
syn match vimIsCommand		    "\<\a\+\>"	      contains=vimCommand
syn match vimVar		    "\<[bwglsav]:\K\k*\>"
syn match vimVar contained	    "\<\K\k*\>"
syn match vimFunc		    "\I\i*\s*("	      contains=vimFuncName,vimCommand

" Insertions And Appends: insert append
" =======================
syn region vimInsert		    matchgroup=vimCommand start="^[: \t]*a\%[ppend]$"	  matchgroup=vimCommand end="^\.$""
syn region vimInsert		    matchgroup=vimCommand start="^[: \t]*i\%[nsert]$"	  matchgroup=vimCommand end="^\.$""

" Behave!
" =======
syn match   vimBehave		    "\<be\%[have]\>" skipwhite nextgroup=vimBehaveModel,vimBehaveError
syn keyword vimBehaveModel contained		      mswin		xterm
syn match   vimBehaveError contained		      "[^ ]\+"

" Filetypes
" =========
syn match   vimFiletype		    "\<filet\%[ype]\(\s\+\I\i*\)*\(|\|$\)"		  skipwhite contains=vimFTCmd,vimFTOption,vimFTError
syn match   vimFTError  contained   "\I\i*"
syn keyword vimFTCmd    contained   filet[ype]
syn keyword vimFTOption contained   on		      off		indent		  plugin

" Augroup : vimAugroupError removed because long augroups caused sync'ing problems.
" ======= : Trade-off: Increasing synclines with slower editing vs augroup END error checking.
syn cluster vimAugroupList	    contains=vimIsCommand,vimFunction,vimFunctionError,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar,vimContinue
syn region  vimAugroup		    start="\<aug\%[roup]\>\s\+\K\k*" end="\<aug\%[roup]\>\s\+[eE][nN][dD]\>"		      contains=vimAugroupKey,vimAutoCmd,@vimAugroupList keepend
syn match   vimAugroupError	    "\<aug\%[roup]\>\s\+[eE][nN][dD]\>"
syn keyword vimAugroupKey contained aug[roup]

" Functions : Tag is provided for those who wish to highlight tagged functions
" =========
syn cluster vimFuncList		    contains=vimFuncKey,Tag,vimFuncSID
syn cluster vimFuncBodyList	    contains=vimIsCommand,vimFunction,vimFunctionError,vimFuncBody,vimLineComment,vimSpecFile,vimOper,vimNumber,vimComment,vimString,vimSubst,vimMark,vimRegister,vimAddress,vimFilter,vimCmplxRepeat,vimComment,vimLet,vimSet,vimAutoCmd,vimRegion,vimSynLine,vimNotation,vimCtrlChar,vimFuncVar,vimContinue
syn match   vimFunctionError	    "\<fu\%[nction]!\=\s\+\U.\{-}("me=e-1		  contains=vimFuncKey,vimFuncBlank nextgroup=vimFuncBody
syn match   vimFunction		    "\<fu\%[nction]!\=\s\+\(<[sS][iI][dD]>\|s:\|\u\)\w*("me=e-1		    contains=@vimFuncList nextgroup=vimFuncBody
syn region  vimFuncBody  contained  start=")"	      end="\<endf"	contains=@vimFuncBodyList
syn match   vimFuncVar   contained  "a:\(\I\i*\|\d\+\)"
syn match   vimFuncSID   contained  "<[sS][iI][dD]>\|\<s:"
syn keyword vimFuncKey   contained  fu[nction]
syn match   vimFuncBlank contained  "\s\+"

syn keyword vimPattern  contained   start	      skip		end

" Operators
syn match vimOper "\(==\|!=\|>=\|<=\|=\~\|!\~\|>\|<\|=\)[?#]\{0,2}"	skipwhite nextgroup=vimString,vimSpecFile
syn match vimOper "||\|&&\|+\|-\|\."					skipwhite nextgroup=vimString,vimSpecFile

" Special Filenames, Modifiers, Extension Removal
syn match vimSpecFile		    "<c\(word\|WORD\)>"			nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile		    "<\([acs]file\|amatch\|abuf\)>"	nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile		    "\s%[ \t:]"ms=s+1,me=e-1		nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile		    "\s%$"ms=s+1			nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile		    "\s%<"ms=s+1,me=e-1			nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFile		    "#\d\+\|[#%]<\>"			nextgroup=vimSpecFileMod,vimSubst
syn match vimSpecFileMod	    "\(:[phtre]\)\+"			contained

" User-Specified Commands
syn cluster vimUserCmdList	    contains=vimAddress,vimSyntax,vimHighlight,vimAutoCmd,vimCmplxRepeat,vimComment,vimCtrlChar,vimEscapeBrace,vimFilter,vimFunc,vimFunction,vimIsCommand,vimMark,vimNotation,vimNumber,vimOper,vimRegion,vimRegister,vimLet,vimSet,vimSetEqual,vimSetString,vimSpecFile,vimString,vimSubst,vimSubstRep,vimSubstRange,vimSynLine
syn keyword vimUserCommand	    contained	      com[mand]
syn match   vimUserCmd		    "\<com\%[mand]!\=\>.*$"				  contains=vimUserAttrb,vimUserCommand,@vimUserCmdList
syn match   vimUserAttrb	    contained	      "-n\%[args]=[01*?+]"		  contains=vimUserAttrbKey,vimOper
syn match   vimUserAttrb	    contained	      "-com\%[plete]="			  contains=vimUserAttrbKey,vimOper nextgroup=vimUserAttrbCmplt,vimUserAttrbCmpltCust,vimUserCmdError
syn match   vimUserAttrb	    contained	      "-ra\%[nge]\(=%\|=\d\+\)\="	  contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	    contained	      "-cou\%[nt]=\d\+"			  contains=vimNumber,vimOper,vimUserAttrbKey
syn match   vimUserAttrb	    contained	      "-bang\=\>"			  contains=vimOper,vimUserAttrbKey
syn match   vimUserAttrb	    contained	      "-bar\>"				  contains=vimOper,vimUserAttrbKey
syn match   vimUserAttrb	    contained	      "-re\%[gister]\>"			  contains=vimOper,vimUserAttrbKey
syn match   vimUserCmdError	    contained	      "\S\+\>"
syn keyword vimUserAttrbKey	    contained	      bar		ban[g]		  cou[nt]	    ra[nge]
syn keyword vimUserAttrbKey	    contained	      com[plete]	n[args]		  re[gister]
syn keyword vimUserAttrbCmplt	    contained	      augroup		environment	  function	    mapping	      tag
syn keyword vimUserAttrbCmplt	    contained	      buffer		event		  help		    menu	      tag_listfiles
syn keyword vimUserAttrbCmplt	    contained	      command		expression	  highlight	    option	      var
syn keyword vimUserAttrbCmplt	    contained	      dir		file
syn match   vimUserAttrbCmpltCust   contained	      "custom,"				  nextgroup=vimUserCmplFuncName
syn match   vimUserCmplFuncName	    contained	      "\u\i*\>"

" Numbers
" =======
syn match vimNumber		    "\<\d\+\([lL]\|\.\d\+\)\="
syn match vimNumber		    "-\d\+\([lL]\|\.\d\+\)\="
syn match vimNumber		    "\<0[xX]\x\+"
syn match vimNumber		    "#\x\{6}"

" Errors
" ======
syn match  vimElseIfErr		    "\<else\s\+if\>"

" Lower Priority Comments: after some vim commands...
" =======================
syn match  vimComment		    excludenl +\s"[^\-:.%#=*].*$+lc=1	contains=@vimCommentGroup,vimCommentString
syn match  vimComment		    +\<endif\s\+".*$+lc=5		contains=@vimCommentGroup,vimCommentString
syn match  vimComment		    +\<else\s\+".*$+lc=4		contains=@vimCommentGroup,vimCommentString
syn region vimCommentString	    contained oneline start='\S\s\+"'ms=s+1		  end='"'

" Environment Variables
" =====================
syn match vimEnvvar		    "\$\I\i*"
syn match vimEnvvar		    "\${\I\i*}"

" Try to catch strings, if nothing else matches (therefore it must precede the others!)
"  vimEscapeBrace handles ["]  []"] (ie. "s don't terminate string inside [])
syn region vimEscapeBrace	    oneline contained transparent	start="[^\\]\(\\\\\)*\[\^\=\]\=" skip="\\\\\|\\\]" end="\]"me=e-1
syn match  vimPatSepErr		    contained	      "\\)"
syn match  vimPatSep		    contained	      "\\|"
syn region vimPatSepZone	    contained transparent matchgroup=vimPatSep start="\\%\=(" skip="\\\\" end="\\)"	      contains=@vimStringGroup
syn region vimPatRegion		    contained transparent matchgroup=vimPatSep start="\\z\=(" end="\\)"			      contains=@vimSubstList oneline
syn match  vimNotPatSep		    contained	      "\\\\"
syn cluster vimStringGroup	    contains=vimEscapeBrace,vimPatSep,vimNotPatSep,vimPatSepErr,vimPatSepZone
syn region vimString		    oneline keepend   start=+[^:a-zA-Z>!\\]"+lc=1 skip=+\\\\\|\\"+ end=+"+		      contains=@vimStringGroup
syn region vimString		    oneline keepend   start=+[^:a-zA-Z>!\\]'+lc=1 end=+'+				      contains=@vimStringGroup
syn region vimString		    oneline	      start=+=!+lc=1	skip=+\\\\\|\\!+ end=+!+			      contains=@vimStringGroup
syn region vimString		    oneline	      start="=+"lc=1	skip="\\\\\|\\+" end="+"			      contains=@vimStringGroup
syn region vimString		    oneline	      start="[^\\]+\s*[^a-zA-Z0-9. \t]"lc=1 skip="\\\\\|\\+" end="+"	      contains=@vimStringGroup
syn region vimString		    oneline	      start="\s/\s*\A"lc=1 skip="\\\\\|\\+" end="/"			      contains=@vimStringGroup
syn match  vimString		    contained	      +"[^"]*\\$+	skipnl nextgroup=vimStringCont
syn match  vimStringCont	    contained	      +\(\\\\\|.\)\{-}[^\\]"+

" Substitutions
" =============
syn cluster vimSubstList	    contains=vimPatSep,vimPatRegion,vimPatSepErr,vimSubstTwoBS,vimSubstRange,vimNotation
syn cluster vimSubstRepList	    contains=vimSubstSubstr,vimSubstTwoBS,vimNotation
syn cluster vimSubstList	    add=vimCollection
syn match   vimSubst				      "\(:\+\s*\|^\s*\||\s*\)\<s\%[ubstitute][:[:alpha:]]\@!" nextgroup=vimSubstPat
syn match   vimSubst1       contained		      "s\%[ubstitute]\>"				    nextgroup=vimSubstPat
syn region  vimSubstPat     contained		      matchgroup=vimSubstDelim start="\z([^a-zA-Z( \t[\]&]\)"rs=s+1 skip="\\\\\|\\\z1" end="\z1"re=e-1,me=e-1	   contains=@vimSubstList	      nextgroup=vimSubstRep4		  oneline
syn region  vimSubstRep4    contained		      matchgroup=vimSubstDelim start="\z(.\)" skip="\\\\\|\\\z1" end="\z1" matchgroup=vimNotation end="<[cC][rR]>" contains=@vimSubstRepList	      nextgroup=vimSubstFlagErr
syn region  vimCollection   contained transparent     start="\\\@<!\[" skip="\\\[" end="\]"		    contains=vimCollClass
syn match   vimCollClassErr contained		      "\[:.\{-\}:\]"
syn match   vimCollClass    contained transparent     "\[:\(alnum\|alpha\|blank\|cntrl\|digit\|graph\|lower\|print\|punct\|space\|upper\|xdigit\|return\|tab\|escape\|backspace\):\]"
syn match   vimSubstSubstr  contained		      "\\z\=\d"
syn match   vimSubstTwoBS   contained		      "\\\\"
syn match   vimSubstFlagErr contained		      "[^< \t\r]\+" contains=vimSubstFlags
syn match   vimSubstFlags   contained		      "[&cegiIpr]\+"

" 'String'
syn match  vimString		    "[^(,]'[^']\{-}'"lc=1		contains=@vimStringGroup

" Marks, Registers, Addresses, Filters
syn match  vimMark		    "[!,:]'[a-zA-Z0-9]"lc=1
syn match  vimMark		    "'[a-zA-Z0-9][,!]"me=e-1
syn match  vimMark		    "'[<>][,!]"me=e-1
syn match  vimMark		    "\<norm\s'[a-zA-Z0-9]"lc=5
syn match  vimMark		    "\<normal\s'[a-zA-Z0-9]"lc=7
syn match  vimPlainMark contained   "'[a-zA-Z0-9]"

syn match  vimRegister		    '[^(,;.]"[a-zA-Z0-9.%#:_\-/][^a-zA-Z_"]'lc=1,me=e-1
syn match  vimRegister		    '\<norm\s\+"[a-zA-Z0-9]'lc=5
syn match  vimRegister		    '\<normal\s\+"[a-zA-Z0-9]'lc=7
syn match  vimPlainRegister contained		      '"[a-zA-Z0-9\-:.%#*+=]'

syn match  vimAddress		    ",[.$]"lc=1	      skipwhite nextgroup=vimSubst1
syn match  vimAddress		    "%\a"me=e-1	      skipwhite nextgroup=vimString,vimSubst1

syn match  vimFilter contained	    "^!.\{-}\(|\|$\)" contains=vimSpecFile
syn match  vimFilter contained	    "\A!.\{-}\(|\|$\)"ms=s+1		contains=vimSpecFile

" Complex repeats (:h complex-repeat)
syn match  vimCmplxRepeat			      '[^a-zA-Z_/\\]q[0-9a-zA-Z"]'lc=1
syn match  vimCmplxRepeat			      '@[0-9a-z".=@:]'

" Set command and associated set-options (vimOptions) with comment
syn region vimSet		    matchgroup=vimCommand start="\<setlocal\|set\>" end="|"me=e-1 end="$" matchgroup=vimNotation end="<[cC][rR]>" keepend contains=vimSetEqual,vimOption,vimErrSetting,vimComment,vimSetString
syn region vimSetEqual  contained   start="="	      skip="\\\\\|\\\s" end="[| \t]\|$"me=e-1 contains=vimCtrlChar,vimSetSep,vimNotation
syn region vimSetString contained   start=+="+hs=s+1  skip=+\\\\\|\\"+  end=+"+		  contains=vimCtrlChar
syn match  vimSetSep    contained   "[,:]"

" Let
" ===
syn keyword vimLet				      let		unl[et]		  skipwhite nextgroup=vimVar

" Autocmd
" =======
syn match   vimAutoEventList	    contained	      "\(!\s\+\)\=\(\a\+,\)*\a\+"	  contains=vimAutoEvent nextgroup=vimAutoCmdSpace
syn match   vimAutoCmdSpace	    contained	      "\s\+"				  nextgroup=vimAutoCmdSfxList
syn match   vimAutoCmdSfxList	    contained	      "\S*"
syn keyword vimAutoCmd				      au[tocmd] do[autocmd] doautoa[ll]	  skipwhite nextgroup=vimAutoEventList

" Echo and Execute -- prefer strings!
" ================
syn region  vimEcho		    oneline excludenl matchgroup=vimCommand start="\<ec\%[ho]\>" skip="\(\\\\\)*\\|" end="$\||" contains=vimFuncName,vimString,vimOper,varVar
syn region  vimExecute		    oneline excludenl matchgroup=vimCommand start="\<exe\%[cute]\>" skip="\(\\\\\)*\\|" end="$\||\|<[cC][rR]>" contains=vimIsCommand,vimString,vimOper,vimVar,vimNotation
syn match   vimEchoHL		    "echohl\="	      skipwhite nextgroup=vimGroup,vimHLGroup,vimEchoHLNone
syn case ignore
syn keyword vimEchoHLNone	    none
syn case match

" Maps
" ====
syn cluster vimMapGroup		    contains=vimMapBang,vimMapLhs,vimMapMod
syn keyword vimMap		    cm[ap]	      map		om[ap]		  skipwhite nextgroup=@vimMapGroup
syn keyword vimMap		    cno[remap]	      nm[ap]		ono[remap]	  skipwhite nextgroup=@vimMapGroup
syn keyword vimMap		    im[ap]	      nn[oremap]	vm[ap]		  skipwhite nextgroup=@vimMapGroup
syn keyword vimMap		    ino[remap]	      no[remap]		vn[oremap]	  skipwhite nextgroup=@vimMapGroup
syn match   vimMapLhs    contained  "\S\+"	      contains=vimNotation,vimCtrlChar
syn match   vimMapBang   contained  "!"		      skipwhite nextgroup=vimMapLhs
syn match   vimMapMod    contained  "\c<\(buffer\|\(local\)\=leader\|plug\|script\|sid\|unique\|silent\)\+>" skipwhite contains=vimMapModKey,vimMapModErr nextgroup=@vimMapGroup
syn case ignore
syn keyword vimMapModKey contained  buffer	      leader		localleader	  plug		    script	      sid		silent		  unique
syn case match

" Menus
" =====
syn keyword vimCommand		    am[enu]	      cnoreme[nu]	me[nu]		  noreme[nu]	    vme[nu]	      skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand		    an[oremenu]	      ime[nu]		nme[nu]		  ome[nu]	    vnoreme[nu]	      skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand		    cme[nu]	      inoreme[nu]	nnoreme[nu]	  onoreme[nu]			      skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand		    aun[menu]	      iunme[nu]		ounme[nu]	  unme[nu]	    vunme[nu]	      skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn keyword vimCommand		    cunme[nu]	      nunme[nu]								      skipwhite nextgroup=vimMenuPriority,vimMenuName,vimMenuSilent
syn match   vimMenuName		    "[^ \t\\<]\+"			contained nextgroup=vimMenuNameMore,vimMenuMap
syn match   vimMenuPriority	    "\d\+\(\.\d\+\)*"			contained skipwhite nextgroup=vimMenuName
syn match   vimMenuNameMore	    "\\\s\|<[tT][aA][bB]>\|\\\."	contained nextgroup=vimMenuName,vimMenuNameMore contains=vimNotation
syn match   vimMenuSilent	    "<[sS][iI][lL][eE][nN][tT]>"	contained skipwhite nextgroup=vimMenuName,vimMenuPriority
syn match   vimMenuMap		    "\t"				contained skipwhite nextgroup=@vimMapGroup

" Angle-Bracket Notation (tnx to Michael Geddes)
" ======================
syn case ignore
syn match vimNotation		    "\(\\\|<lt>\)\=<\([scam]-\)\{0,4}x\=\(f\d\{1,2}\|[^ \t:]\|cr\|lf\|linefeed\|return\|k\=del\%[ete]\|bs\|backspace\|tab\|esc\|right\|left\|help\|undo\|insert\|ins\|k\=home\|k\=end\|kplus\|kminus\|kdivide\|kmultiply\|kenter\|k\=\(page\)\=\(\|down\|up\)\)>" contains=vimBracket
syn match vimNotation		    "\(\\\|<lt>\)\=<\([scam2-4]-\)\{0,4}\(right\|left\|middle\)\(mouse\)\=\(drag\|release\)\=>"			contains=vimBracket
syn match vimNotation		    "\(\\\|<lt>\)\=<\(bslash\|plug\|sid\|space\|bar\|nop\|nul\|lt\)>"			      contains=vimBracket
syn match vimNotation		    '\(\\\|<lt>\)\=<C-R>[0-9a-z"%#:.\-=]'he=e-1						      contains=vimBracket
syn match vimNotation		    '\(\\\|<lt>\)\=<\(line[12]\|count\|bang\|reg\|args\|lt\|[qf]-args\)>'   contains=vimBracket
syn match vimBracket contained	    "[\\<>]"
syn case match

" Syntax
"=======
syn match   vimGroupList	    contained	      "@\=[^ \t,]*"	contains=vimGroupSpecial,vimPatSep
syn match   vimGroupList	    contained	      "@\=[^ \t,]*,"	nextgroup=vimGroupList contains=vimGroupSpecial,vimPatSep
syn keyword vimGroupSpecial	    contained	      ALL		ALLBUT
syn match   vimSynError		    contained	      "\i\+"
syn match   vimSynError		    contained	      "\i\+="		nextgroup=vimGroupList
syn match   vimSynContains	    contained	      "\<contain\(s\|edin\)="		  nextgroup=vimGroupList
syn match   vimSynKeyContainedin    contained	      "\<containedin="	nextgroup=vimGroupList
syn match   vimSynNextgroup	    contained	      "nextgroup="	nextgroup=vimGroupList

syn match   vimSyntax		    "\<sy\%[ntax]\>"			contains=vimCommand skipwhite nextgroup=vimSynType,vimComment
syn match   vimAuSyntax		    contained	      "\s+sy\%[ntax]"	contains=vimCommand skipwhite nextgroup=vimSynType,vimComment

" Syntax: case
syn keyword vimSynType		    contained	      case		skipwhite nextgroup=vimSynCase,vimSynCaseError
syn match   vimSynCaseError	    contained	      "\i\+"
syn keyword vimSynCase		    contained	      ignore		match

" Syntax: clear
syn keyword vimSynType		    contained	      clear		skipwhite nextgroup=vimGroupList

" Syntax: cluster
syn keyword vimSynType				      contained		cluster				    skipwhite nextgroup=vimClusterName
syn region  vimClusterName	    contained	      matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" matchgroup=vimSep end="$\||" contains=vimGroupAdd,vimGroupRem,vimSynContains,vimSynError
syn match   vimGroupAdd		    contained	      "add="				  nextgroup=vimGroupList
syn match   vimGroupRem		    contained	      "remove="		nextgroup=vimGroupList

" Syntax: include
syn keyword vimSynType		    contained	      include				  skipwhite nextgroup=vimGroupList

" Syntax: keyword
syn cluster vimSynKeyGroup	    contains=vimSynNextgroup,vimSynKeyOpt,vimSynKeyContainedin
syn keyword vimSynType		    contained	      keyword				  skipwhite nextgroup=vimSynKeyRegion
syn region  vimSynKeyRegion	    contained keepend matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" matchgroup=vimSep end="|\|$" contains=@vimSynKeyGroup
syn match   vimSynKeyOpt	    contained	      "\<\(contained\|transparent\|skipempty\|skipwhite\|skipnl\)\>"

" Syntax: match
syn cluster vimSynMtchGroup	    contains=vimMtchComment,vimSynContains,vimSynError,vimSynMtchOpt,vimSynNextgroup,vimSynRegPat
syn keyword vimSynType		    contained	      match		skipwhite nextgroup=vimSynMatchRegion
syn region  vimSynMatchRegion	    contained keepend matchgroup=vimGroupName start="\k\+" matchgroup=vimSep end="|\|$" contains=@vimSynMtchGroup
syn match   vimSynMtchOpt	    contained	      "\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|extend\|skipnl\|fold\)\>"

" Syntax: off and on
syn keyword vimSynType		    contained	      enable		list		  manual	    off		      on		reset

" Syntax: region
syn cluster vimSynRegPatGroup	    contains=vimPatSep,vimNotPatSep,vimSynPatRange,vimSynNotPatRange,vimSubstSubstr,vimPatRegion,vimPatSepErr
syn cluster vimSynRegGroup	    contains=vimSynContains,vimSynNextgroup,vimSynRegOpt,vimSynReg,vimSynMtchGrp
syn keyword vimSynType		    contained	      region		skipwhite nextgroup=vimSynRegion
syn region  vimSynRegion	    contained keepend matchgroup=vimGroupName start="\k\+" skip="\\\\\|\\|" end="|\|$" contains=@vimSynRegGroup
syn match   vimSynRegOpt	    contained	      "\<\(transparent\|contained\|excludenl\|skipempty\|skipwhite\|display\|keepend\|oneline\|extend\|skipnl\|fold\)\>"
syn match   vimSynReg		    contained	      "\(start\|skip\|end\)="he=e-1	  nextgroup=vimSynRegPat
syn match   vimSynMtchGrp	    contained	      "matchgroup="	nextgroup=vimGroup,vimHLGroup
syn region  vimSynRegPat	    contained extend  start="\z([[:punct:]]\)"  skip="\\\\\|\\\z1"  end="\z1"  contains=@vimSynRegPatGroup skipwhite nextgroup=vimSynPatMod,vimSynReg
syn match   vimSynPatMod	    contained	      "\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\="
syn match   vimSynPatMod	    contained	      "\(hs\|ms\|me\|hs\|he\|rs\|re\)=[se]\([-+]\d\+\)\=," nextgroup=vimSynPatMod
syn match   vimSynPatMod	    contained	      "lc=\d\+"
syn match   vimSynPatMod	    contained	      "lc=\d\+," nextgroup=vimSynPatMod
syn region  vimSynPatRange	    contained	      start="\["	skip="\\\\\|\\]"   end="]"
syn match   vimSynNotPatRange	    contained	      "\\\\\|\\\["
syn match   vimMtchComment	    contained	      '"[^"]\+$'

" Syntax: sync
" ============
syn keyword vimSynType		    contained	      sync		skipwhite	  nextgroup=vimSyncC,vimSyncLines,vimSyncMatch,vimSyncError,vimSyncLinecont,vimSyncRegion
syn match   vimSyncError	    contained	      "\i\+"
syn keyword vimSyncC		    contained	      ccomment		clear		  fromstart
syn keyword vimSyncMatch	    contained	      match		skipwhite	  nextgroup=vimSyncGroupName
syn keyword vimSyncRegion	    contained	      region		skipwhite	  nextgroup=vimSynReg
syn keyword vimSyncLinecont	    contained	      linecont		skipwhite	  nextgroup=vimSynRegPat
syn match   vimSyncLines	    contained	      "\(min\|max\)\=lines="		  nextgroup=vimNumber
syn match   vimSyncGroupName	    contained	      "\k\+"		skipwhite	  nextgroup=vimSyncKey
syn match   vimSyncKey		    contained	      "\<groupthere\|grouphere\>"	  skipwhite nextgroup=vimSyncGroup
syn match   vimSyncGroup	    contained	      "\k\+"		skipwhite	  nextgroup=vimSynRegPat,vimSyncNone
syn keyword vimSyncNone		    contained	      NONE

" Additional IsCommand, here by reasons of precedence
" ====================
syn match vimIsCommand		    "<Bar>\s*\a\+"    transparent contains=vimCommand,vimNotation

" Highlighting
" ============
syn cluster vimHighlightCluster	    contains=vimHiLink,vimHiClear,vimHiKeyList,vimComment
syn match   vimHighlight			      "\<hi\%[ghlight]\>" skipwhite nextgroup=vimHiBang,@vimHighlightCluster
syn match   vimHiBang		    contained	      "!"		  skipwhite nextgroup=@vimHighlightCluster

syn match   vimHiGroup		    contained	      "\i\+"
syn case ignore
syn keyword vimHiAttrib		    contained	      none bold inverse italic reverse standout underline
syn keyword vimFgBgAttrib	    contained	      none bg background fg foreground
syn case match
syn match   vimHiAttribList	    contained	      "\i\+"		contains=vimHiAttrib
syn match   vimHiAttribList	    contained	      "\i\+,"he=e-1	contains=vimHiAttrib nextgroup=vimHiAttribList,vimHiAttrib
syn case ignore
syn keyword vimHiCtermColor	    contained	      black		darkgray	  darkyellow	    lightcyan	      lightred
syn keyword vimHiCtermColor	    contained	      blue		darkgreen	  gray		    lightgray	      magenta
syn keyword vimHiCtermColor	    contained	      brown		darkgrey	  green		    lightgreen	      red
syn keyword vimHiCtermColor	    contained	      cyan		darkmagenta	  grey		    lightgrey	      white
syn keyword vimHiCtermColor	    contained	      darkBlue		darkred		  lightblue	    lightmagenta      yellow
syn keyword vimHiCtermColor	    contained	      darkcyan
syn case match
syn match   vimHiFontname	    contained	      "[a-zA-Z\-*]\+"
syn match   vimHiGuiFontname	    contained	      "'[a-zA-Z\-* ]\+'"
syn match   vimHiGuiRgb		    contained	      "#\x\{6}"
syn match   vimHiCtermError	    contained	      "[^0-9]\i*"

" Highlighting: hi group key=arg ...
syn cluster vimHiCluster contains=vimHiGroup,vimHiTerm,vimHiCTerm,vimHiStartStop,vimHiCtermFgBg,vimHiGui,vimHiGuiFont,vimHiGuiFgBg,vimHiKeyError,vimNotation
syn region vimHiKeyList		    contained oneline start="\i\+" skip="\\\\\|\\|" end="$\||"		    contains=@vimHiCluster
syn match  vimHiKeyError	    contained	      "\i\+="he=e-1
syn match  vimHiTerm		    contained	      "[tT][eE][rR][mM]="he=e-1						      nextgroup=vimHiAttribList
syn match  vimHiStartStop	    contained	      "\([sS][tT][aA][rR][tT]\|[sS][tT][oO][pP]\)="he=e-1   nextgroup=vimHiTermcap,vimOption
syn match  vimHiCTerm		    contained	      "[cC][tT][eE][rR][mM]="he=e-1					      nextgroup=vimHiAttribList
syn match  vimHiCtermFgBg	    contained	      "[cC][tT][eE][rR][mM][fFbB][gG]="he=e-1				      nextgroup=vimNumber,vimHiCtermColor,vimFgBgAttrib,vimHiCtermError
syn match  vimHiGui		    contained	      "[gG][uU][iI]="he=e-1						      nextgroup=vimHiAttribList
syn match  vimHiGuiFont		    contained	      "[fF][oO][nN][tT]="he=e-1						      nextgroup=vimHiFontname
syn match  vimHiGuiFgBg		    contained	      "[gG][uU][iI][fFbB][gG]="he=e-1					      nextgroup=vimHiGroup,vimHiGuiFontname,vimHiGuiRgb,vimFgBgAttrib
syn match  vimHiTermcap		    contained	      "\S\+"				  contains=vimNotation

" Highlight: clear
syn keyword vimHiClear		    contained	      clear				  nextgroup=vimHiGroup

" Highlight: link
syn region vimHiLink		    contained oneline matchgroup=vimCommand start="\<\(def\s\+\)\=link\>\|\<def\>" end="$"    contains=vimHiGroup,vimGroup,vimHLGroup,vimNotation

" Control Characters
" ==================
syn match vimCtrlChar		    "[--]"

" Beginners - Patterns that involve ^
" =========
syn match  vimLineComment	    +^[ \t:]*".*$+			contains=@vimCommentGroup,vimCommentString,vimCommentTitle
syn match  vimCommentTitle	    '"\s*\u\a*\(\s\+\u\a*\)*:'hs=s+1	contained contains=vimCommentTitleLeader,vimTodo,@vimCommentGroup
syn match  vimContinue		    "^\s*\\"
syn region vimString		    start="^\s*\\\z(['"]\)" skip='\\\\\|\\\z1' end="\z1" oneline keepend contains=@vimStringGroup,vimContinue
syn match  vimCommentTitleLeader    '"\s\+'ms=s+1			contained

" Scripts  : perl,ruby : Benoit Cerrina
" =======    python,tcl: Johannes Zellner

" [-- perl --]
if (has("perl") || g:vimembedscript) && filereadable(expand("<sfile>:p:h")."/perl.vim")
  unlet! b:current_syntax
  syn include @vimPerlScript <sfile>:p:h/perl.vim
  syn region vimPerlRegion matchgroup=vimScriptDelim start=+pe\%[rl]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimPerlScript
  syn region vimPerlRegion matchgroup=vimScriptDelim start=+pe\%[rl]\s*<<\s*$+ end=+\.$+ contains=@vimPerlScript
endif

" [-- ruby --]
if (has("ruby") || g:vimembedscript) && filereadable(expand("<sfile>:p:h")."/ruby.vim")
  unlet! b:current_syntax
  syn include @vimRubyScript <sfile>:p:h/ruby.vim
  syn region vimRubyRegion matchgroup=vimScriptDelim start=+rub[y]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimRubyScript
  syn region vimRubyRegion matchgroup=vimScriptDelim start=+rub[y]\s*<<\s*$+ end=+\.$+ contains=@vimRubyScript
endif

" [-- python --]
if (has("python") || g:vimembedscript) && filereadable(expand("<sfile>:p:h")."/python.vim")
  unlet! b:current_syntax
  syn include @vimPythonScript <sfile>:p:h/python.vim
  syn region vimPythonRegion matchgroup=vimScriptDelim start=+py\%[thon]\s*<<\s*\z(.*\)$+ end=+^\z1$+ contains=@vimPythonScript
  syn region vimPythonRegion matchgroup=vimScriptDelim start=+py\%[thon]\s*<<\s*$+ end=+\.$+ contains=@vimPythonScript
endif

" [-- tcl --]
if (has("tcl") || g:vimembedscript) && filereadable(expand("<sfile>:p:h")."/tcl.vim")
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
syn sync linecont "^\s\+\\"
syn sync match vimAugroupSyncA	    groupthere NONE   "\<aug\%[roup]\>\s\+[eE][nN][dD]"

" Highlighting Settings
" ====================

" The default highlighting.
hi def link vimAuHighlight	    vimHighlight
hi def link vimSubst1		    vimSubst
hi def link vimBehaveModel	    vimBehave

hi def link vimAddress		    vimMark
hi def link vimAugroupKey	    vimCommand
"  hi def link vimAugroupError	    vimError
hi def link vimAutoCmd		    vimCommand
hi def link vimAutoCmdOpt	    vimOption
hi def link vimAutoSet		    vimCommand
hi def link vimBehave		    vimCommand
hi def link vimBehaveError	    vimError
hi def link vimCollClassErr	    vimError
hi def link vimCommentString	    vimString
hi def link vimCondHL		    vimCommand
hi def link vimEchoHL		    vimCommand
hi def link vimEchoHLNone	    vimGroup
hi def link vimElseif		    vimCondHL
hi def link vimErrSetting	    vimError
hi def link vimFgBgAttrib	    vimHiAttrib
hi def link vimFTCmd		    vimCommand
hi def link vimFTOption		    vimSynType
hi def link vimFTError		    vimError
hi def link vimFunctionError	    vimError
hi def link vimFuncKey		    vimCommand
hi def link vimGroupAdd		    vimSynOption
hi def link vimGroupRem		    vimSynOption
hi def link vimHLGroup		    vimGroup
hi def link vimHiAttribList	    vimError
hi def link vimHiCTerm		    vimHiTerm
hi def link vimHiCtermError	    vimError
hi def link vimHiCtermFgBg	    vimHiTerm
hi def link vimHiGroup		    vimGroupName
hi def link vimHiGui		    vimHiTerm
hi def link vimHiGuiFgBg	    vimHiTerm
hi def link vimHiGuiFont	    vimHiTerm
hi def link vimHiGuiRgb		    vimNumber
hi def link vimHiKeyError	    vimError
hi def link vimHiStartStop	    vimHiTerm
hi def link vimHighlight	    vimCommand
hi def link vimInsert		    vimString
hi def link vimKeyCode		    vimSpecFile
hi def link vimKeyCodeError	    vimError
hi def link vimLet		    vimCommand
hi def link vimLineComment	    vimComment
hi def link vimMap		    vimCommand
hi def link vimMapMod		    vimBracket
hi def link vimMapModErr	    vimError
hi def link vimMapModKey	    vimFuncSID
hi def link vimMapBang		    vimCommand
hi def link vimMenuNameMore	    vimMenuName
hi def link vimMtchComment	    vimComment
hi def link vimNotFunc		    vimCommand
hi def link vimNotPatSep	    vimString
hi def link vimPatSepErr	    vimPatSep
hi def link vimPlainMark	    vimMark
hi def link vimPlainRegister	    vimRegister
hi def link vimSetString	    vimString
hi def link vimSpecFileMod	    vimSpecFile
hi def link vimStringCont	    vimString
hi def link vimSubst		    vimCommand
hi def link vimSubstTwoBS	    vimString
hi def link vimSubstFlagErr	    vimError
hi def link vimSynCaseError	    vimError
hi def link vimSynContains	    vimSynOption
hi def link vimSynKeyContainedin    vimSynContains
hi def link vimSynKeyOpt	    vimSynOption
hi def link vimSynMtchGrp	    vimSynOption
hi def link vimSynMtchOpt	    vimSynOption
hi def link vimSynNextgroup	    vimSynOption
hi def link vimSynNotPatRange	    vimSynRegPat
hi def link vimSynPatRange	    vimString
hi def link vimSynRegOpt	    vimSynOption
hi def link vimSynRegPat	    vimString
hi def link vimSyntax		    vimCommand
hi def link vimSynType		    vimSpecial
hi def link vimSyncGroup	    vimGroupName
hi def link vimSyncGroupName	    vimGroupName
hi def link vimUserAttrb	    vimSpecial
hi def link vimUserAttrbCmplt	    vimSpecial
hi def link vimUserAttrbCmpltCust   vimSpecial
hi def link vimUserCmplFuncName	    vimFuncName
hi def link vimUserAttrbKey	    vimOption
hi def link vimUserCommand	    vimCommand

hi def link vimAutoEvent	    Type
hi def link vimBracket		    Delimiter
hi def link vimCmplxRepeat	    SpecialChar
hi def link vimCommand		    Statement
hi def link vimComment		    Comment
hi def link vimCommentTitle	    PreProc
hi def link vimContinue		    Special
hi def link vimCtrlChar		    SpecialChar
hi def link vimElseIfErr	    Error
hi def link vimEnvvar		    PreProc
hi def link vimError		    Error
hi def link vimFuncName		    Function
hi def link vimFuncSID		    Special
hi def link vimFuncVar		    Identifier
hi def link vimGroup		    Type
hi def link vimGroupSpecial	    Special
hi def link vimHLMod		    PreProc
hi def link vimHiAttrib		    PreProc
hi def link vimHiTerm		    Type
hi def link vimKeyword		    Statement
hi def link vimMark		    Number
hi def link vimMenuName		    PreProc
hi def link vimNotation		    Special
hi def link vimNumber		    Number
hi def link vimOper		    Operator
hi def link vimOption		    PreProc
hi def link vimPatSep		    SpecialChar
hi def link vimPattern		    Type
hi def link vimRegister		    SpecialChar
hi def link vimScriptDelim	    Comment
hi def link vimSep		    Delimiter
hi def link vimSetSep		    Statement
hi def link vimSpecFile		    Identifier
hi def link vimSpecial		    Type
hi def link vimStatement	    Statement
hi def link vimString		    String
hi def link vimSubstDelim	    Delimiter
hi def link vimSubstFlags	    Special
hi def link vimSubstSubstr	    SpecialChar
hi def link vimSynCase		    Type
hi def link vimSynCaseError	    Error
hi def link vimSynError		    Error
hi def link vimSynOption	    Special
hi def link vimSynReg		    Type
hi def link vimSyncC		    Type
hi def link vimSyncError	    Error
hi def link vimSyncKey		    Type
hi def link vimSyncNone		    Type
hi def link vimTodo		    Todo
hi def link vimUserCmdError	    Error

let b:current_syntax = "vim"

" vim: ts=8
