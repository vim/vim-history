" Vim syntax file
" Language:	Fvwm{1,2} configuration file
" Maintainer:	Haakon Riiser <hakonrk@fys.uio.no>
" Last change:	1998 Aug 11
"
" Put
"
"	let is_fvwm1 = 1
"
" in your vimrc for more specific Fvwm1 syntax, or
"
"	let is_fvwm2 = 1
"
" if you're using Fvwm2.

set iskeyword=_,-,+,a-z,A-Z,48-57

" Clear old syntax defs
syn clear

" Case insensitive
syn case ignore

syn match fvwmComment		"^#.*$"
syn match fvwmDimension		"\<\d\+\(x\d\+\)\=\>"
syn match fvwmDirectory		"/[@[:alnum:]/.-_+,$~]*/\=" contains=fvwmEnvVar
syn match fvwmEnvVar		"\${\=\w\+}\="
syn match fvwmModule		"^\*\w\+"
syn match fvwmNumber		"\(+\|-\)\d\+\(+\|-\)\d\+\>"
syn match fvwmNumber		"\<\d\+\>"
syn match fvwmNumber		"\<\(+\|-\)\d\+\>"
syn match fvwmRGBValue		"\s#\x\{3}"
syn match fvwmRGBValue		"\s#\x\{6}"
syn match fvwmRGBValue		"\s#\x\{9}"
syn match fvwmRGBValue		"\s#\x\{12}"
syn match fvwmRGBValue		"rgb:\x\{1,4}/\x\{1,4}/\x\{1,4}"
syn match fvwmSpecial		"[,()!@]"
syn match fvwmShortcutKey	"&."hs=s+1 contained
syn match fvwmString		"'.\{-}'" contains=fvwmExec
syn match fvwmString		"`.\{-}`" contains=fvwmExec
syn match fvwmString		'".\{-}"' contains=fvwmExec,fvwmShortcutKey,fvwmMenuIcon

if exists("is_fvwm1")
 " Fvwm1 functions
 syn keyword fvwmFunctions	CirculateDown CirculateUp EndFunction
 syn keyword fvwmFunctions	EndPopup TogglePage Warp

 " Fvwm1 commands
 syn keyword fvwmKeywords	AppsBackingStore AutoRaise BackingStore
 syn keyword fvwmKeywords	BoundaryWidth CenterOnCirculate
 syn keyword fvwmKeywords	CirculateSkipIcons Cursor DecorateTransients
 syn keyword fvwmKeywords	DeskTopScale DontMoveOff Font HiBackColor
 syn keyword fvwmKeywords	HiForeColor MenuBackColor MenuForeColor
 syn keyword fvwmKeywords	MenuStippleColor MWMBorders MWMDecorHints
 syn keyword fvwmKeywords	MWMFunctionHints MWMHintOverride MWMMenus
 syn keyword fvwmKeywords	NoBoundaryWidth OpaqueMove OpaqueResize Pager
 syn keyword fvwmKeywords	PagerBackColor PagerFont PagerForeColor
 syn keyword fvwmKeywords	PagingDefault SaveUnders StdBackColor
 syn keyword fvwmKeywords	StdForeColor StickyBackColor StickyForeColor
 syn keyword fvwmKeywords	StickyIcons StubbornIconPlacement StubbornIcons
 syn keyword fvwmKeywords	StubbornPlacement SuppressIcons
