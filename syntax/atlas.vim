" Vim syntax file
" Language:	ATLAS
" Maintainer:	Inaki Saez <jisaez@sfe.indra.es>
" Last change:	1998-01-28

syn clear

syn case ignore

syn keyword atlasStatement	begin terminate
syn keyword atlasStatement	fill calculate compare
syn keyword atlasStatement	setup connect close open disconnect reset
syn keyword atlasStatement	init fetch
syn keyword atlasStatement	apply measure verify remove
syn keyword atlasStatement	perform leave finish output delay
syn keyword atlasStatement	prepare execute
syn keyword atlasStatement	do
syn match atlasStatement	"\<go[	 ]\+to\>"
syn match atlasStatement	"\<wait[	 ]\+for\>"

syn keyword atlasInclude	include
syn keyword atlasDefine		define require declare identify

"syn keyword atlasReserved	true false go nogo
syn keyword atlasReserved	true false

syn keyword atlasStorageClass	external global

syn keyword atlasConditional	if then else end
syn keyword atlasRepeat		while for thru

" Flags BEF and statement number
syn match atlasSpecial		"^[BE 	][	 ]*[0-9]*\>"
" Highlight Start Test statement numbers differently (100 multiples)
syn match atlasStartTest	"^[	 ]*[0-9]*00\>"

" Number formats
syn match atlasHexNumber	"X'[0-9A-F]\+'"
syn match atlasOctalNumber	"O'[0-7]\+'"
syn match atlasBinNumber	"B'[01]\+'"
syn match atlasNumber		"\<[0-9]\+\>"
"Floating point number part only
syn match atlasDecimalNumber	"\.[0-9]\+\([eE][-+]\=[0-9]\)\=\>"

syn region  atlasFormatString	start=+((+	end=+))+
syn region  atlasString		start=+\<C'+	end=+'+   oneline
syn region  atlasComment	start=+^C+	end=+\$+

syn match  atlasIdentifier	"'[A-Za-z0-9 ._-]\+'"

"Synchronization with Statement terminator $
syn sync match atlasTerminator grouphere atlasComment "^C"
syn sync match atlasTerminator groupthere NONE "\$"
syn sync maxlines=100


if !exists("did_atlas_syntax_inits")
  let did_atlas_syntax_inits = 1

  hi link atlasConditional	Conditional
  hi link atlasRepeat		Repeat
  hi link atlasStatement	Statement
  hi link atlasNumber		Number
  hi link atlasHexNumber	Number
  hi link atlasOctalNumber	Number
  hi link atlasBinNumber	Number
  hi link atlasDecimalNumber	Float
  hi link atlasFormatString	String
  hi link atlasString		String
  hi link atlasComment		Comment
  hi link atlasInclude		Include
  hi link atlasDefine		Macro
  hi link atlasReserved		PreCondit
  hi link atlasStorageClass	StorageClass
  hi link atlasIdentifier	NONE
  hi link atlasSpecial		Special
  hi atlasStartTest		guifg=black guibg=yellow
endif

let b:current_syntax = "atlas"

" vim: ts=8
