" Vim syntax file
" Language:	lilo
" Maintainer:	David Necas (Yeti) <yeti@physics.muni.cz>
" Last Change:	2001 Jan 15
" URL:		http://physics.muni.cz/~yeti/download/lilo.vim
"
" slightly overengineered now. 
" deny-all based: virtually everything is error and only what is recognized
" is higlighted otherwise

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore
set iskeyword=a-z,A-Z,48-57,.,-,_

" base constructs
syn match	liloError	"\S\+"
syn match	liloComment	"#.*$"
syn match	liloEnviron	"\$\w\+" contained
syn match	liloEnviron	"\${[^}]\+}" contained
syn match	liloDecNumber	"-\=\d\+" contained
syn match	liloHexNumber	"0[xX]\x\+" contained
syn match	liloSpecial	contained "\\\(\"\|\\\|$\)"
syn region	liloString	start=+"+ skip=+\\\\\|\\"+ end=+"+ contained contains=liloSpecial,liloEnviron
syn region	liloPath	start=+/+ skip=+\\\\\|\\ \|\\$"+ end=+ \|$+ contained contains=liloSpecial,liloEnviron
syn region	liloAnything	start=+[^ 	#]+ skip=+\\\\\|\\ \|\\$+ end=+ \|$+ contained contains=liloSpecial,liloEnviron,liloString

" path
syn keyword	liloOption	backup boot force-backup install keytable map message nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty
syn keyword	liloKernelOpt	initrd root nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty
syn keyword	liloImageOpt	path loader nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty
syn keyword	liloDiskOpt	partition nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty

" other
syn keyword	liloOption	default serial nextgroup=liloEqAnything,liloEqAnythingComment,liloError skipwhite skipempty
syn keyword	liloKernelOpt	ramdisk nextgroup=liloEqAnything,liloEqAnythingComment,liloError skipwhite skipempty
syn keyword	liloImageOpt	alias label password range nextgroup=liloEqAnything,liloEqAnythingComment,liloError skipwhite skipempty
syn keyword	liloDiskOpt	set nextgroup=liloEqAnything,liloEqAnythingComment,liloError skipwhite skipempty

" symbolic
syn keyword	liloKernelOpt	vga nextgroup=liloEqVga,liloEqVgaComment,liloError skipwhite skipempty

" number
syn keyword	liloOption	delay timeout verbose nextgroup=liloEqDecNumber,liloEqDecNumberComment,liloError skipwhite skipempty
syn keyword	liloDiskOpt	sectors heads cylinders start nextgroup=liloEqDecNumber,liloEqDecNumberComment,liloError skipwhite skipempty

" string
syn keyword	liloKernelOpt	append nextgroup=liloEqString,liloEqStringComment,liloError skipwhite skipempty
syn keyword	liloImageOpt	fallback literal nextgroup=liloEqString,liloEqStringComment,liloError skipwhite skipempty

" hex number
syn keyword	liloImageOpt	map-drive to nextgroup=liloEqHexNumber,liloEqHexNumberComment,liloError skipwhite skipempty
syn keyword	liloDiskOpt	bios normal hidden nextgroup=liloEqHexNumber,liloEqHexNumberComment,liloError skipwhite skipempty

" flag
syn keyword	liloOption	compact fix-table ignore-table linear nowarn prompt
syn keyword	liloKernelOpt	read-only read-write
syn keyword	liloImageOpt	lock optional restricted single-key unsafe
syn keyword	liloDiskOpt	change activate deactivate inaccessible reset

" image
syn keyword	liloImage	image other nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty
syn keyword	liloDisk	disk nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty
syn keyword	liloChRules	change-rules

" vga keywords
syn keyword	liloVgaKeyword	ask ext extended normal contained

" comment followed by equal sign and ...
syn match	liloEqPathComment	"#.*$" contained nextgroup=liloEqPath,liloEqPathComment,liloError skipwhite skipempty
syn match	liloEqVgaComment	"#.*$" contained nextgroup=liloEqVga,liloEqVgaComment,liloError skipwhite skipempty
syn match	liloEqDecNumberComment	"#.*$" contained nextgroup=liloEqDecNumber,liloEqDecNumberComment,liloError skipwhite skipempty
syn match	liloEqHexNumberComment	"#.*$" contained nextgroup=liloEqHexNumber,liloEqHexNumberComment,liloError skipwhite skipempty
syn match	liloEqStringComment	"#.*$" contained nextgroup=liloEqString,liloEqStringComment,liloError skipwhite skipempty
syn match	liloEqAnythingComment	"#.*$" contained nextgroup=liloEqAnything,liloEqAnythingComment,liloError skipwhite skipempty

" equal sign followed by ...
syn match	liloEqPath	"=" contained nextgroup=liloPath,liloPathComment,liloError skipwhite skipempty
syn match	liloEqVga	"=" contained nextgroup=liloVgaKeyword,liloHexNumber,liloDecNumber,liloVgaComment,liloError skipwhite skipempty
syn match	liloEqDecNumber	"=" contained nextgroup=liloDecNumber,liloDecNumberComment,liloError skipwhite skipempty
syn match	liloEqHexNumber	"=" contained nextgroup=liloHexNumber,liloHexNumberComment,liloError skipwhite skipempty
syn match	liloEqString	"=" contained nextgroup=liloString,liloStringComment,liloError skipwhite skipempty
syn match	liloEqAnything	"=" contained nextgroup=liloAnything,liloAnythingComment,liloError skipwhite skipempty

" comment followed by ...
syn match	liloPathComment	"#.*$" contained nextgroup=liloPath,liloPathComment,liloError skipwhite skipempty
syn match	liloVgaComment	"#.*$" contained nextgroup=liloVgaKeyword,liloHexNumber,liloVgaComment,liloError skipwhite skipempty
syn match	liloDecNumberComment	"#.*$" contained nextgroup=liloDecNumber,liloDecNumberComment,liloError skipwhite skipempty
syn match	liloHexNumberComment	"#.*$" contained nextgroup=liloHexNumber,liloHexNumberComment,liloError skipwhite skipempty
syn match	liloStringComment	"#.*$" contained nextgroup=liloString,liloStringComment,liloError skipwhite skipempty
syn match	liloAnythingComment	"#.*$" contained nextgroup=liloAnything,liloAnythingComment,liloError skipwhite skipempty

" The default highlighting.
hi def link liloEqPath			liloEquals
hi def link liloEqWord			liloEquals
hi def link liloEqVga			liloEquals
hi def link liloEqDecNumber		liloEquals
hi def link liloEqHexNumber		liloEquals
hi def link liloEqString		liloEquals
hi def link liloEqAnything		liloEquals
hi def link liloEquals			Special

hi def link liloError			Error

hi def link liloEqPathComment		liloComment
hi def link liloEqVgaComment		liloComment
hi def link liloEqDecNumberComment	liloComment
hi def link liloEqHexNumberComment	liloComment
hi def link liloEqStringComment		liloComment
hi def link liloEqAnythingComment	liloComment
hi def link liloPathComment		liloComment
hi def link liloVgaComment		liloComment
hi def link liloDecNumberComment	liloComment
hi def link liloHexNumberComment	liloComment
hi def link liloStringComment		liloComment
hi def link liloAnythingComment		liloComment
hi def link liloComment			Comment

hi def link liloDiskOpt			liloOption
hi def link liloKernelOpt		liloOption
hi def link liloImageOpt		liloOption
hi def link liloOption			Keyword

hi def link liloString			String
hi def link liloSpecial			Special

hi def link liloPath			Constant
hi def link liloAnything		Normal
hi def link liloEnviron			Identifier

hi def link liloDecNumber		liloNumber
hi def link liloHexNumber		liloNumber
hi def link liloNumber			Number

hi def link liloVgaKeyword		Identifier

hi def link liloImage			Type
hi def link liloChRules			Preproc
hi def link liloDisk			Preproc

let b:current_syntax = "lilo"
" vim: ts=2
