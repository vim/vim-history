" Vim syntax file
" Language:	pilrc - a resource compiler for Palm OS development
" Maintainer:	Brian Schau <brian@schau.dk>
" Last change:	2001 Jul 12
" Available on:	http://www.brisse.dk/vim/

" Remove any old syntax
if version < 600
	syn clear
elseif exists("b:current_syntax")
	finish
endif

syn case ignore

" Keywords - basic
syn keyword pilrcKeyword ALERT APPLICATION APPLICATIONICONNAME AREA
syn keyword pilrcKeyword BITMAP BITMAPCOLOR BITMAPCOLOR16 BITMAPCOLOR16K
syn keyword pilrcKeyword BITMAPFAMILY BITMAPFAMILYSPECIAL BITMAPGREY
syn keyword pilrcKeyword BITMAPGREY16 BITMAPSCREENFAMILY BUTTON BUTTONS BYTELIST
syn keyword pilrcKeyword CATEGORIES CHECKBOX COUNTRYLOCALISATION
syn keyword pilrcKeyword DATA
syn keyword pilrcKeyword FEATURE FIELD FONT FONTINDEX FORM FORMBITMAP
syn keyword pilrcKeyword GADGET GRAFFITIINPUTAREA GRAFFITISTATEINDICATOR
syn keyword pilrcKeyword HEX
syn keyword pilrcKeyword ICON ICONFAMILY ID INTEGER
syn keyword pilrcKeyword KEYBOARD
syn keyword pilrcKeyword LABEL LAUNCHERCATEGORY LIST LONGWORDLIST
syn keyword pilrcKeyword MENU MENUITEM MESSAGE  MIDI
syn keyword pilrcKeyword PALETTETABLE POPUPLIST POPUPTRIGGER
syn keyword pilrcKeyword PULLDOWN PUSHBUTTON
syn keyword pilrcKeyword REPEATBUTTON
syn keyword pilrcKeyword SCROLLBAR SELECTORTRIGGER SLIDER SMALLICON
syn keyword pilrcKeyword SMALLICONFAMILY STRING STRINGTABLE
syn keyword pilrcKeyword TABLE TITLE TRANSLATION TRAP
syn keyword pilrcKeyword VERSION
syn keyword pilrcKeyword WORDLIST

" Types
syn keyword pilrcType AT AUTOSHIFT
syn keyword pilrcType BACKGROUNDID BITMAPID BOLDFRAME
syn keyword pilrcType CHECKED COLORTABLE COLUMNS COLUMNWIDTH COMPRESS
syn keyword pilrcType CONFIRMATION COUNTRY CREATOR CURRENCYDECIMALPLACES
syn keyword pilrcType CURRENCYNAME CURRENCYSYMBOL CURRENCYUNIQUESYMBOL
syn keyword pilrcType DATEFORMAT DAYLIGHTSAVINGS DEFAULTBTNID DEFAULTBUTTON
syn keyword pilrcType DISABLED DYNAMICSIZE
syn keyword pilrcType EDITABLE ENTRY ERROR
syn keyword pilrcType FEEDBACK FILE FONT FONTID FORCECOMPRESS FRAME
syn keyword pilrcType GRAFFITI GRAPHICAL GROUP
syn keyword pilrcType HASSCROLLBAR HELPID
syn keyword pilrcType INDEX INFORMATION
syn keyword pilrcType KEYDOWNCHR KEYDOWNKEYCODE KEYDOWNMODIFIERS
syn keyword pilrcType LANGUAGE LEFTANCHOR LONGDATEFORMAT
syn keyword pilrcType MAX MAXCHARS MEASUREMENTSYSTEM MENUID MIN
syn keyword pilrcType MINUTESWESTOFGMT MODAL MULTIPLELINES
syn keyword pilrcType NAME NOCOLORTABLE NOCOMPRESS NOFRAME NONEDITABLE
syn keyword pilrcType NONUSABLE NOSAVEBEHIND NUMBER NUMBERFORMAT NUMERIC
syn keyword pilrcType PAGESIZE
syn keyword pilrcType RIGHTALIGN RIGHTANCHOR ROWS
syn keyword pilrcType SAVEBEHIND SEPARATOR SCREEN SELECTEDBITMAPID SINGLELINE
syn keyword pilrcType THUMBID TIMEFORMAT TRANSPARENT TRANSPARENTINDEX
syn keyword pilrcType UNDERLINED USABLE
syn keyword pilrcType VALUE VERTICAL VISIBLEITEMS
syn keyword pilrcType WARNING WEEKSTARTDAY

" Country
syn keyword pilrcCountry Australia Austria Belgium Brazil Canada Denmark
syn keyword pilrcCountry Finland France Germany HongKong Iceland Indian
syn keyword pilrcCountry Indonesia Ireland Italy Japan Korea Luxembourg Malaysia
syn keyword pilrcCountry Mexico Netherlands NewZealand Norway Philippines
syn keyword pilrcCountry RepChina Singapore Spain Sweden Switzerland Thailand
syn keyword pilrcCountry Taiwan UnitedKingdom UnitedStates

" Language
syn keyword pilrcLanguage English French German Italian Japanese Spanish

" String
syn match pilrcString "\"[^"]*\""

" Number
syn match pilrcNumber "\<\d\+\>"

" Comment
syn region pilrcComment start="/\*" end="\*/"
syn region pilrcComment start="//" end="$"

" Constants
syn keyword pilrcConstant AUTO AUTOID BOTTOM CENTER PREVBOTTOM PREVHEIGHT
syn keyword pilrcConstant PREVLEFT PREVRIGHT PREVTOP PREVWIDTH RIGHT

" Identifier
syn match pilrcIdentifier "\<[A-Za-z_][A-Za-z0-9_]*\>"

" Function
syn keyword pilrcFunction BEGIN END

" Include
syn match pilrcInclude "\#include"
syn match pilrcInclude "\#define"
syn keyword pilrcInclude equ
syn keyword pilrcInclude package
syn region pilrcInclude start="public class" end="}"

if version >= 508 || !exists("did_pilrc_syntax_inits")
	if version < 508
		command -nargs=+ HiLink hi link <args>
	else
		command -nargs=+ HiLink hi def link <args>
	endif

	let did_pilrc_syntax_inits = 1

	" The default methods for highlighting
	HiLink pilrcKeyword		Statement
	HiLink pilrcType		Type
	HiLink pilrcError		Error
	HiLink pilrcCountry		SpecialChar
	HiLink pilrcLanguage		SpecialChar
	HiLink pilrcString		SpecialChar
	HiLink pilrcNumber		Number
	HiLink pilrcComment		Comment
	HiLink pilrcConstant		Constant
	HiLink pilrcFunction		Function
	HiLink pilrcInclude		SpecialChar

	delcommand HiLink
endif

let b:current_syntax = "pilrc"
