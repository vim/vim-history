" Menu Translations:	Italian
" Maintainer:		Antonio Colombo <antonio.colombo@jrc.org>
" Last Change:	2001 May 30

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-1

" Help menu
menut &Help			&Aiuto
menut &Overview<Tab><F1>	&Panoramica<Tab><F1>
menut &How-to\ links	Co&me\.\.\.
"menutrans &GUI			&GUI
menut &Credits		&Crediti
menut Co&pying		C&opie
menut &Find\.\.\.		Ce&rca\.\.\.
menut &Version		&Versione
menut &About		&Intro

" File menu
"menutrans &File				&File
menut &Open\.\.\.<Tab>:e		&Apri\.\.\.<Tab>:e
menut Sp&lit-Open\.\.\.<Tab>:sp	A&pri\ nuova\ Finestra\.\.\.<Tab>:sp
menut &New<Tab>:enew		&Nuovo<Tab>:enew
menut &Close<Tab>:q			&Chiudi<Tab>:q
menut &Save<Tab>:w			&Salva<Tab>:w
menut Save\ &As\.\.\.<Tab>:sav	Salva\ &con\ nome\.\.\.<Tab>:sav
menut &Print			S&tampa
menut Sa&ve-Exit<Tab>:wqa		Sa&lva\ ed\ esci<Tab>:wqa
menut E&xit<Tab>:qa			&Esci<Tab>:qa
menut Split\ &Diff\ with\.\.\.	Finestra\ &Differenza\ con\.\.\.
menut Split\ Patched\ &By\.\.\.	Finestra\ Patc&hes\ da\.\.\.

" Edit menu
" menutrans &Edit			&Edit
menut &Undo<Tab>u			&Disfa<Tab>u
menut &Redo<Tab>^R			Ri&fai<Tab>^R
menut Rep&eat<Tab>\.		&Ripeti \ultimo \comando<Tab>\.
menut Cu&t<Tab>"+x			T&aglia<Tab>"+x
menut &Copy<Tab>"+y			&Copia<Tab>"+y
menut &Paste<Tab>"+P		&Incolla<Tab>"+P
menut Put\ &Before<Tab>[p		&Metti\ davanti<Tab>[p
menut Put\ &After<Tab>]p		M&etti\ dietro<Tab>]p
menut &Delete<Tab>x			Cance&lla<Tab>x
menut &Select\ all<Tab>ggVG		&Seleziona\ tutto<Tab>ggVG
menut &Find\.\.\.			C&erca\.\.\.
menut &Find<Tab>/			C&erca<Tab>/
menut Find\ and\ Rep&lace		Cerca\ e\ Rim&piazza
menut Find\ and\ Rep&lace\.\.\.	Cerca\ e\ Rim&piazza\.\.\.
menut Find\ and\ Rep&lace<Tab>:s	Cerca\ e\ Rim&piazza<Tab>:s
menut Find\ and\ Rep&lace<Tab>:%s	Cerca\ e\ Rim&piazza<Tab>:%s
menut Settings\ &Window		&Finestra\ Impostazioni
menut Se&ttings			Im&postazioni
menut Qwestion			Domanda

" Edit/Global Settings
menut &Global\ Settings	Impostazioni\ &Globali
menut Toggle\ Pattern\ &Highlight<Tab>:set\ hls!	&Evidenzia\ ricerche\ Sì/No<Tab>:set\ hls!
menut Toggle\ &Ignore-case<Tab>:set\ ic!	&Ignora\ maius\.-minusc\.\ Sì/No<Tab>:set\ ic!
menut Toggle\ &Showmatch<Tab>:set\ sm!	Indica\ &Corrispondente\ Sì/No<Tab>:set\ sm!
menut &Context\ lines	&Linee\ di\ contesto
menut &Virtual\ Edit		&Edit\ Virtuale
menut Never		Mai
menut Block\ Selection		Selezione\ Blocco
menut Insert\ mode	Modalità\ Inserimento
menut Block\ and\ Insert	Selezione\ Blocco+Inserimento
menut Always		Sempre
menut Toggle\ Insert\ &Mode<Tab>:set\ im!	&Modalità\ Inserimento\ Sì/No<Tab>:set\ im!
menut Search\ &Path\.\.\.	Ca&mmino\ di\ ricerca\.\.\.
menut Ta&g\ Files\.\.\.		File\ Ta&g\.\.\.
menut C&olor\ Scheme		Schema\ C&olori
menut default		per\ difetto
menut evening		notturno
menut morning		diurno
menut shine		brillante
menut peachpuff         alla\ pesca
menut &Keymap				&Mappa\ tastiera

" Edit/File Settings

menut F&ile\ Settings	&Impostazioni\ File

