" Menu Translations:	Italian
" Maintainer:		Antonio Colombo <antonio.colombo@jrc.org>
" Last Change:	2001 apr 26

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-1

" Help menu
menutrans &Help			&Aiuto
menutrans &Overview<Tab><F1>	&Panoramica<Tab><F1>
menutrans &How-to\ links	Co&me\.\.\.
"menutrans &GUI			&GUI
menutrans &Credits		&Crediti
menutrans Co&pying		C&opie
menutrans &Find\.\.\.		Ce&rca\.\.\.
menutrans &Version		&Versione
menutrans &About		&Intro

" File menu
"menutrans &File				&File
menutrans &Open\.\.\.<Tab>:e		&Apri\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	A&pri\ nuova\ Finestra\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nuovo<Tab>:enew
menutrans &Close<Tab>:q			&Chiudi<Tab>:q
menutrans &Save<Tab>:w			&Salva<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav	Salva\ &con\ nome\.\.\.<Tab>:sav
menutrans &Print			S&tampa
menutrans Sa&ve-Exit<Tab>:wqa		Sa&lva\ ed\ esci<Tab>:wqa
menutrans E&xit<Tab>:qa			&Esci<Tab>:qa
menutrans Show\ &Diff\ with\.\.\.	Vedi\ &Differenza\ con\.\.\.
menutrans Show\ &Patched\ by\.\.\.	Vedi\ Patc&hes\ da\.\.\.

" Edit menu
" menutrans &Edit			&Edit
menutrans &Undo<Tab>u			&Disfa<Tab>u
menutrans &Redo<Tab>^R			Ri&fai<Tab>^R
menutrans Rep&eat<Tab>\.		&Ripeti \ultimo \comando<Tab>\.
menutrans Cu&t<Tab>"+x			T&aglia<Tab>"+x
menutrans &Copy<Tab>"+y			&Copia<Tab>"+y
menutrans &Paste<Tab>"+p		&Incolla<Tab>"+p
menutrans Put\ &Before<Tab>[p		&Metti\ davanti<Tab>[p
menutrans Put\ &After<Tab>]p		M&etti\ dietro<Tab>]p
menutrans &Delete<Tab>x			Cance&lla<Tab>x
menutrans &Select\ all<Tab>ggVG		&Seleziona\ tutto<Tab>ggVG
menutrans &Find\.\.\.			C&erca\.\.\.
menutrans &Find<Tab>/			C&erca<Tab>/
menutrans Find\ and\ Rep&lace		Cerca\ e\ Rim&piazza
menutrans Find\ and\ Rep&lace\.\.\.	Cerca\ e\ Rim&piazza\.\.\.
menutrans Find\ and\ Rep&lace<Tab>:s	Cerca\ e\ Rim&piazza<Tab>:s
menutrans Find\ and\ Rep&lace<Tab>:%s	Cerca\ e\ Rim&piazza<Tab>:%s
menutrans Settings\ &Window		&Finestra\ Impostazioni
menutrans Se&ttings			Im&postazioni

" Build boolean options
menutrans Toggle\ Line\ Numbering<Tab>:set\ number!	Numerazione\ \ Sì/No<Tab>:set\ number!
menutrans Toggle\ Line\ Wrap<Tab>:set\ wrap!		Linee\ lunghe\ Sì/No<Tab>:set\ wrap!
menutrans Toggle\ hlsearch<Tab>:set\ hlsearch!		Evidenzia\ ricerche\ Sì/No<Tab>:set\ hlsearch!
menutrans Toggle\ expandtab<Tab>:set\ expandtab!	TAB\ a\ spazi\ Sì/No<Tab>:set\ expandtab!

" Build GUI options
menutrans Toggle\ Toolbar		Barra\ Strumenti\ Sì/No
menutrans Toggle\ Bottom\ Scrollbar	Scorrimento\ in\ basso\ Sì/No
menutrans Toggle\ Left\ Scrollbar	Scorrimento\ a\ sinistra\ Sì/No
menutrans Toggle\ Right\ Scrolbar	Scorrimento\ a\ destra\ Sì/No

" Build variable options
menutrans Shiftwidth			Spazi\ Rientranza
menutrans Text\ Width\.\.\.		Lunghezza\ Riga\.\.\.

" Programming menu
menutrans &Tools			&Strumenti
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Vai\ a\ questa\ Tag<Tab>g^]
menutrans Jump\ &back<Tab>^T		Torna\ &indietro<Tab>^T
menutrans Build\ &Tags\ File		Costruisci\ File\ &Tags\
menutrans &Folding			&Piegature
menutrans &Make<Tab>:make		Esegui\ &Make<Tab>:make
menutrans &List\ Errors<Tab>:cl		Lista\ &Errori<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	Lista\ &Messaggi<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		Errore\ S&uccessivo<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	Errore\ &Precedente<Tab>:cp
menutrans &Older\ List<Tab>:cold	Lista\ men&o\ recente<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	Lista\ più\ rece&nte<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	&Finestra\ errori<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	Converti\ a\ Esadecimale<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	Converti\ da\ Esadecimale<Tab>:%!xxd\ -r

