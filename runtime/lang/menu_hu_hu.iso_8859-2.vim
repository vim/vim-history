" Menu Translations:	Hungarian (Magyar)
" Translated By:	Zoltan Arpadffy <arpadffy@altavista.net>
" Last Change:		2001 Sep 13

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-2

" Help menu
menutrans &Help			&Segély
menutrans &Overview<Tab><F1>	&Áttekintés<Tab><F1>
menutrans &How-to\ links	&HOGYAN\ linkek
menutrans &User\ Manual		&Kézikönyv
menutrans &Credits		&Érdemjegy
menutrans &Co&pying		&Copyright
menutrans &Find\.\.\.		K&eres\.\.\.
menutrans &Version		&Verzió
menutrans &About		&Programról

" File menu
menutrans &File				&Adattár
menutrans &Open\.\.\.<Tab>:e		&Kinyit\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	&Választ\ és\ nyit\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Új<Tab>:enew
menutrans &Close<Tab>:close		&Bezár<Tab>:close
menutrans &Save<Tab>:w			&Ment<Tab>:w
menutrans Split\ &Diff\ with\.\.\.	Oszt\ &diff\ -el\.\.\.
menutrans Split\ Patched\ &By\.\.\.	Oszt\ &patch\ -el\.\.\.
menutrans Save\ &As\.\.\.<Tab>:w	Ment\ más n&éven\.\.\.<Tab>:w
menutrans &Print			&Nyomtat
menutrans Sa&ve-Exit<Tab>:wqa		Ment\ és\ k&ilép\<Tab>:wqa
menutrans E&xit<Tab>:qa			Kilé&p<Tab>:qa

" Edit menu
menutrans &Edit				&Szerkeszt
menutrans &Undo<Tab>u			&Vissza<Tab>u
menutrans &Redo<Tab>^R			&Elôre<Tab>^R
menutrans Repea&t<Tab>\.		&Újra<Tab>\.
menutrans Cu&t<Tab>"+x			&Vág<Tab>"+x
menutrans &Copy<Tab>"+y			&Másol<Tab>"+y
menutrans &Paste<Tab>"+p		&Bemásol<Tab>"+p
menutrans Put\ &Before<Tab>[p		Betûz\ e&lé<Tab>[p
menutrans Put\ &After<Tab>]p		Betûz\ m&ögé<Tab>]p
menutrans &Select\ all<Tab>ggvG		Válaszd\ &mind<Tab>ggvG
menutrans &Find\.\.\.			&Keres\.\.\.
menutrans Find\ and\ R&eplace\.\.\.	Keres\ és\ &cserél\.\.\.
menutrans Find\ and\ Rep&lace<Tab>:%s	Keres\ és\ cserél<Tab>:%s
menutrans Find\ and\ Rep&lace		Keres\ és\ cserél
menutrans Find\ and\ Rep&lace<Tab>:s	Keres\ és\ cserél<Tab>:s
menutrans Settings\ &Window		Beállítás\ &Ablak
menutrans &Global\ Settings		Beállítás\ &Globális
menutrans F&ile\ Settings		Beállítás\ A&dattár
menutrans C&olor\ Scheme		Beállítás\ &Színek
menutrans &Keymap			Beállítás\ &Billentyüzet

" Edit.Global Settings
menutrans Toggle\ Pattern\ &Highlight<Tab>:set\ hls!	Minta\ kiemelés BE<Tab>:set\ hls!
menutrans Toggle\ &Ignore-case<Tab>:set\ ic!	Kis/nagybetüt elhanyagol<Tab>:set\ ic!
menutrans Toggle\ &Showmatch<Tab>:set\ sm!	Találatjalzö<Tab>:set\ sm!
menutrans &Context\ lines		Tartalom\ mutató
menutrans &Virtual\ Edit		Virtualis\ szerkesztés
menutrans Never				Soha
menutrans Block\ Selection		Blok\ kijelöl
menutrans Insert\ mode			Insert\ mode
menutrans Block\ and\ Insert		Blok\ és\ insert\ mode
menutrans Always			Mindég
menutrans Toggle\ Insert\ &Mode<Tab>:set\ im!	Insert\ mode\ BE<Tab>:set\ im!
menutrans Search\ &Path\.\.\.		Keres\.\.\.
menutrans Ta&g\ Files\.\.\.		Tag\ kezelés\.\.\.
menutrans Toggle\ &Toolbar		Szerszám\ bár\ BE
menutrans Toggle\ &Bottom\ Scrollbar	Csúszobárok\ BE
menutrans Toggle\ &Left\ Scrollbar	Bal\ csuszóbár\ BE
menutrans Toggle\ &Right\ Scrollbar	Jobb\ csuszóbár\ BE
menutrans None				Üres

