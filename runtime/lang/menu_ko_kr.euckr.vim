" Menu Translations:	Korean
" Maintainer:		Sung-Hyun Nam <namsh@kldp.org>
" Last Change:		2001 apr 26

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding euc-kr

" Help menu
menutrans &Help			도움말(&H)
menutrans &Overview<Tab><F1>	개관(&O)<Tab><F1>
menutrans &How-to\ links	하우투\ 색인(&H)
menutrans &GUI			구이(&G)
menutrans &Credits		고마운\ 분들(&C)
menutrans Co&pying		저작권(&p)
menutrans &Find\.\.\.		찾기(&F)\.\.\.
menutrans &Version		버전(&V)
menutrans &About		이\ 프로그램은(&A)

" File menu
menutrans &File				파일(&F)
menutrans &Open\.\.\.<Tab>:e		열기(&O)\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	분할해서\ 열기(&l)\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		새로운(&N)<Tab>:enew
menutrans &Close<Tab>:q			닫기(&C)<Tab>:q
menutrans &Save<Tab>:w			저장(&S)<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	다른\ 이름으로\ 저장(&A)\.\.\.<Tab>:w
menutrans &Print			인쇄(&P)
menutrans Sa&ve-Exit<Tab>:wqa		저장하고\ 종료(&v)<Tab>:wqa
menutrans E&xit<Tab>:qa			종료(&x)<Tab>:qa

" Edit menu
menutrans &Edit				편집(&E)
menutrans &Undo<Tab>u			취소(&U)<Tab>u
menutrans &Redo<Tab>^R			재실행(&R)<Tab>^R
menutrans Rep&eat<Tab>\.		반복(&e)<Tab>\.
menutrans Cu&t<Tab>"+x			자르기(&t)<Tab>"+x
menutrans &Copy<Tab>"+y			복사(&C)<Tab>"+y
menutrans &Paste<Tab>"+p		붙이기(&P)<Tab>"+p
menutrans Put\ &Before<Tab>[p		앞에\ 붙이기(&B)<Tab>[p
menutrans Put\ &After<Tab>]p		뒤에\ 붙이기(&A)<Tab>]p
menutrans &Select\ all<Tab>ggVG		모두\ 선택(&S)<Tab>ggVG
menutrans &Find\.\.\.			찾기(&F)\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.	찾아\ 바꾸기(&l)\.\.\.
menutrans Options\.\.\.			옵션들\.\.\.

" Programming menu
menutrans &Tools			도구(&T)
menutrans &Jump\ to\ this\ tag<Tab>g^]	이\ 태그로\ 점프(&J)<Tab>g^]
menutrans Jump\ &back<Tab>^T		뒤로\ 점프(&b)<Tab>^T
menutrans Build\ &Tags\ File		태그\ 파일\ 생성(&T)
menutrans &Make<Tab>:make		Make(&M)<Tab>:make
menutrans &List\ Errors<Tab>:cl		에러\ 목록\ 보기(&L)<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	메시지\ 목록\ 보기(&i)<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		다음\ 에러(&N)<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	이전\ 에러(&P)<Tab>:cp
menutrans &Older\ List<Tab>:cold	오래된\ 목록(&O)<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	새로운\ 목록(&e)<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	에러\ 창(&W)<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	십육진으로\ 변환<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	원래대로\ 변환<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	버퍼(&B)
menutrans Refresh	다시그리기
menutrans Delete	삭제
menutrans Alternate	교체
menutrans [No\ File]	[파일\ 없음]

" Window menu
menutrans &Window			창(&W)
menutrans &New<Tab>^Wn			새\ 창(&N)<Tab>^Wn
menutrans S&plit<Tab>^Ws		분할(&p)<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	#으로\ 분할(&l)<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	수직\ 분할(&V)<Tab>^Wv
menutrans &Close<Tab>^Wc		닫기(&C)<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	다른\ 창\ 닫기(&O)<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			다음(&x)<Tab>^Ww
menutrans P&revious<Tab>^WW		이전(&r)<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		같은\ 높이로(&E)<Tab>^W=
menutrans &Max\ Height<Tab>^W_		최대\ 높이로(&M)<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		최소\ 높이로(&i)<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		최대\ 넓이로<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		최소\ 넓이로<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		위로\ 회전(&U)<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		아래로\ 회전(&D)<Tab>^Wr
menutrans Select\ &Font\.\.\.		글꼴\ 선택(&F)\.\.\.

" The popup menu
menutrans &Undo			취소(&U)
menutrans Cu&t			자르기(&t)
menutrans &Copy			복사(&C)
menutrans &Paste		붙이기(&P)
menutrans &Delete		삭제(&D)
menutrans Select\ Blockwise 	사각형\ 선택
menutrans Select\ &Word		단어\ 선택(&W)
menutrans Select\ &Line		라인\ 선택(&L)
menutrans Select\ &Block	구획\ 선택(&B)
menutrans Select\ &All		모두\ 선택(&A)
 
" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		열기
    tmenu ToolBar.Save		저장
    tmenu ToolBar.SaveAll	모두 저장
    tmenu ToolBar.Print		인쇄
    tmenu ToolBar.Undo		취소
    tmenu ToolBar.Redo		재실행
    tmenu ToolBar.Cut		자르기
    tmenu ToolBar.Copy		복사
    tmenu ToolBar.Paste		붙이기
    tmenu ToolBar.Find		찾기...
    tmenu ToolBar.FindNext	다음 찾기
    tmenu ToolBar.FindPrev	이전 찾기
    tmenu ToolBar.Replace	바꾸기
    tmenu ToolBar.LoadSesn	세션 읽어오기
    tmenu ToolBar.SaveSesn	세션 저장
    tmenu ToolBar.RunScript	스크립트 실행
    tmenu ToolBar.Make		Make
    tmenu ToolBar.Shell		쉘
    tmenu ToolBar.RunCtags	태그 생성
    tmenu ToolBar.TagJump	태그 점프
    tmenu ToolBar.Help		도움말
    tmenu ToolBar.FindHelp	도움말 찾기...
  endfun
endif

" Syntax menu
menutrans &Syntax		문법(&S)
menutrans Set\ 'syntax'\ only	'syntax'만\ 설정
menutrans Set\ 'filetype'\ too	'filetype'도\ 설정
menutrans &Off			끄기(&O)
menutrans &Manual		수동(&M)
menutrans A&utomatic		자동(&u)
menutrans o&n\ (this\ file)	켜기\ (이\ 파일)(&n)
menutrans o&ff\ (this\ file)	끄기\ (이\ 파일)(&f)
menutrans Co&lor\ test		색\ 시험(&l)
menutrans &Highlight\ test	Highlight\ 시험(&H)
menutrans &Convert\ to\ HTML	HTML로\ 변환(&C)
