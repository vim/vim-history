" Menu Translations:	Nederlands
" Maintainer:		Bram Moolenaar
" Last Change:	2001 Jul 28

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-1

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
menutrans Save\ &As\.\.\.<Tab>:sav	Bewaren\ als\.\.\.<Tab>:sav
menutrans Split\ &Diff\ with\.\.\.	Toon\ diff\ met\.\.\.
menutrans Split\ Patched\ &By\.\.\.	Toon\ gewijzigd\ door\.\.\.
menutrans &Print			Af&drukken
menutrans Sa&ve-Exit<Tab>:wqa		Bewaren\ en\ Afluiten<Tab>:wqa
menutrans E&xit<Tab>:qa			&Afsluiten<Tab>:qa

" Edit menu
menutrans &Edit				Be&werken
menutrans &Undo<Tab>u			Terug<Tab>u
menutrans &Redo<Tab>^R			Voo&ruit<Tab>^R
menutrans Rep&eat<Tab>\.		&Herhalen<Tab>\.
menutrans Cu&t<Tab>"+x			&Knippen<Tab>"+x
menutrans &Copy<Tab>"+y			K&opiëeren<Tab>"+y
menutrans &Paste<Tab>"+P		Plakken<Tab>"+P
menutrans Put\ &Before<Tab>[p		Ervoor\ invoegen<Tab>[p
menutrans Put\ &After<Tab>]p		Erachter\ invoegen<Tab>]p
menutrans &Select\ all<Tab>ggVG		Alles\ &Markeren<Tab>ggVG
menutrans &Find\.\.\.			&Zoeken\.\.\.
menutrans &Find<Tab>/			&Zoeken<Tab>/
menutrans Find\ and\ Rep&lace\.\.\.	Zoeken\ en\ &Vervangen\.\.\.
menutrans Find\ and\ Rep&lace<Tab>:%s	Zoeken\ en\ &Vervangen<Tab>:%s
menutrans Find\ and\ Rep&lace		Zoeken\ en\ &Vervangen
menutrans Find\ and\ Rep&lace<Tab>:s	Zoeken\ en\ &Vervangen<Tab>:s
menutrans Settings\ &Window		Opties\ Venster
menutrans &Global\ Settings		Globale\ Opties
menutrans F&ile\ Settings		Bestand\ Opties
menutrans C&olor\ Scheme		Kleuren\ Schema
menutrans &Keymap			Toetsenbord\ Indeling

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
menutrans Error\ &Window		Fouten\ Venster
menutrans &Update<Tab>:cwin		&Aanpassen<Tab>:cwin
menutrans &Open<Tab>:copen		&Openen<Tab>:copen
menutrans &Close<Tab>:cclose		&Sluiten<Tab>:cclose
menutrans Convert\ to\ HEX<Tab>:%!xxd	Converteer\ naar\ HEX<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	Converteer\ terug<Tab>:%!xxd\ -r
menutrans &Set\ Compiler		Kies\ Compiler

" Names for buffer menu.
menutrans &Buffers		&Buffer
menutrans &Refresh\ menu	Ververs\ menu
menutrans Delete		Wissen
menutrans A&lternate		Vorige
menutrans &Next			Vooruit
menutrans &Previous		Achteruit

" Window menu
menutrans &Window			&Venster
menutrans &New<Tab>^Wn			&Nieuw<Tab>^Wn
menutrans S&plit<Tab>^Ws		Splitsen<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Splits\ naar\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Splits\ &Vertikaal<Tab>^Wv
menutrans &Close<Tab>^Wc		&Sluiten<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	&Sluit\ Andere<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			Volgende<Tab>^Ww
menutrans P&revious<Tab>^WW		&Vorige<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		&Gelijke\ afmetingen<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&Maximale\ hoogte<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Mi&nimale\ hoogte<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Maximale\ breedte<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Minimale\ breedte<Tab>^W1\|
menutrans Move\ &To			Verplaats\ naar
menutrans &Top<Tab>^WK			Bovenkant<Tab>^WK
menutrans &Bottom<Tab>^WJ		Onderkant<Tab>^WJ
menutrans &Left\ side<Tab>^WH		Linkerkant<Tab>^WH
menutrans &Right\ side<Tab>^WL		Rechterkant<Tab>^WL
menutrans Rotate\ &Up<Tab>^WR		Roteren\ naar\ &boven<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Roteren\ naar\ &onder<Tab>^Wr
menutrans Select\ Fo&nt\.\.\.		Selecteer\ font\.\.\.

" The popup menu
menutrans &Undo			&Terug
menutrans Cu&t			Knip
menutrans &Copy			&Kopiëer
menutrans &Paste		&Plak
menutrans &Delete		&Wissen
menutrans Select\ Blockwise	Selecteer\ per\ Rechthoek
menutrans Select\ &Word		Selecteer\ een\ &Woord
menutrans Select\ &Line		Selecteer\ een\ &Regel
menutrans Select\ &Block	Selecteer\ een\ Recht&hoek
menutrans Select\ &All		Selecteer\ &Alles

" The GUI toolbar (for Win32 or GTK)
if has("toolbar")
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
menutrans o&n\ (this\ file)	Aa&n\ (dit\ Bestand)
menutrans o&ff\ (this\ file)	&Uit\ (dit\ Bestand)
menutrans Co&lor\ test		Test\ de\ &Kleuren
menutrans &Highlight\ test	Test\ de\ Markeringen
menutrans &Convert\ to\ HTML	Converteren\ naar\ &HTML

" dialog texts
let menutrans_no_file = "[Geen Bestand]"
let menutrans_help_dialog = "Typ een commando of woord om help voor te vinden:\n\nVoeg i_ in voor Input mode commandos (bijv. i_CTRL-X)\nVoeg c_ in voor een commando-regel edit commando (bijv. c_<Del>)\nVoeg ' in \voor een optie naam (bijv. 'shiftwidth')"
let g:menutrans_path_dialog = "Typ het zoekpad voor bestanden.\nGebruik commas tussen de padnamen."
let g:menutrans_tags_dialog = "Typ namen van tag bestanden.\nGebruik commas tussen de namen."
let g:menutrans_textwidth_dialog = "Typ de nieuwe tekst breedte (0 om formatteren uit the schakelen): "
let g:menutrans_fileformat_dialog = "Selecteer formaat voor het schrijven van het bestand"
