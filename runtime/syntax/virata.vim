" Vim syntax file
" Language:	Virata Configuration Script
" Maintainer:	Manuel M.H. Stol	<mmh.stol@gmx.net>
" Last Change:	2001-08-24
" Vim URL:	http://www.vim.org/lang.html
" Virata URL:	http://www.virata.com/


" Virata Configuration Script syntax
"  Might be detected by: 1) Extension .hw, .pkg and .module (and .cfg)
"                        2) The word "Virata" in the first 5 lines


" Setup Syntax:
if version < 600
  "  Clear old syntax settings
  syn clear
elseif exists("b:current_syntax")
  finish
endif
"  Virata syntax is case insensitive (mostly)
syn case ignore



" Comments:
" Virata comments start with %, but % is not a keyword character
syn region  virataComment	start="^%" start="\s%"lc=1 keepend end="$" contains=@virataGrpInComments
syn region  virataSpclComment	start="^%%" start="\s%%"lc=1 keepend end="$" contains=@virataGrpInComments
syn keyword virataInCommentTodo	contained TODO FIXME XXX[XXXXX] REVIEW
syn cluster virataGrpInComments	contains=virataInCommentTodo
syn cluster virataGrpComments	contains=@virataGrpInComments,virataComment,virataSpclComment


" Constants:
syn match   virataStringError	+["]+
syn region  virataString	start=+"+ skip=+\(\\\\\|\\"\)+ end=+"+ oneline contains=virataSpclCharError,virataSpclChar,@virataGrpDefSubsts
syn match   virataCharacter	+'[^']\{-}'+ contains=virataSpclCharError,virataSpclChar
syn match   virataSpclChar	contained +\\\(x\x\+\|\o\{1,3}\|['\"?\\abefnrtv]\)+
syn match   virataNumberError	"\<\d\{-1,}\I\{-1,}\>"
syn match   virataNumberError	"\<0x\x*\X\x*\>"
syn match   virataNumberError	"\<\d\+\.\d*\(e[+-]\=\d\+\)\=\>"
syn match   virataDecNumber	"\<\d\+U\=L\=\>"
syn match   virataHexNumber	"\<0x\x\+U\=L\=\>"
syn match   virataSizeNumber	"\<\d\+[BKM]\>"he=e-1
syn match   virataSizeNumber	"\<\d\+[KM]B\>"he=e-2
syn cluster virataGrpNumbers	contains=virataNumberError,virataDecNumber,virataHexNumber,virataSizeNumber
syn cluster virataGrpConstants	contains=@virataGrpNumbers,virataStringError,virataString,virataCharacter,virataSpclChar


" Identifiers:
syn match   virataIdentError	contained "\<\D\S*\>"
syn match   virataIdentifier	contained "\<\I\i\{-}\(\-\i\{-1,}\)*\>" contains=@virataGrpDefSubsts
syn match   virataFileIdent	contained "\F\f*" contains=@virataGrpDefSubsts
syn cluster virataGrpIdents	contains=virataIdentifier,virataIdentError
syn cluster virataGrpFileIdents	contains=virataFileIdent,virataIdentError


" Statements:
syn match   virataStatement	"^\s*Config\(\.\(hs\=\|s\)\)\=\>"
syn match   virataStatement	"^\s*Config\s\+\I\i\{-}\(\-\i\{-1,}\)*\.\(hs\=\|s\)\>"
syn match   virataStatement	"^\s*Undefine\>"
syn match   virataStatement	"^\s*Make\.\I\i\{-}\(\-\i\{-1}\)*\>" skipwhite nextgroup=@virataGrpIdents
syn match   virataStatement	"^\s*Make\.c\(at\)\=++\s"me=e-1 skipwhite nextgroup=@virataGrpIdents
syn match   virataStatement	"^\s*\(Architecture\|Colour\|Reserved\)\>" skipwhite nextgroup=@virataGrpIdents
syn match   virataStatement	"^\s*\(Hardware\|ModuleSource\|Path\)\>" skipwhite nextgroup=@virataGrpFileIdents
syn match   virataStatement	"^\s*\(DefaultPri\(ority\)\=\|Hydrogen\)\>" skipwhite nextgroup=virataDecNumber,virataNumberError
syn match   virataStatement	"^\s*\(Allow\s\+ModuleConfig\|NoInit\|PCI\|SysLink\)\>"

" Import (Package <exec>|Module <name> from <dir>)
syn region  virataImportDef	transparent matchgroup=virataStatement start="^\s*Import\>" keepend end="$" contains=virataInImport,virataModuleDef,virataNumberError,virataStringError,@virataGrpDefSubsts
" Export (Header <header file>|SLibrary <obj file>)
syn region  virataExportDef	transparent matchgroup=virataStatement start="^\s*Export\>" keepend end="$" contains=virataInExport,virataNumberError,virataStringError,@virataGrpDefSubsts
syn match   virataInExport	contained "\<\(Header\|[SU]Library\)\>" skipwhite nextgroup=@virataGrpFileIdents
" Process <name> Is <dir/exec>
syn region  virataProcessDef	transparent matchgroup=virataStatement start="^\s*Process\>" keepend end="$" contains=virataInProcess,virataInExec,virataNumberError,virataStringError,@virataGrpDefSubsts,@virataGrpIdents
syn match   virataInProcess	contained "\<is\>"
" Instance <name> from <module>
syn region  virataInstanceDef	transparent matchgroup=virataStatement start="^\s*Instance\>" keepend end="$" contains=virataInInstance,virataNumberError,virataStringError,@virataGrpDefSubsts,@virataGrpIdents
syn match   virataInInstance	contained "\<of\>"
" Module <name> from <dir>
syn region  virataModuleDef	transparent matchgroup=virataStatement start="^\s*\(Module\|Package\)\>" keepend end="$" contains=virataInModule,virataNumberError,virataStringError,@virataGrpDefSubsts
syn match   virataInModule	contained "^\s*\(Module\|Package\)\>" skipwhite nextgroup=@virataGrpIdents
syn match   virataInModule	contained "\<from\>" skipwhite nextgroup=@virataGrpFileIdents
" Link {<link cmds>}
" Object {Executable [<ExecOptions>]}
syn match   virataStatement	"^\s*\(Link\|Object\)"
" Executable <name> [<ExecOptions>]
syn region  virataExecDef	transparent matchgroup=virataStatement start="^\s*Executable\>" keepend end="$" contains=virataInExec,virataNumberError,virataStringError
syn match   virataInExec	contained "^\s*Executable\>" skipwhite nextgroup=@virataGrpDefSubsts,@virataGrpIdents
syn match   virataInExec	contained "\<\(epilogue\|pro\(logue\|cess\)\|qhandler\)\>" skipwhite nextgroup=@virataGrpDefSubsts,@virataGrpIdents
syn match   virataInExec	contained "\<\(priority\|stack\)\>" skipwhite nextgroup=@virataGrpDefSubsts,@virataGrpNumbers
" Message <name> {<msg format>}
" MessageId <number>
syn match   virataStatement	"^\s*Message\(Id\)\=\>" skipwhite nextgroup=@virataGrpNumbers
" MakeRule <make suffix=file> {<make cmds>}
syn region  virataMakeDef	transparent matchgroup=virataStatement start="^\s*MakeRule\>" keepend end="$" contains=virataInMake,@virataGrpDefSubsts
syn case match
syn match   virataInMake	contained "\<N\>"
syn case ignore
" (Append|Edit|Copy)Rule <make suffix=file> <subst cmd>
syn match   virataStatement	"^\s*\(Append\|Copy\|Edit\)Rule\>"
" AlterRules in <file> <subst cmd>
syn region  virataAlterDef	transparent matchgroup=virataStatement start="^\s*AlterRules\>" keepend end="$" contains=virataInAlter,@virataGrpDefSubsts
syn match   virataInAlter	contained "\<in\>" skipwhite nextgroup=@virataGrpIdents
" Clustering
syn cluster virataGrpInStatmnts	contains=virataInImport,virataInExport,virataInExec,virataInProcess,virataInAlter,virataInInstance,virataInModule
syn cluster virataGrpStatements	contains=@virataGrpInStatmnts,virataStatement,virataImportDef,virataExportDef,virataExecDef,virataProcessDef,virataAlterDef,virataInstanceDef,virataModuleDef

" Cfg File Statements:
syn region  virataCfgFileDef	transparent matchgroup=virataCfgStatement start="^\s*Dir\>" start="^\s*\a\{-}File\>" start="^\s*OutputFile\d\d\=\>" start="^\s*\a\w\{-}[NP]PFile\>" keepend end="$" contains=@virataGrpFileIdents
syn region  virataCfgSizeDef	transparent matchgroup=virataCfgStatement start="^\s*\a\{-}Size\>" start="^\s*ConfigInfo\>" keepend end="$" contains=@virataGrpNumbers,@virataGrpDefSubsts,virataIdentError
syn region  virataCfgNumberDef	transparent matchgroup=virataCfgStatement start="^\s*FlashchipNum\(b\(er\=\)\=\)\=\>" start="^\s*Granularity\>" keepend end="$" contains=@virataGrpNumbers,@virataGrpDefSubsts
syn region  virataCfgMacAddrDef	transparent matchgroup=virataCfgStatement start="^\s*MacAddress\>" keepend end="$" contains=virataNumberError,virataStringError,virataIdentError,virataInMacAddr,@virataGrpDefSubsts
syn match   virataInMacAddr	contained "\x[:]\x\{1,2}\>"lc=2
syn match   virataInMacAddr	contained "\s\x\{1,2}[:]\x"lc=1,me=e-1,he=e-2 nextgroup=virataInMacAddr
syn match   virataCfgStatement	"^\s*Target\>" skipwhite nextgroup=@virataGrpIdents
syn cluster virataGrpCfgs	contains=virataCfgStatement,virataCfgFileDef,virataCfgSizeDef,virataCfgNumberDef,virataCfgMacAddrDef,virataInMacAddr



" PreProcessor Instructions:
"  Defines
syn match   virataDefine	"^\s*\(Un\)\=Set\>"
syn match   virataDefSubstError	"[^$]$"lc=1
syn match   virataDefSubstError	"$\(\w\|{\(.\{-}}\)\=\)"
syn case match
syn match   virataDefSubst	"$\(\d\|[DINORS]\|{\I\i\{-}\(\-\i\{-1,}\)*}\)"
syn case ignore
"  Conditionals
syn cluster virataGrpCntnPreCon	contains=ALLBUT,@virataGrpInComments,@virataGrpFileIdents,@virataGrpInStatmnts
syn region  virataPreConDef	transparent matchgroup=virataPreCondit start="^\s*If\>" end="^\s*Endif\>" contains=@virataGrpCntnPreCon
syn match   virataPreCondit	contained "^\s*Else\(\s\+If\)\=\>"
syn region  virataPreConDef	transparent matchgroup=virataPreCondit start="^\s*ForEach\>" end="^\s*Done\>" contains=@virataGrpCntnPreCon
"  Pre-Processors
syn region  virataPreProc	start="^\s*Error\>" oneline end="$" contains=@virataGrpConstants,@virataGrpDefSubsts
syn cluster virataGrpDefSubsts	contains=virataDefSubstError,virataDefSubst
syn cluster virataGrpPreProcs	contains=@virataGrpDefSubsts,virataDefine,virataPreConDef,virataPreCondit


" Synchronize Syntax:
syn sync clear
syn sync minlines=50		"for multiple region nesting



" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later  : only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_virata_syntax_inits")
  if version < 508
    let did_virata_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  " Sub Links:
  HiLink virataDefSubstError	virataPreProcError
  HiLink virataDefSubst		virataPreProc
  HiLink virataInAlter		virataOperator
  HiLink virataInExec		virataOperator
  HiLink virataInExport		virataOperator
  HiLink virataInImport		virataOperator
  HiLink virataInInstance	virataOperator
  HiLink virataInMake		virataOperator
  HiLink virataInModule		virataOperator
  HiLink virataInProcess	virataOperator
  HiLink virataInMacAddr	virataHexNumber

  " Comment Group:
  HiLink virataComment		Comment
  HiLink virataSpclComment	SpecialComment
  HiLink virataInCommentTodo	Todo

  " Constant Group:
  HiLink virataString		String
  HiLink virataStringError	Error
  HiLink virataCharacter	Character
  HiLink virataSpclChar		Special
  HiLink virataDecNumber	Number
  HiLink virataHexNumber	Number
  HiLink virataSizeNumber	Number
  HiLink virataNumberError	Error

  " Identifier Group:
  HiLink virataIdentError	Error

  " PreProc Group:
  HiLink virataPreProc		PreProc
  HiLink virataDefine		Define
  HiLink virataInclude		Include
  HiLink virataPreCondit	PreCondit
  HiLink virataPreProcError	Error
  HiLink virataPreProcWarn	Todo

  " Directive Group:
  HiLink virataStatement	Statement
  HiLink virataCfgStatement	Statement
  HiLink virataOperator		Operator
  HiLink virataDirective	Keyword

  delcommand HiLink
endif

let b:current_syntax = "virata"

" vim:ts=8:sw=2:noet:
