" Vim syntax file
" This is a GENERATED FILE. Please always refer to source file at the URI below.
" Language: Gtk+ theme files `gtkrc'
" Maintainer: David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change: 2002-05-05
" URL: http://physics.muni.cz/~yeti/download/syntax/gtkrc.vim

" Setup
if version >= 600
  if exists("b:current_syntax")
    finish
  endif
else
  syntax clear
endif

if version >= 600
  setlocal iskeyword=_,-,a-z,A-Z,48-57
else
  set iskeyword=_,-,a-z,A-Z,48-57
endif

syn case match

" Base constructs
syn match gtkrcComment "#.*$" contains=gtkrcFixme
syn keyword gtkrcFixme FIXME TODO XXX NOT contained
syn region gtkrcACString start=+"+ skip=+\\\\\|\\"+ end=+"+ oneline contains=gtkrcWPathSpecial,gtkrcClassName,gtkrcClassNameGnome contained
syn region gtkrcBString start=+"+ skip=+\\\\\|\\"+ end=+"+ oneline contains=gtkrcKeyMod contained
syn region gtkrcString start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=gtkrcStockName,gtkrcPathSpecial,gtkrcRGBColor
syn match gtkrcPathSpecial "<parent>" contained
syn match gtkrcWPathSpecial "[*?.]" contained
syn match gtkrcNumber "^\(\d\+\)\=\.\=\d\+"
syn match gtkrcNumber "\W\(\d\+\)\=\.\=\d\+"lc=1
syn match gtkrcRGBColor "#\(\x\{12}\|\x\{9}\|\x\{6}\|\x\{3}\)" contained
syn cluster gtkrcPRIVATE add=gtkrcFixme,gtkrcPathSpecial,gtkrcWPathSpecial,gtkrcRGBColor,gtkrcACString

" Keywords
syn keyword gtkrcInclude include
syn keyword gtkrcPathSet module_path pixmap_path
syn keyword gtkrcTop binding style
syn keyword gtkrcTop widget widget_class nextgroup=gtkrcACString skipwhite
syn keyword gtkrcTop class nextgroup=gtkrcACString skipwhite
syn keyword gtkrcBind bind nextgroup=gtkrcBString skipwhite
syn keyword gtkrcStateName NORMAL INSENSITIVE PRELIGHT ACTIVE SELECTED
syn keyword gtkrcPriorityName HIGHEST RC APPLICATION GTK LOWEST
syn keyword gtkrcPriorityName highest rc application gtk lowest
syn keyword gtkrcTextDirName LTR RTL
syn keyword gtkrcStyleKeyword fg bg fg_pixmap bg_pixmap bg_text base font font_name fontset stock text
syn match gtkrcKeyMod "<\(alt\|ctrl\|control\|mod[1-5]\|release\|shft\|shift\)>" contained
syn cluster gtkrcPRIVATE add=gtkrcKeyMod

" Enums and engine words
syn keyword gtkrcKeyword engine image
syn keyword gtkrcImage arrow_direction border detail file gap_border gap_end_border gap_end_file gap_file gap_side gap_side gap_start_border gap_start_file orientation overlay_border overlay_file overlay_stretch recolorable shadow state stretch thickness
syn keyword gtkrcConstant TRUE FALSE NONE IN OUT LEFT RIGHT TOP BOTTOM UP DOWN VERTICAL HORIZONTAL ETCHED_IN ETCHED_OUT
syn keyword gtkrcFunction function nextgroup=gtkrcFunctionEq skipwhite
syn match gtkrcFunctionEq "=" nextgroup=gtkrcFunctionName contained skipwhite
syn keyword gtkrcFunctionName ARROW BOX BOX_GAP CHECK CROSS DIAMOND EXTENSION FLAT_BOX FOCUS HANDLE HLINE OPTION OVAL POLYGON RAMP SHADOW SHADOW_GAP SLIDER STRING TAB VLINE contained
syn cluster gtkrcPRIVATE add=gtkrcFunctionName,gtkrcFunctionEq

" Class names
syn keyword gtkrcClassName GtkObject GtkWidget GtkMisc GtkLabel GtkAccelLabel GtkTipsQuery GtkArrow GtkImage GtkPixmap GtkContainer GtkBin GtkAlignment GtkFrame GtkAspectFrame GtkButton GtkToggleButton GtkCheckButton GtkRadioButton GtkOptionMenu GtkItem GtkMenuItem GtkCheckMenuItem GtkRadioMenuItem GtkTearoffMenuItem GtkListItem GtkTreeItem GtkWindow GtkColorSelectionDialog GtkDialog GtkInputDialog GtkFileSelection GtkFontSelectionDialog GtkPlug GtkEventBox GtkHandleBox GtkScrolledWindow GtkViewport GtkBox GtkButtonBox GtkHButtonBox GtkVButtonBox GtkVBox GtkColorSelection GtkGammaCurve GtkHBox GtkCombo GtkStatusbar GtkCList GtkCTree GtkFixed GtkNotebook GtkFontSelection GtkPaned GtkHPaned GtkVPaned GtkLayout GtkList GtkMenuShell GtkMenu GtkMenuBar GtkPacker GtkSocket GtkTable GtkToolbar GtkTree GtkCalendar GtkDrawingArea GtkCurve GtkEditable GtkEntry GtkSpinButton GtkText GtkRuler GtkHRuler GtkVRuler GtkRange GtkScale GtkHScale GtkVScale GtkScrollbar GtkHScrollbar GtkVScrollbar GtkSeparator GtkHSeparator GtkVSeparator GtkInvisible GtkPreview GtkProgress GtkProgressBar GtkData GtkAdjustment GtkTooltips GtkItemFactory contained
syn keyword gtkrcClassName Object Widget Misc Label AccelLabel TipsQuery Arrow Image Pixmap Container Bin Alignment Frame AspectFrame Button ToggleButton CheckButton RadioButton OptionMenu Item MenuItem CheckMenuItem RadioMenuItem TearoffMenuItem ListItem TreeItem Window ColorSelectionDialog Dialog InputDialog FileSelection FontSelectionDialog Plug EventBox HandleBox ScrolledWindow Viewport Box ButtonBox HButtonBox VButtonBox VBox ColorSelection GammaCurve HBox Combo Statusbar CList CTree Fixed Notebook FontSelection Paned HPaned VPaned Layout List MenuShell Menu MenuBar Packer Socket Table Toolbar Tree Calendar DrawingArea Curve Editable Entry SpinButton Text Ruler HRuler VRuler Range Scale HScale VScale Scrollbar HScrollbar VScrollbar Separator HSeparator VSeparator Invisible Preview Progress ProgressBar Data Adjustment Tooltips ItemFactory contained
syn keyword gtkrcClassNameGnome GnomeAbout GnomeAnimator GnomeApp GnomeAppBar GnomeCalculator GnomeCanvas GnomeCanvasEllipse GnomeCanvasGroup GnomeCanvasImage GnomeCanvasItem GnomeCanvasLine GnomeCanvasPolygon GnomeCanvasRE GnomeCanvasRect GnomeCanvasText GnomeCanvasWidget GnomeClient GnomeColorPicker GnomeDEntryEdit GnomeDateEdit GnomeDialog GnomeDock GnomeDockBand GnomeDockItem GnomeDockLayout GnomeDruid GnomeDruidPage GnomeDruidPageFinish GnomeDruidPageStandard GnomeDruidPageStart GnomeEntry GnomeFileEntry GnomeFontPicker GnomeFontSelector GnomeHRef GnomeIconEntry GnomeIconList GnomeIconSelection GnomeIconTextItem GnomeLess GnomeMDI GnomeMDIChild GnomeMDIGenericChild GnomeMessageBox GnomeNumberEntry GnomePaperSelector GnomePixmap GnomePixmapEntry GnomeProcBar GnomePropertyBox GnomeScores GnomeSpell GnomeStock GtkClock GtkDial GtkPixmapMenuItem GtkTed contained
syn cluster gtkrcPRIVATE add=gtkrcClassName,gtkrcClassNameGnome