elseif exists("is_fvwm2")
 syn match fvwmMenuIcon		"%.\{-}\.xpm%" contained
 syn match fvwmMenuIcon		"[*].\{-}\.xpm[*]" contained

 " Fvwm2 functions
 syn keyword fvwmFunctions	Current Destroy DestroyDecor DestroyFunc
 syn keyword fvwmFunctions	DestroyMenu DestroyModuleConfig Echo FlipFocus
 syn keyword fvwmFunctions	KillModule Menu Next None PipeRead Prev Read
 syn keyword fvwmFunctions	Recapture RefreshWindow SendToModule

 " Fvwm2 keywords
 syn keyword fvwmKeywords	Active ActiveDown ActivePlacement ActiveUp
 syn keyword fvwmKeywords	AddButtonStyle AddTitleStyle AddToDecor
 syn keyword fvwmKeywords	AddToFunc AddToMenu All BackColor BorderStyle
 syn keyword fvwmKeywords	Bottom Centered ChangeDecor CirculateSkipIcon
 syn keyword fvwmKeywords	ColormapFocus CursorStyle DecorateTransient
 syn keyword fvwmKeywords	Default Down DumbPlacement ExecUseShell Flat
 syn keyword fvwmKeywords	FocusFollowsMouse FollowsFocus ForeColor
 syn keyword fvwmKeywords	FvwmBorder GlobalOpts Height HGradient
 syn keyword fvwmKeywords	HilightColor HintOverride Inactive Left
 syn keyword fvwmKeywords	LeftJustified MenuStyle MiniIcon MouseFocus
 syn keyword fvwmKeywords	MWMBorder MWMButtons MWMDecor MWMDecorMax
 syn keyword fvwmKeywords	MWMDecorMenu MWMDecorMin MWMFunctions
 syn keyword fvwmKeywords	NakedTransient NoDecorHint NoFuncHint NoIcon
 syn keyword fvwmKeywords	NoLenience NoOLDecor NoOverride OLDecor
 syn keyword fvwmKeywords	OpaqueMoveSize Raised Reset Right RightJustified
 syn keyword fvwmKeywords	ShowMapping Simple SkipMapping Slippery
 syn keyword fvwmKeywords	SlipperyIcon Solid StickyIcon Sunk TiledPixmap
 syn keyword fvwmKeywords	TitleStyle Top Up UpdateDecor UseBorderStyle
 syn keyword fvwmKeywords	UseDecor UsePPosition UseStyle UseTitleStyle
 syn keyword fvwmKeywords	Vector VGradient Width WindowId WindowListHit
endif

" Functions common in Fvwm{1,2}
syn keyword fvwmCommonFuncs	Beep Close CursorMove Delete Desk Destroy
syn keyword fvwmCommonFuncs	Focus Function GotoPage Iconify Lower Maximize
syn keyword fvwmCommonFuncs	Module Move Nop Popup Quit Raise RaiseLower
syn keyword fvwmCommonFuncs	Refresh Resize Restart Scroll Stick Title
syn keyword fvwmCommonFuncs	Wait WindowList WindowShade WindowsDesk

" Keywords common in Fvwm{1,2}
syn keyword fvwmCommonKeywords	BorderWidth Button ButtonStyle CirculateHit
syn keyword fvwmCommonKeywords	CirculateSkip ClickTime ClickToFocus Color
syn keyword fvwmCommonKeywords	DeskTopSize EdgeResistance EdgeScroll
syn keyword fvwmCommonKeywords	HandleWidth Handles Icon IconBox IconFont
syn keyword fvwmCommonKeywords	IconPath IconTitle Key Lenience MWMButtons
syn keyword fvwmCommonKeywords	ModulePath Mouse NoBorder NoButton NoHandles
syn keyword fvwmCommonKeywords	NoIconTitle NoPPosition NoTitle PixmapPath
syn keyword fvwmCommonKeywords	RandomPlacement SloppyFocus SmartPlacement
syn keyword fvwmCommonKeywords	StartIconic StartNormal StartsAnyWhere
syn keyword fvwmCommonKeywords	StartsOnDesk StaysOnTop StaysPut Sticky Style
syn keyword fvwmCommonKeywords	WindowFont WindowListSkip XORvalue

" Key names
syn keyword fvwmKeys		Alt_L Alt_R Caps_Lock Control_L Control_R Delete
syn keyword fvwmKeys		End Escape F1 F10 F11 F12 F2 F3 F4 F5 F6 F7 F8 F9
syn keyword fvwmKeys		Home Insert KP_0 KP_1 KP_2 KP_3 KP_4 KP_5 KP_6 KP_7
syn keyword fvwmKeys		KP_8 KP_9 KP_Add KP_Divide KP_Enter KP_Multiply
syn keyword fvwmKeys		KP_Separator KP_Subtract Meta Multi_key NoSymbol
syn keyword fvwmKeys		Num_Lock Pause Prior Return Shift_L Shift_R Tab

" System colors
syn keyword fvwmColors		alice AliceBlue almond antique AntiqueWhite
syn keyword fvwmColors		AntiqueWhite1 AntiqueWhite2 AntiqueWhite3
syn keyword fvwmColors		AntiqueWhite4 aquamarine aquamarine1 aquamarine2
syn keyword fvwmColors		aquamarine3 aquamarine4 azure azure1 azure2 azure3
syn keyword fvwmColors		azure4 beige bisque bisque1 bisque2 bisque3
syn keyword fvwmColors		bisque4 black blanched BlanchedAlmond blue
syn keyword fvwmColors		blue1 blue2 blue3 blue4 BlueViolet blush brown
syn keyword fvwmColors		brown1 brown2 brown3 brown4 burlywood burlywood1
syn keyword fvwmColors		burlywood2 burlywood3 burlywood4 cadet CadetBlue
syn keyword fvwmColors		CadetBlue1 CadetBlue2 CadetBlue3 CadetBlue4
syn keyword fvwmColors		chartreuse chartreuse1 chartreuse2 chartreuse3
syn keyword fvwmColors		chartreuse4 chiffon chocolate chocolate1
syn keyword fvwmColors		chocolate2 chocolate3 chocolate4 coral coral1
syn keyword fvwmColors		coral2 coral3 coral4 cornflower CornflowerBlue
syn keyword fvwmColors		cornsilk cornsilk1 cornsilk2 cornsilk3 cornsilk4
syn keyword fvwmColors		cream cyan cyan1 cyan2 cyan3 cyan4 dark
syn keyword fvwmColors		DarkBlue DarkCyan DarkGoldenrod DarkGoldenrod1
syn keyword fvwmColors		DarkGoldenrod2 DarkGoldenrod3 DarkGoldenrod4
syn keyword fvwmColors		DarkGray DarkGreen DarkGrey DarkKhaki DarkMagenta
syn keyword fvwmColors		DarkOliveGreen DarkOliveGreen1 DarkOliveGreen2
syn keyword fvwmColors		DarkOliveGreen3 DarkOliveGreen4 DarkOrange
syn keyword fvwmColors		DarkOrange1 DarkOrange2 DarkOrange3 DarkOrange4
syn keyword fvwmColors		DarkOrchid DarkOrchid1 DarkOrchid2 DarkOrchid3
syn keyword fvwmColors		DarkOrchid4 DarkRed DarkSalmon DarkSeaGreen
syn keyword fvwmColors		DarkSeaGreen1 DarkSeaGreen2 DarkSeaGreen3
syn keyword fvwmColors		DarkSeaGreen4 DarkSlateBlue DarkSlateGray
syn keyword fvwmColors		DarkSlateGray1 DarkSlateGray2 DarkSlateGray3
syn keyword fvwmColors		DarkSlateGray4 DarkSlateGrey DarkTurquoise
syn keyword fvwmColors		DarkViolet deep DeepPink DeepPink1 DeepPink2
syn keyword fvwmColors		DeepPink3 DeepPink4 DeepSkyBlue DeepSkyBlue1
syn keyword fvwmColors		DeepSkyBlue2 DeepSkyBlue3 DeepSkyBlue4 dim
syn keyword fvwmColors		DimGray DimGrey dodger DodgerBlue DodgerBlue1
syn keyword fvwmColors		DodgerBlue2 DodgerBlue3 DodgerBlue4 drab firebrick
syn keyword fvwmColors		firebrick1 firebrick2 firebrick3 firebrick4
syn keyword fvwmColors		floral FloralWhite forest ForestGreen gainsboro
syn keyword fvwmColors		ghost GhostWhite gold gold1 gold2 gold3 gold4
syn keyword fvwmColors		goldenrod goldenrod1 goldenrod2 goldenrod3
syn keyword fvwmColors		goldenrod4 gray gray0 gray1 gray10 gray100
syn keyword fvwmColors		gray11 gray12 gray13 gray14 gray15 gray16 gray17
syn keyword fvwmColors		gray18 gray19 gray2 gray20 gray21 gray22 gray23
syn keyword fvwmColors		gray24 gray25 gray26 gray27 gray28 gray29 gray3
syn keyword fvwmColors		gray30 gray31 gray32 gray33 gray34 gray35 gray36
syn keyword fvwmColors		gray37 gray38 gray39 gray4 gray40 gray41 gray42
syn keyword fvwmColors		gray43 gray44 gray45 gray46 gray47 gray48 gray49
syn keyword fvwmColors		gray5 gray50 gray51 gray52 gray53 gray54 gray55
syn keyword fvwmColors		gray56 gray57 gray58 gray59 gray6 gray60 gray61
syn keyword fvwmColors		gray62 gray63 gray64 gray65 gray66 gray67 gray68
syn keyword fvwmColors		gray69 gray7 gray70 gray71 gray72 gray73 gray74
syn keyword fvwmColors		gray75 gray76 gray77 gray78 gray79 gray8 gray80
syn keyword fvwmColors		gray81 gray82 gray83 gray84 gray85 gray86 gray87
syn keyword fvwmColors		gray88 gray89 gray9 gray90 gray91 gray92 gray93
syn keyword fvwmColors		gray94 gray95 gray96 gray97 gray98 gray99 green
syn keyword fvwmColors		green1 green2 green3 green4 GreenYellow grey
syn keyword fvwmColors		grey0 grey1 grey10 grey100 grey11 grey12 grey13
syn keyword fvwmColors		grey14 grey15 grey16 grey17 grey18 grey19 grey2
syn keyword fvwmColors		grey20 grey21 grey22 grey23 grey24 grey25 grey26
syn keyword fvwmColors		grey27 grey28 grey29 grey3 grey30 grey31 grey32
syn keyword fvwmColors		grey33 grey34 grey35 grey36 grey37 grey38 grey39
syn keyword fvwmColors		grey4 grey40 grey41 grey42 grey43 grey44 grey45
syn keyword fvwmColors		grey46 grey47 grey48 grey49 grey5 grey50 grey51
syn keyword fvwmColors		grey52 grey53 grey54 grey55 grey56 grey57 grey58
syn keyword fvwmColors		grey59 grey6 grey60 grey61 grey62 grey63 grey64
syn keyword fvwmColors		grey65 grey66 grey67 grey68 grey69 grey7 grey70
syn keyword fvwmColors		grey71 grey72 grey73 grey74 grey75 grey76 grey77
syn keyword fvwmColors		grey78 grey79 grey8 grey80 grey81 grey82 grey83
syn keyword fvwmColors		grey84 grey85 grey86 grey87 grey88 grey89 grey9
syn keyword fvwmColors		grey90 grey91 grey92 grey93 grey94 grey95 grey96
syn keyword fvwmColors		grey97 grey98 grey99 honeydew honeydew1 honeydew2
syn keyword fvwmColors		honeydew3 honeydew4 hot HotPink HotPink1 HotPink2
syn keyword fvwmColors		HotPink3 HotPink4 indian IndianRed IndianRed1
syn keyword fvwmColors		IndianRed2 IndianRed3 IndianRed4 ivory ivory1
syn keyword fvwmColors		ivory2 ivory3 ivory4 khaki khaki1 khaki2 khaki3
syn keyword fvwmColors		khaki4 lace lavender LavenderBlush LavenderBlush1
syn keyword fvwmColors		LavenderBlush2 LavenderBlush3 LavenderBlush4
syn keyword fvwmColors		lawn LawnGreen lemon LemonChiffon LemonChiffon1
syn keyword fvwmColors		LemonChiffon2 LemonChiffon3 LemonChiffon4 light
syn keyword fvwmColors		LightBlue LightBlue1 LightBlue2 LightBlue3
syn keyword fvwmColors		LightBlue4 LightCoral LightCyan LightCyan1
syn keyword fvwmColors		LightCyan2 LightCyan3 LightCyan4 LightGoldenrod
syn keyword fvwmColors		LightGoldenrod1 LightGoldenrod2 LightGoldenrod3
syn keyword fvwmColors		LightGoldenrod4 LightGoldenrodYellow LightGray
syn keyword fvwmColors		LightGreen LightGrey LightPink LightPink1
syn keyword fvwmColors		LightPink2 LightPink3 LightPink4 LightSalmon
syn keyword fvwmColors		LightSalmon1 LightSalmon2 LightSalmon3
syn keyword fvwmColors		LightSalmon4 LightSeaGreen LightSkyBlue
syn keyword fvwmColors		LightSkyBlue1 LightSkyBlue2 LightSkyBlue3
syn keyword fvwmColors		LightSkyBlue4 LightSlateBlue LightSlateGray
syn keyword fvwmColors		LightSlateGrey LightSteelBlue LightSteelBlue1
syn keyword fvwmColors		LightSteelBlue2 LightSteelBlue3 LightSteelBlue4
syn keyword fvwmColors		LightYellow LightYellow1 LightYellow2 LightYellow3
syn keyword fvwmColors		LightYellow4 lime LimeGreen linen magenta magenta1
syn keyword fvwmColors		magenta2 magenta3 magenta4 maroon maroon1 maroon2
syn keyword fvwmColors		maroon3 maroon4 medium MediumAquamarine MediumBlue
syn keyword fvwmColors		MediumOrchid MediumOrchid1 MediumOrchid2
syn keyword fvwmColors		MediumOrchid3 MediumOrchid4 MediumPurple
syn keyword fvwmColors		MediumPurple1 MediumPurple2 MediumPurple3
syn keyword fvwmColors		MediumPurple4 MediumSeaGreen MediumSlateBlue
syn keyword fvwmColors		MediumSpringGreen MediumTurquoise MediumVioletRed
syn keyword fvwmColors		midnight MidnightBlue mint MintCream misty
syn keyword fvwmColors		MistyRose MistyRose1 MistyRose2 MistyRose3
syn keyword fvwmColors		MistyRose4 moccasin navajo NavajoWhite
syn keyword fvwmColors		NavajoWhite1 NavajoWhite2 NavajoWhite3
syn keyword fvwmColors		NavajoWhite4 navy NavyBlue old OldLace olive
syn keyword fvwmColors		OliveDrab OliveDrab1 OliveDrab2 OliveDrab3
syn keyword fvwmColors		OliveDrab4 orange orange1 orange2 orange3 orange4
syn keyword fvwmColors		OrangeRed OrangeRed1 OrangeRed2 OrangeRed3
syn keyword fvwmColors		OrangeRed4 orchid orchid1 orchid2 orchid3
syn keyword fvwmColors		orchid4 pale PaleGoldenrod PaleGreen PaleGreen1
syn keyword fvwmColors		PaleGreen2 PaleGreen3 PaleGreen4 PaleTurquoise
syn keyword fvwmColors		PaleTurquoise1 PaleTurquoise2 PaleTurquoise3
syn keyword fvwmColors		PaleTurquoise4 PaleVioletRed PaleVioletRed1
syn keyword fvwmColors		PaleVioletRed2 PaleVioletRed3 PaleVioletRed4
syn keyword fvwmColors		papaya PapayaWhip peach PeachPuff PeachPuff1
syn keyword fvwmColors		PeachPuff2 PeachPuff3 PeachPuff4 peru pink pink1
syn keyword fvwmColors		pink2 pink3 pink4 plum plum1 plum2 plum3 plum4
syn keyword fvwmColors		powder PowderBlue puff purple purple1 purple2
syn keyword fvwmColors		purple3 purple4 red red1 red2 red3 red4 rose
syn keyword fvwmColors		rosy RosyBrown RosyBrown1 RosyBrown2 RosyBrown3
syn keyword fvwmColors		RosyBrown4 royal RoyalBlue RoyalBlue1 RoyalBlue2
syn keyword fvwmColors		RoyalBlue3 RoyalBlue4 saddle SaddleBrown salmon
syn keyword fvwmColors		salmon1 salmon2 salmon3 salmon4 sandy SandyBrown
syn keyword fvwmColors		sea SeaGreen SeaGreen1 SeaGreen2 SeaGreen3
syn keyword fvwmColors		SeaGreen4 seashell seashell1 seashell2 seashell3
syn keyword fvwmColors		seashell4 sienna sienna1 sienna2 sienna3 sienna4
syn keyword fvwmColors		sky SkyBlue SkyBlue1 SkyBlue2 SkyBlue3 SkyBlue4
syn keyword fvwmColors		slate SlateBlue SlateBlue1 SlateBlue2 SlateBlue3
syn keyword fvwmColors		SlateBlue4 SlateGray SlateGray1 SlateGray2
syn keyword fvwmColors		SlateGray3 SlateGray4 SlateGrey smoke snow snow1
syn keyword fvwmColors		snow2 snow3 snow4 spring SpringGreen SpringGreen1
syn keyword fvwmColors		SpringGreen2 SpringGreen3 SpringGreen4 steel
syn keyword fvwmColors		SteelBlue SteelBlue1 SteelBlue2 SteelBlue3
syn keyword fvwmColors		SteelBlue4 tan tan1 tan2 tan3 tan4 thistle
syn keyword fvwmColors		thistle1 thistle2 thistle3 thistle4 tomato tomato1
syn keyword fvwmColors		tomato2 tomato3 tomato4 turquoise turquoise1
syn keyword fvwmColors		turquoise2 turquoise3 turquoise4 violet VioletRed
syn keyword fvwmColors		VioletRed1 VioletRed2 VioletRed3 VioletRed4 wheat
syn keyword fvwmColors		wheat1 wheat2 wheat3 wheat4 whip white WhiteSmoke
syn keyword fvwmColors		yellow yellow1 yellow2 yellow3 yellow4 YellowGreen

syn keyword fvwmExec		Exec

if !exists("did_fvwm_syntax_inits")
 let did_fvwm_syntax_inits = 1
 hi link fvwmColors		Type
 hi link fvwmComment		Comment
 hi link fvwmCommonFuncs	Function
 hi link fvwmCommonKeywords	Statement
 hi link fvwmDimension		Number
 hi link fvwmDirectory		String
 hi link fvwmEnvVar		Macro
 hi link fvwmExec		Function
 hi link fvwmFunctions		Function
 hi link fvwmKeys		Constant
 hi link fvwmKeywords		Keyword
 hi link fvwmMenuIcon		Comment
 hi link fvwmModule		Macro
 hi link fvwmNumber		Number
 hi link fvwmRGBValue		Type
 hi link fvwmShortcutKey	Special
 hi link fvwmSpecial		Special
 hi link fvwmString		String
endif

let b:current_syntax = "fvwm"
" vim: ts=8 nowrap
