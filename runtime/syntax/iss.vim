" Vim syntax file
" Language:	Inno Setup File (*.iss) 
" Maintainer:	Dominique Stéphan (stephan@my-deja.com)
" URL: 		http://www.geocities.com/SiliconValley/Bit/1809/vim/syntax/iss.vim
" Last change:	2000 Aug 05

" clear any unwanted syntax defs
syn clear

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



if !exists("did_iss_syntax_inits")
	let did_iss_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi link issHeader	Special
	hi link issComment	Comment
	hi link issLabel	Type
	hi link issName		Type
	hi link issFolder	Special
	hi link issString	String
	hi link issValue	String
	hi link issURL		Include

	hi link issDirsFlags	Keyword
	hi link issFilesCopyMode Keyword
	hi link issFilesAttribs Keyword
	hi link issFilesFlags 	Keyword
	hi link issIconsFlags	Keyword
	hi link issINIFlags	Keyword
	hi link issRegRootKey	Keyword
	hi link issRegValueType	Keyword
	hi link issRegFlags	Keyword
	hi link issRunFlags	Keyword

endif

let b:current_syntax = "iss"

" vim:ts=8
