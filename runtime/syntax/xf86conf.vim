" Maintainer	: Nikolai 'pcp' Weibull <da.box@home.se>
" URL		: http://www.pcppopper.org/
" Revised on	: Thu, 14 Feb 2002 20:42:46 +0100
" Language	: XFree86 Configuration File

if version < 600
    syntax clear
elseif exists("b:current_syntax")
    finish
endif

" comments
syn region  xf86confComment	matchgroup=xf86confComment start="#" end="$" contains=xf86confTodo

" todo
syn keyword xf86confTodo	contained TODO FIXME

" strings
syn region  xf86confString	matchgroup=xf86confString start='"' skip='\\\\\|\\"' end='"' contains=xf86confSection,xf86confBoolean,xf86confFreq

" booleans
syn case ignore
syn keyword xf86confBoolean	contained on true yes off false no
syn case match

" frequencies
syn keyword xf86confFreq	contained Hz k kHz M MHz

" numbers
syn case ignore
syn match   xf86confInt		display "\<\d\+\>"
syn match   xf86confHex		display "\<0x\x\+\>"
syn match   xf86confOctal	display "\<0\o\+\>"
syn match   xf86confFloat	display "\<\d\+\.\d*\>"
syn match   xf86confOctalError	display "\<0\o*[89]\d*"
syn case match

" keywords
syn keyword xf86confKeyword	Section EndSection Subsection EndSubsection

" sections
syn keyword xf86confSection	contained Files ServerFlags Module InputDevice
syn keyword xf86confSection	contained Device VideoAdapter Monitor Modes
syn keyword xf86confSection	contained Screen ServerLayout DRI Vendor Display

" other
syn keyword xf86confOption	Identifier Driver BusID Screen Chipset Ramdac Dacspeed Clocks ClockChip VideoRam BiosBase MemBase IOBase ChipID ChipRev
syn keyword xf86confOption	TextClockFreq VendorName ModelName HorizSync VertRefresh DisplaySize Gamma UseModes Mode DotClock HTimings VTimings
syn keyword xf86confOption	Flags HSkew VScan ModeLine Device Monitor VideoAdaptor DefaultDepth DefaultFbBpp Depth FbBpp Weight Virtual ViewPort
syn keyword xf86confOption	Visual Black White Screen InputDevice Option Load BoardName Modes RgbPath FontPath ModulePath

if exists("xf86conf_minlines")
    let b:xf86conf_minlines = xf86conf_minlines
else
    let b:xf86conf_minlines = 10
endif
exec "syn sync minlines=" . b:xf86conf_minlines

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_xf86conf_syn_inits")
    if version < 508
	let did_xf86conf_syn_inits = 1
	command -nargs=+ HiLink hi link <args>
    else
	command -nargs=+ HiLink hi def link <args>
    endif

    HiLink xf86confComment	Comment
    HiLink xf86confTodo		Todo
    HiLink xf86confString	String
    HiLink xf86confBoolean	Boolean
    HiLink xf86confFreq		Number
    HiLink xf86confInt		Number
    HiLink xf86confHex		Number
    HiLink xf86confOctal	Number
    HiLink xf86confFloat	Number
    HiLink xf86confOctalError	Error
    HiLink xf86confKeyword	Keyword
    HiLink xf86confSection	Type
    HiLink xf86confOption	Type
    delcommand HiLink
endif

let b:current_syntax = "xf86conf"

" vim: set sw=4 sts=4:

