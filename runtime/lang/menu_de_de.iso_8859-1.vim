" Menu Translations:	German
" Maintainer:		Johannes Zellner <johannes@zellner.org>
" Originally By:	Marcin Dalecki <dalecki@cs.net.pl>
" Last Change:	Sam, 25 Nov 2000 02:54:50 +0100

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-1

" Help menu
menutrans &Help			&Hilfe
menutrans &Overview<Tab><F1>	&Überblick<Tab><F1>
menutrans &How-to\ links	&How-to\ Index
menutrans &GUI			&Graphische\ Oberfläche
menutrans &Credits		&Autoren
menutrans Co&pying		&Copyright	" Urheberrecht ?
menutrans &Find\.\.\.		&Suchen\.\.\.	" conflicts with Edit.Find
menutrans &Version		&Version
menutrans &About		&Titelseite

" File menu
menutrans &File				&Datei
menutrans &Open\.\.\.<Tab>:e		&Öffnen\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	In\ geteiltem\ &Fenster\ Öffnen\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Neue\ Datei<Tab>:enew
menutrans &Close<Tab>:q			Sch&liessen<Tab>:q
menutrans &Save<Tab>:w			&Speichern<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	Speichern\ &Als\.\.\.<Tab>:w
menutrans &Print			&Drucken
menutrans Sa&ve-Exit<Tab>:wqa		Sp&eichern\ und\ Beenden<Tab>:wqa
menutrans E&xit<Tab>:qa			&Beenden<Tab>:qa

" Edit menu
menutrans &Edit				&Editieren
menutrans &Undo<Tab>u			Z&urück<Tab>u
menutrans &Redo<Tab>^R			Vo&r<Tab>^R
menutrans Rep&eat<Tab>\.		&Wiederholen<Tab>\.
menutrans Cu&t<Tab>"+x			&Ausschneiden<Tab>"+x
menutrans &Copy<Tab>"+y			&Kopieren<Tab>"+y
menutrans &Paste<Tab>"+p		Ein&fügen<Tab>"+p
menutrans Put\ &Before<Tab>[p		Da&vor\ Einfügen<Tab>[p
menutrans Put\ &After<Tab>]p		Da&nach\ Einfügen<Tab>]p
menutrans &Select\ all<Tab>ggVG		Alles\ &Markieren<Tab>ggVG
menutrans &Find\.\.\.			&Suchen\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.	Suchen\ und\ &Ersetzen\.\.\.
menutrans Options\.\.\.			Einstellungen\.\.\.

" Programming menu
menutrans &Tools			&Werkzeuge
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Springe\ zum\ Tag<Tab>g^]
menutrans Jump\ &back<Tab>^T		Springe\ &Zurück<Tab>^T
menutrans Build\ &Tags\ File		Erstelle\ &Tags\ Datei
menutrans &Make<Tab>:make		&Erstellen<Tab>:make
menutrans &List\ Errors<Tab>:cl		&Fehler\ Anzeigen<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	&Hinweise\ Anzeigen<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		Zum\ &Nächsten\ Fehler<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	Zum\ &Vorherigen\ Fehler<Tab>:cp
menutrans &Older\ List<Tab>:cold	&Ältere\ Liste<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	&Neuere\ Liste<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	Feh&ler-Fenster<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	Wandle\ nach\ HEX<Tab>:%!xxd
menutrans Convert\ to\ HEX<Tab>:%!mc\ vim:xxd	Wandle\ nach\ HEX<Tab>:%!ms\ vim:xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	Wandle\ zurück<Tab>:%!xxd\ -r
menutrans Convert\ back<Tab>:%!mc\ vim:xxd\ -r	Wandle\ zurück<Tab>:%!mc\ vim:xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Puffer
menutrans Refresh	Aktualisieren
menutrans Delete	Löschen
menutrans Alternate	Wechseln
menutrans [No\ File]	[Keine\ Datei]

