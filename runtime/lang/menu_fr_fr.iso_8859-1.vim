" Menu Translations:	Français
" Maintainer:		Adrien Beau <adrien.beau@free.fr>
" First Version:	François Thunus <thunus@systran.fr>
" Last Change:		2001 Aug 31

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-1


menutrans &Help				&Aide

menutrans &Overview<Tab><F1>			&Sommaire<Tab><F1>
menutrans &User\ Manual				&Manuel\ utilisateur
menutrans &How-to\ links			&Tâches\ courantes
menutrans &GUI					&Interface
menutrans &Credits				&Remerciements
menutrans Co&pying				&License
menutrans &Find\.\.\.				Rec&hercher\.\.\.
" -sep-
menutrans &Version				&Version
menutrans &About				À\ &propos\ de\ Vim

let g:menutrans_help_dialog = "Entrez une commande ou un mot à rechercher dans l'aide.\n\nAjoutez i_ pour les commandes du mode Insertion (ex: i_CTRL-X)\nAjoutez c_ pour l'édition de la ligne de commande (ex: c_<Del>)\nEntourez les options avec des apostrophes (ex: 'shiftwidth')"


menutrans &File				&Fichier

menutrans &Open\.\.\.<Tab>:e			&Ouvrir\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp		Ouvrir\ sép&arément\.\.\.<Tab>:sp
menutrans &New<Tab>:enew			&Nouveau<Tab>:enew
menutrans &Close<Tab>:close			&Fermer<Tab>:close
" -SEP1-
menutrans &Save<Tab>:w				&Enregistrer<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav		Enregistrer\ &sous\.\.\.<Tab>:sav
" -SEP2-
menutrans Split\ &Diff\ with\.\.\.		&Difference\ avec\.\.\.
menutrans Split\ Patched\ &By\.\.\.		&Patcher\ avec\.\.\.
" -SEP3-
menutrans &Print				&Imprimer
" -SEP4-
menutrans Sa&ve-Exit<Tab>:wqa			En&registrer\ et\ quitter<Tab>:wqa
menutrans E&xit<Tab>:qa				&Quitter<Tab>:qa


menutrans &Edit				&Edition

menutrans &Undo<Tab>u				&Annuler<Tab>u
menutrans &Redo<Tab>^R				Re&faire<Tab>^R
menutrans Rep&eat<Tab>\.			R&épéter<Tab>\.
" -SEP1-
menutrans Cu&t<Tab>"+x				Co&uper<Tab>"+x
menutrans &Copy<Tab>"+y				Cop&ier<Tab>"+y
menutrans &Paste<Tab>"+P			C&oller<Tab>"+P
menutrans Put\ &Before<Tab>[p			Placer\ a&vant<Tab>[p
menutrans Put\ &After<Tab>]p			Placer\ apr&ès<Tab>]p
menutrans &Delete<Tab>x				Effa&cer<Tab>x
menutrans &Select\ all<Tab>ggVG			&Sélectionner\ tout<Tab>ggVG
" -SEP2-
menutrans &Find\.\.\.				Rec&hercher\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.		Re&mplacer\.\.\.
menutrans &Find<Tab>/				Rec&hercher<Tab>/
menutrans Find\ and\ Rep&lace<Tab>:%s		Re&mplacer<Tab>:%s
menutrans Find\ and\ Rep&lace<Tab>:s		Re&mplacer<Tab>:s
" -SEP3-
menutrans Settings\ &Window			Fe&nêtre\ des\ réglages
menutrans &Global\ Settings			Réglages\ globau&x

menutrans Toggle\ Pattern\ &Highlight<Tab>:set\ hls!	Basculer\ &surbrillance\ motif<Tab>:set\ hls!
menutrans Toggle\ &Ignore-case<Tab>:set\ ic!		Basculer\ &ignorer\ casse<Tab>:set\ ic!
menutrans Toggle\ &Showmatch<Tab>:set\ sm!		Basculer\ montrer\ &correspondance<Tab>:set\ sm!

menutrans &Context\ lines				Lignes\ &autour\ du\ curseur

menutrans &Virtual\ Edit				Édition\ &virtuelle
menutrans Never							&Jamais
menutrans Block\ Selection					&Sélection\ en\ bloc
menutrans Insert\ mode						&Mode\ insertion
menutrans Block\ and\ Insert					&Bloc\ et\ insertion
menutrans Always						&Toujours

