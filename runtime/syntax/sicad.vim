" Vim syntax file
" Language:	SiCAD (procedure language)
" Maintainer:	Zsolt Branyiczky <zbranyiczky@lmark.mgx.hu>
" Last change:	1999 Jun 14

" remove any old syntax stuff hanging around
syn clear

" ignore case
syn case ignore

" spaces are used in (auto)indents
set expandtab

set shiftwidth=1

" most important commands
syn keyword sicadStatement	define
syn keyword sicadStatement	dialog
syn keyword sicadStatement	do
syn keyword sicadStatement	dop
syn keyword sicadStatement	end
syn keyword sicadStatement	enddo
syn keyword sicadStatement	endp
syn keyword sicadStatement	erroff
syn keyword sicadStatement	erron
syn keyword sicadStatement	exitp
syn keyword sicadGoto		goto contained
syn keyword sicadStatement	hh
syn keyword sicadStatement	if
syn keyword sicadStatement	in
syn keyword sicadStatement	msgsup
syn keyword sicadStatement	out
syn keyword sicadStatement	padd
syn keyword sicadStatement	parbeg
syn keyword sicadStatement	parend
syn keyword sicadStatement	pdoc
syn keyword sicadStatement	pprot
syn keyword sicadStatement	procd
syn keyword sicadStatement	procn
syn keyword sicadStatement	psav
syn keyword sicadStatement	psel
syn keyword sicadStatement	psymb
syn keyword sicadStatement	ptrace
syn keyword sicadStatement	ptstat
syn keyword sicadStatement	set
syn keyword sicadStatement	step
syn keyword sicadStatement	ww

" functions
syn match sicadStatement	"\<atan("me=e-1
syn match sicadStatement	"\<atan2("me=e-1
syn match sicadStatement	"\<cos("me=e-1
syn match sicadStatement	"\<dist("me=e-1
syn match sicadStatement	"\<exp("me=e-1
syn match sicadStatement	"\<log("me=e-1
syn match sicadStatement	"\<log10("me=e-1
syn match sicadStatement	"\<sin("me=e-1
syn match sicadStatement	"\<sqrt("me=e-1
syn match sicadStatement	"\<tanh("me=e-1
syn match sicadStatement	"\<x("me=e-1
syn match sicadStatement	"\<y("me=e-1

" other keywords
syn match sicadOperator	"\.and\."
syn match sicadOperator	"\.ne\."
syn match sicadOperator	"\.not\."
syn match sicadOperator	"\.eq\."
syn match sicadOperator	"\.ge\."
syn match sicadOperator	"\.gt\."
syn match sicadOperator	"\.le\."
syn match sicadOperator	"\.lt\."
syn match sicadOperator	"\.or\."

" catch error caused by tabulator key
syn match sicadError		"	"

" variable name
syn match sicadIdentifier	"%g\=[irpt][0-9]\{1,2}\>"
syn match sicadIdentifier	"%g\=l[0-9]\>"   " separated logical varible
syn match sicadIdentifier	"%g\=[irptl]("me=e-1
syn match sicadIdentifier	"%error\>"
syn match sicadIdentifier	"%nsel\>"
syn match sicadIdentifier	"%nvar\>"
syn match sicadIdentifier	"%scl\>"
syn match sicadIdentifier	"%wd\>"

" label
syn match sicadLabel1	"^ *\.[a-z][a-z0-9]\{0,7} \+[^ ]"me=e-1
syn match sicadLabel2	"\<goto \.\=[a-z][a-z0-9]\{0,7}\>" contains=sicadGoto

" boolean
syn match sicadBoolean	"\.f\."
syn match sicadBoolean	"\.t\."
" integer without sign
syn match sicadNumber	"\<[0-9]\+\>"
" floating point number, with dot, optional exponent
syn match sicadFloat	"\<[0-9]\+\.[0-9]*\(e[-+]\=[0-9]\+\)\=\>"
" floating point number, starting with a dot, optional exponent
syn match sicadFloat	"\.[0-9]\+\(e[-+]\=[0-9]\+\)\=\>"
" floating point number, without dot, with exponent
syn match sicadFloat	"\<[0-9]\+e[-+]\=[0-9]\+\>"

" comments
syn region sicadComment	start="^ *\*" end=";"me=e-1 end="$" contains=sicadString_
syn region sicadComment start="^ *\.[a-z][a-z0-9]\{0,7} \+\*" end=";"me=e-1 end="$" contains=sicadLabel1,sicadString_  " just here the odd number of apostrophes are disallowed - it is not realized yet
syn region sicadComment start="; *\*"ms=s+1 end=";"me=e-1 end="$" contains=sicadString_
" comment between docbeg and docend
syn region sicadComment	matchgroup=sicadStatement start="\<docbeg\>" end="\<docend\>"

