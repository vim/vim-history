" Filename:	spec.vim
" Purpose:	Vim syntax file
" Language:	SPEC: Build/install scripts for Linux RPM packages
" Maintainer:	Donovan Rebbechi elflord@pegasus.rutgers.edu
" URL:		http://pegasus.rutgers.edu/~elflord/vim/syntax/spec.vim
" Last Change:	Wed Sep 22 21:42:34 EDT 1999 (modified 2000 Jul 10 by Bram)

syn clear


syn region specDeref	     start="\${" end="}" contains=specSpecialVariables,specSpecialChar
syn match  specDeref	     "\$\<[a-zA-Z_][a-zA-Z0-9_]*\>" contains=specSpecialVariables,specSpecialChar

syn match specSpecialChar +[][!\$()\\|>^;:{}]+
syn match specColon contained ':'

syn match specSpecialVariables '\$[0-9]\|\${[0-9]}'

syn match specInstallOption contained '-[ogmd]'
syn match specNoNumberHilite 'X11\|X11R6\|[a-zA-Z]*\.\d\|[a-zA-Z][-/]\d'
syn match specManpageFile '[a-zA-Z]\.1'

syn case ignore

"rudimentary macros with little-nothing in the way of scripts inside them
syn region specMacroBody matchgroup=specMacro  start='%changelog\|%description\|%patch\(\d\)?' end='^%'me=e-1 contains=specEmail,specURL,specWeekday,specMonth,specNumber,specComment
syn region specMacroBody2 matchgroup=specMacro  start='%define\|%vendor\|%distribution' end='$' oneline


"PreAmble
syn region specPreAmble matchgroup=specCommand start='^\(Prereq\|Summary\|Name\|Version\|Packager\|Requires\|Icon\|URL\|Source\d*\|Patch\d*\|Prefix\|Packager\|Group\|Copyright\|License\|Release\|BuildRoot\|Distribution\|Vendor\|Provides\|ExclusiveArch\|ExclusiveOS\|Serial\|Obsoletes\|BuildArch\|BuildRequires\|BuildConflicts\|BuildPreReq\|AutoRequires\|AutoReq\)' end='$' contains=specEmail,specURL,specSpecial,specColon oneline

syn region specPackage matchgroup=specMacro start='%package' end='^%'me=e-1 contains=specPreAmble
" sections that contain a lot of scripts
syn region specScriptMacro  matchgroup=specMacro start='%setup\|%install\|%postun\|%post\|%prep\|%preun\|%pre\($\|\s\)\|%build\|%clean' end='^%'me=e-1 contains=specSpecialVariables,@specCommands,specDeref,shDo,shFor,shCaseEsac,specNoNumberHilite,specCommandOpts,specComment,shIf,specSpecialChar,specBabyMacro
syn match specBabyMacro '%else\|%configure\|%GNUconfigure'

syn case match

syn match specURL '\(ftp\|http\)://[^ \t]*'
syn match specWeekday 'Mon\|Tue\|Wed\|Thu\|Fri\|Sat\|\<Sun\>'
syn match specMonth 'January\|February\|March\|April\|May\|June\|July\|August\|September\|October\|November\|December\|Jan\|Feb\|Mar\|Apr\|Jun\|Jul\|Aug\|Sep\|Oct\|Nov\|Dec'
syn match specNumber +^-\=[0-9.-]*[0-9]\|[ \t]-\=[0-9.-]*[0-9]\|-[0-9.-]*[0-9]+
syn match specListedFilesPrefix		contained '%{prefix}/\|/usr/\|/local/\|/opt/\|/X11R6/\|/X11/'me=e-1
syn match specListedFilesBin		contained '/bin/\|/sbin/'me=e-1
syn match speclistedFilesLib		contained '/lib/\|/include/'me=e-1
syn match specListedFilesDoc		contained '/man\d*\|/doc\|/info'
syn match specListedFilesEtc		contained '/etc/'me=e-1
syn match specListedFilesShare	contained '/share/'me=e-1

syn cluster specListedFiles contains=specListedFilesBin,specListedFilesLib,specListedFilesDoc,specListedFilesEtc,specListedFilesShare,specListedFilesPrefix

syn region specFiles matchgroup=specMacro start='^%[Ff]iles' start='^%[Cc]onfig' start='%[Dd]oc' skip='%docdir\|%attr\|%dir\|%config\|%doc\|%defattr' end='^%[a-zA-Z]'me=e-2 contains=@specListedFiles,specFilesMacro

syn match specFilesMacro '%attrib\|%defattr\|%attr\|%dir\|%config\|%docdir\|%doc'


syn match specEmail '<[a-zA-Z0-9\-_]*@.*>'

syn match specCommandOpts '[ \t]--\=[a-zA-Z-]\+'

syn match specConfigure '\./configure'
syn match specTarCommand 'tar[ \t]*[xvzf]\{,5}[ \t]'
syn cluster specCommands contains=specCommand,specTarCommand,specConfigureCommand,specCommandSpecial

