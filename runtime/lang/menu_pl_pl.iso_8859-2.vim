" Menu Translations:	Polish
" Translated By: 	Marcin Dalecki <dalecki@cs.net.pl>
" Last Change:		2001 Jan 15

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

" Help menu
menutrans &Help			Po&moc
menutrans &Overview<Tab><F1>	&Spis\ Tre¶ci<Tab><F1>
menutrans &How-to\ links	&Odno¶niki\ JTZ
menutrans &Credits		Po&dziêkowania
menutrans &Co&pying		&Kopiowanie
menutrans &Version		&Wersja
menutrans &About		o\ &Aplikacji

" File menu
menutrans &File				&Plik
menutrans &Open\.\.\.<Tab>:e		&Otwórz\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	Otwórz\ z\ &podzia³em\.\.\.<Tab>:sp
menutrans &Close<Tab>:q			&Zamknij<Tab>:q
menutrans &Save<Tab>:w			Za&pisz<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	Zapisz\ &jako\.\.\.<Tab>:w
menutrans &Print			&Drukuj
menutrans Sa&ve-Exit<Tab>:wqa		Zapisz\ i\ w&yjd¼<Tab>:wqa
menutrans E&xit<Tab>:qa			&Wyj¶cie<Tab>:qa

" Edit menu
menutrans &Edit				&Edycja
menutrans &Undo<Tab>u			&Cofnij<Tab>u
menutrans &Redo<Tab>^R			&Ponownij<Tab>^R
menutrans Repea&t<Tab>\.		P&owtórz<Tab>\.
menutrans Cu&t<Tab>"*x			W&ytnij<Tab>"*x
menutrans &Copy<Tab>"*y			&Kopiuj<Tab>"*y
menutrans &Paste<Tab>"*p		&Wklej<Tab>"*p
menutrans Put\ &Before<Tab>[p		W³ó¿\ p&rzed<Tab>[p
menutrans Put\ &After<Tab>]p		W³ó¿\ p&o<Tab>]p
menutrans &Select\ all<Tab>ggvG		&Naznacz\ Ca³o¶æ<Tab>ggvG
menutrans &Find\.\.\.			&Szukaj\.\.\.
menutrans Find\ and\ R&eplace\.\.\.	&Zamieniaj\.\.\.
menutrans Options\.\.\.			Opcje\.\.\.

" Programming menu
menutrans &Tools			&Narzêdzia
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Skocz\ do\ taga<Tab>g^]
menutrans Jump\ &back<Tab>^T		Skok\ w\ &ty³<Tab>^T
menutrans Build\ &Tags\ File		&Zrób\ plik\ tagów
menutrans &Make<Tab>:make		P&rzeróbka<Tab>:make
menutrans &List\ Errors<Tab>:cl		&Wylicz\ b³êdy<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	W&ylicz\ powiadomienia<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		&Nastêpny\ b³±d<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	&Poprzedni\ b³±d<Tab>:cp
menutrans &Older\ List<Tab>:cold	&Starsza\ lista<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	N&owsza\ lista<Tab>:cnew

" Names for buffer menu.
menutrans &Buffers	&Bufory
menutrans Refresh	Od¶wierz
menutrans Delete	Skasuj
menutrans Alternate	Zmieñ
menutrans [No\ File]	[Brak\ Pliku]

" Window menu
menutrans &Window			&Widoki
menutrans &New<Tab>^Wn			&Nowy<Tab>^Wn
menutrans S&plit<Tab>^Ws		Po&dziel<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	P&odziel\ na\ #<Tab>^W^^
menutrans &Close<Tab>^Wc		&Zamknij<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Zamknij\ &inne<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			&Nastêpny<Tab>^Ww
menutrans P&revious<Tab>^WW		&Poprzedni<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	&Wyrównaj\ wysoko¶ci<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Z&maksymalizuj\ wysoko¶æ<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Zminim&alizuj\ wysoko¶æ<Tab>^W1_
menutrans Rotate\ &Up<Tab>^WR		Obróæ\ w\ &górê<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Obróæ\ w\ &dó³<Tab>^Wr
menutrans Select\ &Font\.\.\.		Wybierz\ &czcionkê\.\.\.

" The popup menu
menutrans &Undo			&Cofnij
menutrans Cu&t			W&ytnij
menutrans &Copy			&Kopiuj
menutrans &Paste		&Wklej
menutrans &Delete		&Skasuj
menutrans Select\ Blockwise 	Naznacz\ &Blok
menutrans Select\ &Word		Naznacz\ &S³owo
menutrans Select\ &Line		Naznacz\ W&iersz
menutrans Select\ &Block	Naznacz\ &Blok
menutrans Select\ &All		Naznacz\ C&a³o¶æ
 
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Otwórz plik
    tmenu ToolBar.Save		Zapisz bie¿±cy plik
    tmenu ToolBar.SaveAll		Zapisz wszystkie pliki
    tmenu ToolBar.Print		Drukuj
    tmenu ToolBar.Undo		Cofnij
    tmenu ToolBar.Redo		Ponownij
    tmenu ToolBar.Cut		Wytnij na planszê
    tmenu ToolBar.Copy		Skopiuj do planszy
    tmenu ToolBar.Paste		Wklej z planszy
    tmenu ToolBar.Find		Szukaj...
    tmenu ToolBar.FindNext	Szukaj Nastêpnego
    tmenu ToolBar.FindPrev	Szukaj Poprzedniego
    tmenu ToolBar.Replace		Szukaj i Zamieniaj...
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New		Nowy Widok
      tmenu ToolBar.WinSplit	Podziel Widok
      tmenu ToolBar.WinMax		Zmaksymalizuj Widok
      tmenu ToolBar.WinMin		Zminimalizuj Widok
      tmenu ToolBar.WinClose	Zamknij Widok
    endif
    tmenu ToolBar.LoadSesn	Za³aduj sesjê
    tmenu ToolBar.SaveSesn	Zachowaj bierz±c± sesjê
    tmenu ToolBar.RunScript	Odpal script Vim-a
    tmenu ToolBar.Make		Zrób bie¿±cy projekt
    tmenu ToolBar.Shell		Otwó¿ otoczkê
    tmenu ToolBar.RunCtags	Zrób tag-i w bie¿cym poddrzewku katalogów
    tmenu ToolBar.TagJump		Skok do tag-a pod kursorem
    tmenu ToolBar.Help		Vim Pomoc
    tmenu ToolBar.FindHelp	Przeszukuj Pomoc Vim-a
  endfun
endif

" Syntax menu
menutrans &Syntax &Sk³adnia
menutrans Set\ 'syntax'\ only	Ustaw\ tylko\ 'syntax'
menutrans Set\ 'filetype'\ too	Ustaw\ równie¿\ 'filetype'
menutrans &Off			&Wy³±cz
menutrans &Manual		&Rêcznie
menutrans A&utomatic		A&utomatyczne
menutrans &on\ (this\ file)	w&³±cz\ (dla\ danego\ pliku)
menutrans o&ff\ (this\ file)	w&y³±cz\ (dla\ danego\ pliku)
menutrans Co&lor\ test		Test\ &kolorów
menutrans &Highlight\ test	&Test\ pod¶wietlania
menutrans &Convert\ to\ HTML	Przetwórz\ na\ &HTML
