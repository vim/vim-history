" Vim syntax file
" Language:	Debian control files
" Maintainer:	Wichert Akkerman <wakkerma@debian.org>
" URL:		none
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Define some common expressions we can use later on
syn match debcontrolArchitecture contained "\(all\|any\|alpha\|arm\|hppa\|i386\|m68k\|powerpc\|sh\|sparc\|hurd-i386\)"
syn match debcontrolName contained "[a-z][a-z0-9+-]*"
syn match debcontrolPriority contained "\(extra\|important\|optional\|required\|standard\)"
syn match debcontrolSection contained "\(\(contrib\|non-free\)/\)\=\(admin\|base\|comm\|devel\|docs\|editors\|electronics\|games\|graphics\|hamradio\|interpreters\|libs\|mail\|math\|misc\|net\|news\|oldlibs\|otherosfs\|science\|shells\|sound\|tex\|text\|utils\|web\|x11\)"
syn match debcontrolVariable contained "\${.\{-}}"

" List of all legal keys
syn match debcontrolKey contained "^\(Source\|Package\|Section\|Priority\|Maintainer\|Build-Depends\|Standards-Version\|Pre-Depends\|Depends\|Recommends\|Suggests\|Provides\|Replaces\|Conflicts\|Essential\|Architecture\|Description\): *"

" Fields for which we do strict syntax checking
syn region debcontrolStrictField start="^Architecture" end="$" contains=debcontrolKey,debcontrolArchitecture oneline
syn region debcontrolStrictField start="^\(Package\|Source\)" end="$" contains=debcontrolKey,debcontrolName oneline
syn region debcontrolStrictField start="^Priority" end="$" contains=debcontrolKey,debcontrolPriority oneline
syn region debcontrolStrictField start="^Section" end="$" contains=debcontrolKey,debcontrolSection oneline

" Catch-all for the other legal fields
syn region debcontrolField start="^\(Maintainer\|Build-Depends\|Standards-Version\|Pre-Depends\|Depends\|Recommends\|Suggests\|Provides\|Replaces\|Conflicts\|Essential\):" end="$" contains=debcontrolKey,debcontrolVariable oneline
syn region debcontrolMultiField start="^\(Description\):" skip="^ " end="^$"me=s-1 end="^[^ ]"me=s-1 contains=debcontrolKey

" Associate our matches and regions with pretty colours
hi def link debcontrolKey		Statement
hi def link debcontrolField		Constant
hi def link debcontrolStrictField	Error
hi def link debcontrolMultiField	Constant
hi def link debcontrolArchitecture	Constant
hi def link debcontrolName		Constant
hi def link debcontrolPriority		Constant
hi def link debcontrolSection		Constant
hi def link debcontrolVariable		Identifier
