" Vim syntax file
" Language:	Windows Registry Export with regedit (*.reg)
" Maintainer:	Dominique Stéphan (dstephan@my-deja.com)
" URL: http://www.geocities.com/SiliconValley/Bit/1809/vim/syntax/registry.zip
" Last change:	2000 Jul 26

" clear any unwanted syntax defs
syn clear

" shut case off
syn case ignore

" Header of regedit .reg files
" REGEDIT4 on Win9#/NT
" REGEDIT on Win3.1
syn match registryHead		"^REGEDIT[0-9]*\s*$"

" Comment (Usually lines begin by ';', I also saw '*' and '//')
syn match  registryComment	"^\s*;.*$"
syn match  registryComment	"^\s*\*.*$"
syn match  registryComment	"^\s*\/\/.*$"

" Registry Key constant
syn keyword registryHKEY contained HKEY_LOCAL_MACHINE HKEY_CLASSES_ROOT HKEY_CURRENT_USER
syn keyword registryHKEY contained HKEY_USERS HKEY_CURRENT_CONFIG HKEY_DYN_DATA
" Registry Key shortcuts (I only see HKLM in Dmimgmt.reg)
" syn keyword registryHKEY	HKLM HKCR HKCU HKU HKCC HKDD

" Some values often found in the registry
" GUID (Global Unique IDentifier)
syn match   registryGUID "{[0-9A-Fa-f]\{8}\-[0-9A-Fa-f]\{4}\-[0-9A-Fa-f]\{4}\-[0-9A-Fa-f]\{4}\-[0-9A-Fa-f]\{12}}" contains=registryDelimiter

" Disk
" syn match   registryDisk	"[a-zA-Z]:\\\\"

" Separator characters
syn match   registryDelimiter contained	"\\"
syn match   registryDelimiter contained	"\."
syn match   registryDelimiter contained	","
syn match   registryDelimiter contained	":"
syn match   registryDelimiter contained	"-"
syn match   registryDelimiter contained	"\/"
syn match   registryDelimiter contained	"|"

" String
" Special characters in a string
syn match   registrySpecial contained "\\\\"
syn match   registrySpecial contained "\\\""

" String
syn region  registryString	start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=registryGUID,registrySpecial,registryDelimiter

" Path
syn region  registryPath        start="\[" end="\]" contains=registryHKEY,registryGUID,registryDelimiter

" Path to remove
" like preceding path but with a "-" at begin
syn region registryRemove	start="\[\-" end="\]" contains=registryHKEY,registryGUID,registryDelimiter

" Subkey
syn match  registrySubKey	"^\s*\".*\"="
" Default value
syn match  registrySubKey	"^\s*@="
" REGEDIT style (win 3.1) (HKEY_CLASSES_ROOT only I think)
syn match  registrySubKey	"^\s*\(HKEY_LOCAL_MACHINE\|HKEY_CLASSES_ROOT\|HKEY_CURRENT_USER\|HKEY_USERS\|HKEY_CURRENT_CONFIG\|HKEY_DYN_DATA\)\\[^=]*" contains=registryHKEY,registryGUID,registryDelimiter


" Numbers

" Hex or Binary 
" The format can be precised between () :
" 0    REG_NONE                            
" 1    REG_SZ                          
" 2    REG_EXPAND_SZ
" 3    REG_BINARY                          
" 4    REG_DWORD, REG_DWORD_LITTLE_ENDIAN
" 5    REG_DWORD_BIG_ENDIAN
" 6    REG_LINK
" 7    REG_MULTI_SZ
" 8    REG_RESOURCE_LIST
" 9    REG_FULL_RESOURCE_DESCRIPTOR 
" 10   REG_RESOURCE_REQUIREMENTS_LIST
" The value can take several lines, if \ ends the line
syn match registryHex		"hex\(([0-9]\{0,2})\)\=:\([0-9a-fA-F]\{2},\)*\([0-9a-fA-F]\{2}\|\\\)$" contains=registryDelimiter
syn match registryHex		"^\s*\([0-9a-fA-F]\{2},\)*\([0-9a-fA-F]\{2}\|\\\)$" contains=registryDelimiter

" Dword (32 bits)
syn match registryDword		"dword:[0-9a-fA-F]\{1,8}$" contains=registryDelimiter


if !exists("did_registry_syntax_inits")
	let did_registry_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi link registryComment	Comment
	hi link registryHead	Constant
	hi link registryHKEY	Constant
	hi link registryPath	Special
	hi link registryRemove	PreProc
	hi link registryGUID	Identifier
	hi link registrySpecial	Special
	hi link registryDelimiter	Delimiter
	hi link registrySubKey	Type
	hi link registryString	String
	hi link registryHex	Number
	hi link registryDword	Number
endif

let b:current_syntax = "registry"

" vim:ts=8
