" Vim syntax file
" Language:	splint (C with lclint/splint Annotations)
" Maintainer:	Ralf Wildenhues <Ralf.Wildenhues@gmx.de>
" Last Change:	$Date$
" $Revision$

" Note:		Splint annotated files are not detected by default.
"		If you want to use this file for highlighting C code,
"		please make sure splint.vim is sourced instead of c.vim,
"		for example by putting
"			/* vim: set filetype=splint : */
"		at the end of your code or something like
"			au! BufRead,BufNewFile *.c	setfiletype splint
"		in your vimrc file or filetype.vim


" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Read the C syntax to start with
if version < 600
  so <sfile>:p:h/c.vim
else
  runtime! syntax/c.vim
endif


" FIXME: uses and changes several clusters defined in c.vim
"        so watch for changes there

" TODO: make a little more grammar explicit
"	match flags with hyphen and underscore notation
"	match flag expanded forms
"	accept other comment char than @

syn case match
" splint annotations (taken from 'splint -help annotations')
syn match   splintStateAnnot	contained "\(pre\|post\):\(only\|shared\|owned\|dependent\|observer\|exposed\|isnull\|notnull\)"
syn keyword splintSpecTag	contained uses sets defines allocated releases
syn keyword splintModifies	contained modifies requires ensures
syn keyword splintRequires	contained requires ensures
syn keyword splintGlobals	contained globals
syn keyword splintGlobitem	contained internalState fileSystem
syn keyword splintGlobannot	contained undef killed
syn keyword splintWarning	contained warn

syn keyword splintModitem	contained internalState fileSystem nothing maxSet maxRead result
syn keyword splintReqitem	contained MaxSet MaxRead result
syn keyword splintIter		contained iter
syn keyword splintConst		contained constant
syn keyword splintAlt		contained alt

syn keyword splintType		contained abstract concrete mutable immutable refcounted
syn keyword splintGlobalType	contained unchecked checkmod checked checkedstrict
syn keyword splintMemMgm	contained dependent keep killref only owned shared temp
syn keyword splintAlias		contained unique returned
syn keyword splintExposure	contained observer exposed
syn keyword splintDefState	contained out in partial reldef
syn keyword splintGlobState	contained undef killed
syn keyword splintNullState	contained null notnull relnull
syn keyword splintNullPred	contained truenull falsenull nullwhentrue falsewhennull
syn keyword splintExit		contained exits mayexit trueexit falseexit neverexit
syn keyword splintExec		contained noreturn maynotreturn noreturnwhentrue noreturnwhenfalse alwaysreturns
syn keyword splintSef		contained sef
syn keyword splintDecl		contained unused external
syn keyword splintCase		contained fallthrough
syn keyword splintBreak		contained innerbreak loopbreak switchbreak innercontinue
syn keyword splintUnreach	contained notreached
syn keyword splintSpecFunc	contained printflike scanflike messagelike

" TODO: make these region or match
syn keyword splintErrSupp	contained i ignore end t
syn match   splintErrSupp	contained "[it]\d\+\>"
syn keyword splintTypeAcc	contained access noaccess

syn keyword splintMacro		contained notfunction
syn match   splintSpecType	contained "\(\|unsigned\|signed\)integraltype"

" Flags taken from 'splint -help flags full' divided in local and global flags
"                                Local Flags:
syn keyword splintFlag contained abstract accessall accessczech accessczechoslovak accessfile
syn keyword splintFlag contained accessmodule accessslovak aliasunique allblock allempty
syn keyword splintFlag contained allglobs allimponly allmacros alwaysexits annotationerror
syn keyword splintFlag contained ansi89limits assignexpose badflag bitwisesigned boolcompare
syn keyword splintFlag contained boolfalse boolint boolops booltrue booltype
syn keyword splintFlag contained bounds boundscompacterrormessages boundsread boundswrite branchstate
syn keyword splintFlag contained bufferoverflow bufferoverflowhigh bugslimit casebreak castexpose
syn keyword splintFlag contained castfcnptr charindex charint charintliteral charunsignedchar
syn keyword splintFlag contained checkedglobalias checkmodglobalias checkpost checkstrictglobalias checkstrictglobs
syn keyword splintFlag contained codeimponly commentchar commenterror compdef compdestroy
syn keyword splintFlag contained compmempass constmacros constprefix constprefixexclude constuse
syn keyword splintFlag contained continuecomment controlnestdepth cppnames czech czechconsts
syn keyword splintFlag contained czechfcns czechmacros czechoslovak czechoslovakconsts czechoslovakfcns
syn keyword splintFlag contained czechoslovakmacros czechoslovaktypes czechoslovakvars czechtypes czechvars
syn keyword splintFlag contained debugfcnconstraint declundef deepbreak deparrays dependenttrans
syn keyword splintFlag contained distinctexternalnames distinctinternalnames duplicatecases duplicatequals elseifcomplete
syn keyword splintFlag contained enumindex enumint enummembers enummemuse enumprefix
syn keyword splintFlag contained enumprefixexclude evalorder evalorderuncon exitarg exportany
syn keyword splintFlag contained exportconst exportfcn exportheader exportheadervar exportiter
syn keyword splintFlag contained exportlocal exportmacro exporttype exportvar exposetrans
syn keyword splintFlag contained externalnamecaseinsensitive externalnamelen externalprefix externalprefixexclude fcnderef
syn keyword splintFlag contained fcnmacros fcnpost fcnuse fielduse fileextensions
syn keyword splintFlag contained filestaticprefix filestaticprefixexclude firstcase fixedformalarray floatdouble
syn keyword splintFlag contained forblock forcehints forempty forloopexec formalarray
syn keyword splintFlag contained formatcode formatconst formattype forwarddecl freshtrans
syn keyword splintFlag contained fullinitblock globalias globalprefix globalprefixexclude globimponly
syn keyword splintFlag contained globnoglobs globs globsimpmodsnothing globstate globuse
syn keyword splintFlag contained gnuextensions grammar hasyield hints ifblock
syn keyword splintFlag contained ifempty ignorequals ignoresigns immediatetrans impabstract
syn keyword splintFlag contained impcheckedglobs impcheckedspecglobs impcheckedstatics impcheckedstrictglobs impcheckedstrictspecglobs
syn keyword splintFlag contained impcheckedstrictstatics impcheckmodglobs impcheckmodinternals impcheckmodspecglobs impcheckmodstatics
syn keyword splintFlag contained impconj implementationoptional implictconstraint impouts imptype
syn keyword splintFlag contained includenest incompletetype incondefs incondefslib indentspaces
syn keyword splintFlag contained infloops infloopsuncon initallelements initsize internalglobs
syn keyword splintFlag contained internalglobsnoglobs internalnamecaseinsensitive internalnamelen internalnamelookalike iso99limits
syn keyword splintFlag contained isoreserved isoreservedinternal iterbalance iterloopexec iterprefix
syn keyword splintFlag contained iterprefixexclude iteryield its4low its4moderate its4mostrisky
syn keyword splintFlag contained its4risky its4veryrisky keep keeptrans kepttrans
syn keyword splintFlag contained legacy libmacros likelybool limit linelen
syn keyword splintFlag contained lintcomments localprefix localprefixexclude longintegral longsignedintegral
syn keyword splintFlag contained longunsignedintegral longunsignedunsignedintegral loopexec looploopbreak looploopcontinue
syn keyword splintFlag contained loopswitchbreak macroassign macroconstdecl macrodecl macroempty
syn keyword splintFlag contained macrofcndecl macromatchname macroparams macroparens macroredef
syn keyword splintFlag contained macrostmt macrounrecog macrovarprefix macrovarprefixexclude maintype
syn keyword splintFlag contained matchanyintegral matchfields mayaliasunique memchecks memimp
syn keyword splintFlag contained memtrans misplacedsharequal misscase modfilesys modglobs
syn keyword splintFlag contained modglobsnomods modglobsunchecked modinternalstrict modnomods modobserver
syn keyword splintFlag contained modobserveruncon mods modsimpnoglobs modstrictglobsnomods moduncon
syn keyword splintFlag contained modunconnomods modunspec multithreaded mustdefine mustfree
syn keyword splintFlag contained mustfreefresh mustfreeonly mustmod mustnotalias mutrep
syn keyword splintFlag contained namechecks needspec nestcomment nestedextern newdecl
syn keyword splintFlag contained newreftrans nextlinemacros noaccess nocomments noeffect
syn keyword splintFlag contained noeffectuncon noparams nopp noret null
syn keyword splintFlag contained nullassign nullderef nullpass nullptrarith nullret
syn keyword splintFlag contained nullstate nullterminated nullterminated numenummembers numliteral
syn keyword splintFlag contained numstructfields observertrans obviousloopexec oldstyle onlytrans
syn keyword splintFlag contained onlyunqglobaltrans orconstraint overload ownedtrans paramimptemp
syn keyword splintFlag contained paramuse parenfileformat partial passunknown portability
syn keyword splintFlag contained predassign predbool predboolint predboolothers predboolptr
syn keyword splintFlag contained preproc protoparammatch protoparamname protoparamprefix protoparamprefixexclude
syn keyword splintFlag contained ptrarith ptrcompare ptrnegate quiet readonlystrings
syn keyword splintFlag contained readonlytrans realcompare redecl redef redundantconstraints
syn keyword splintFlag contained redundantsharequal refcounttrans relaxquals relaxtypes repeatunrecog
syn keyword splintFlag contained repexpose retalias retexpose retimponly retval
syn keyword splintFlag contained retvalbool retvalint retvalother sefparams sefuncon
syn keyword splintFlag contained shadow sharedtrans shiftimplementation shiftnegative showallconjs
syn keyword splintFlag contained showcolumn showconstraintlocation showconstraintparens showfunc showscan
syn keyword splintFlag contained showsourceloc showsummary sizeofformalarray sizeoftype skipansiheaders
syn keyword splintFlag contained skipposixheaders slashslashcomment slovak slovakconsts slovakfcns
syn keyword splintFlag contained slovakmacros slovaktypes slovakvars specglobimponly specimponly
syn keyword splintFlag contained specmacros specretimponly specstructimponly specundecl specundef
syn keyword splintFlag contained stackref statemerge statetransfer staticinittrans statictrans
syn keyword splintFlag contained strictbranchstate strictdestroy strictops strictusereleased stringliterallen
syn keyword splintFlag contained stringliteralnoroom stringliteralnoroomfinalnull stringliteralsmaller stringliteraltoolong structimponly
syn keyword splintFlag contained superuser switchloopbreak switchswitchbreak syntax sysdirerrors
syn keyword splintFlag contained sysdirexpandmacros sysunrecog tagprefix tagprefixexclude temptrans
syn keyword splintFlag contained tmpcomments toctou topuse trytorecover type
syn keyword splintFlag contained typeprefix typeprefixexclude typeuse uncheckedglobalias uncheckedmacroprefix
syn keyword splintFlag contained uncheckedmacroprefixexclude uniondef unixstandard unqualifiedinittrans unqualifiedtrans
syn keyword splintFlag contained unreachable unrecog unrecogcomments unrecogdirective unrecogflagcomments
syn keyword splintFlag contained unsignedcompare unusedspecial usedef usereleased usestderr
syn keyword splintFlag contained usevarargs varuse voidabstract warnflags warnlintcomments
syn keyword splintFlag contained warnmissingglobs warnmissingglobsnoglobs warnposixheaders warnrc warnsysfiles
syn keyword splintFlag contained warnunixlib warnuse whileblock whileempty whileloopexec
syn keyword splintFlag contained zerobool zeroptr
"                                      Global Flags:
syn keyword splintGlobalFlag contained dump expect help isolib
syn keyword splintGlobalFlag contained larchpath lclexpect lclimportdir lcs lh
syn keyword splintGlobalFlag contained load mts neverinclude nof nolib
syn keyword splintGlobalFlag contained posixlib posixstrictlib showalluses singleinclude skipsysheaders
syn keyword splintGlobalFlag contained stats strictlib supcounts sysdirs timedist
syn keyword splintGlobalFlag contained tmpdir unixlib unixstrictlib whichlib
syn match   splintFlagExpr contained "[\+\-\=]" nextgroup=splintFlag,splintGlobalFlag

