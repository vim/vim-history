" Menu Translations:	Français
" Maintainer:		François Thunus (thunus@systran.fr)
" Last Change:	2000 Aug 19

menutrans clear
let did_menu_trans = 1

" Help menu
menutrans &Help			&Aide
menutrans &Overview<Tab><F1>	A&perçu<Tab><F1>
menutrans &How-to\ links	&How-to\ Index
"menutrans &GUI			&GUI
menutrans &Credits		&Remerciements
menutrans Co&pying		C&opyright
menutrans &Find\.\.\.		&Chercher\.\.\.
menutrans &Version		&Version
menutrans &About		A\ p&ropos

" File menu
menutrans &File				&Fichier
menutrans &Open\.\.\.<Tab>:e		&Ouvrir\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	&Dans\ nouvelle\ Fenêtre\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nouveau<Tab>:enew
menutrans &Close<Tab>:q			F&ermer<Tab>:q
menutrans &Save<Tab>:w			&Sauver<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:w	Sauver\ &comme\.\.\.<Tab>:w
menutrans &Print			&Imprimer
menutrans Sa&ve-Exit<Tab>:wqa		Sauve&r\ et\ quitter<Tab>:wqa
menutrans E&xit<Tab>:qa			&Quitter<Tab>:qa

" Edit menu
menutrans &Edit				&Editer
menutrans &Undo<Tab>u			&Défaire<Tab>u
menutrans &Redo<Tab>^R			Re&faire<Tab>^R
menutrans Repea&t<Tab>\.		&Répéter<Tab>\.
menutrans Cu&t<Tab>"*x			&Couper<Tab>"*x
menutrans &Copy<Tab>"*y			C&opier<Tab>"*y
menutrans &Paste<Tab>"*p		Co&ller<Tab>"*p
menutrans Put\ &Before<Tab>[p		&Mettre\ devant<Tab>[p
menutrans Put\ &After<Tab>]p		Me&ttre\ derriere<Tab>]p
menutrans &Select\ all<Tab>ggVG		&Marquer\ Tout<Tab>ggVG
menutrans &Find\.\.\.			&Chercher\.\.\.
menutrans Find\ and\ R&eplace\.\.\.	Chercher\ et\ Rem&placer\.\.\.
menutrans Options\.\.\.			Options\.\.\.

" Programming menu
menutrans &Tools			&Outils
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Sauter\ vers\ Tag<Tab>g^]
menutrans Jump\ &back<Tab>^T		Sauter\ en\ &arrière<Tab>^T
menutrans Build\ &Tags\ File		Générer\ Fichier\ &Tags\
menutrans &Make<Tab>:make		Lancer\ &Make<Tab>:make
menutrans &List\ Errors<Tab>:cl		Liste\ des\ &Erreurs<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	Liste\ des\ &Messages<Tab>:cl!
menutrans &Next\ Error<Tab>:cn	Erreur\ S&uivante<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	Erreur\ &Precedente<Tab>:cp
menutrans &Older\ List<Tab>:cold	&Vielle\ Liste<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	&Nouvelle\ Liste<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	&Fenêtre\ des\ erreurs<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	Convertir\ vers\ HEX<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	Convertir\ &vers\ l'arrière<Tab>:%!xxd\ -r

" Names for buffer menu.
menutrans &Buffers	&Buffer
menutrans Refresh	Rafraîchir
menutrans Delete	Effacer
menutrans Alternate	Autre
menutrans [No\ File]	[Pas\ de\ Fichier]

" Window menu
menutrans &Window			&Fenêtre
menutrans &New<Tab>^Wn			&Nouveau<Tab>^Wn
menutrans S&plit<Tab>^Ws		Couper\ écran<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	couper\ vers\ #<Tab>^W^^
menutrans S&plit\ Vertically<Tab>^Wv	C&ouper\ Verticalement<Tab>^Wv
menutrans &Close<Tab>^Wc		&Fermer<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Fermer\ &Autre(s)<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			Suivant<Tab>^Ww
menutrans P&revious<Tab>^WW		&Précedent<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	&Même\ hauteur<Tab>^W=
menutrans &Max\ Height<Tab>^W_		&hauteur\ maximale<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		hauteur\ Mi&nimale<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		largeur\ maximale<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		largeur\ minimale<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		Rotation\ vers\ le\ &haut<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotation\ vers\ le\ &bas<Tab>^Wr
menutrans Select\ &Font\.\.\.		Choisir\ police\.\.\.

" The popup menu
menutrans &Undo		        &Défaire
menutrans Cu&t			&Couper
menutrans &Copy			C&opier
menutrans &Paste		Co&ller
menutrans &Delete		&Effacer
menutrans Select\ Blockwise 	Choisir\ par\ Bloc
menutrans Select\ &Word		Choisir\ un\ &mot
menutrans Select\ &Line		Choisir\ une\ &line
menutrans Select\ &Block	Choisir\ un\ &bloc
menutrans Select\ &All		Choisir\ &tout
 
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Ouvrir fichier
    tmenu ToolBar.Save		Sauver fichier
    tmenu ToolBar.SaveAll	Sauver tous les fichiers
    tmenu ToolBar.Print		Imprimer
    tmenu ToolBar.Undo		Défaire
    tmenu ToolBar.Redo		Refaire
    tmenu ToolBar.Cut		Couper
    tmenu ToolBar.Copy		Copier
    tmenu ToolBar.Paste		Coller
    tmenu ToolBar.Find		Chercher...
    tmenu ToolBar.FindNext	Chercher suivant
    tmenu ToolBar.FindPrev	Chercher Précédent
    tmenu ToolBar.Replace	Chercher et remplacer...
    tmenu ToolBar.LoadSesn	Charger session
    tmenu ToolBar.SaveSesn	Sauver session
    tmenu ToolBar.RunScript	lancer un script
    tmenu ToolBar.Make		lancer Make 
    tmenu ToolBar.Shell		lancer un Shell
    tmenu ToolBar.RunCtags	générer un fichier Tags
    tmenu ToolBar.TagJump	Aller a un tag
    tmenu ToolBar.Help		Aide!
    tmenu ToolBar.FindHelp	Chercher dans l'aide...
  endfun
endif

" Syntax menu
menutrans &Syntax		&Syntaxe
menutrans Set\ 'syntax'\ only	activer\ uniquement\ 'syntaxe'
menutrans Set\ 'filetype'\ too	activer\ aussi\ 'filetype'
menutrans &Off			st&op
menutrans &Manual		&Manuel
menutrans A&utomatic		A&utomatique
menutrans &on\ (this\ file)	A&ctiver\ (ce\ fichier)
menutrans o&ff\ (this\ file)	&désactiver\ (ce\ fichier)
menutrans Co&lor\ test		Test\ des\ &Couleurs
menutrans &Highlight\ test	Test\ de\ Marcage
menutrans &Convert\ to\ HTML	Convertir\ en\ &HTML
