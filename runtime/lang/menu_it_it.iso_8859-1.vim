" Menu Translations:	Italian
" Maintainer:		Antonio Colombo <antonio.colombo@jrc.org>
" Last Change:	2000 Dic 20

menutrans clear
let did_menu_trans = 1

" Help menu
menutrans &Help			&Aiuto
menutrans &Overview<Tab><F1>	&Panoramica<Tab><F1>
menutrans &How-to\ links	&How-to\ collegamenti
"menutrans &GUI			&GUI
menutrans &Credits		&Crediti
menutrans Co&pying		C&opie
menutrans &Find\.\.\.		&Ricerca\.\.\.
menutrans &Version		&Versione
menutrans &About		&Informazioni

" File menu
"menutrans &File				&File
menutrans &Open\.\.\.<Tab>:e		&Apri\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	A&pri\ in\ nuova\ Finestra\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nuovo<Tab>:enew
menutrans &Close<Tab>:q			&Chiudi<Tab>:q
menutrans &Save<Tab>:w			&Salva<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	Salva\ &con\ nome\.\.\.<Tab>:w
menutrans &Print			S&tampa
menutrans Sa&ve-Exit<Tab>:wqa		Sa&lva\ ed\ esci<Tab>:wqa
menutrans E&xit<Tab>:qa			&Esci<Tab>:qa

" Edit menu
" menutrans &Edit			&Edit
menutrans &Undo<Tab>u			&Torna\ indietro<Tab>u
menutrans &Redo<Tab>^R			&Vai\ in\ avanti<Tab>^R
menutrans Rep&eat<Tab>\.		&Ripeti<Tab>\.
menutrans Cu&t<Tab>"*x			T&aglia<Tab>"*x
menutrans &Copy<Tab>"*y			&Copia<Tab>"*y
menutrans &Paste<Tab>"*p		&Incolla<Tab>"*p
menutrans Put\ &Before<Tab>[p		&Metti\ davanti<Tab>[p
menutrans Put\ &After<Tab>]p		M&etti\ dietro<Tab>]p
menutrans &Delete<Tab>x			Cance&lla<Tab>x
menutrans &Select\ all<Tab>ggVG		&Seleziona\ tutto<Tab>ggVG
menutrans &Find\.\.\.			C&erca\.\.\.
menutrans &Find<Tab>/			C&erca<Tab>/
menutrans Find\ and\ Rep&lace		Cerca\ e\ rim&piazza
menutrans Find\ and\ Rep&lace\.\.\.	Cerca\ e\ rim&piazza\.\.\.
menutrans Find\ and\ Rep&lace<Tab>:s	Cerca\ e\ rim&piazza<Tab>:s
menutrans Find\ and\ Rep&lace<Tab>:%s	Cerca\ e\ rim&piazza<Tab>:%s
menutrans Options\.\.\.			Opzioni\.\.\.

" Programming menu
menutrans &Tools			&Strumenti
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Salta\ a\ questa\ Tag<Tab>g^]
menutrans Jump\ &back<Tab>^T		Salta\ &indietro<Tab>^T
menutrans Build\ &Tags\ File		Costruisci\ File\ &Tags\
menutrans &Make<Tab>:make		Esegui\ &Make<Tab>:make
menutrans &List\ Errors<Tab>:cl		Lista\ &Errori<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	Lista\ &Messaggi<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		Errore\ S&uccessivo<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	Errore\ &Precedente<Tab>:cp
menutrans &Older\ List<Tab>:cold	Lista\ men&o\ recente<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	Lista\ più\ rece&nte<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	&Finestra\ errori<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	Tradurre\ in\ Esadecimale<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	Tradurre\ indietro<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Buffer
menutrans Refresh	Rinfrescare
menutrans Delete	Cancellare
menutrans Alternate	Alternato
menutrans [No\ File]	[Nessun\ File]

" Window menu
menutrans &Window			&Finestra
menutrans &New<Tab>^Wn			&Nuova<Tab>^Wn
menutrans S&plit<Tab>^Ws		&Dividi\ lo\ schermo<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	D&ividi\ verso\ #<Tab>^W^^
menutrans S&plit\ Vertically<Tab>^Wv	Di&vidi\ in\ verticale<Tab>^Wv
menutrans &Close<Tab>^Wc		&Chiudi<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	C&hiudi\ altro(i)<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			S&uccessivo<Tab>^Ww
menutrans P&revious<Tab>^WW		&Precedente<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	&Uguale\ altezza<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&Altezza\ massima<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		A&ltezza\ minima<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Larghezza\ massima<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Larghezza\ minima<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		Ruotare\ verso\ l'&alto<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotation\ verso\ il &basso<Tab>^Wr
menutrans Select\ &Font\.\.\.		Scegli\ &font\.\.\.

" The popup menu
menutrans &Undo		        Torna \&indietro
menutrans Cu&t			&Taglia
menutrans &Copy			C&opia
menutrans &Paste		I&ncolla
menutrans &Delete		C&ancella
menutrans Select\ Blockwise 	Seleziona\ in\ blocco
menutrans Select\ &Word		Seleziona\ una\ &parola
menutrans Select\ &Line		Seleziona\ una\ &linea
menutrans Select\ &Block	Seleziona\ un\ &blocco
menutrans Select\ &All		Seleziona\ &tutto
 
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Apri file
    tmenu ToolBar.Save		Salva file
    tmenu ToolBar.SaveAll	Salva tutti i files
    tmenu ToolBar.Print		Stampa
    tmenu ToolBar.Undo		Torna indietro
    tmenu ToolBar.Redo		Vai in avanti
    tmenu ToolBar.Cut		Taglia
    tmenu ToolBar.Copy		Copia
    tmenu ToolBar.Paste		Incolla
    tmenu ToolBar.Find		Cerca...
    tmenu ToolBar.FindNext	Cerca successivo
    tmenu ToolBar.FindPrev	Cerca precedente
    tmenu ToolBar.Replace	Cerca e rimpiazza...
    tmenu ToolBar.LoadSesn	Carica sessione
    tmenu ToolBar.SaveSesn	Salva sessione
    tmenu ToolBar.RunScript	esegui uno script
    tmenu ToolBar.Make		esegui Make 
    tmenu ToolBar.Shell		esegui il tuo Shell
    tmenu ToolBar.RunCtags	genera un file di Tags
    tmenu ToolBar.TagJump	Salta a una Tag
    tmenu ToolBar.Help		Aiuto!
    tmenu ToolBar.FindHelp	Cerca nella documentazione...
  endfun
endif

" Syntax menu
menutrans &Syntax		&Sintassi
menutrans Set\ 'syntax'\ only	attivare\ soltanto\ 'syntax'
menutrans Set\ 'filetype'\ too	attivare\ anche\ 'filetype'
menutrans &Off			&basta
menutrans &Manual		&Manuale
menutrans A&utomatic		A&utomatico
menutrans &on\ (this\ file)	A&ttivare\ (per\ questo\ file)
menutrans o&ff\ (this\ file)	&disattivare\ (per\ \questo\ file)
menutrans Co&lor\ test		Test\ dei\ &Colori
menutrans &Highlight\ test	Test\ dell'&evidenziamento
menutrans &Convert\ to\ HTML	Converti\ ad\ &HTML