menutrans Toggle\ Insert\ &Mode<Tab>:set\ im!		Basculer\ i&nsertion\ permanente<Tab>:set\ im!
menutrans Search\ &Path\.\.\.				Chemin\ de\ rec&herche\ des\ fichiers\.\.\.
menutrans Ta&g\ Files\.\.\.				Fichiers\ d'&étiquettes\.\.\.
" -SEP1-
menutrans Toggle\ &Toolbar				Basculer\ barre\ d'&outils
menutrans Toggle\ &Bottom\ Scrollbar			Basculer\ défilement\ en\ &bas
menutrans Toggle\ &Left\ Scrollbar			Basculer\ défilement\ à\ ga&uche
menutrans Toggle\ &Right\ Scrollbar			Basculer\ défilement\ à\ &droite

let g:menutrans_path_dialog = "Entrez le chemin de recherche des fichiers.\nSéparez les répertoires par des virgules."
let g:menutrans_tags_dialog = "Entrez les noms des fichiers d'étiquettes.\nSéparez les noms par des virgules."

menutrans F&ile\ Settings			Réglages\ fichie&r

menutrans Toggle\ Line\ &Numbering<Tab>:set\ nu!	Basculer\ &numérotation<Tab>:set\ nu!
menutrans Toggle\ &List\ Mode<Tab>:set\ list!		Basculer\ mode\ &listing<Tab>:set\ list!
menutrans Toggle\ Line\ &Wrap<Tab>:set\ wrap!		Basculer\ &retour\ ligne<Tab>:set\ wrap!
menutrans Toggle\ W&rap\ at\ word<Tab>:set\ lbr!	Basculer\ retour\ sur\ &mot<Tab>:set\ lbr!
menutrans Toggle\ &expand-tab<Tab>:set\ et!		Basculer\ &tab\.\ en\ espaces<Tab>:set\ et!
menutrans Toggle\ &auto-indent<Tab>:set\ ai!		Basculer\ indentation\ &auto<Tab>:set\ ai!
menutrans Toggle\ &C-indenting<Tab>:set\ cin!		Basculer\ indent\.\ lang\.\ &C<Tab>:set\ cin!
" -SEP2-
menutrans &Shiftwidth					Largeur\ in&dentation
menutrans Soft\ &Tabstop				&Pseudo-tabulations
menutrans Te&xt\ Width\.\.\.				Largeur\ du\ te&xte\.\.\.
menutrans &File\ Format\.\.\.				Format\ du\ &fichier\.\.\.

let g:menutrans_textwidth_dialog = "Entrez la nouvelle largeur du texte\n(0 pour désactiver le formattage)."
let g:menutrans_fileformat_dialog = "Choisissez le format dans lequel écrire le fichier."

menutrans C&olor\ Scheme			&Jeu\ de\ couleurs
menutrans &Keymap				&Type\ de\ clavier
menutrans None						(aucun)
menutrans Select\ Fo&nt\.\.\.			Sélectionner\ &police\.\.\.


menutrans &Tools			&Outils

menutrans &Jump\ to\ this\ tag<Tab>g^]		&Atteindre\ cette\ étiquette<Tab>g^]
menutrans Jump\ &back<Tab>^T			Repartir\ en\ arri&ère<Tab>^T
menutrans Build\ &Tags\ File			&Générer\ fichier\ d'étiquettes

" -SEP1-
menutrans &Folding				&Replis

menutrans &Enable/Disable\ folds<Tab>zi			Dés/&Activer\ replis<Tab>zi
menutrans &View\ Cursor\ Line<Tab>zv			Déplier\ ligne\ &curseur<Tab>zv
menutrans Vie&w\ Cursor\ Line\ only<Tab>zMzx		Tout\ plier\ &sauf\ ligne\ curseur<Tab>zMzx
menutrans C&lose\ more\ folds<Tab>zm			&Fermer\ plus\ de\ replis<Tab>zm
menutrans &Close\ all\ folds<Tab>zM			F&ermer\ tous\ les\ replis<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr			&Ouvrir\ plus\ de\ replis<Tab>zr
menutrans &Open\ all\ folds<Tab>zR			O&uvrir\ tous\ les\ replis<Tab>zR
" -SEP1-
menutrans Fold\ Met&hod					&Méthode\ de\ repli

