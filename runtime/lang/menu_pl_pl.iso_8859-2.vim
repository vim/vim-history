" Menu Translations:	Polish
" Maintainer:  		Rafal M. Sulejman <rms@poczta.onet.pl>
" Initial Translation: 	Marcin Dalecki <dalecki@cs.net.pl>
" Last Change:      	2001 May 18

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-2

" Help menu
menutrans &Help			Po&moc
menutrans &Overview<Tab><F1>	&Spis\ Tre¶ci<Tab><F1>
menutrans &How-to\ links	&Odno¶niki\ JTZ
menutrans &GUI			&GIU
menutrans &Credits		Po&dziêkowania
menutrans Co&pying		&Kopiowanie
menutrans &Version		&Wersja
menutrans &About		o\ &Aplikacji

" File menu
menutrans &File				&Plik
menutrans &Open\.\.\.<Tab>:e		&Otwórz\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	Otwórz\ z\ &podzia³em\.\.\.<Tab>:sp
menutrans &New<Tab>:enew       &Nowy<Tab>:enew
menutrans &Close<Tab>:q			&Zamknij<Tab>:q
menutrans &Save<Tab>:w			Za&pisz<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav	Zapisz\ &jako\.\.\.<Tab>:sav
menutrans Split\ &Diff\ with\.\.\.	Podziel\ na\ diff-a\ miêdzy\.\.\.
menutrans Split\ Patched\ &By\.\.\.	Podziel\ ³atane\ przez\.\.\.
menutrans &Print			&Drukuj
menutrans Sa&ve-Exit<Tab>:wqa		Zapisz\ i\ w&yjd¼<Tab>:wqa
menutrans E&xit<Tab>:qa			&Wyj¶cie<Tab>:qa

" Edit menu
menutrans &Edit				&Edycja
menutrans &Undo<Tab>u			&Cofnij<Tab>u
menutrans &Redo<Tab>^R			&Ponów<Tab>^R
menutrans Rep&eat<Tab>\.		P&owtórz<Tab>\.
menutrans Cu&t<Tab>"+x			W&ytnij<Tab>"+x
menutrans &Copy<Tab>"+y			&Kopiuj<Tab>"+y
menutrans &Paste<Tab>"+p		&Wklej<Tab>"+p
menutrans Put\ &Before<Tab>[p		Wstaw\ p&rzed<Tab>[p
menutrans Put\ &After<Tab>]p		Wstaw\ p&o<Tab>]p
menutrans &Select\ all<Tab>ggVG		Z&aznacz\ ca³o¶æ<Tab>ggVG
menutrans &Find\.\.\.			&Szukaj\.\.\.
menutrans &Find<Tab>/			&Szukaj<Tab>/
menutrans Find\ and\ Rep&lace\.\.\.	&Zamieniaj\.\.\.
menutrans Find\ and\ Rep&lace<Tab>:%s	&Zamieniaj<Tab>:%s
menutrans Find\ and\ Rep&lace		&Zamieniaj
menutrans Find\ and\ Rep&lace<Tab>:s	&Zamieniaj<Tab>:s
menutrans Options\.\.\.			Opcje\.\.\.
menutrans Settings\ &Window		Ustawienia
menutrans &Global\ Settings		Globalne\ Ustawienia
menutrans F&ile\ Settings		Ustawienia\ dla\ pliku
menutrans C&olor\ Scheme		Zestawy\ kolorów
menutrans &Keymap			Uk³ady\ klawiatury
menutrans None				¯aden
menutrans Toggle\ Pattern\ &Highlight<Tab>:set\ hls!	Prze³±cz\ podkre¶lanie\ &wzorców<Tab>:set\ hls!
menutrans Toggle\ &Ignore-case<Tab>:set\ ic!		Prze³±cz\ &ignorowanie\ wielko¶ci<Tab>:set\ ic!
menutrans Toggle\ &Showmatch<Tab>:set\ sm!		Prze³±cz\ &pokazywanie\ pasuj±cych<Tab>:set\ sm!

menutrans &Context\ lines	Wiersze\ kontekstowe
menutrans &Virtual\ Edit	Wirtualna\ Obróbka

menutrans Never			Nigdy
menutrans Block\ Selection 	Zaznaczanie\ zakresu
menutrans Insert\ mode 		Tryb\ wprowadzania
menutrans Block\ and\ Insert	Zakres\ i\ Wprowadzanie
menutrans Always		Zawsze

menutrans Toggle\ Insert\ &Mode<Tab>:set\ im!	Prze³±cz\ tryb\ wprowadzania<Tab>:set\ im!
menutrans Search\ &Path\.\.\.  	Trop\ poszukiwania\.\.\.
menutrans Ta&g\ Files\.\.\. 	Pilki\ Szyldów\.\.\.

"
" GUI options
menutrans Toggle\ &Toolbar		Prze³±cz\ pasek\ narzêdzi
menutrans Toggle\ &Bottom\ Scrollbar	Prze³±cz\ dolny\ przewijacz
menutrans Toggle\ &Left\ Scrollbar	Prze³±cz\ lewy\ przewijacz
menutrans Toggle\ &Right\ Scrollbar	Prze³±cz\ prawy\ przewijacz