syn keyword specSpecialVariables	RPM_BUILD_ROOT RPM_SOURCE_DIR RPM_OPT_FLAGS LDFLAGS CC CC_FLAGS CPPNAME CFLAGS CXX CXXFLAGS CPPFLAGS

syn keyword specCommand	make xmkmf mkdir  chmod ln find sed rm strip moc echo grep ls rm mv mkdir install cp pwd cat tail then else elif cd gzip rmdir ln
syn keyword specSpecial		GPL Makefile  LGPL
syn keyword specCommandSpecial root

syn region specComment start='#' end='$'
syn region  shDo transparent matchgroup=specBlock start="\<do\>" end="\<done\>" contains=ALLBUT,shFunction,shDoError,shCase,specPreAmble,@specListedFiles

syn region specIf  matchgroup=specBlock start="%ifosf\|%ifos\|%ifnos\|%ifarch"  end='%endif'  contains=ALLBUT, specIfError, shCase

syn region  shIf transparent matchgroup=specBlock start="\<if\>" end="\<fi\>" contains=ALLBUT,shFunction,shIfError,shCase,@specListedFiles

syn region  shFor  matchgroup=specBlock start="\<for\>" end="\<in\>" contains=ALLBUT,shFunction,shInError,shCase,@specListedFiles

syn region shCaseEsac transparent matchgroup=specBlock start="\<case\>" matchgroup=NONE end="\<in\>"me=s-1 contains=ALLBUT,shFunction,shCaseError,@specListedFiles nextgroup=shCaseEsac
syn region shCaseEsac matchgroup=specBlock start="\<in\>" end="\<esac\>" contains=ALLBUT,shFunction,shCaseError,@specListedFilesBin
syn region shCase matchgroup=specBlock contained start=")"  end=";;" contains=ALLBUT,shFunction,shCaseError,shCase,@specListedFiles

syn sync match shDoSync       grouphere  shDo       "\<do\>"
syn sync match shDoSync       groupthere shDo       "\<done\>"
syn sync match shIfSync       grouphere  shIf       "\<if\>"
syn sync match shIfSync       groupthere shIf       "\<fi\>"
syn sync match specIfSync     grouphere  specIf     "%ifarch\|%ifos\|%ifnos"
syn sync match specIfSync     groupthere specIf     "%endIf"
syn sync match shForSync      grouphere  shFor      "\<for\>"
syn sync match shForSync      groupthere shFor      "\<in\>"
syn sync match shCaseEsacSync grouphere  shCaseEsac "\<case\>"
syn sync match shCaseEsacSync groupthere shCaseEsac "\<esac\>"

if !exists("did_spec_syntax_inits")
        let did_spec_syntax_inits = 1
        " The default methods for highlighting. Can be overridden later.

          "main types color definitions
          hi link specSection                   Structure
          hi link specSectionMacro              Macro
          hi link specWWWlink                   PreProc
          hi link specOpts                      Operator

          "yes, it's ugly, but white is sooo cool
          if &background == "dark"
            hi      specGlobalMacro            ctermfg=white
          else
            hi link specGlobalMacro            Identifier
          endif

          "sh colors
          hi link shComment                     Comment
          hi link shIf                          Statement
          hi link shOperator                    Special
          hi link shQuote1                      String
          hi link shQuote2                      String
          hi link shQuoteDelim                  Statement

          "spec colors
          hi link specBlock                     Function
          hi link specColon                     Special
          hi link specCommand                   Statement
          hi link specCommandOpts               specOpts
          hi link specCommandSpecial            Special
          hi link specComment                   Comment
          hi link specConfigure                 specCommand
          hi link specDate                      String
          hi link specDescriptionOpts           specOpts
          hi link specEmail                     specWWWlink
          hi link specError                     Error
          hi link specFilesDirective            specSectionMacro
          hi link specFilesOpts                 specOpts
          hi link specLicense                   String
          hi link specMacroNameLocal            specGlobalMacro
          hi link specMacroNameOther            specGlobalMacro
          hi link specManpageFile               NONE
          hi link specMonth                     specDate
          hi link specNoNumberHilite            NONE
          hi link specNumber                    Number
          hi link specPackageOpts               specOpts
          hi link specPercent                   Special
          hi link specSpecialChar               Special
          hi link specSpecialVariables          specGlobalMacro
          hi link specSpecialVariablesNames     specGlobalMacro
          hi link specTarCommand                specCommand
          hi link specURL                       specWWWlink
          hi link specURLMacro                  specWWWlink
          hi link specVariables                 Identifier
          hi link specWeekday                   specDate
          hi link specListedFilesBin            Statement
          hi link specListedFilesDoc            Statement
          hi link specListedFilesEtc            Statement
          hi link specListedFilesLib            Statement
          hi link specListedFilesPrefix         Statement
          hi link specListedFilesShare          Statement
  endif

let b:current_syntax = "spec"

" vim: ts=8