" catch \ at the end of line
syn match sicadStatement "\\ *$"

" catch errors caused by wrong parenthesis
syn region sicadParen transparent start='(' end=')' contains=ALLBUT,sicadParenError
syn match sicadParenError ")"

" string in comment is transparent - use contains=sicadString_ in sicadComment regions
syn region sicadString_ transparent start=+'+ end=+'+ oneline contained
" string
syn region sicadString start=+'+ end=+'+ oneline

" synchronizing
syn sync match sicadSyncComment groupthere NONE "\<docend\>"
syn sync match sicadSyncComment grouphere sicadComment "\<docbeg\>"
"syn sync match sicadSyncParen groupthere NONE ')'
"syn sync match sicadSyncParen grouphere sicadParen '('
" next line must be examined too
syn sync linecont "\\ *$"

" other keywords
syn keyword sicadStatement	abst add adrin aib aibzsn
syn keyword sicadStatement	aidump aifgeo aisbrk alknam alknr
syn keyword sicadStatement	alksav alksel alktrc alopen ansbo
syn keyword sicadStatement	aractiv ararea arareao arbuffer archeck
syn keyword sicadStatement	arcomv arcont arconv arcopy arcopyo
syn keyword sicadStatement	arcorr arcreate arerror areval arflfm
syn keyword sicadStatement	arflop arfrast argbkey argenf argraph
syn keyword sicadStatement	argrapho arinters arkompfl arlisly arnext
syn keyword sicadStatement	aroverl arovers arpars arrefp arselect
syn keyword sicadStatement	arset arstruct arunify arupdate arvector
syn keyword sicadStatement	arveinfl arvflfl arvoroni ausku basis
syn keyword sicadStatement	basisaus basisdar basisnr bebos befl
syn keyword sicadStatement	befli befls beo beorta beortn
syn keyword sicadStatement	bep bepan bepap bepola bepoln
syn keyword sicadStatement	bepsn bepsp ber berili berk
syn keyword sicadStatement	bewz bkl bli bma bmakt
syn keyword sicadStatement	bmbm bmerk bmerw bminit bmk
syn keyword sicadStatement	bmorth bmos bmpar bmsl bmsum
syn keyword sicadStatement	bmver bmvero bmw bo bta
syn keyword sicadStatement	buffer bvl bw bza bzap
syn keyword sicadStatement	bzd bzgera bzorth cat catel
syn keyword sicadStatement	cdbdiff ce close comp conclose
syn keyword sicadStatement	coninfo conopen conread contour conwrite
syn keyword sicadStatement	cop copel cr cs cstat
syn keyword sicadStatement	cursor d da dal dasp
syn keyword sicadStatement	dasps dataout dcol dd defsr
syn keyword sicadStatement	del delel deskrdef df dfn
syn keyword sicadStatement	dfns dfpos dfr dgd dgm
syn keyword sicadStatement	dgp dgr dh diaus dir
syn keyword sicadStatement	disbsd dkl dktx dkur dlgfix
syn keyword sicadStatement	dlgfre dma dprio dr druse
syn keyword sicadStatement	dsel dskinfo dsr dv dve
syn keyword sicadStatement	eba ebd ebs edbsdbin edbssnin
syn keyword sicadStatement	edbsvtin edt egaus egdef egdefs
syn keyword sicadStatement	eglist egloe egloenp egloes egxx
syn keyword sicadStatement	eib ekur ekuradd elpos epg
syn keyword sicadStatement	esau esauadd esek eta etap
syn keyword sicadStatement	feparam ficonv filse fl fli
syn keyword sicadStatement	flinit flkor fln flnli flop
syn keyword sicadStatement	flowert flparam flraster flsy flsyd
syn keyword sicadStatement	flsym flsyms flsymt fmtatt fmtdia
syn keyword sicadStatement	fmtlib fpg gbadddb gbanrs gbatw
syn keyword sicadStatement	gbau gbaudit gbclosp gbcreem gbcreld
syn keyword sicadStatement	gbcresdb gbcretd gbde gbdeldb gbdelem
syn keyword sicadStatement	gbdelld gbdeltd gbdisdb gbdisem gbdisld
syn keyword sicadStatement	gbdistd gbebn gbemau gbepsv gbgetdet
syn keyword sicadStatement	gbgetmas gbgqel gbgqelr gbgqsa gbgrant
syn keyword sicadStatement	gbler gblerb gblerf gbles gblocdic
syn keyword sicadStatement	gbmgmg gbmntdb gbmoddb gbnam gbneu
syn keyword sicadStatement	gbopenp gbpoly gbpos gbpruef gbps
syn keyword sicadStatement	gbqgel gbqgsa gbreldic gbresem gbrevoke
syn keyword sicadStatement	gbsav gbsbef gbsddk gbsicu gbsrt
syn keyword sicadStatement	gbss gbstat gbsysp gbszau gbubp
syn keyword sicadStatement	gbueb gbunmdb gbuseem gbw gbweg
syn keyword sicadStatement	gbwieh gbzt gelp gera hgw
syn keyword sicadStatement	hpg hr0 hra hrar inchk
syn keyword sicadStatement	inf infd inst kbml kbmm
syn keyword sicadStatement	kbmt khboe khbol khdob khe
syn keyword sicadStatement	khetap khfrw khlang khld khmfrp
syn keyword sicadStatement	khmks khpd khpfeil khpl khprofil
syn keyword sicadStatement	khsa khsabs khse khskbz khsna
syn keyword sicadStatement	khsnum khsob khspos khzpe khzpl
syn keyword sicadStatement	kib kldat klleg klsch klsym
syn keyword sicadStatement	klvert kmpg kmtlage kmtp kodef
syn keyword sicadStatement	kodefp kok kokp kolae kom
syn keyword sicadStatement	kopar koparp kopg kosy kp
syn keyword sicadStatement	kr krsek krtclose krtopen ktk
syn keyword sicadStatement	lad lae laesel language lasso
syn keyword sicadStatement	lbdes lcs ldesk ldesks le
syn keyword sicadStatement	leak leattdes leba lebas lebaznp
syn keyword sicadStatement	lebd lebm lebv lebvaus lebvlist
syn keyword sicadStatement	lede ledel ledepo ledepol ledepos
syn keyword sicadStatement	leder ledm lee leeins lees
syn keyword sicadStatement	lege lekr lekrend lekwa lekwas
syn keyword sicadStatement	lel lelh lell lelp lem
syn keyword sicadStatement	lena lend lenm lep lepe
syn keyword sicadStatement	lepee lepko lepl lepmko lepmkop
syn keyword sicadStatement	lepos leqs leqsl leqssp leqsv
syn keyword sicadStatement	leqsvov les lesch lesr less
syn keyword sicadStatement	lestd let letaum letl lev
syn keyword sicadStatement	levtm levtp levtr lew lewm
syn keyword sicadStatement	lexx lfs li lldes lmode
syn keyword sicadStatement	loedk loepkt lop lose lp
syn keyword sicadStatement	lppg lppruef lr ls lsop
syn keyword sicadStatement	lsta lstat ly lyaus lz
syn keyword sicadStatement	lza lzae lzbz lze lznr
syn keyword sicadStatement	lzo lzpos ma ma0 ma1
syn keyword sicadStatement	mad map mapoly mcarp mccfr
syn keyword sicadStatement	mccgr mcclr mccrf mcdf mcdma
syn keyword sicadStatement	mcdr mcdrp mcdve mcebd mcgse
syn keyword sicadStatement	mcinfo mcldrp md me mefd
syn keyword sicadStatement	mefds minmax mipg ml mmdbf
syn keyword sicadStatement	mmdellb mmfsb mminfolb mmlapp mmlbf
syn keyword sicadStatement	mmlistlb mmreadlb mmsetlb mnp mpo
syn keyword sicadStatement	mr mra ms msav msgout
syn keyword sicadStatement	msgsnd msp mspf mtd nasel
syn keyword sicadStatement	ncomp new nlist nlistlt nlistly
syn keyword sicadStatement	nlistnp nlistpo np npa npdes
syn keyword sicadStatement	npe npem npinfa npruef npsat
syn keyword sicadStatement	npss npssa ntz oa oan
syn keyword sicadStatement	odel odf odfx oj oja
syn keyword sicadStatement	ojaddsk ojaef ojaefs ojaen ojak
syn keyword sicadStatement	ojaks ojakt ojakz ojalm ojatkis
syn keyword sicadStatement	ojatt ojbsel ojckon ojde ojdtl
syn keyword sicadStatement	ojeb ojebd ojel ojesb ojesbd
syn keyword sicadStatement	ojex ojezge ojko ojlb ojloe
syn keyword sicadStatement	ojlsb ojmos ojnam ojpda ojpoly
syn keyword sicadStatement	ojprae ojs ojsak ojsort ojstrukt
syn keyword sicadStatement	ojsub ojtdef ojx old op
syn keyword sicadStatement	opa opa1 open opnbsd orth
syn keyword sicadStatement	osanz ot otp otrefp paranf
syn keyword sicadStatement	pas passw pda pg pg0
syn keyword sicadStatement	pgauf pgaufsel pgb pgko pgm
syn keyword sicadStatement	pgr pgvs pily pkpg plot
syn keyword sicadStatement	plotf plotfr pnrver poa pos
syn keyword sicadStatement	posa posaus post protect prs
syn keyword sicadStatement	prsym ps psadd psclose psopen
syn keyword sicadStatement	psparam psprw psres psstat psw
syn keyword sicadStatement	pswr qualif rahmen raster rasterd
syn keyword sicadStatement	rbbackup rbchange rbcmd rbcopy rbcut
syn keyword sicadStatement	rbdbcl rbdbload rbdbop rbdbwin rbdefs
syn keyword sicadStatement	rbedit rbfill rbfload rbfree rbg
syn keyword sicadStatement	rbinfo rbpaste rbrstore rbsnap rbsta
syn keyword sicadStatement	rbvtor rcol re reb refunc
syn keyword sicadStatement	ren renel rk rkpos rohr
syn keyword sicadStatement	rohrpos rpr rr rr0 rra
syn keyword sicadStatement	rrar rs samtosdb sav savx
syn keyword sicadStatement	scol scopy scopye sddk sdwr
syn keyword sicadStatement	se selaus selpos seman semi
syn keyword sicadStatement	sesch setscl sge sid sie
syn keyword sicadStatement	sig sigp skk skks sn
syn keyword sicadStatement	sn21 snpa snpar snpd snpi
syn keyword sicadStatement	snpkor snpl snpm sof sop
syn keyword sicadStatement	split spr sqdadd sqdlad sqdold
syn keyword sicadStatement	sqdsav sql sr sres srt
syn keyword sicadStatement	sset stat stdtxt string strukt
syn keyword sicadStatement	strupru suinfl suinfls supo supo1
syn keyword sicadStatement	sva svr sy sya syly
syn keyword sicadStatement	sys sysout syu syux taa
syn keyword sicadStatement	tabeg tabl tabm tam tanr
syn keyword sicadStatement	tapg tapos tarkd tas tase
syn keyword sicadStatement	tb tbadd tbd tbext tbget
syn keyword sicadStatement	tbint tbout tbput tbsat tbsel
syn keyword sicadStatement	tbstr tcaux tccable tcchkrep tccond
syn keyword sicadStatement	tcinit tcmodel tcnwe tcpairs tcpath
syn keyword sicadStatement	tcscheme tcse tcselc tcstar tcstrman
syn keyword sicadStatement	tcsymbol tctable tctest tcthrcab tctrans
syn keyword sicadStatement	tctst tdb tdbdel tdbget tdblist
syn keyword sicadStatement	tdbput tgmod titel tmoff tmon
syn keyword sicadStatement	tp tpa tps tpta tra
syn keyword sicadStatement	trans transkdo transopt transpro trm
syn keyword sicadStatement	trpg trrkd trs ts tsa
syn keyword sicadStatement	tx txa txchk txcng txju
syn keyword sicadStatement	txl txp txtcmp txz uiinfo
syn keyword sicadStatement	uistatus umdk umdk1 umdka umge
syn keyword sicadStatement	umr verbo verflli verif verly
syn keyword sicadStatement	versinfo vfg wabsym wzmerk zdrhf
syn keyword sicadStatement	zdrhfn zdrhfw zdrhfwn zefp zfl
syn keyword sicadStatement	zflaus zka zlel zlels zortf
syn keyword sicadStatement	zortfn zortfw zortfwn zortp zortpn
syn keyword sicadStatement	zparb zparbn zparf zparfn zparfw
syn keyword sicadStatement	zparfwn zparp zparpn zwinkp zwinkpn

"syn match sicadParameter " [a-z][a-z0-9]*[=:]"me=e-1

"if !exists("did_sicad_syntax_inits")
  let did_sicad_syntax_inits = 1
  hi link sicadLabel1		sicadLabel
  hi link sicadLabel2		sicadLabel
  hi link sicadLabel		PreProc
  hi link sicadConditional	Conditional
  hi link sicadBoolean		Boolean
  hi link sicadNumber		Number
  hi link sicadFloat		Float
  hi link sicadOperator		Operator
  hi link sicadStatement	Statement
  hi link sicadParameter	sicadStatement
  hi link sicadGoto		sicadStatement
  hi link sicadString		String
  hi link sicadComment		Comment
  hi link sicadSpecial		Special
  hi link sicadIdentifier	Type
  "hi link sicadIdentifier	Identifier
  hi link sicadError		Error
  hi link sicadParenError	sicadError
  hi link sicadStringError	sicadError
  hi link sicadCommentError	sicadError
"endif

let b:current_syntax = "sicad"

" vim: ts=8