" Programming menu
menutrans &Tools			&Narzêdzia
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Skocz\ do\ taga<Tab>g^]
menutrans Jump\ &back<Tab>^T		Skok\ w\ &ty³<Tab>^T
menutrans Build\ &Tags\ File		&Zrób\ plik\ tagów
menutrans &Make<Tab>:make		M&ake<Tab>:make
menutrans &List\ Errors<Tab>:cl		&Wylicz\ b³êdy<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	W&ylicz\ powiadomienia<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		&Nastêpny\ b³±d<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	&Poprzedni\ b³±d<Tab>:cp
menutrans &Older\ List<Tab>:cold	&Starsza\ lista<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	N&owsza\ lista<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin  Okno\ komu&nikatów<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd     Kody\ szesnastkowe<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r     Zwyk³y\ tekst<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Bufory
menutrans Refresh	Od¶wie¿
menutrans Delete	Skasuj
menutrans Alternate	Zmieñ
menutrans &Next		&Nastêpny
menutrans &Previous	&Poprzedni
menutrans [No\ File]	[Brak\ Pliku]

" Window menu
menutrans &Window			&Widoki
menutrans &New<Tab>^Wn			&Nowy<Tab>^Wn
menutrans S&plit<Tab>^Ws		Po&dziel<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	P&odziel\ na\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Podziel\ pionowo<Tab>^Wv
menutrans &Close<Tab>^Wc		&Zamknij<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Zamknij\ &inne<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			&Nastêpny<Tab>^Ww
menutrans P&revious<Tab>^WW		&Poprzedni<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		&Wyrównaj\ wysoko¶ci<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Z&maksymalizuj\ wysoko¶æ<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Zminim&alizuj\ wysoko¶æ<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Maksymalna\ szeroko¶æ<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Minimalna\ szeroko¶æ<Tab>^W1\|
menutrans Max\ &Width<Tab>^W\|		Zmaksymalizuj\ szeroko¶æ<Tab>^W\|
menutrans Min\ Widt&h<Tab>^W1\|		Zminimalizuj\ szeroko¶æ<Tab>^W1\|
menutrans Move\ &To			&Id¼\ do
menutrans &Top<Tab>^WK			&Góra<Tab>^WK
menutrans &Bottom<Tab>^WJ		&Dó³<Tab>^WJ
menutrans &Left\ side<Tab>^WH		&Lewa\ strona<Tab>^WH
menutrans &Right\ side<Tab>^WL		&Prawa\ strona<Tab>^WL
menutrans Rotate\ &Up<Tab>^WR		Obróæ\ w\ &górê<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Obróæ\ w\ &dó³<Tab>^Wr
menutrans Split\ &Vertically<Tab>^Wv	&Podziel\ w\ poziomie<Tab>^Wv
menutrans Select\ Fo&nt\.\.\.		Wybierz\ &czcionkê\.\.\.

" The popup menu
menutrans &Undo			&Cofnij
menutrans Cu&t			W&ytnij
menutrans &Copy			&Kopiuj
menutrans &Paste		&Wklej
menutrans &Delete		&Skasuj
menutrans Select\ Blockwise 	Zaznacz\ &Blok
menutrans Select\ &Word		Zaznacz\ &S³owo
menutrans Select\ &Line		Zaznacz\ W&iersz
menutrans Select\ &Block	Zaznacz\ &Blok
menutrans Select\ &All		Zaznacz\ C&a³o¶æ
 
" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Otwórz plik
    tmenu ToolBar.Save		Zapisz bie¿±cy plik
    tmenu ToolBar.SaveAll		Zapisz wszystkie pliki
    tmenu ToolBar.Print		Drukuj
    tmenu ToolBar.Undo		Cofnij
    tmenu ToolBar.Redo		Ponów
    tmenu ToolBar.Cut		Wytnij
    tmenu ToolBar.Copy		Skopiuj
    tmenu ToolBar.Paste		Wklej
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
    tmenu ToolBar.SaveSesn	Zachowaj bie¿±c± sesjê
    tmenu ToolBar.RunScript	Uruchom script Vim-a
    tmenu ToolBar.Make		Zrób bie¿±cy projekt
    tmenu ToolBar.Shell		Otwórz otoczkê
    tmenu ToolBar.RunCtags	Zrób tag-i w bie¿±cym katalogu
    tmenu ToolBar.TagJump		Skok do tag-a pod kursorem
    tmenu ToolBar.Help		Pomoc Vima
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
menutrans o&n\ (this\ file)	w&³±cz\ (dla\ danego\ pliku)
menutrans o&ff\ (this\ file)	w&y³±cz\ (dla\ danego\ pliku)
menutrans Co&lor\ test		Test\ &kolorów
menutrans &Highlight\ test	&Test\ pod¶wietlania
menutrans &Convert\ to\ HTML	Przetwórz\ na\ &HTML

" dialog texts
let menutrans_no_file = "[Brak pliku]"
let menutrans_help_dialog = "Wprowad¼ komendê lub s³owo, aby otrzymaæ pomoc o:\n\nPrzedpis i_ oznacza komendê trybu Wprowadzania (np. i_CTRL-X)\nPrzedpis c_ oznacza komendê edycji wiersza komend (np. c_<Del>)\nPrzedpis ' oznacza nazwê opcji (np. 'shiftwidth')"
let g:menutrans_path_dialog = "Proszê wprowadziæ trop poszukiwania plików.\nProszê rozdzielaæ nazwy katalogów przecinkiem."
let g:menutrans_tags_dialog = "Proszê podaæ nazwy plików szyldów.\nProszê rozdzielaæ nazwy przecinkiem."
let g:menutrans_textwidth_dialog = "Proszê wprowadziæ now± szeroko¶æ tekstu (0 wy³±cza przewijanie): "
let g:menutrans_fileformat_dialog = "Proszê wybraæ format w którym ten pilk ma byæ zapisany"