" detect missing /*@ and wrong */
syn match	splintAnnError	"@\*/"
syn cluster	cCommentGroup	add=splintAnnError
syn match	splintAnnError2	"[^@]\*/"hs=s+1 contained
syn region	splintAnnotation start="/\*@" end="@\*/" contains=@splintAnnotElem,cType keepend
syn match	splintShortAnn	"/\*@\*/"
syn cluster	splintAnnotElem	contains=splintStateAnnot,splintSpecTag,splintModifies,splintRequires,splintGlobals,splintGlobitem,splintGlobannot,splintWarning,splintModitem,splintIter,splintConst,splintAlt,splintType,splintGlobalType,splintMemMgm,splintAlias,splintExposure,splintDefState,splintGlobState,splintNullState,splintNullPred,splintExit,splintExec,splintSef,splintDecl,splintCase,splintBreak,splintUnreach,splintSpecFunc,splintErrSupp,splintTypeAcc,splintMacro,splintSpecType,splintAnnError2,splintFlagExpr
syn cluster	splintAllStuff	contains=@splintAnnotElem,splintFlag,splintGlobalFlag
syn cluster	cParenGroup	add=@splintAllStuff
syn cluster	cPreProcGroup	add=@splintAllStuff
syn cluster	cMultiGroup	add=@splintAllStuff

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_splint_syntax_inits")
  if version < 508
    let did_splint_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink splintShortAnn		splintAnnotation
  HiLink splintAnnotation	Comment
  HiLink splintAnnError		splintError
  HiLink splintAnnError2	splintError
  HiLink splintFlag		SpecialComment
  HiLink splintGlobalFlag	splintError
  HiLink splintSpecialAnnot	splintAnnKey
  HiLink splintStateAnnot	splintAnnKey
  HiLink splintSpecTag		splintAnnKey
  HiLink splintModifies		splintAnnKey
  HiLink splintRequires		splintAnnKey
  HiLink splintGlobals		splintAnnKey
  HiLink splintGlobitem		Constant
  HiLink splintGlobannot	splintAnnKey
  HiLink splintWarning		splintAnnKey
  HiLink splintModitem		Constant
  HiLink splintIter		splintAnnKey
  HiLink splintConst		splintAnnKey
  HiLink splintAlt		splintAnnKey
  HiLink splintType		splintAnnKey
  HiLink splintGlobalType	splintAnnKey
  HiLink splintMemMgm		splintAnnKey
  HiLink splintAlias		splintAnnKey
  HiLink splintExposure		splintAnnKey
  HiLink splintDefState		splintAnnKey
  HiLink splintGlobState	splintAnnKey
  HiLink splintNullState	splintAnnKey
  HiLink splintNullPred		splintAnnKey
  HiLink splintExit		splintAnnKey
  HiLink splintExec		splintAnnKey
  HiLink splintSef		splintAnnKey
  HiLink splintDecl		splintAnnKey
  HiLink splintCase		splintAnnKey
  HiLink splintBreak		splintAnnKey
  HiLink splintUnreach		splintAnnKey
  HiLink splintSpecFunc		splintAnnKey
  HiLink splintErrSupp		splintAnnKey
  HiLink splintTypeAcc		splintAnnKey
  HiLink splintMacro		splintAnnKey
  HiLink splintSpecType		splintAnnKey
  HiLink splintAnnKey		Type
  HiLink splintError		Error

  delcommand HiLink
endif

let b:current_syntax = "splint"

" vim: ts=8
