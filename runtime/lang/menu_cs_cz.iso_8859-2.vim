" Menu Translations:	Czech for MS-Windows
" Maintainer:		Jiri Brezina <brz@centrum.cz>
" Based on:		ISO-8859-2 menus by Jiri Pavlovsky's <pavlovsk@ff.cuni.cz>
" Last Change:		2001 Oct 23
" Version cz menu	vim6.0-cz1.5

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding ISO-8859-2

" Help menu
menutrans &Help 		&Nápovìda
menutrans &Overview<Tab><F1>	&Pøehled<Tab><F1>
menutrans &User\ Manual		&U¾ivatelský\ Manuál
menutrans &How-to\ links	&Tipy
menutrans &GUI 			&Grafické\ rozhraní
menutrans &Credits		Po&dìkování
menutrans Co&pying		&Licenèní\ politika
menutrans &Find\.\.\.		&Najít\.\.\.
menutrans &Version		&Verze
menutrans &About		&O\ aplikaci

" File menu
menutrans &File				&Soubor
menutrans &Open\.\.\.<Tab>:e		&Otevøít\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	Otevøít\ v\ no&vém\ oknì\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nový<Tab>:enew
menutrans &Close<Tab>:close		&Zavøít<Tab>:close
menutrans &Save<Tab>:w			&Ulo¾it<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav	Ulo¾it\ &jako\.\.\.<Tab>:sav
menutrans Split\ &Diff\ with\.\.\.	Rozdìlit\ okno\ -Diff...
menutrans Split\ Patched\ &By\.\.\.	Rozdìlit\ okno\ -Patch...
menutrans &Print			&Tisk
menutrans Sa&ve-Exit<Tab>:wqa		U&lo¾it-Konec<Tab>:wqa
menutrans E&xit<Tab>:qa			&Konec<Tab>:qa

" Edit menu
menutrans &Edit				&Úpravy
menutrans &Undo<Tab>u			&Zpìt<Tab>u
menutrans &Redo<Tab>^R			Z&ru¹it\ vrácení<Tab>^R
menutrans Rep&eat<Tab>\.		&Opakovat<Tab>\.
menutrans Cu&t<Tab>"+x			&Vyøíznout<Tab>"+x
menutrans &Copy<Tab>"+y			&Kopírovat<Tab>"+y
menutrans &Paste<Tab>"+P		V&lo¾it<Tab>"+P
menutrans Put\ &Before<Tab>[p		Vlo¾it\ &pøed<Tab>[p
menutrans Put\ &After<Tab>]p		Vlo¾i&t\ za<Tab>]p
menutrans &Delete<Tab>x 			Smazat<Tab>x
menutrans &Select\ all<Tab>ggVG		Vy&brat\ v¹e<Tab>ggVG
menutrans &Find\.\.\.			&Hledat\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.	&Nahradit\.\.\.
menutrans Options\.\.\.			Volby\.\.\.
menutrans Settings\ &Window 		Nastavení\ okna
menutrans &Global\ Settings 		&Globální\ nastavení
menutrans Toggle\ Pattern\ &Highlight<Tab>:set\ hls!		Pøehodit\ nalezení\ vzoru<Tab>:set\ hls!
menutrans Toggle\ &Ignore-case<Tab>:set\ ic! 			Pøehodit\ ignorování\ VERZÁLEK<Tab>:set\ ic!
menutrans Toggle\ &Showmatch<Tab>:set\ sm! 			Pøehodit\ &Showmatch\ \{\(\[\])\}<Tab>:set\ sm!
menutrans &Context\ lines 		Zobrazit\ kontextové\ øádky
menutrans &Virtual\ Edit 		Virtuální\ editace
menutrans Never 			Nikdy
menutrans Block\ Selection 		Výbìr\ Bloku
menutrans Insert\ mode 			Insert\ Mód
menutrans Block\ and\ Insert 		Blok\ a\ Insert
menutrans Always 			V¾dycky
menutrans Toggle\ Insert\ &Mode<Tab>:set\ im! 			Pøehodit\ Insert\ Mód<Tab>:set\ im!
menutrans Search\ &Path\.\.\. 		Nastavit\ cestu\ k\ prohledávání\.\.\.
menutrans Ta&g\ Files\.\.\. 		Ta&g\ Soubory\.\.\.
menutrans Toggle\ &Toolbar 		Pøehodit\ &Toolbar
menutrans Toggle\ &Bottom\ Scrollbar 	Pøehodit\ Dolní\ rolovací\ li¹tu  
menutrans Toggle\ &Left\ Scrollbar 	Pøehodit\ Levou\ rolovací\ li¹tu
menutrans Toggle\ &Right\ Scrollbar 	Pøehodit\ Pravou\ rolovací\ li¹tu
menutrans F&ile\ Settings 		Nastavení\ souboru
menutrans Toggle\ Line\ &Numbering<Tab>:set\ nu! 		Pøehodit\ èíslování\ øádkù<Tab>:set\ nu!
menutrans Toggle\ &List\ Mode<Tab>:set\ list! 			Pøehodit\ &List\ Mode<Tab>:set\ list!
menutrans Toggle\ Line\ &Wrap<Tab>:set\ wrap! 			Pøehodit\ zalamování\ øádkù<Tab>:set\ wrap!
menutrans Toggle\ W&rap\ at\ word<Tab>:set\ lbr! 		Pøehodit\ zlom\ ve\ slovì<Tab>:set\ lbr!
menutrans Toggle\ &expand-tab<Tab>:set\ et! 			Pøehodit\ &expand-tab<Tab>:set\ et!
menutrans Toggle\ &auto-indent<Tab>:set\ ai! 			Pøehodit\ &auto-indent<Tab>:set\ ai!
menutrans Toggle\ &C-indenting<Tab>:set\ cin! 			Pøehodit\ &C-indenting<Tab>:set\ cin!
menutrans &Shiftwidth 			Nastavit\ ¹íøku\ odsazení
menutrans Te&xt\ Width\.\.\. 		©íøka\ textu\.\.\.
menutrans &File\ Format\.\.\. 		&Formát\ souboru\.\.\.
menutrans C&olor\ Scheme 		Barevné\ schéma
menutrans &Keymap 			&Klávesová\ mapa

" Programming menu
menutrans &Tools			&Nástroje
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Skoèit\ na\ znaèku<Tab>g^]
menutrans Jump\ &back<Tab>^T		Skoèit\ &zpìt<Tab>^T
menutrans Build\ &Tags\ File		&Vytvoøit\ soubor\ tagù

menutrans &Folding 			&Foldy
menutrans &Enable/Disable\ folds<Tab>zi &Ano/Ne
menutrans &View\ Cursor\ Line<Tab>zv 	&Zobrazit\ øádek\ kurzoru<Tab>zv
menutrans Vie&w\ Cursor\ Line\ only<Tab>zMzx 		Zobr&azit\ pouze\ øádek\ kurzoru\ <Tab>zMzx
menutrans C&lose\ more\ folds<Tab>zm 	Vyjmout\ jednu\ úroveò\ foldù<Tab>zm
menutrans &Close\ all\ folds<Tab>zM 	&Zavøít\ v¹echny\ foldy<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr 	Pøidat\ jednu\ úroveò\ foldù<Tab>zr
menutrans &Open\ all\ folds<Tab>zR 	&Otevøít\ v¹echny\ foldy<Tab>zR
menutrans Fold\ Met&hod 		Me&toda\ skládání
menutrans M&anual 			&Ruènì
menutrans I&ndent 			&Odsazení
menutrans E&xpression 			&Výraz
menutrans S&yntax 			&Syntax
menutrans &Diff 			&Diff
menutrans Ma&rker 			Ma&rker
menutrans Create\ &Fold<Tab>zf 		Vytvoøit\ &Fold<Tab>zf
menutrans &Delete\ Fold<Tab>zd 		Vymazat\ fold<Tab>zd 
menutrans Delete\ &All\ Folds<Tab>zD 	V&ymazat\ v¹echny\ foldy<Tab>zD
menutrans Fold\ column\ &width 		©íø&ka\ foldu

menutrans &Update 			&Obnovit
menutrans &Get\ Block 			&Sejmout\ Blok
menutrans &Put\ Block 			&Vlo¾it\ Blok
menutrans &Make<Tab>:make		&Make<Tab>:make
menutrans &List\ Errors<Tab>:cl		Výpis\ &chyb<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	Výp&is\ zpráv<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		&Dal¹í\ chyba<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	&Pøedchozí\ chyba<Tab>:cp
menutrans &Older\ List<Tab>:cold	Sta&r¹í\ seznam<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	N&ovìj¹í\ seznam<Tab>:cnew
menutrans Error\ &Window		Chybové\ o&kno
menutrans &Update<Tab>:cwin 		&Obnovit<Tab>:cwin
menutrans &Open<Tab>:copen 		&Otevøít<Tab>:copen
menutrans &Close<Tab>:cclose 		&Zavøít<Tab>:cclose
menutrans &Set\ Compiler		Nastavit\ kompilátor

menutrans &Convert\ to\ HEX<Tab>:%!xxd 	&Pøevést\ do\ ¹estnáctkového\ formátu<Tab>:%!xxd
menutrans Conve&rt\ back<Tab>:%!xxd\ -r Pøevést\ &Zpìt<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Buffery
menutrans &Refresh\ menu	&Obnovení\ menu
menutrans &Delete	Zru¹it
menutrans &Alternate	Vystøídat
menutrans &Next 	Dal¹í
menutrans &Previous 	Pøedchozí
menutrans [No\ File]	[¾ádný soubor]

" Window menu
menutrans &Window			&Okna
menutrans &New<Tab>^Wn			&Nové<Tab>^Wn
menutrans S&plit<Tab>^Ws		&Rozdìlit<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Ro&zdìlit\ na\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Rozdìlit\ &vertikálnì<Tab>^Wv
menutrans Split\ File\ E&xplorer 	Rozdìlit\ -File\ E&xplorer
menutrans Move\ &To 			Pøesun
menutrans &Top<Tab>^WK 			&Nahoru<Tab>^WK
menutrans &Bottom<Tab>^WJ 		&Dolu<Tab>^WJ
menutrans &Left\ side<Tab>^WH 		&Vlevo<Tab>^WH
menutrans &Right\ side<Tab>^WL 		Vp&ravo<Tab>^WL

menutrans &Close<Tab>^Wc		Zavøí&t<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Zavøít\ &jiná<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			&Dal¹í<Tab>^Ww
menutrans P&revious<Tab>^WW		&Pøedchozí<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		&Stejná\ vý¹ka<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Maximální\ vý¹ka<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Minimální\ vý¹ka<Tab>^W1_
menutrans Max\ &Width<Tab>^W\|		Maximální\ ¹íøka<Tab>^W\|
menutrans Min\ Widt&h<Tab>^W1\|		Minimální\ ¹íøka<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		R&otovat\ nahoru<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotovat\ &dolù<Tab>^Wr
menutrans Select\ Fo&nt\.\.\.		Vy&brat\ písmo\.\.\.

" The popup menu
menutrans &Undo			&Zpìt
menutrans Cu&t      &Vyøíznout
menutrans &Copy			&Kopírovat
menutrans &Paste		&Vlo¾it
menutrans &Delete		&Smazat
menutrans Select\ Blockwise	Vybrat\ blokovì
menutrans Select\ &Word		Vybrat\ &s³ovo
menutrans Select\ &Line		Vybrat\ &øádek
menutrans Select\ &Block	Vybrat\ &blok
menutrans Select\ &All		Vybrat\ &v¹e

" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Otevøít soubor
    tmenu ToolBar.Save		Ulo¾it soubor
    tmenu ToolBar.SaveAll		Ulo¾it v¹echny soubory
    tmenu ToolBar.Print		Tisk
    tmenu ToolBar.Undo		Zpìt
    tmenu ToolBar.Redo		Zru¹it vrácení
    tmenu ToolBar.Cut		Vyøíznout
    tmenu ToolBar.Copy		Kopírovat
    tmenu ToolBar.Paste		Vlo¾it
    tmenu ToolBar.Find		Hledat...
    tmenu ToolBar.FindNext	Hledat dal¹í
    tmenu ToolBar.FindPrev	Hledat pøedchozí
    tmenu ToolBar.Replace		Nahradit...
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New		Nové okno
      tmenu ToolBar.WinSplit	Rozdìlit okno
      tmenu ToolBar.WinMax		Maximalizovat okno
      tmenu ToolBar.WinMin		Minimalizovat okno
      tmenu ToolBar.WinClose	Zavøít okno
    endif
    tmenu ToolBar.LoadSesn	Naèíst sezení
    tmenu ToolBar.SaveSesn	Ulo¾it sezení
    tmenu ToolBar.RunScript	Spustit skript
    tmenu ToolBar.Make		Spustit make
    tmenu ToolBar.Shell		Spustit shell
    tmenu ToolBar.RunCtags	Spustit ctags
    tmenu ToolBar.TagJump		Skoèit na tag pod kurzorem
    tmenu ToolBar.Help		Nápovìda
    tmenu ToolBar.FindHelp	Najít nápovìdu k...
  endfun
endif

" Syntax menu
menutrans &Syntax 		&Syntaxe
menutrans Set\ '&syntax'\ only	Nastavit\ pouze\ 'syntax'
menutrans Set\ '&filetype'\ too	Nastavit\ také\ 'filetype'
menutrans &Off			&Vypnout
menutrans &Manual		&Ruènì
menutrans A&utomatic		A&utomaticky
menutrans on/off\ for\ &This\ file	&Zapnout-Vypnout\ (pro\ tento\ soubor)
menutrans o&ff\ (this\ file)	vyp&nout\ (pro\ tento\ soubor)
menutrans Co&lor\ test		Test\ &barev
menutrans &Highlight\ test	&Test\ zvýrazòování
menutrans &Convert\ to\ HTML	&Pøevést\ do\ HTML