" Boolean options
menut Toggle\ Line\ &Numbering<Tab>:set\ nu!	&Numerazione\ \ Sì/No<Tab>:set\ nu!
menut Toggle\ Line\ &Wrap<Tab>:set\ wrap!		Linee\ &Continuate\ Sì/No<Tab>:set\ wrap!
menut Toggle\ &List\ Mode<Tab>:set\ list!		Modalità\ &Lista\ Sì/No<Tab>:set\ list!
menut Toggle\ W&rap\ at\ word<Tab>:set\ lbr!		A\ capo\ alla\ &parola\ Sì/No<Tab>:set\ lbr!
menut Toggle\ &expand-tab<Tab>:set\ et!	&Espandi\ Tabulazione\ Sì/No<Tab>:set\ et!
menut Toggle\ &auto-indent<Tab>:set\ ai!	Indentazione\ &automatica\ Sì/No<Tab>:set ai!
menut Toggle\ &C-indenting<Tab>:set\ cin!	Indentazione\ stile\ &C\ Sì/No<Tab>:set cin!
menut Toggle\ Search\ Patn\ Highl<Tab>:set\ hls!	Evidenzia\ ricerche\ Sì/No<Tab>:set\ hls!
menut Toggle\ expand-tab<Tab>:set\ et!	Tab\ a\ spazi\ Sì/No<Tab>:set\ et!
menut Toggle\ auto-indent<Tab>:set\ ai!	Rientro\ automatico\ Sì/No<Tab>:set\ ai!
" Build GUI options
menut Toggle\ &Toolbar	Barra\ S&trumenti\ Sì/No
menut Toggle\ &Bottom\ Scrollbar	Barra\ Scorrimento\ in\ &fondo\ Sì/No
menut Toggle\ &Left\ Scrollbar	Barra\ Scorrimento\ a\ &sinistra\ Sì/No
menut Toggle\ &Right\ Scrollbar	Barra\ Scorrimento\ a\ &destra\ Sì/No

" Build variable options
menut &Shiftwidth			&Spazi\ Rientranza
menut Soft\ &Tabstop		&Tabulazione\ Software
menut Te&xt\ Width\.\.\.		Lunghe&zza\ Riga\.\.\.
menut &File\ Format\.\.\.	Formato\ &File\.\.\.

let g:menutrans_help_dialog = "Batti un comando o una parola per cercare aiuto:\n\nPremetti i_ per comandi in modalità Input (ad.es.: i_CTRL-X)\nPremetti c_ per comandi che editano la linea-comandi (ad.es.: c_<Del>)\nPremetti ' per un nome di opzione (ad.es.: 'shiftwidth')"
let g:menutrans_path_dialog = "Batti percorso di ricerca per i file.\nSepara fra loro i nomi di directory con una virgola."
let g:menutrans_tags_dialog = "Batti nome dei file di tag.\nSepara fra loro i nomi di directory con una virgola."
let g:menutrans_textwidth_dialog = "Batti nuova lunghezza linea (0 per inibire la formattazione): "
let g:menutrans_fileformat_dialog = "Scegli formato con cui scrivere il file"
let g:menutrans_no_file = "[Senza nome]"

" Programming menu
menut &Tools			&Strumenti
menut &Jump\ to\ this\ tag<Tab>g^]	&Vai\ a\ questa\ Tag<Tab>g^]
menut Jump\ &back<Tab>^T		Torna\ &indietro<Tab>^T
menut Build\ &Tags\ File		Costruisci\ File\ &Tags\
menut &Folding			&Piegature
menut &Make<Tab>:make		Esegui\ &Make<Tab>:make
menut &List\ Errors<Tab>:cl		Lista\ &Errori<Tab>:cl
menut L&ist\ Messages<Tab>:cl!	Lista\ &Messaggi<Tab>:cl!
menut &Next\ Error<Tab>:cn		Errore\ S&uccessivo<Tab>:cn
menut &Previous\ Error<Tab>:cp	Errore\ &Precedente<Tab>:cp
menut &Older\ List<Tab>:cold	Lista\ men&o\ recente<Tab>:cold
menut N&ewer\ List<Tab>:cnew	Lista\ più\ rece&nte<Tab>:cnew
menut Error\ &Window<Tab>:cwin	&Finestra\ errori<Tab>:cwin
menut &Convert\ to\ HEX<Tab>:%!xxd	&Converti\ a\ Esadecimale<Tab>:%!xxd
menut Conve&rt\ back<Tab>:%!xxd\ -r	Conve&rti\ da\ Esadecimale<Tab>:%!xxd\ -r

" open close folds 
menut &Enable/Disable\ folds<Tab>zi		Pi&egature\ Sì/No<Tab>zi
menut &View\ Cursor\ Line<Tab>zv		&Vedi\ linea\ col\ Cursore<Tab>zv
menut Vie&w\ Cursor\ Line\ only<Tab>zMzx	Vedi\ &solo\ linea\ col\ Cursore<Tab>zMzx
menut C&lose\ more\ folds<Tab>zm		C&hiudi\ più\ Piegature<Tab>zm
menut &Close\ all\ folds<Tab>zM		&Chiudi\ tutte\ le\ Piegature<Tab>zM
menut O&pen\ more\ folds<Tab>zr		A&pri\ più\ Piegature<Tab>zr
menut &Open\ all\ folds<Tab>zR		&Apri\ tutte\ le\ Piegature<Tab>zR
" fold method
menut Fold\ Met&hod		Meto&do\ Piegatura
menut M&anual	&Manuale
menut I&ndent	&Nidificazione
menut E&xpression	&Espressione\ Reg\.
menut S&yntax	&Sintassi
menut &Diff		&Differenza
menut &Update		&Aggiorna
menut &Get\ Block	&Importa\ Differenze
menut &Put\ Block	&Esporta\ DIfferenze
menut Ma&rker	Mar&catura
" create and delete folds
menut Create\ &Fold<Tab>zf		Crea\ &Piegatura<Tab>zf
menut &Delete\ Fold<Tab>zd		&Leva\ Piegatura<Tab>zd
menut Delete\ &All\ Folds<Tab>zD	Leva\ &tutte\ le\ Piegature<Tab>zD
" moving around in folds
menut Fold\ column\ &width		Larg&hezza\ piegature\ in\ colonne