" Stock item names
syn keyword gtkrcStockName gtk-add gtk-apply gtk-bold gtk-cancel gtk-cdrom gtk-clear gtk-close gtk-convert gtk-copy gtk-cut gtk-delete gtk-dialog-error gtk-dialog-info gtk-dialog-question gtk-dialog-warning gtk-dnd gtk-dnd-multiple gtk-execute gtk-find gtk-find-and-replace gtk-floppy gtk-goto-bottom gtk-goto-first gtk-goto-last gtk-goto-top gtk-go-back gtk-go-down gtk-go-forward gtk-go-up gtk-help gtk-home gtk-index gtk-italic gtk-jump-to gtk-justify-center gtk-justify-fill gtk-justify-left gtk-justify-right gtk-missing-image gtk-new gtk-no gtk-ok gtk-open gtk-paste gtk-preferences gtk-print gtk-print-preview gtk-properties gtk-quit gtk-redo gtk-refresh gtk-remove gtk-revert-to-saved gtk-save gtk-save-as gtk-select-color gtk-select-font gtk-sort-ascending gtk-sort-descending gtk-spell-check gtk-stop gtk-strikethrough gtk-undelete gtk-underline gtk-undo gtk-yes gtk-zoom-100 gtk-zoom-fit gtk-zoom-in gtk-zoom-out contained
syn cluster gtkrcPRIVATE add=gtkrcStockName

" Catch errors caused by wrong parenthesization
syn region gtkrcParen start='(' end=')' transparent contains=ALLBUT,gtkrcParenError,@gtkrcPRIVATE
syn match gtkrcParenError ")"
syn region gtkrcBrace start='{' end='}' transparent contains=ALLBUT,gtkrcBraceError,@gtkrcPRIVATE
syn match gtkrcBraceError "}"
syn region gtkrcBracket start='\[' end=']' transparent contains=ALLBUT,gtkrcBracketError,@gtkrcPRIVATE
syn match gtkrcBracketError "]"

" Synchronization
syn sync minlines=50
syn sync match gtkrcSyncClass groupthere NONE "^\s*class\>"

" Define the default highlighting
if version >= 508 || !exists("did_gtkrc_syntax_inits")
  if version < 508
    let did_gtkrc_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink gtkrcComment Comment
  HiLink gtkrcFixme Todo

  HiLink gtkrcInclude Preproc

  HiLink gtkrcACString gtkrcString
  HiLink gtkrcBString gtkrcString
  HiLink gtkrcString String
  HiLink gtkrcNumber Number
  HiLink gtkrcStateName gtkrcConstant
  HiLink gtkrcPriorityName gtkrcConstant
  HiLink gtkrcTextDirName gtkrcConstant
  HiLink gtkrcStockName Function
  HiLink gtkrcConstant Constant

  HiLink gtkrcPathSpecial gtkrcSpecial
  HiLink gtkrcWPathSpecial gtkrcSpecial
  HiLink gtkrcRGBColor gtkrcSpecial
  HiLink gtkrcKeyMod gtkrcSpecial
  HiLink gtkrcSpecial Special

  HiLink gtkrcTop gtkrcKeyword
  HiLink gtkrcPathSet gtkrcKeyword
  HiLink gtkrcStyleKeyword gtkrcKeyword
  HiLink gtkrcFunction gtkrcKeyword
  HiLink gtkrcBind gtkrcKeyword
  HiLink gtkrcKeyword Keyword

  HiLink gtkrcClassNameGnome gtkrcGtkClass
  HiLink gtkrcClassName gtkrcGtkClass
  HiLink gtkrcFunctionName gtkrcGtkClass
  HiLink gtkrcGtkClass Type

  HiLink gtkrcImage gtkrcOtherword
  HiLink gtkrcOtherword Function

  HiLink gtkrcParenError gtkrcError
  HiLink gtkrcBraceError gtkrcError
  HiLink gtkrcBracketError gtkrcError
  HiLink gtkrcError Error

  delcommand HiLink
endif

let b:current_syntax = "gtkrc"