" Edit.File Settings
menutrans Toggle\ Line\ &Numbering<Tab>:set\ nu!	Sorszamozás<Tab>:set\ nu!
menutrans Toggle\ &List\ Mode<Tab>:set\ list!		List\ mode<Tab>:set\ list!
menutrans Toggle\ Line\ &Wrap<Tab>:set\ wrap!		Sortörés<Tab>:set\ wrap!
menutrans Toggle\ W&rap\ at\ word<Tab>:set\ lbr!	Sortörés\ egy\ szóban<tab>:set\ lbr!
menutrans Toggle\ &expand-tab<Tab>:set\ et!		Tab\ kifejtés<Tab>:set\ et!
menutrans Toggle\ &auto-indent<Tab>:set\ ai!		Auto\ behúzás<Tab>:set\ ai!
menutrans Toggle\ &C-indenting<Tab>:set\ cin!		C-behúzás<Tab>:set\ cin!
menutrans Te&xt\ Width\.\.\.				Szöveg\ szélesseg\.\.\.
menutrans &File\ Format\.\.\.				File\ formatum\.\.\.

" Tools menu
menutrans &Tools			&Szerszámok
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Ugorj\ a\ taghoz<Tab>g^]
menutrans Jump\ &back<Tab>^T		Ugorj\ &vissza<Tab>^T
menutrans Build\ &Tags\ File		&Készíts\ tag\ file-t
menutrans &Make<Tab>:make		&Épít<Tab>:make
menutrans &List\ Errors<Tab>:cl		&Hibák\ lisája<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	&Üzenetek\ listája<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		Következô\ &hiba<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	&Elôzô\ hiba<Tab>:cp
menutrans &Older\ List<Tab>:cold	&Régi\ lista<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	&Újabb\ lista<Tab>:cnew
menutrans Error\ &Window		Hiba\ ablak
menutrans &Update<Tab>:cwin		&Frissít<Tab>:cwin
menutrans &Open<Tab>:copen		&Megnyit<Tab>:copen
menutrans &Close<Tab>:cclose		&Bezár<Tab>:cclose
menutrans &Convert\ to\ HEX<Tab>:%!xxd	Fordít\ HEX-re<Tab>:%!xxd
menutrans Conve&rt\ back<Tab>:%!xxd\ -r	Fordít\ HEX-böl<Tab>:%!xxd\ -r
menutrans &Set\ Compiler		Fordító\ beállítás

" Tools.Folding
menutrans &Enable/Disable\ folds<Tab>zi	BE/KI\ hajtogat<Tab>zi
menutrans &View\ Cursor\ Line<Tab>zv	Cursor\ sor<Tab>zv
menutrans Vie&w\ Cursor\ Line\ only<Tab>zMzx	Csak\ cursor\ sor<Tab>zMzx
menutrans C&lose\ more\ folds<Tab>zm	Becsuk\ behajtást<Tab>zm
menutrans &Close\ all\ folds<Tab>zM	Becsuk\ mind<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr	Kinyit\behajtást<Tab>zr
menutrans &Open\ all\ folds<Tab>zR	Kinyit\ mind<Tab>zR
menutrans Fold\ Met&hod			Behajt\ metodus
menutrans M&anual			Kézi
menutrans I&ndent			Behúzás
menutrans E&xpression			Kifejezés
menutrans S&yntax			Szintaxis
menutrans &Diff				diff-külömbség
menutrans Ma&rker			Jelölés
menutrans Create\ &Fold<Tab>zf		Új\ behajás<Tab>zf
menutrans &Delete\ Fold<Tab>zd		Töröl\ behajtást<Tab>zd
menutrans Delete\ &All\ Folds<Tab>zD	Töröl\ mind<Tab>zD
menutrans Fold\ column\ &width		Behajt\ sort
 
" Tools.Diff
menutrans &Update		Frissít
menutrans &Get\ Block		Block\ BE
menutrans &Put\ Block		Block\ KI



" Names for buffer menu.
menutrans &Buffers	&Pufferok
menutrans Refresh	F&rissít
menutrans Delete	&Töröl
menutrans Alternate	&Felcserél
menutrans &Next			&Következö
menutrans &Previous		&Elözö