" Window menu
menutrans &Window			&Ansicht
menutrans &New<Tab>^Wn			&Neu<Tab>^Wn
menutrans S&plit<Tab>^Ws		Aufs&palten<Tab>^Ws
menutrans Split\ &Vertically<Tab>^Wv	&Vertikal\ Aufspalten<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Aufspa&lten\ in\ #<Tab>^W^^
menutrans &Close<Tab>^Wc		&Schliessen<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	&Andere\ Schliessen<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			N&ächstes<Tab>^Ww
menutrans P&revious<Tab>^WW		Vor&heriges<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		&Gleiche\ Höhen<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&Maximale\ Höhe<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Mi&nimale\ Höhe<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Maximale\ Breite<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Minimale\ Breite<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		Rotiere\ nach\ &oben<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotiere\ nach\ &unten<Tab>^Wr
menutrans Select\ &Font\.\.\.		Auswahl\ der\ Schriftart\.\.\.

" The popup menu
menutrans &Undo			&Zurück
menutrans Cu&t			Aus&schneiden
menutrans &Copy			&Kopieren
menutrans &Paste		&Einfügen
menutrans &Delete		&Löschen
menutrans Select\ Blockwise 	Auswahl\ Blockartig
menutrans Select\ &Word		Auswahl\ des\ &Wortes
menutrans Select\ &Line		Auswahl\ der\ &Zeile
menutrans Select\ &Block	Auswahl\ des\ &Blocks
menutrans Select\ &All		&Alles\ Auswählen
 
" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Datei Öffnen
    tmenu ToolBar.Save		Datei Speichern
    tmenu ToolBar.SaveAll	Alle Dateien Speichern
    tmenu ToolBar.Print		Drucken
    tmenu ToolBar.Undo		Zurück
    tmenu ToolBar.Redo		Wiederholen
    tmenu ToolBar.Cut		Ausschneiden
    tmenu ToolBar.Copy		Kopieren
    tmenu ToolBar.Paste		Einfügen
    tmenu ToolBar.Find		Suchen...
    tmenu ToolBar.FindNext	Suche Nächsten
    tmenu ToolBar.FindPrev	Suche Vorherigen
    tmenu ToolBar.Replace	Suchen und Ersetzen...
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New		Neue Ansicht
      tmenu ToolBar.WinSplit	Ansicht Aufspalten
      tmenu ToolBar.WinMax	Ansicht Maximale Höhen
      tmenu ToolBar.WinMin	Ansicht Minimale Höhen
      tmenu ToolBar.WinClose	Ansicht Schliessen
    endif
    tmenu ToolBar.LoadSesn	Sitzung Laden
    tmenu ToolBar.SaveSesn	Sitzung Speichern
    tmenu ToolBar.RunScript	Vim Skript Ausführen
    tmenu ToolBar.Make		Make Ausführen
    tmenu ToolBar.Shell		Shell Starten
    tmenu ToolBar.RunCtags	Erstelle Tags Datei
    tmenu ToolBar.TagJump	Springe zum Tag
    tmenu ToolBar.Help		Hilfe!
    tmenu ToolBar.FindHelp	Hilfe Durchsuchen...
  endfun
endif

" Syntax menu
menutrans &Syntax		&Syntax
menutrans Set\ 'syntax'\ only	Nur\ 'syntax'\ Setzen
menutrans Set\ 'filetype'\ too	Auch\ 'filetype'\ Setzen
menutrans &Off			&Aus
menutrans &Manual		&Manuell
menutrans A&utomatic		A&utomatisch
menutrans o&n\ (this\ file)	A&n\ (diese\ Datei)
menutrans o&ff\ (this\ file)	Au&s\ (diese\ Datei)
menutrans Co&lor\ test		Test\ der\ &Farben
menutrans &Highlight\ test	Test\ der\ Un&terstreichungen
menutrans &Convert\ to\ HTML	Konvertieren\ nach\ &HTML