menutrans M&anual						&Manuelle
menutrans I&ndent						&Indentation
menutrans E&xpression						&Expression
menutrans S&yntax						&Syntaxe
menutrans &Diff							&Différence
menutrans Ma&rker						Ma&rqueurs

menutrans Create\ &Fold<Tab>zf				Créer\ &repli<Tab>zf
menutrans &Delete\ Fold<Tab>zd				E&ffacer\ repli<Tab>zd
menutrans Delete\ &All\ Folds<Tab>zD			Effacer\ &tous\ les\ replis<Tab>zD
" -SEP2-
menutrans Fold\ column\ &width				&Largeur\ colonne\ de\ repli

menutrans &Diff					&Différence

menutrans &Update					&Mettre\ à\ jour
menutrans &Get\ Block					Corriger\ &ce\ tampon
menutrans &Put\ Block					Corriger\ l'&autre\ tampon

" -SEP2-
menutrans &Make<Tab>:make			Lancer\ ma&ke<Tab>:make
menutrans &List\ Errors<Tab>:cl			Lister\ &erreurs<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!		Lister\ &messages<Tab>:cl!
menutrans &Next\ Error<Tab>:cn			Erreur\ &suivante<Tab>:cn
menutrans &Previous\ Error<Tab>:cp		Erreur\ pr&écédente<Tab>:cp
menutrans &Older\ List<Tab>:cold		A&ncienne\ liste<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew		No&uvelle\ liste<Tab>:cnew

menutrans Error\ &Window			&Fenêtre\ d'erreurs

menutrans &Update<Tab>:cwin				&Mettre\ à\ jour<Tab>:cwin
menutrans &Open<Tab>:copen				&Ouvrir<Tab>:copen
menutrans &Close<Tab>:cclose				&Fermer<Tab>:cclose

" -SEP3-
menutrans &Convert\ to\ HEX<Tab>:%!xxd		Convertir\ en\ he&xa<Tab>:%!xxd
menutrans Conve&rt\ back<Tab>:%!xxd\ -r		Décon&vertir<Tab>:%!xxd\ -r

menutrans &Set\ Compiler			Type\ de\ &compilateur


menutrans &Buffers			&Tampons

menutrans Dummy					Plom
menutrans &Refresh\ menu			&Mettre\ menu\ à\ jour
menutrans &Delete				S&upprimer
menutrans &Alternate				&Alterner
menutrans &Next					&Suivant
menutrans &Previous				&Précédent
" -SEP-

let g:menutrans_no_file = "[Aucun fichier]"


menutrans &Window			Fe&nêtre

menutrans &New<Tab>^Wn				&Nouvelle\ fenêtre<Tab>^Wn
menutrans S&plit<Tab>^Ws			&Fractionner<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^		Fractionner\ p&our\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv		Fractionner\ &verticalement<Tab>^Wv
menutrans Split\ File\ E&xplorer		Fractionner\ &explorateur
" -SEP1-
menutrans &Close<Tab>^Wc			Fer&mer<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo		Fermer\ les\ &autres<Tab>^Wo
" -SEP2-
menutrans Move\ &To				&Déplacer\ vers\ le

menutrans &Top<Tab>^WK					&Haut<Tab>^WK
menutrans &Bottom<Tab>^WJ				&Bas<Tab>^WJ
menutrans &Left\ side<Tab>^WH				Côté\ &gauche<Tab>^WH
menutrans &Right\ side<Tab>^WL				Côté\ &droit<Tab>^WL

menutrans Rotate\ &Up<Tab>^WR			Rotation\ vers\ le\ &haut<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr			Rotation\ vers\ le\ &bas<Tab>^Wr
" -SEP3-
menutrans &Equal\ Size<Tab>^W=			Égaliser\ ta&illes<Tab>^W=
menutrans &Max\ Height<Tab>^W_			Hau&teur\ maximale<Tab>^W_
menutrans M&in\ Height<Tab>^W1_			Ha&uteur\ minimale<Tab>^W1_
menutrans Max\ &Width<Tab>^W\|			&Largeur\ maximale<Tab>^W\|
menutrans Min\ Widt&h<Tab>^W1\|			La&rgeur\ minimale<Tab>^W1\|


" PopUp