" Window menu
menutrans &Window			&Ablak
menutrans &New<Tab>^Wn			&Új<Tab>^Wn
menutrans S&plit<Tab>^Ws		Választ<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Választ\ a\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Választ\ &Függölegesen<Tab>^Wv
menutrans Split\ File\ E&xplorer	Választ\ filet
menutrans &Close<Tab>^Wc		&Bezár<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Bezár\ m&ásikat<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			&Következô<Tab>^Ww
menutrans P&revious<Tab>^WW		&Elôzô<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		Max\ magasság<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Min\ magasság<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Alap\ szé&lesség<Tab>^W1_
menutrans Max\ &Width<Tab>^W\|		Max\ szélesség<Tab>^W\|
menutrans Min\ Widt&h<Tab>^W1\|		Min\ szélesség<Tab>^W1\|
menutrans Move\ &To			Elmozdít
menutrans &Top<Tab>^WK			Fel<Tab>^WK
menutrans &Bottom<Tab>^WJ		Le<Tab>^WJ
menutrans &Left\ side<Tab>^WH		Balra<Tab>^WH
menutrans &Right\ side<Tab>^WL		Jobbra<Tab>^WL
menutrans Rotate\ &Up<Tab>^WR		Gördít\ &felfelé<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Gördít\ &lefelé<Tab>^Wr
menutrans Select\ Fo&nt\.\.\.		&Fontot\ választ\.\.\.

" The popup menu
menutrans &Undo			&Vissza
menutrans Cu&t			&Kivág
menutrans &Copy			&Másol
menutrans &Paste		&Bemásol
menutrans &Delete		&Töröl
menutrans Select\ Blockwise	Jelöl\ Blo&kként
menutrans Select\ &Word		Jelöl\ &Szó
menutrans Select\ &Line		Jelöl\ So&r
menutrans Select\ &Block	Jelöl\ B&lok
menutrans Select\ &All		Jelöl\ &Egészet

" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Megnyit
    tmenu ToolBar.Save		Ment
    tmenu ToolBar.SaveAll	Mindet ment
    tmenu ToolBar.Print		Nyomtat
    tmenu ToolBar.Undo		Vissza
    tmenu ToolBar.Redo		Élôre
    tmenu ToolBar.Cut		Kivág
    tmenu ToolBar.Copy		Másol
    tmenu ToolBar.Paste		Bemásol
    tmenu ToolBar.Find		Keres...
    tmenu ToolBar.FindNext	Keres következôt
    tmenu ToolBar.FindPrev	Keres elôzôt
    tmenu ToolBar.Replace	Keres/cserél...
    tmenu ToolBar.LoadSesn	Sessiót beolvas
    tmenu ToolBar.SaveSesn	Sessiót ment
    tmenu ToolBar.RunScript	Vim sriptet indít
    tmenu ToolBar.Make		Projectet épít
    tmenu ToolBar.Shell		OS-t indít
    tmenu ToolBar.RunCtags	Tagokat épít
    tmenu ToolBar.TagJump	Ugorj a kurzor alatti tagra
    tmenu ToolBar.Help		Vim súgó
    tmenu ToolBar.FindHelp	Vim segély
  endfun
endif

" Syntax menu
menutrans &Syntax		&Szintaxis
menutrans Set\ 'syntax'\ only	Csak\ 'syntax'
menutrans Set\ 'filetype'\ too	'filetype'\ is
menutrans &Off			&Ki
menutrans &Manual		K&ézikönyv
menutrans A&utomatic		A&utómatikus
menutrans &on\ (this\ file)	&be\ (ezt\ a\ file-t)
menutrans o&ff\ (this\ file)	k&i\ (ezt\ a\ file-t)
menutrans Co&lor\ test		&Színteszt
menutrans &Highlight\ test	Kiemelés\ &teszt
menutrans &Convert\ to\ HTML	&HTML-re\ fordít

" dialog texts
let menutrans_no_file = "[Nincs file]"
let menutrans_help_dialog = "Kérem irja be a kívant szót vagy parancsot:\n\nÍrjon elé i_ -t az Input mode parancsokhoz (pl.: i_CTRL-X)\nÍrjon elé c_ -t a sorszerkeszö parancsokhoz (pl.: c_<Del>)\nÍrjon elé ' jelet a beallítasi parancsokhoz (pl.: 'shiftwidth')"
let g:menutrans_path_dialog = "Írja be a keresett file lehetséges elérési utait, vesszövel elválasztva "
let g:menutrans_tags_dialog = "Írja be a tag file lehetséges elérési utait, vesszövel elválasztva"
let g:menutrans_textwidth_dialog = "Írja be a szöveg szélességet (0 = formázas kikapcsolva)"
let g:menutrans_fileformat_dialog = "Vállasza ki a file formátumot"