" open close folds 
menutrans &Enable/Disable\ folds<Tab>zi		Pi&egature\ Sì/No<Tab>zi
menutrans &View\ Cursor\ Line<Tab>zv		&Vedi\ linea\ col\ Cursore<Tab>zv
menutrans Vie&w\ Cursor\ Line\ only<Tab>zMzx	Vedi\ &solo\ linea\ col\ Cursore<Tab>zMzx
menutrans C&lose\ more\ folds<Tab>zm		C&hiudi\ più\ Piegature<Tab>zm
menutrans &Close\ all\ folds<Tab>zM		&Chiudi\ tutte\ le\ Piegature<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr		A&pri\ più\ Piegature<Tab>zr
menutrans &Open\ all\ folds<Tab>zR		&Apri\ tutte\ le\ Piegature<Tab>zR
" fold method
menutrans Fold\ Met&hod		Meto&do\ Piegatura
menutrans M&anual	&Manuale
menutrans I&ndent	&Nidificazione
menutrans E&xpression	&Espressione\ Reg\.
menutrans S&yntax	&Sintassi
menutrans &Diff		&Differenza
menutrans Ma&rker	Mar&catura
" create and delete folds
menutrans Create\ &Fold<Tab>zf		Crea\ &Piegatura<Tab>zf
menutrans &Delete\ Fold<Tab>zd		&Leva\ Piegatura<Tab>zd
menutrans Delete\ &All\ Folds<Tab>zD	Leva\ &tutte\ le\ Piegature<Tab>zD
" moving around in folds
menutrans Fold\ column\ &width		Larg&hezza\ piegature\ in\ colonne

" Names for buffer menu.
menutrans &Buffers		&Buffer
menutrans &Refresh\ menu	&Rinfresca\ menu
menutrans &Delete		&Cancella
menutrans &Alternate		&Alternato
menutrans &Next			&Successivo
menutrans &Previous		&Precedente
menutrans [No\ File]		[Nessun\ File]

" Window menu
menutrans &Window			&Finestra
menutrans &New<Tab>^Wn			&Nuova<Tab>^Wn
menutrans S&plit<Tab>^Ws		&Dividi\ lo\ schermo<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	D&ividi\ verso\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Di&vidi\ verticalmente<Tab>^Wv
menutrans Split\ File\ E&xplorer	Aggiungi\ finestra\ E&xplorer
menutrans &Close<Tab>^Wc		&Chiudi<Tab>^Wc
menutrans Move\ &To			&Muovi\ verso
menutrans &Top<Tab>^WK			&Cima<Tab>^WK
menutrans &Bottom<Tab>^WJ		&Fondo<Tab>^WJ
menutrans &Left\ side<Tab>^WH		Lato\ &Sinistro<Tab>^WH
menutrans &Right\ side<Tab>^WL		Lato\ &Destro<Tab>^WL
menutrans &Close<Tab>^Wc		&Chiudi<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	C&hiudi\ altro(i)<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			S&uccessivo<Tab>^Ww
menutrans P&revious<Tab>^WW		&Precedente<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		&Uguale\ ampiezza<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&Altezza\ massima<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		A&ltezza\ minima<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Larghezza\ massima<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Larghezza\ minima<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		Ruota\ verso\ l'&alto<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Ruota\ verso\ il\ &basso<Tab>^Wr
menutrans Select\ &Font\.\.\.		Scegli\ &Font\.\.\.

" The popup menu
menutrans &Undo		        &Disfa
menutrans Cu&t			&Taglia
menutrans &Copy			C&opia
menutrans &Paste		I&ncolla
menutrans &Delete		C&ancella
menutrans Select\ Blockwise 	Seleziona\ in\ blocco
menutrans Select\ &Word		Seleziona\ &Parola
menutrans Select\ &Line		Seleziona\ &Linea
menutrans Select\ &Block	Seleziona\ &Blocco
menutrans Select\ &All		Seleziona\ &Tutto
 
" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Apri
    tmenu ToolBar.Save		Salva
    tmenu ToolBar.SaveAll	SalvaTutto
    tmenu ToolBar.Print		Stampa
    tmenu ToolBar.Undo		Disfa
    tmenu ToolBar.Redo		Rifai
    tmenu ToolBar.Cut		Taglia
    tmenu ToolBar.Copy		Copia
    tmenu ToolBar.Paste		Incolla
    tmenu ToolBar.Find		Cerca
    tmenu ToolBar.FindNext	CercaSuccessivo
    tmenu ToolBar.FindPrev	CercaPrecedente
    tmenu ToolBar.Replace	Rimpiazza
    tmenu ToolBar.LoadSesn	CaricaSessione
    tmenu ToolBar.SaveSesn	SalvaSessione
    tmenu ToolBar.RunScript	EseguiScript
    tmenu ToolBar.Make		Make 
    tmenu ToolBar.Shell		Shell
    tmenu ToolBar.RunCtags	EseguiCtags
    tmenu ToolBar.TagJump	VaiATag
    tmenu ToolBar.Help		Aiuto
    tmenu ToolBar.FindHelp	CercaInAiuto
  endfun
endif

" Syntax menu
menutrans &Syntax		&Sintassi
menutrans Set\ 'syntax'\ only	Attiva\ solo\ \ 'syntax'
menutrans Set\ 'filetype'\ too	Attiva\ anche\ 'filetype'
menutrans &Off			&Basta
menutrans &Manual		&Manuale
menutrans A&utomatic		A&utomatico
menutrans o&n\ (this\ file)	A&ttiva\ (con\ questo\ file)
menutrans o&ff\ (this\ file)	&Disattiva\ (con\ \questo\ file)
menutrans Co&lor\ test		Test\ &Colori
menutrans &Highlight\ test	Test\ &Evidenziamento
menutrans &Convert\ to\ HTML	Converti\ ad\ &HTML
