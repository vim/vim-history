" Menu Translations:	Hungarian
" Translated By: 	Zoltan Arpadffy <arpadffy@altavista.net>
" Last Change:		2001 Apr 01

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
menutrans &Credits		&Érdemjegy
menutrans &Co&pying		&Copyright
menutrans &Version		&Verzió
menutrans &About		a\ &Programról

" File menu
menutrans &File				&Adattár
menutrans &Open\.\.\.<Tab>:e		&Kinyit\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	&Választ\ és\ nyit\.\.\.<Tab>:sp
menutrans &Close<Tab>:q			&Bezár<Tab>:q
menutrans &Save<Tab>:w			&Ment<Tab>:w
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
menutrans Options\.\.\.			Opciók\.\.\.

" Programming menu
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

" Names for buffer menu.
menutrans &Buffers	&Pufferok
menutrans Refresh	Frissít
menutrans Delete	Töröl
menutrans Alternate	Felcserél
menutrans [No\ File]	[Nincs\ file]

" Window menu
menutrans &Window			&Ablak
menutrans &New<Tab>^Wn			&Új<Tab>^Wn
menutrans S&plit<Tab>^Ws		Választ<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Választ\ a\ #<Tab>^W^^
menutrans &Close<Tab>^Wc		&Bezár<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Bezár\ m&ásikat<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			&Következô<Tab>^Ww
menutrans P&revious<Tab>^WW		&Elôzô<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	Egyenlô\ &magasság<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Egyenlô\ &szélesség<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Alap\ szé&lesség<Tab>^W1_
menutrans Rotate\ &Up<Tab>^WR		Gördít\ &felfelé<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Gördít\ &lefelé<Tab>^Wr
menutrans Select\ &Font\.\.\.		&Fontot\ választ\.\.\.

" The popup menu
menutrans &Undo			&Vissza
menutrans Cu&t			&Kivág
menutrans &Copy			&Másol
menutrans &Paste		&Bemásol
menutrans &Delete		&Töröl
menutrans Select\ Blockwise 	Jelöl\ Blo&kként
menutrans Select\ &Word		Jelöl\ &Szó
menutrans Select\ &Line		Jelöl\ So&r
menutrans Select\ &Block	Jelöl\ B&lok
menutrans Select\ &All		Jelöl\ &Egészet
 
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
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
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New		Új ablak
      tmenu ToolBar.WinSplit	Elválaszt
      tmenu ToolBar.WinMax	Ablakot maximalizal
      tmenu ToolBar.WinMin	Ablakot minimalizal
      tmenu ToolBar.WinClose	Ablakot bezár
    endif
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
menutrans &Syntax 		&Syntaxis
menutrans Set\ 'syntax'\ only	Csak\ 'syntax'
menutrans Set\ 'filetype'\ too	'filetype'\ is
menutrans &Off			&Ki
menutrans &Manual		K&ézikönyv
menutrans A&utomatic		A&utómatokus
menutrans &on\ (this\ file)	&be\ (ez\ a\ file)
menutrans o&ff\ (this\ file)	k&i\ (ez\ a\ file)
menutrans Co&lor\ test		&Színteszt
menutrans &Highlight\ test	Kiemelés\ &teszt
menutrans &Convert\ to\ HTML	&HTML-re\ fordít
