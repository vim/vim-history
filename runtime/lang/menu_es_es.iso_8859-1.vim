" Menu Translations:	Español
" Maintainer:		Eduardo F. Amatria <eferna1@platea.pntic.mec.es>
" Last Change:		2001 May 02

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
menutrans &Paste<Tab>"+P		&Pegar<Tab>"+P
menutrans Put\ &Before<Tab>[p		Poner\ &antes<Tab>[p
menutrans Put\ &After<Tab>]p		Poner\ &después<Tab>]p
if has("win32") || has("win16")
  menutrans &Delete<Tab>x		S&uprimir<Tab>x
endif
menutrans &Select\ all<Tab>ggVG		&Seleccionar\ todo<Tab>ggVG
menutrans &Find\.\.\.			&Buscar\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.     Buscar\ y\ R&eemplazar\.\.\.
menutrans Settings\ &Window		&Ventana\ de\ opciones

" Edit/Global Settings
menutrans Global\ Settings		Opciones\ globales

menutrans Toggle\ Pattern\ Highlight<Tab>:set\ hls!	Activar/Desactivar\ realzado\ de\ patrones<Tab>:set\ hls!
menutrans Toggle\ ignore-case<Tab>:set\ ic!		Activar/Desactivar\ ignorar\ la\ caja<Tab>:set\ ic!
menutrans Toggle\ showmatch<Tab>:set\ sm!		Activar/Desactivar\ mostrar\ coincidencias<Tab>:set\ sm!

menutrans Context\ lines		Líneas\ de\ contexto

menutrans Virtual\ Edit			Edición\ virtual
menutrans Never				Nunca
menutrans Block\ Selection		Selección\ de\ bloque
menutrans Insert\ mode			Modo\ insertar
menutrans Block\ and\ Insert		Bloque\ e\ insertar
menutrans Always			Siempre

menutrans Toggle\ Insert\ mode<Tab>:set\ im!	Activar/Desactivar\ modo\ de\ inserción<Tab>:set\ im!

menutrans Search\ Path\.\.\.		Ruta\ de\ búsqueda\.\.\.

menutrans Tag\ Files\.\.\.		Ficheros\ de\ «tags»\.\.\.

" GUI options
menutrans Toggle\ Toolbar		Ocultar/Mostrar\ barra\ de\ herramientas
menutrans Toggle\ Bottom\ Scrollbar	Ocultar/Mostrar\ barra\ de\ desplazamiento\ inferior
menutrans Toggle\ Left\ Scrollbar	Ocultar/Mostrar\ barra\ de\ desplazamiento\ izquierda
menutrans Toggle\ Right\ Scrolbar	Ocultar/Mostrar\ barra\ de\ desplazamiento\ derecha

" Edit/File Settings
menutrans File\ Settings		Opciones\ del\ fichero

" Boolean options
menutrans Toggle\ Line\ Numbering<Tab>:set\ nu!		Activar/Desactivar\ numeración\ de\ líneas<Tab>:set\ nu!
menutrans Toggle\ List\ Mode<Tab>:set\ list!		Activar/Desactivar\ modo\ «list»<Tab>:set\ list!
menutrans Toggle\ Line\ Wrap<Tab>:set\ wrap!		Activar/Desactivar\ doblado\ de\ líneas<Tab>:set\ wrap!	
menutrans Toggle\ Wrap\ at\ word<Tab>:set\ lbr!		Activar/Desactivar\ doblado\ en\ palabra<Tab>:set\ lbr!
menutrans Toggle\ expand-tab<Tab>:set\ et!		Activar/Desactivar\ expansión\ de\ tabs<Tab>:set\ et!	
menutrans Toggle\ auto-indent<Tab>:set\ ai!		Activar/Desactivar\ auto-sangrado<Tab>:set\ ai!
menutrans Toggle\ C-indenting<Tab>:set\ cin!		Activar/Desactivar\ sangrado\ C<Tab>:set\ cin!

" other options
menutrans Shiftwidth	 		Anchura\ del\ sangrado

menutrans Soft\ Tabstop			Tabulado\ suave

menutrans Text\ Width\.\.\. 		Anchura\ del\ texto\.\.\.

menutrans File\ Format\.\.\.		Formato\ del\ fichero\.\.\.
let g:menutrans_fileformat_dialog = "Seleccione el formato para escribir el fichero"

menutrans Color\ Scheme			Esquema\ de\ colores

menutrans Keymap	Asociación\ de\ teclas
menutrans None		Ninguna

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
menutrans &Enable/Disable\ folds<Tab>zi		&Activar/Desactivar\ pliegues<Tab>zi
menutrans &View\ Cursor\ Line<Tab>zv		&Ver\ línea\ del\ cursor<Tab>zv
menutrans Vie&w\ Cursor\ Line\ only<Tab>zMzx	Ve&r\ sólo\ la\ línea\ del\ cursor<Tab>zMzx
menutrans C&lose\ more\ folds<Tab>zm		C&errar\ más\ pliegues<Tab>zm
menutrans &Close\ all\ folds<Tab>zM		&Cerrar\ todos\ los\ pliegues<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr		Abrir\ &más\ pliegues<Tab>zr
menutrans &Open\ all\ folds<Tab>zR		&Abrir\ todos\ los\ pliegues<Tab>zR
" fold method
menutrans Fold\ Met&hod				&Método\ de\ plegado
" create and delete folds
menutrans Create\ &Fold<Tab>zf			Crear\ &pliegue<Tab>zf
menutrans &Delete\ Fold<Tab>zd			&Suprimir\ pliegue<Tab>zd
menutrans Delete\ &All\ Folds<Tab>zD		Suprimir\ &todos\ los\ pligues<Tab>zD
" moving around in folds
menutrans Fold\ column\ &width			A&nchura\ de\ columna\ del\ pliegue

menutrans &Update	&Actualizar
menutrans &Get\ Block	&Obtener\ bloque
menutrans &Put\ Block	&Poner\ bloque

" Names for buffer menu.
menutrans &Buffers		&Buffers
menutrans &Refresh\ menu	&Refrescar\ menú
menutrans &Delete		&Suprimir
menutrans A&lternate		&Alternar
menutrans &Next			Si&guiente
menutrans &Previous		&Previo 
let g:menutrans_no_file = "[Sin fichero]"

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
menutrans Rotate\ &Up<Tab>^WR		&Rotar\ hacia\ arriba<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		Rotar\ hacia\ a&bajo<Tab>^Wr
menutrans &Equal\ Size<Tab>^W=		Mismo\ &tamaño<Tab>^W=
menutrans &Max\ Height<Tab>^W_		Altura\ &máxima<Tab>^W_
menutrans M&in\ Height<Tab>^W1_		Altura\ mí&nima<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		Anchura\ máxima<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		Anchura\ mínima<Tab>^W1\|
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

" Find Help dialog text
let find_help_dialog = "Introduzca un mandato o palabra para obtener ayuda;\n\nAnteponga i_ para mandatos de entrada (e.g.: i_CTRL-X)\nAnteponga c_ para mandatos de la línea de mandatos (e.g.: c_<Del>)\nAnteponga ` para un nombre de opción 8(e.g.: `shiftwidth`)"
