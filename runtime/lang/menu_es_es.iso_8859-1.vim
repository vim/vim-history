" Menu Translations:	Español
" Maintainer:		Eduardo F. Amatria <eferna1@platea.pntic.mec.es>
" Last Change:	2001 Apr 17

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding iso-8859-1

" Help menu
menutrans &Help			Ay&uda
menutrans &Overview<Tab><F1>	&Principal<Tab><F1>
menutrans &How-to\ links	&Enlaces\ a\ ¿Cómo\.\.\.?
menutrans &GUI			&Interfaz\ gráfica
menutrans &Credits		&Reconocimientos
menutrans Co&pying		&Copyright
menutrans &Find\.\.\.		&Buscar\.\.\.
menutrans &Version		&Versión
menutrans &About		&Acerca\ de\.\.\.

" File menu
menutrans &File				&Archivo
menutrans &Open\.\.\.<Tab>:e		&Abrir\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	A&brir\ en\ otra\ ventana\.\.\.<Tab>:sp
menutrans &New<Tab>:enew		&Nuevo<Tab>:enew
menutrans &Close<Tab>:q			&Cerrar<Tab>:q
menutrans &Save<Tab>:w			&Guardar<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav	Guardar\ &como\.\.\.<Tab>:sav
menutrans Show\ &Diff\ with\.\.\.	&Mostrar\ diferencias\ con\.\.\.
menutrans &Print			&Imprimir
menutrans Sa&ve-Exit<Tab>:wqa		Gua&rdar\ y\ salir<Tab>:wqa
menutrans E&xit<Tab>:qa			&Salir<Tab>:qa

" Edit menu
menutrans &Edit				&Editar
menutrans &Undo<Tab>u			&Deshacer<Tab>u
menutrans &Redo<Tab>^R			&Rehacer<Tab>^R
menutrans Rep&eat<Tab>\.                Repe&tir<Tab>\.
menutrans Cu&t<Tab>"+x			Cor&tar<Tab>"+x
menutrans &Copy<Tab>"+y			&Copiar<Tab>"+y
menutrans &Paste<Tab>"+p		&Pegar<Tab>"+p
menutrans Put\ &Before<Tab>[p		Poner\ &antes<Tab>[p
menutrans Put\ &After<Tab>]p		Poner\ &después<Tab>]p
if has("win32") || has("win16")
  menutrans &Delete<Tab>x		S&uprimir<Tab>x
endif
menutrans &Select\ all<Tab>ggVG		&Seleccionar\ todo<Tab>ggVG
menutrans &Find\.\.\.			&Buscar\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.     Buscar\ y\ R&eemplazar\.\.\.
menutrans Settings\ &Window		&Ventana\ de\ opciones
menutrans Se&ttings			&Opciones

" Build boolean options
menutrans Toggle\ Line\ Numbering<TAB>:set\ number!	Activar/Desactivar\ numeración\ de\ líneas<TAB>:set\ number!
menutrans Toggle\ Line\ Wrap<TAB>:set\ wrap!		Activar/Desactivar\ doblado\ de\ líneas<TAB>:set\ wrap!	
menutrans Toggle\ hlsearch<TAB>:set\ hlsearch!		Activar/Desactivar\ búsqueda\ realzada<TAB>:set\ hlsearch!	
menutrans Toggle\ expandtab<TAB>:set\ expandtab!	Activar/Desactivar\ expansión\ de\ tabs<TAB>:set\ expandtab!	

" Build GUI options
menutrans Toggle\ Toolbar		Activar/Desactivar\ barra\ de\ herramientas
menutrans Toggle\ Bottom\ Scrollbar	Activar/Desactivar\ barra\ de\ desplazamiento\ inferior
menutrans Toggle\ Left\ Scrollbar	Activar/Desactivar\ barra\ de\ desplazamiento\ izquierda
menutrans Toggle\ Right\ Scrolbar	Activar/Desactivar\ barra\ de\ desplazamiento\ derecha
" Build variable options
menutrans Text\ Width\.\.\. 		Anchura\ del\ texto\.\.\. 
menutrans Shiftwidth	 		Anchura\ del\ sangrado

" Programming menu
menutrans &Tools			&Herramientas
menutrans &Jump\ to\ this\ tag<Tab>g^]	&Saltar\ a\ este\ «tag»<Tab>g^]
menutrans Jump\ &back<Tab>^T		Saltar\ &atrás<Tab>^T
menutrans Build\ &Tags\ File		&Generar\ fichero\ de\ «tags»\
menutrans &Folding			&Plegado
menutrans &Make<Tab>:make		Ejecutar\ «&Make»<Tab>:make
menutrans &List\ Errors<Tab>:cl		&Lista\ de\ errores<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	L&ista\ de\ mensajes<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		&Error\ siguiente<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	Error\ p&revio<Tab>:cp
menutrans &Older\ List<Tab>:cold	Lista\ de\ &viejos\ a\ nuevos<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	Lista\ de\ &nuevos\ a\ viejos<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	Ven&tana\ de\ errores<Tab>:cwin
menutrans Convert\ to\ HEX<Tab>:%!xxd	Convertir\ a\ &HEX<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	&Convertir\ al\ anterior<Tab>:%!xxd\ -r

" Tools.Fold Menu
menutrans &Enable/Disable\ folds<TAB>zi		&Activar/Desactivar\ pliegues<TAB>zi
menutrans &View\ Cursor\ Line<TAB>zv		&Ver\ línea\ del\ cursor<TAB>zv
menutrans Vie&w\ Cursor\ Line\ only<TAB>zMzx	Ve&r\ sólo\ la\ línea\ del\ cursor<TAB>zMzx
menutrans C&lose\ more\ folds<Tab>zm		C&errar\ más\ pliegues<Tab>zm
menutrans &Close\ all\ folds<Tab>zM		&Cerrar\ todos\ los\ pliegues<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr		Abrir\ &más\ pliegues<Tab>zr
menutrans &Open\ all\ folds<Tab>zR		&Abrir\ todos\ los\ pliegues<Tab>zR
" fold method
menutrans Fold\ Met&hod				&Método\ de\ plegado
" create and delete folds
menutrans Create\ &Fold<TAB>zf			Crear\ &pliegue<TAB>zf
menutrans &Delete\ Fold<TAB>zd			&Suprimir\ pliegue<TAB>zd
menutrans Delete\ &All\ Folds<TAB>zD		Suprimir\ &todos\ los\ pligues<TAB>zD
" moving around in folds
menutrans Fold\ column\ &width			A&nchura\ de\ columna\ del\ pliegue

" Names for buffer menu.
menutrans &Buffers		&Buffers
menutrans &Refresh\ menu	&Refrescar\ menú
menutrans &Delete		&Suprimir
menutrans A&lternate		&Alternar
menutrans &Next			Si&guiente
menutrans &Previous		&Previo 
menutrans [No File]    		[Sin fichero]

" Window menu
menutrans &Window			&Ventana
menutrans &New<Tab>^Wn			&Nueva<Tab>^Wn
menutrans S&plit<Tab>^Ws		&Dividir<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	D&ividir\ a\ #<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv    Dividir\ &verticalmente<Tab>^Wv
menutrans Split\ File\ E&xplorer	&Abrir\ Explorador\ de\ ficheros
menutrans &Close<Tab>^Wc		&Cerrar<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	Cerrar\ &otra(s)<Tab>^Wo
menutrans Move\ &To			Mov&er\ a
menutrans &Top<Tab>^WK			&Arriba<Tab>^WK		
menutrans &Bottom<Tab>^WJ		A&bajo<Tab>^WJ		
menutrans &Left\ side<Tab>^WH		Lado\ &izquierdo<Tab>^WH		
menutrans &Right\ side<Tab>^WL		Lado\ &derecho<Tab>^WL		
" menutrans Ne&xt<Tab>^Ww		&Siguiente<Tab>^Ww
" menutrans P&revious<Tab>^WW		&Previa<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	&Misma\ altura<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Altura\ &máxima<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Altura\ mí&nima<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Anchura\ máxima<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Anchura\ mínima<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		&Rotar\ hacia\ arriba<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotar\ hacia\ a&bajo<Tab>^Wr
menutrans Select\ &Font\.\.\.		Seleccionar\ &fuente\.\.\.

" The popup menu
menutrans &Undo		        &Deshacer
menutrans Cu&t			Cor&tar
menutrans &Copy			&Copiar
menutrans &Paste		&Pegar
menutrans &Delete		&Borrar
menutrans Select\ Blockwise 	Seleccionar\ por\ bloque
menutrans Select\ &Word		Seleccionar\ &palabra
menutrans Select\ &Line		Seleccionar\ una\ &línea
menutrans Select\ &Block	Seleccionar\ un\ &bloque
menutrans Select\ &All		Seleccionar\ &todo
 
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		Abrir fichero 
    tmenu ToolBar.Save		Guardar fichero
    tmenu ToolBar.SaveAll	Guardar todos los ficheros
    tmenu ToolBar.Print		Imprimir
    tmenu ToolBar.Undo		Deshacer
    tmenu ToolBar.Redo		Rehacer
    tmenu ToolBar.Cut		Cortar
    tmenu ToolBar.Copy		Copiar
    tmenu ToolBar.Paste		Pegar
    tmenu ToolBar.Find		Buscar...
    tmenu ToolBar.FindNext	Buscar siguiente 
    tmenu ToolBar.FindPrev	Buscar precedente
    tmenu ToolBar.Replace	Buscar y reemplazar
    tmenu ToolBar.LoadSesn	Cargar sesión
    tmenu ToolBar.SaveSesn	Guardar sesión
    tmenu ToolBar.RunScript	Ejecutar un «script»
    tmenu ToolBar.Make		Ejecutar «Make»
    tmenu ToolBar.Shell		Abrir una «Shell»
    tmenu ToolBar.RunCtags	Generar un fichero de «tags»
    tmenu ToolBar.TagJump	Saltar a un «tag»
    tmenu ToolBar.Help		Ayuda
    tmenu ToolBar.FindHelp	Buscar en la ayuda...
  endfun
endif

" Syntax menu
menutrans &Syntax		&Sintaxis
menutrans Set\ 'syntax'\ only	Activar\ sólo\ «sintaxis»
menutrans Set\ 'filetype'\ too	Activar\ también\ «tipo\ de\ fichero»
menutrans &Off			&Desactivar
menutrans &Manual		&Manual
menutrans A&utomatic		A&utomática
menutrans o&n\ (this\ file)	&Activar\ (en\ este\ fichero)
menutrans o&ff\ (this\ file)	D&esactivar (en\ este\ fichero)
menutrans Co&lor\ test		&Prueba\ del\ color
menutrans &Highlight\ test	Prueba\ del\ &realzado
menutrans &Convert\ to\ HTML	&Convertir\ en\ HTML
