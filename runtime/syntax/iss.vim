" Vim syntax file
" Language:	Inno Setup File (*.iss) 
" Maintainer:	Dominique Stéphan (stephan@my-deja.com)
" URL:		http://www.geocities.com/SiliconValley/Bit/1809/vim/syntax/iss.vim
" Last change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" shut case off
syn case ignore

" Section
syn region issHeader		start="\[" end="\]"

" Label in the [Setup] Section
syn match  issLabel	        "^[^=]\+="

" URL
syn match  issURL		"http[s]\=:\/\/.*$"

" syn match  issName		"[^: ]\+:"
syn match  issName		"Name:"
syn match  issName		"MinVersion:\|OnlyBelowVersion:"
syn match  issName		"Source:\|DestDir:\|DestName:\|CopyMode:"
syn match  issName		"Attribs:\|FontInstall:\|Flags:"
syn match  issName		"FileName:\|Parameters:\|WorkingDir:\|Comment:"
syn match  issName		"IconFilename:\|IconIndex:"
syn match  issName		"Section:\|Key:\|String:"
syn match  issName		"Root:\|SubKey:\|ValueType:\|ValueName:\|ValueData:"
syn match  issName		"RunOnceId:"
syn match  issName		"Type:"

syn match  issComment		"^;.*$"

" folder constant
syn match  issFolder		"{[^{]*}"

" string
syn region issString	start=+"+  end=+"+ contains=issFolder

" [Dirs]
syn keyword issDirsFlags deleteafterinstall uninsalwaysuninstall uninsneveruninstall 

" [Files]
syn keyword issFilesCopyMode normal onlyifdoesntexist alwaysoverwrite alwaysskipifsameorolder 
syn keyword issFilesAttribs readonly hidden system
syn keyword issFilesFlags comparetimestampalso confirmoverwrite deleteafterinstall 
syn keyword issFilesFlags external fontisnttruetype isreadme overwritereadonly 
syn keyword issFilesFlags regserver regtypelib restartreplace 
syn keyword issFilesFlags sharedfile skipifsourcedoesntexist uninsneveruninstall 

" [Icons]
syn keyword issIconsFlags createonlyiffileexists runminimized 
syn keyword uninsneveruninstall useapppaths closeonexit dontcloseonexit

" [INI]
syn keyword issINIFlags createkeyifdoesntexist uninsdeleteentry uninsdeletesection uninsdeletesectionifempty 

" [Registry] 
syn keyword issRegRootKey   HKCR HKCU HKLM HKU HKCC
syn keyword issRegValueType none string expandsz multisz dword binary 
syn keyword issRegFlags createvalueifdoesntexist deletekey deletevalue preservestringtype 
syn keyword issRegFlags uninsclearvalue uninsdeletekey uninsdeletekeyifempty uninsdeletevalue noerror

" [Run] and [UninstallRun]
syn keyword issRunFlags nowait shellexec skipifdoesntexist runminimized waituntilidle 


" The default highlighting.
hi def link issHeader		Special
hi def link issComment		Comment
hi def link issLabel		Type
hi def link issName		Type
hi def link issFolder		Special
hi def link issString		String
hi def link issValue		String
hi def link issURL		Include

hi def link issDirsFlags	Keyword
hi def link issFilesCopyMode	Keyword
hi def link issFilesAttribs	Keyword
hi def link issFilesFlags	Keyword
hi def link issIconsFlags	Keyword
hi def link issINIFlags		Keyword
hi def link issRegRootKey	Keyword
hi def link issRegValueType	Keyword
hi def link issRegFlags		Keyword
hi def link issRunFlags		Keyword

let b:current_syntax = "iss"

" vim:ts=8
