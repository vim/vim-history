"  vim: set sw=4 sts=4:
"  File        : bdf.vim
"  Language    : BDF Font
"  Maintainer  : Nikolai 'pcp' Weibull <da.box@home.se>
"  Revised on  : Wed, 11 Jul 2001 19:01:07 +0200
"  TODO	       : Floating point values aren't displayed right (not a major
"	       : issue though)

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if !exists("main_syntax")
    if version < 600
	syntax clear
    elseif exists("b:current_syntax")
	finish
endif
     let main_syntax = 'bdf'
endif

" comments
syn region bdfComment start="^COMMENT " end="$" contains=bdfTodo

" todo
syn keyword bdfTodo contained TODO

" numbers
syn match bdfNumber display "\<\(\x\+\|\d\+\.\d\+\)\>"

" strings
syn region bdfString start=+"+ skip=+\\\\\|\\"+ end=+"+

" properties
syn keyword bdfProperties contained FONT SIZE FONTBOUNDINGBOX CHARS

" X11 properties
syn keyword bdfXProperties contained FONT_ASCENT FONT_DESCENT DEFAULT_CHAR
syn keyword bdfXProperties contained FONTNAME_REGISTRY FOUNDRY FAMILY_NAME
syn keyword bdfXProperties contained WEIGHT_NAME SLANT SETWIDTH_NAME PIXEL_SIZE
syn keyword bdfXProperties contained POINT_SIZE RESOLUTION_X RESOLUTION_Y SPACING
syn keyword bdfXProperties contained CHARSET_REGISTRY CHARSET_ENCODING COPYRIGHT
syn keyword bdfXProperties contained ADD_STYLE_NAME WEIGHT RESOLUTION X_HEIGHT
syn keyword bdfXProperties contained QUAD_WIDTH FONT AVERAGE_WIDTH

syn match bdfDefDelim contained "\<\(STARTPROPERTIES\|ENDPROPERTIES\)\>"

syn region bdfDefinition transparent matchgroup=bdfDefDelim start="STARTPROPERTIES" end="ENDPROPERTIES" contains=bdfXProperties,bdfNumber,bdfString

" characters
syn keyword bdfCharProperties contained ENCODING SWIDTH DWIDTH BBX BITMAP

syn match bdfCharDelim contained "\<\(STARTCHAR\|ENDCHAR\)\>"

syn region bdfCharDefinition transparent matchgroup=bdfCharDelim start="STARTCHAR" end="ENDCHAR" contains=bdfCharProperties,bdfNumber,bdfCharDelim

" font
syn match bdfFontDelim contained "\<\(STARTFONT\|ENDFONT\)\>"

syn region bdfFontDefinition transparent matchgroup=bdfFontdelim start="STARTFONT" end="ENDFONT" contains=ALL

if exists("bdf_minlines")
    let b:bdf_minlines = bdf_minlines
else
    let b:bdf_minlines = 50
endif
exec "syn sync minlines=" . b:bdf_minlines
" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_bdf_syn_inits")
    if version < 508
	let did_bdf_syn_inits = 1
	command -nargs=+ HiLink hi link <args>
    else
	command -nargs=+ HiLink hi def link <args>
    endif

    HiLink bdfComment Comment
    HiLink bdfTodo Todo
    HiLink bdfNumber Number
    HiLink bdfString String
    HiLink bdfProperties Keyword
    HiLink bdfXProperties Keyword
    HiLink bdfCharProperties StorageClass
    HiLink bdfDefDelim Delimiter
    HiLink bdfCharDelim Delimiter
    HiLink bdfFontDelim Delimiter
    delcommand HiLink
endif

let b:current_syntax = "bdf"

if main_syntax == 'bdf'
  unlet main_syntax
endif
