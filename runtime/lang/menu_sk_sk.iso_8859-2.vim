" Menu Translations:	Slovak
" Translated By:	Martin Lacko <lacko@host.sk>
" Last Change:		2001 Aug 2

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-2

" Help menu
menutrans &Help &Pomocník
menutrans &Overview<Tab><F1>	&Prehµad<Tab><F1>
menutrans &How-to\ links	&Tipy
menutrans &GUI        &Grafické\ rozhranie
menutrans &Credits		Poï&akovanie
menutrans Co&pying		&Licencia
menutrans &Find\.\.\.	&Hµada»\.\.\.
menutrans &Version		&Verzia
menutrans &About		  &O\ programe

" File menu
menutrans &File				&Súbor
menutrans &Open\.\.\.<Tab>:e		&Otv&oi»\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	Ot&voi»\ v\ no&vom\ okne\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nový<Tab>:enew
menutrans &Close<Tab>:q			&Zav&ori»<Tab>:q
menutrans &Save<Tab>:w			&Ulo¾i»<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	Ul¾i» &ako\.\.\.<Tab>:w
menutrans &Print			&Tlaè
menutrans Sa&ve-Exit<Tab>:wqa		U&lo¾i»-Koniec<Tab>:wqa
menutrans E&xit<Tab>:qa			&Koniec<Tab>:qa

" Edit menu
menutrans &Edit				&Úpravy
menutrans &Undo<Tab>u			&Spä»<Tab>u
menutrans &Redo<Tab>^R			Z&ru¹i»\ spä»<Tab>^R
menutrans Rep&eat<Tab>\.		&Opakova»<Tab>\.
menutrans Cu&t<Tab>"+x			&Vystrihnú»<Tab>"+x
menutrans &Copy<Tab>"+y			&Kopírova»<Tab>"+y
menutrans &Paste<Tab>"+p		V&lo¾i»<Tab>"+p
menutrans Put\ &Before<Tab>[p		Vlo¾i»\ &pred<Tab>[p
menutrans Put\ &After<Tab>]p		Vlo¾i»\ za<Tab>]p
menutrans &Select\ all<Tab>ggVG		Vy&bra»\ v¹etko<Tab>ggVG
menutrans &Find\.\.\.			&Hµada»\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.	&Nahradi»\.\.\.
menutrans Options\.\.\.			Mo¾nosti\.\.\.

" Programming menu
menutrans &Tools			&Nástroje
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Skoèi»\ na\ znaèku<Tab>g^]
menutrans Jump\ &back<Tab>^T		Skoèi»\ &s&pä»<Tab>^T
menutrans Build\ &Tags\ File		&Vytvori»\ súbor\ tagov
menutrans &Make<Tab>:make		&Make<Tab>:make
menutrans &List\ Errors<Tab>:cl		Výpis\ &chýb<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	Výp&is\ správ<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		Ïa&l¹ia\ chyba<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	&P&redchádzajúca\ chyba<Tab>:cp
menutrans &Older\ List<Tab>:cold	Sta&r¹í\ zoznam<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	N&ov¹í\ zoznam<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	Chybové\ o&kno<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd Pr&vies»\ do\ ¹estnástkového\ formátu<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r Previe&s»\ spä»<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Buffery
menutrans Refresh Obnovi»
menutrans Delete	Vymaza»
menutrans Alternate	Zmeni»
menutrans [No\ File]	[®iadny\ súbor]

" Window menu
menutrans &Window			&Okná
menutrans &New<Tab>^Wn			&Nové<Tab>^Wn
menutrans S&plit<Tab>^Ws		&Rozdeli»<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	Ro&zdeli»\ na\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Rozdeli»\ &vertikálne<Tab>^Wv
menutrans &Close<Tab>^Wc		Zatvor&i»<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Zatvori»\ i&né<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			Ï&al¹ie<Tab>^Ww
menutrans P&revious<Tab>^WW		&Predchádzajúce<Tab>^WW
menutrans &Equal\ Size<Tab>^W=		Rovnaká\ vý¹&ka<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&Maximálna\ vý¹ka<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		M&inimální\ vý¹ka<Tab>^W1_
menutrans Max\ Width<Tab>^W\| Maximálna\ ¹írka<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|  Minimálna ¹irka<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		R&otova»\ nahor<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotova»\ na&dol<Tab>^Wr
menutrans Select\ Fo&nt\.\.\.		Vy&bra»\ písmo\.\.\.

" The popup menu
menutrans &Undo			&Spä»
menutrans Cu&t      &Vystrihnú»
menutrans &Copy			&Kopírova»
menutrans &Paste		&Vlo¾i»
menutrans &Delete		V&ymaza»
menutrans Select\ Blockwise	Vybra»\ blokovo
menutrans Select\ &Word		Vybra»\ &slovo
menutrans Select\ &Line		Vybra»\ &riadok
menutrans Select\ &Block	Vybra»\ &blok
menutrans Select\ &All		Vybra»\ &v¹etko

" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Otvori» súbor
    tmenu ToolBar.Save		Ulo¾i» súbor
    tmenu ToolBar.SaveAll		Ulo¾i» v¹etky 
    tmenu ToolBar.Print		Tlaè
    tmenu ToolBar.Undo		Spä»
    tmenu ToolBar.Redo		Zru¹i» vrátenie
    tmenu ToolBar.Cut		Vystrihnú»
    tmenu ToolBar.Copy		Kopírova»
    tmenu ToolBar.Paste		Vlo¾i»
    tmenu ToolBar.Find		Hµada»...
    tmenu ToolBar.FindNext	Hµada» ïal¹ie
    tmenu ToolBar.FindPrev	Hµada» predchádzajúce
    tmenu ToolBar.Replace		Nahradi»...
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New		Nové okno
      tmenu ToolBar.WinSplit	Rozdeli» okno
      tmenu ToolBar.WinMax		Maximalizova» okno
      tmenu ToolBar.WinMin		Minimalizova» okno
      tmenu ToolBar.WinClose	Zatvori» okno
    endif
    tmenu ToolBar.LoadSesn	Naèíta» sedenie
    tmenu ToolBar.SaveSesn	Ulo¾i» sedenie
    tmenu ToolBar.RunScript	Spustit skript
    tmenu ToolBar.Make		Spusti» make
    tmenu ToolBar.Shell		Spusti» shell
    tmenu ToolBar.RunCtags	Spusti» ctags
    tmenu ToolBar.TagJump		Skoèi» na tag pod kurzorom
    tmenu ToolBar.Help		Pomocník
    tmenu ToolBar.FindHelp	Najájs» pomocníka k...
  endfun
endif

" Syntax menu
menutrans &Syntax &Syntax
menutrans Set\ 'syntax'\ only	Nastavi»\ iba\ 'syntax'
menutrans Set\ 'filetype'\ too	Nastavi»\ aj\ 'filetype'
menutrans &Off			&Vypnú»
menutrans &Manual		&Ruène;
menutrans A&utomatic		A&utomaticky
menutrans o&n\ (this\ file)	&zapnú»\ (pre\ tento\ súbor)
menutrans o&ff\ (this\ file)	vyp&nú»\ (pre\ tento\ súbor )
menutrans Co&lor\ test		Test\ &farieb
menutrans &Highlight\ test	&Test\ zvýrazòovania
menutrans &Convert\ to\ HTML	&Previes»\ do\ HTML