" Names for buffer menu.
menut &Buffers		&Buffer
menut &Refresh\ menu	&Rinfresca\ menu
menut &Delete		&Cancella
menut &Alternate		&Alternato
menut &Next			&Successivo
menut &Previous		&Precedente
menut [No\ File]		[Nessun\ File]

" Window menu
menut &Window			&Finestra
menut &New<Tab>^Wn			&Nuova<Tab>^Wn
menut S&plit<Tab>^Ws		&Dividi\ lo\ schermo<Tab>^Ws
menut Sp&lit\ To\ #<Tab>^W^^	D&ividi\ verso\ #<Tab>^W^^
menut Split\ &Vertically<Tab>^Wv	Di&vidi\ verticalmente<Tab>^Wv
menut Split\ File\ E&xplorer	Aggiungi\ finestra\ E&xplorer
menut &Close<Tab>^Wc		&Chiudi<Tab>^Wc
menut Move\ &To			&Muovi\ verso
menut &Top<Tab>^WK			&Cima<Tab>^WK
menut &Bottom<Tab>^WJ		&Fondo<Tab>^WJ
menut &Left\ side<Tab>^WH		Lato\ &Sinistro<Tab>^WH
menut &Right\ side<Tab>^WL		Lato\ &Destro<Tab>^WL
menut &Close<Tab>^Wc		&Chiudi<Tab>^Wc
menut Close\ &Other(s)<Tab>^Wo	C&hiudi\ altro(i)<Tab>^Wo
menut Ne&xt<Tab>^Ww			S&uccessivo<Tab>^Ww
menut P&revious<Tab>^WW		&Precedente<Tab>^WW
menut &Equal\ Size<Tab>^W=		&Uguale\ ampiezza<Tab>^W=
menut &Max\ Height<Tab>^W_		&Altezza\ massima<Tab>^W_
menut M&in\ Height<Tab>^W1_		A&ltezza\ minima<Tab>^W1_
menut Max\ &Width<Tab>^W\|		Larghezza\ massima<Tab>^W\|
menut Min\ Widt&h<Tab>^W1\|		Larghezza\ minima<Tab>^W1\|
menut Rotate\ &Up<Tab>^WR		Ruota\ verso\ l'&alto<Tab>^WR
menut Rotate\ &Down<Tab>^Wr		Ruota\ verso\ il\ &basso<Tab>^Wr
menut Select\ Fo&nt\.\.\.		Scegli\ &Font\.\.\.

" The popup menu
menut &Undo		        &Disfa
menut Cu&t			&Taglia
menut &Copy			C&opia
menut &Paste		I&ncolla
menut &Delete		C&ancella
menut Select\ Blockwise 	Seleziona\ in\ blocco
menut Select\ &Word		Seleziona\ &Parola
menut Select\ &Line		Seleziona\ &Linea
menut Select\ &Block	Seleziona\ &Blocco
menut Select\ &All		Seleziona\ &Tutto
 
" The GUI toolbar (for MS-Windows and GTK)
menut ToolBar		Barra\ Strumenti
if has("toolbar")
menut Open		Apri
menut Save		Salva
menut SaveAll	SalvaTutto
menut Print		Stampa
menut Undo		Disfa
menut Redo		Rifai
menut Cut		Taglia
menut Copy		Copia
menut Paste		Incolla
menut Find		Cerca
menut FindNext	CercaSuccessivo
menut FindPrev	CercaPrecedente
menut Replace	Rimpiazza
menut LoadSesn	CaricaSessione
menut SaveSesn	SalvaSessione
menut RunScript	EseguiScript
menut Make		Make 
"menutrans Shell		Shell
menut RunCtags	EseguiCtags
menut TagJump	VaiATag
menut Help		Aiuto
menut FindHelp	CercaInAiuto
endif
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
menut &Syntax		&Sintassi
menut Set\ '&syntax'\ only	Attiva\ solo\ \ '&syntax'
menut Set\ '&filetype'\ too	Attiva\ anche\ '&filetype'
menut &Off			&Basta
menut &Manual		&Manuale
menut A&utomatic		A&utomatico
menut on/off\ for\ &This\ file  Attiva\ Sì/No\ su\ ques&to\ file
menut Co&lor\ test		Test\ &Colori
menut &Highlight\ test	Test\ &Evidenziamento
menut &Convert\ to\ HTML	Converti\ ad\ &HTML
