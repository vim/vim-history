" Vim syntax file
" Language:	Virata Configuration Script
" Maintainer:	Manuel M.H. Stol	<mmh.stol@gmx.net>
" Last Change:	2001 Jan 15
" Vim URL:	http://www.vim.org/lang.html
" Virata URL:	http://www.virata.com/


" Virata Configuration Script syntax
"  Might be detected by: 1) Extension .hw, .pkg and .module (and .cfg)
"                        2) The word "Virata" in the first 5 lines


" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Setup Syntax:
"  Virata syntax is case insensetive (mostly)
syn case ignore



" Comments:
" Virata comments start with %, but % is not a keyword character
syn region  virataComment	start="^\s*%" keepend end="$" contains=@virataGrpInComments
syn region  virataSpclComment	start="^\s*%%" keepend end="$" contains=@virataGrpInComments
syn keyword virataInCommentTodo	contained TODO FIXME XXX[XXXXX] REVIEW
syn cluster virataGrpInComments	contains=virataInCommentTodo
syn cluster virataGrpComments	contains=@virataGrpInCommenta,virataComment,virataSpclComment


" Constants:
syn match   virataStringError	+["]+
syn region  virataString	start=+"+ skip=+\(\\\\\|\\"\)+ end=+"+ oneline contains=virataSpclCharError,virataSpclChar,virataDefSubst
syn match   virataCharacter	+'[^']\{-}'+ contains=virataSpclCharError,virataSpclChar
syn match   virataSpclChar	contained +\\\(x\x\+\|\o\{1,3}\|['\"?\\abefnrtv]\)+
syn match   virataNumberError	"\<\d\d\{-}\D\w\{-}\>"
syn match   virataNumberError	"\<0x\x*\X\x*\>"
syn match   virataNumberError	"\<\d\+\.\d*\(e[+-]\=\d\+\)\=\>"
syn match   virataDecNumber	"\<\d\+U\=L\=\>"
syn match   virataHexNumber	"\<0x\x\+U\=L\=\>"
syn match   virataSizeNumber	"\<\d\+[KM]\>"he=e-1
syn cluster virataGrpNumbers	contains=virataNumberError,virataDecNumber,virataHexNumber,virataSizeNumber
syn cluster virataGrpConstants	contains=@virataGrpNumbers,virataStringError,virataString,virataCharacter,virataSpclChar


" File Names:
syn match   virataFileName	"\<\F\f\{-}\>"
" Identifier:
syn match   virataIdentifier	contained "\<\I\i\{-}\(\-\i\{-1,}\)\{-}\>"


" Statements:
syn match   virataStatement	"^\s*Config\(\.hs\=\)\=\>"
syn match   virataStatement	"^\s*Make\.\I\i\{-}\(\-\i\{-1}\)\{-}\>"
syn match   virataStatement	"^\s*Make\.c\(at\)\=++\s"me=e-1
syn match   virataStatement	"^\s*\(Architecture\|Colour\|Hardware\|ModuleSource\|NoInit\|Path\|Reserved\|SysLink\)\>"

" Import (Package <exec>|Module <name> from <dir>)
syn region  virataImportDef	transparent matchgroup=virataStatement start="^\s*Import\>" keepend end="$" contains=virataInImport,virataModuleDef,virataNumberError,virataStringError,virataDefSubst
syn match   virataInImport	contained "\<\(Module\|Package\|from\)\>"
" Export (Header <header file>|SLibrary <obj file>)
syn region  virataExportDef	transparent matchgroup=virataStatement start="^\s*Export\>" keepend end="$" contains=virataInExport,virataNumberError,virataStringError,virataDefSubst
syn match   virataInExport	contained "\<\(Header\|[SU]Library\)\>"
" Process <name> Is <dir/exec>
syn region  virataProcessDef	transparent matchgroup=virataStatement start="^\s*Process\>" keepend end="$" contains=virataInProcess,virataInExec,virataNumberError,virataStringError,virataDefSubst
syn match   virataInProcess	contained "\<is\>"
" Instance <name> from <module>
syn region  virataInstanceDef	transparent matchgroup=virataStatement start="^\s*Instance\>" keepend end="$" contains=virataInInstance,virataNumberError,virataStringError,virataDefSubst
syn match   virataInInstance	contained "\<of\>"
" Module <name> from <dir>
syn region  virataModuleDef	transparent matchgroup=virataStatement start="^\s*Module\>" start="^\s*Package" keepend end="$" contains=virataInModule,virataNumberError,virataStringError,virataDefSubst
syn match   virataInModule	contained "\<from\>"
" Link {<link cmds>}
" Object {Executable [<ExecOptions>]}
syn match   virataStatement	"^\s*\(Link\|Object\)"
" Executable <name> [<ExecOptions>]
syn region  virataExecDef	transparent matchgroup=virataStatement start="^\s*Executable\>" keepend end="$" contains=virataInExec,@virataGrpConstants,virataIdentifier,virataDefSubst
syn match   virataInExec	contained "\<\(epilogue\|pro\(logue\|cess\)\|qhandler\)\>" skipwhite nextgroup=virataIdentifier,virataDefSubst
syn match   virataInExec	contained "\<\(priority\|stack\)\>" skipwhite nextgroup=@virataGrpNumber,virataDefSubst
" Message <name> {<msg format>}
" MessageId <number>
syn match   virataStatement	"^\s*Message\(Id\)\=\>"
" MakeRule <make suffix=file> {<make cmds>}
syn region  virataMakeDef	transparent matchgroup=virataStatement start="^\s*MakeRule\>" keepend end="$" contains=virataInMake,virataDefSubst
syn case match
syn match   virataInMake	contained "\<N\>"
syn case ignore
" (Append|Edit|Copy)Rule <make suffix=file> <subst cmd>
syn match   virataStatement	"^\s*\(Append\|Copy\|Edit\)Rule\>"
" AlterRules in <file> <subst cmd>
syn region  virataAlterDef	transparent matchgroup=virataStatement start="^\s*AlterRules\>" keepend end="$" contains=virataInAlter,virataDefSubst
syn match   virataInAlter	contained "\<in\>"
" Clustering
syn cluster virataGrpInStatmnts	contains=virataInImport,virataInExport,virataInExec,virataInProcess,virataInAlter,virataInInstance,virataInModule
syn cluster virataGrpStatements	contains=@virataGrpInStatmnts,virataStatement,virataImportDef,virataExportDef,virataExecDef,virataProcessDef,virataAlterDef,virataInstanceDef,virataModuleDef

" Cfg File Statements:
syn region  virataCfgFileDef	transparent matchgroup=virataCfgStatement start="^\s*\a\{-}File\>" start="^\s*OutputFile\d\d\=\>" start="^\s*\a\w\{-}[NP]PFile\>" keepend end="$" contains=NONE
syn region  virataCfgSizeDef	transparent matchgroup=virataCfgStatement start="^\s*\a\{-}Size\>" start="^\s*ConfigInfo\>" keepend end="$" contains=@virataGrpNumbers,virataDefSubst
syn region  virataCfgNumberDef	transparent matchgroup=virataCfgStatement start="^\s*FlashchipNum\(b\(er\=\)\=\)\=\>" start="^\s*Granularity\>" keepend end="$" contains=@virataGrpNumbers,virataDefSubst
syn region  virataCfgMacAddrDef	transparent matchgroup=virataCfgStatement start="^\s*MacAddress\>" keepend end="$" contains=virataNumberError,virataInMacAddr,virataDefSubst
syn match   virataInMacAddr	contained "[:]\x\{1,2}"lc=1
syn match   virataInMacAddr	contained "\s\x\{1,2}[:]"lc=1,me=e-1
syn match   virataCfgStatement	"^\s*Target\>"
syn cluster virataGrpCfgs	contains=virataCfgStatement,virataCfgFileDef,virataCfgSizeDef,virataCfgNumberDef,virataCfgMacAddrDef,virataInMacAddr



" PreProcessor Instructions:
"  Defines
syn match   virataDefine	"^\s*\(Un\)\=Set\>"
syn case match
syn match   virataDefSubst	"$\(\d\|[DINRS]\|{\I\i\{-}}\)"
syn case ignore
"  Conditionals
syn cluster virataGrpCntnPreCon	contains=ALLBUT,@virataGrpInComments,@virataGrpInStatmnts
syn region  virataPreConDef	transparent matchgroup=virataPreCondit start="^\s*If\>" end="\<Endif\>" contains=@virataGrpCntnPreCon
syn match   virataPreCondit	contained "\<Else\(\s\+If\)\=\>"
syn region  virataPreConDef	transparent matchgroup=virataPreCondit start="^\s*ForEach\>" end="\<Done\>" contains=@virataGrpCntnPreCon
"  Pre-Processors
syn region  virataPreProc	start="^\s*Error\>" oneline end="$" contains=@virataGrpConstants
syn cluster virataGrpPreProcs	contains=virataDefine,virataDefSubst,virataPreConDef,virataPreCondit


" Synchronize Syntax:
syn sync clear
syn sync minlines=50		"for multiple if region nesting


" The default highlighting.

" Sub Links:
hi def link virataDefSubst	virataPreProc
hi def link virataInAlter	virataOperator
hi def link virataInExec	virataOperator
hi def link virataInExport	virataOperator
hi def link virataInImport	virataOperator
hi def link virataInInstance	virataOperator
hi def link virataInMake	virataOperator
hi def link virataInModule	virataOperator
hi def link virataInProcess	virataOperator
hi def link virataInMacAddr	virataHexNumber


" Comment Group:
hi def link virataComment	Comment
hi def link virataSpclComment	SpecialComment
hi def link virataInCommentTodo	Todo

" Constant Group:
hi def link virataString	String
hi def link virataStringError	Error
hi def link virataCharacter	Character
hi def link virataSpclChar	Special
hi def link virataDecNumber	Number
hi def link virataHexNumber	Number
hi def link virataSizeNumber	Number
hi def link virataNumberError	Error

" PreProc Group:
hi def link virataPreProc	PreProc
hi def link virataDefine	Define
hi def link virataInclude	Include
hi def link virataPreCondit	PreCondit
hi def link virataPreProcError	Error
hi def link virataPreProcWarn	Todo

" Directive Group:
hi def link virataStatement	Statement
hi def link virataCfgStatement	Statement
hi def link virataOperator	Operator
hi def link virataDirective	Keyword

let b:current_syntax = "virata"

" vim:ts=8:sw=2:noet:
