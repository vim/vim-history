" Menu Translations:	Nederlands
" Maintainer:		Bram Moolenaar
" Last Change:	2001 Jan 15

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

" Help menu
menutrans &Help			&Help
menutrans &Overview<Tab><F1>	&Overzicht<Tab><F1>
menutrans &How-to\ links	&How-to\ Index
"menutrans &GUI			&GUI
menutrans &Credits		&Met\ dank\ aan
menutrans Co&pying		&Copyright
menutrans &Find\.\.\.		&Zoeken\.\.\.
menutrans &Version		&Versie
menutrans &About		&Introductie\ scherm

" File menu
menutrans &File				&Bestand
menutrans &Open\.\.\.<Tab>:e		&Openen\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	In\ nieuw\ &Venster\ openen\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nieuw<Tab>:enew
menutrans &Close<Tab>:q			&Sluiten<Tab>:q
menutrans &Save<Tab>:w			&Bewaren<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	Bewaren\ als\.\.\.<Tab>:w
menutrans &Print			Af&drukken
menutrans Sa&ve-Exit<Tab>:wqa		Bewaren\ en\ Afluiten<Tab>:wqa
menutrans E&xit<Tab>:qa			&Afsluiten<Tab>:qa

" Edit menu
menutrans &Edit				Be&werken
menutrans &Undo<Tab>u			Terug<Tab>u
menutrans &Redo<Tab>^R			Voo&ruit<Tab>^R
menutrans Repea&t<Tab>\.		&Herhalen<Tab>\.
menutrans Cu&t<Tab>"*x			&Knippen<Tab>"*x
menutrans &Copy<Tab>"*y			&Kopiëeren<Tab>"*y
menutrans &Paste<Tab>"*p		Plakken<Tab>"*p
menutrans Put\ &Before<Tab>[p		Ervoor\ invoegen<Tab>[p
menutrans Put\ &After<Tab>]p		Erachter\ invoegen<Tab>]p
menutrans &Select\ all<Tab>ggVG		Alles\ &Markeren<Tab>ggVG
menutrans &Find\.\.\.			&Zoeken\.\.\.
menutrans Find\ and\ R&eplace\.\.\.	Zoeken\ en\ &Vervangen\.\.\.
menutrans Options\.\.\.			Opties\.\.\.

" Programming menu
menutrans &Tools			&Gereedschap
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Spring\ naar\ Tag<Tab>g^]
menutrans Jump\ &back<Tab>^T		Spring\ &Terug<Tab>^T
menutrans Build\ &Tags\ File		Genereer\ &Tags\ Bestand
menutrans &Make<Tab>:make		&Make\ uitvoeren<Tab>:make
menutrans &List\ Errors<Tab>:cl		&Fouten\ lijst<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	&Berichten\ lijst<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		Volgende\ Fout<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	Vorige\ Fout<Tab>:cp
menutrans &Older\ List<Tab>:cold	&Oudere\ Lijst<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	&Nieuwere\ Lijst<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	Fouten\ Venster<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	Converteer\ naar\ HEX<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	Converteer\ terug<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Buffer
menutrans Refresh	Verversen
menutrans Delete	Wissen
menutrans Alternate	Vorige
menutrans [No\ File]	[Keine\ Datei]

" Window menu
menutrans &Window			&Venster
menutrans &New<Tab>^Wn			&Nieuw<Tab>^Wn
menutrans S&plit<Tab>^Ws		Splitsen<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Splits\ naar\ #<Tab>^W^^
menutrans S&plit\ Vertically<Tab>^Wv	S&plits\ Vertikaal<Tab>^Wv
menutrans &Close<Tab>^Wc		&Sluiten<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	&Sluit\ Andere<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			Volgende<Tab>^Ww
menutrans P&revious<Tab>^WW		&Vorige<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	&Gelijke\ hoogte<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&Maximale\ hoogte<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Mi&nimale\ hoogte<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Maximale\ breedte<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Minimale\ breedte<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		Roteren\ naar\ &boven<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Roieren\ naar\ &onder<Tab>^Wr
menutrans Select\ &Font\.\.\.		Selecteer\ font\.\.\.

" The popup menu
menutrans &Undo			&Terug
menutrans Cu&t			Knip
menutrans &Copy			&Kopiëer
menutrans &Paste		&Plak
menutrans &Delete		&Wissen
menutrans Select\ Blockwise 	Selecteer\ per\ Rechthoek
menutrans Select\ &Word		Selecteer\ een\ &Woord
menutrans Select\ &Line		Selecteer\ een\ &Regel
menutrans Select\ &Block	Selecteer\ een\ Recht&hoek
menutrans Select\ &All		Selecteer\ &Alles
 
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Bestand openen
    tmenu ToolBar.Save		Bestand opslaan
    tmenu ToolBar.SaveAll	Alle bestanden opslaan
    tmenu ToolBar.Print		afdrukken
    tmenu ToolBar.Undo		terug
    tmenu ToolBar.Redo		vooruit
    tmenu ToolBar.Cut		knippen
    tmenu ToolBar.Copy		Kopiëren
    tmenu ToolBar.Paste		Plakken
    tmenu ToolBar.Find		Zoeken...
    tmenu ToolBar.FindNext	Zoek volgende
    tmenu ToolBar.FindPrev	Zoek vorige
    tmenu ToolBar.Replace	Zoek en vervang...
    tmenu ToolBar.LoadSesn	Sessie Laden
    tmenu ToolBar.SaveSesn	Sessie opslaan
    tmenu ToolBar.RunScript	Vim script uitvoeren
    tmenu ToolBar.Make		Make uitvoeren
    tmenu ToolBar.Shell		Shell starten
    tmenu ToolBar.RunCtags	Tags bestand genereren
    tmenu ToolBar.TagJump	Spring naar tag
    tmenu ToolBar.Help		Help!
    tmenu ToolBar.FindHelp	Help vinden...
  endfun
endif

" Syntax menu
menutrans &Syntax		&Syntax
menutrans Set\ 'syntax'\ only	Alleen\ 'syntax'\ wijzigen
menutrans Set\ 'filetype'\ too	Ook\ 'filetype'\ wijzigen
menutrans &Off			&Uit
menutrans &Manual		&Handmatig
menutrans A&utomatic		A&utomatisch
menutrans &on\ (this\ file)	Aa&n\ (dit\ Bestand)
menutrans o&ff\ (this\ file)	&Uit\ (dit\ Bestand)
menutrans Co&lor\ test		Test\ de\ &Kleuren
menutrans &Highlight\ test	Test\ de\ Markeringen
menutrans &Convert\ to\ HTML	Converteren\ naar\ &HTML