menutrans &Undo					&Annuler
" -SEP1-
menutrans Cu&t					Co&uper
menutrans &Copy					Cop&ier
menutrans &Paste				C&oller
menutrans &Delete				Effa&cer
" -SEP2-
menutrans Select\ Blockwise			Sélectionner\ &bloc
menutrans Select\ &Word				Sélectionner\ &mot
menutrans Select\ &Line				Sélectionner\ &ligne
menutrans Select\ &Block			Sélectionner\ &bloc
menutrans Select\ &All				Sélectionner\ &tout


" ToolBar

menutrans Open					Ouvrir
menutrans Save					Sauver
menutrans SaveAll				SauverTout
menutrans Print					Imprimer
" -sep1-
menutrans Undo					Annuler
menutrans Redo					Refaire
" -sep2-
menutrans Cut					Couper
menutrans Copy					Copier
menutrans Paste					Coller
" -sep3-
menutrans Find					Chercher
menutrans FindNext				CherchSuiv
menutrans FindPrev				CherchPrec
menutrans Replace				Remplacer
" -sep4-
menutrans New					Nouvelle
menutrans WinSplit				FenFract
menutrans WinMax				FenMax
menutrans WinMin				FenMin
menutrans WinVSplit				FenVFract
menutrans WinMaxWidth				FenMaxLarg
menutrans WinMinWidth				FenMinLarg
menutrans WinClose				FenFerme
" -sep5-
menutrans LoadSesn				OuvreSess
menutrans SaveSesn				SauveSess
menutrans RunScript				LanceScript
" -sep6-
menutrans Make					Make
menutrans Shell					Shell
menutrans RunCtags				LanceCtags
menutrans TagJump				AllerEtiqu
" -sep7-
menutrans Help					Aide
menutrans FindHelp				CherchAide

fun! Do_toolbar_tmenu()
  let did_toolbar_tmenu = 1
  tmenu ToolBar.Open				Ouvrir fichier
  tmenu ToolBar.Save				Sauver fichier courant
  tmenu ToolBar.SaveAll				Sauver tous les fichiers
  tmenu ToolBar.Print				Imprimer
  tmenu ToolBar.Undo				Annuler
  tmenu ToolBar.Redo				Refaire
  tmenu ToolBar.Cut				Couper vers presse-papier
  tmenu ToolBar.Copy				Copier vers presse-papier
  tmenu ToolBar.Paste				Coller du presse-papier
  tmenu ToolBar.Find				Lancer une recherche
  tmenu ToolBar.FindNext			Correspondance suivante
  tmenu ToolBar.FindPrev			Correspondance précédente
  tmenu ToolBar.Replace				Lancer une substitution
 if 0	" disabled; These are in the Windows menu
  tmenu ToolBar.New				Nouvelle fenêtre
  tmenu ToolBar.WinSplit			Fractionner fenêtre
  tmenu ToolBar.WinMax				Maximiser fenêtre
  tmenu ToolBar.WinMin				Minimiser fenêtre
  tmenu ToolBar.WinVSplit			Fractionner verticalement
  tmenu ToolBar.WinMaxWidth			Maximiser largeur fenêtre
  tmenu ToolBar.WinMinWidth			Minimiser largeur fenêtre
  tmenu ToolBar.WinClose			Fermer fenêtre
 endif
  tmenu ToolBar.LoadSesn			Ouvrir session
  tmenu ToolBar.SaveSesn			Sauver session courante
  tmenu ToolBar.RunScript			Exécuter script Vim
  tmenu ToolBar.Make				Lancer make
  tmenu ToolBar.Shell				Ouvrir un terminal
  tmenu ToolBar.RunCtags			Créer étiquettes
  tmenu ToolBar.TagJump				Atteindre étiquette sous curseur
  tmenu ToolBar.Help				Aide de Vim
  tmenu ToolBar.FindHelp			Rechercher dans l'aide
endfun


menutrans &Syntax			&Syntaxe

menutrans &Manual				Activation\ &manuelle
menutrans A&utomatic				Activation\ &automatique
menutrans on/off\ for\ &This\ file		Dés/Activer\ pour\ &ce\ fichier

" -SEP1-
menutrans Set\ '&syntax'\ only			Régler\ seulement\ '&syntax'
menutrans Set\ '&filetype'\ too			Régler\ '&filetype'\ aussi
menutrans &Off					Arrêter\ c&oloration\ syntactique
" -SEP3-
menutrans Co&lor\ test				Tester\ les\ co&uleurs
menutrans &Highlight\ test			Tester\ les\ g&roupes\ syntactiques
menutrans &Convert\ to\ HTML			Con&vertir\ en\ HTML
