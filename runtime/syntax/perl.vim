" Vim syntax file
" Language:	Perl
" Maintainer:	Nick Hibma <n_hibma@webweaving.org>
" Last Change:	1999 Dec 27
" Location:	http://www.etla.net/~n_hibma/vim/syntax/perl.vim
"
" Please download most recent version first before mailing
" any comments.
" See also the file perl.vim.regression.pl to check whether your
" modifications work in the most odd cases
"       http://www.etla.net/~n_hibma/vim/syntax/perl.vim.regression.pl
"
" Original version: Sonia Heimann <niania@netsurf.org>
" Thanks to many people for their contribution. They made it work, not me.

" The following parameters are available for tuning the
" perl syntax highlighting, with defaults given:
"
" unlet perl_include_pod
" unlet perl_want_scope_in_variables
" unlet perl_extended_vars
" unlet perl_string_as_statement
" unlet perl_no_sync_on_sub
" unlet perl_no_sync_on_global_var
" let perl_sync_dist = 100

" Remove any old syntax stuff hanging around
syn clear

" POD starts with ^=<word> and ends with ^=cut

if exists("perl_include_pod")
  " Include a while extra syntax file
  syntax include @Pod <sfile>:p:h/pod.vim
  syntax region perlPOD start="^=[a-z]" end="^=cut" contains=@Pod,perlTodo keepend
else
  " Use only the bare minimum of rules
  syntax region perlPOD start="^=[a-z]" end="^=cut"
endif


" All keywords
"
syn keyword perlConditional      if elsif unless switch eq ne gt lt ge le cmp not and or xor
syn keyword perlConditional      else nextgroup=perlElseIfError skipwhite skipnl skipempty
syn keyword perlRepeat           while for foreach do until
syn keyword perlOperator         defined undef and or not bless ref
syn keyword perlControl          BEGIN END

syn keyword perlStatementStorage my local
syn keyword perlStatementControl goto return last next continue redo
syn keyword perlStatementScalar  chomp chop chr crypt index lc lcfirst length ord pack reverse rindex sprintf substr uc ucfirst
syn keyword perlStatementRegexp  pos quotemeta split study
syn keyword perlStatementNumeric abs atan2 cos exp hex int log oct rand sin sqrt srand
syn keyword perlStatementList    splice unshift shift push pop split join reverse grep map sort unpack
syn keyword perlStatementHash    each exists keys values
syn keyword perlStatementIOfunc  binmode carp close closedir confess croak dbmclose dbmopen die eof fileno flock getc print printf read readdir rewinddir seek seekdir select syscall sysopen sysread syswrite tell telldir truncate warn write
syn keyword perlStatementVector  pack vec
syn keyword perlStatementFiles   chdir chmod chown chroot fcntl glob ioctl link lstat mkdir open opendir readlink rename rmdir stat symlink umask unlink utime
syn match   perlStatementFiles   "-[rwxoRWXOezsfdlpSbctugkTBMAC]\>"
syn keyword perlStatementFlow    caller die dump eval exit wantarray
syn keyword perlStatementInclude require
syn match   perlStatementInclude "\(use\|no\)\s\+\(integer\>\|strict\>\|lib\>\|sigtrap\>\|subs\>\|vars\>\)\="
syn keyword perlStatementScope   import
syn keyword perlStatementProc    alarm exec fork getpgrp getppid getpriority kill pipe setpgrp setpriority sleep system times wait waitpid
syn keyword perlStatementSocket  accept bind connect getpeername getsockname getsockopt listen recv send setsockopt shutdown socket socketpair
syn keyword perlStatementIPC     msgctl msgget msgrcv msgsnd semctl semget semop shmctl shmget shmread shmwrite
syn keyword perlStatementNetwork endprotoent endservent gethostbyaddr gethostbyname gethostent getnetbyaddr getnetbyname getnetent getprotobyname getprotobynumber getprotoent getservbyname getservbyport getservent sethostent setnetent setprotoent setservent
syn keyword perlStatementPword   getgrent getgrgid getgrnam getlogia
syn keyword perlStatementTime    gmtime localtime time times

syn keyword perlStatementMisc    print warn formline reset scalar new delete STDIN STDOUT STDERR

syn keyword perlTodo             TODO TBD FIXME XXX contained

" Perl Identifiers.
"
" Should be cleaned up to better handle identifiers in particular situations
" (in hash keys for example)
"
" Plain identifiers: $foo, @foo, $#foo, %foo, &foo and dereferences $$foo, @$foo, etc.
" We do not process complex things such as @{${"foo"}}. Too complicated, and
" too slow. And what is after the -> is *not* considered as part of the
" variable - there again, too complicated and too slow.

" Special variables first ($^A, ...) and ($|, $', ...)
syn match perlVarPlain "$^[ADEFHILMOPSTWX]\="
syn match perlVarPlain "$[\\\"\[\]'&`+*.,;=%~!?@$<>(0-9-]"
" Same as above, but avoids confusion in $::foo (equivalent to $main::foo)
syn match perlVarPlain "$:[^:]"
" These variables are not recognized within matches.
syn match perlVarNotInMatches "$[|)]"
" This variable is not recognized within matches delimited by m//.
syn match perlVarSlash "$/"

" And plain identifiers
syn match perlPackageRef "\(\h\w*\)\=\(::\|'\)\I"me=e-1 contained

" To highlight packages in variables as a scope reference - i.e. in $pack::var,
" pack:: is a scope, just set "perl_want_scope_in_variables"
" If you *want* complex things like @{${"foo"}} to be processed,
" just set the variable "perl_extended_vars"...

" FIXME value between {} should be marked as string. is treated as such by Perl.
" At the moment it is marked as something greyish instead of read. Probably todo
" with transparency. Or maybe we should handle the bare word in that case. or make it into

if exists("perl_want_scope_in_variables")
  syn match perlVarPlain   "\\\=\([@%&$]\|\$#\)\$*\(\I\i*\)\=\(\(::\|'\)\I\i*\)*\>" contains=perlPackageRef nextgroup=perlVarMember,perlVarSimpleMember
else
  syn match perlVarPlain   "\\\=\([@%&$]\|\$#\)\$*\(\I\i*\)\=\(\(::\|'\)\I\i*\)*\>" nextgroup=perlVarMember,perlVarSimpleMember
endif

if exists("perl_extended_vars")
  syn cluster perlExpr contains=perlStatementScalar,perlStatementRegexp,perlStatementNumeric,perlStatementList,perlStatementHash,perlStatementFiles,perlStatementTime,perlStatementMisc,perlVarPlain,perlVarNotInMatches,perlVarSlash,perlVarBlock,perlShellCommand,perlNumber,perlStringUnexpanded,perlString,perlQQ
  syn region perlVarBlock matchgroup=perlVarPlain start="\($#\|[@%\$]\){" skip="\\}" end="}" contains=@perlExpr nextgroup=perlVarMember,perlVarSimpleMember
  syn match perlVarPlain   "\\\=\(\$#\|[@%&$]\)\$*{\I\i*}" nextgroup=perlVarMember,perlVarSimpleMember
  syn region perlVarMember matchgroup=perlVarPlain start="\(->\)\={" skip="\\}" end="}" contained contains=@perlExpr nextgroup=perlVarMember,perlVarSimpleMember
  syn match perlVarSimpleMember "\(->\)\={\I\i*}" nextgroup=perlVarMember,perlVarSimpleMember contains=perlVarSimpleMemberName
  syn match perlVarSimpleMemberName "\I\i*" contained
  syn region perlVarMember matchgroup=perlVarPlain start="\(->\)\=\[" skip="\\]" end="]" contained contains=@perlExpr nextgroup=perlVarMember,perlVarSimpleMember
endif


" Special characters in strings and matches
syn match perlSpecialString  "\\\(\d\+\|[xX]\x\+\|c\u\|.\)" contained
syn match perlSpecialStringU "\\['\\]" contained
syn match perlSpecialMatch   "{\d\(,\d\)\=}" contained
syn match perlSpecialMatch   "\[\(\]\|-\)\=[^\[\]]*\(\[\|\-\)\=\]" contained
syn match perlSpecialMatch   "[+*()?.]" contained
syn match perlSpecialMatch   "(?[#:=!]" contained
syn match perlSpecialMatch   "(?[imsx]\+)" contained
" FIXME the line below does not work. It should mark end of line and
" begin of line as perlSpecial.
" syn match perlSpecialBEOM    "^\^\|\$$" contained

" Possible errors
"
" Highlight lines with only whitespace (only in blank delimited here documents) as errors
syn match perlNotEmptyLine  "^\s\+$" contained
syn keyword perlElseIfError if contained


" Variable interpolation
"
" These items are interpolated inside "" strings and similar constructs.
syn cluster perlInterpDQ contains=perlSpecialString,perlVarPlain,perlVarNotInMatches,perlVarSlash,perlVarBlock
" These items are interpolated inside '' strings and similar constructs.
syn cluster perlInterpSQ contains=perlSpecialStringU
" These items are interpolated inside m// matches and s/// substitutions.
syn cluster perlInterpSlash contains=perlSpecialString,perlSpecialMatch,perlVarPlain,perlVarBlock,perlSpecialBEOM
" These items are interpolated inside m## matches and s### substitutions.
syn cluster perlInterpMatch contains=@perlInterpSlash,perlVarSlash

" Shell commands
syn region  perlShellCommand matchgroup=perlMatchStartEnd start="`" end="`" contains=@perlInterpDQ

" Constants
"
" Numbers
syn match perlNumber "-\=\<\d\+L\=\>\|0[xX]\x\+\>"

" Simple version of searches and matches
" caters for m//, m## and m[] (and the !/ variant)
syn region perlMatch matchgroup=perlMatchStartEnd start=+[m!]/+ end=+/[xosmige]*+ contains=@perlInterpSlash
syn region perlMatch matchgroup=perlMatchStartEnd start=+[m!]#+ end=+#[xosmige]*+ contains=@perlInterpMatch
syn region perlMatch matchgroup=perlMatchStartEnd start=+[m!]\[+ end=+\][xosmige]*+ contains=@perlInterpMatch

" Below some hacks to recognise the // variant. This is virtually impossible to catch in all
" cases as the / is used in so many other ways, but these should be the most obvious ones.
syn region perlMatch matchgroup=perlMatchStartEnd start=+split /+lc=5 start=+[!=]\~\s*/+lc=2 start=+[(~]/+lc=1 start=+\.\./+lc=2 start=+\s/[^= \t\d$@%]+lc=1,me=e-1,rs=e-1 start=+^/+ skip=+\\/+ end=+/[xosmige]*+ contains=@perlInterpSlash

" Substitutions
" caters for s///, s### and s[][]
" perlMatch is the first part, perlSubstitution is the substitution part
syn region perlMatch matchgroup=perlMatchStartEnd start=+\<\(tr\|s\)/+ end=+/+me=e-1 contains=@perlInterpSlash nextgroup=perlSubstitution
syn region perlMatch matchgroup=perlMatchStartEnd start=+\<\(tr\|s\)#+ end=+#+me=e-1 contains=@perlInterpMatch nextgroup=perlSubstitution
syn region perlMatch matchgroup=perlMatchStartEnd start=+\<\(tr\|s\)\[+ end=+\]+ contains=@perlInterpMatch nextgroup=perlSubstitution
syn region perlSubstitution matchgroup=perlMatchStartEnd start=+/+ end=+/[xosmigecd]*+ contained contains=@perlInterpSQ
syn region perlSubstitution matchgroup=perlMatchStartEnd start=+#+ end=+#[xosmige]*+ contained contains=@perlInterpSQ
syn region perlSubstitution matchgroup=perlMatchStartEnd start=+\[+ end=+\][xosmige]*+ contained contains=@perlInterpSQ

" Substitutions
" caters for tr///, tr### and tr[][]
" perlMatch is the first part, perlTranslation is the second, translator part.
syn region perlMatch matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)/+ end=+/+me=e-1 contains=@perlInterpSQ nextgroup=perlTranslation
syn region perlMatch matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)#+ end=+#+me=e-1 contains=@perlInterpSQ nextgroup=perlTranslation
syn region perlMatch matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)\[+ end=+\]+ contains=@perlInterpSQ nextgroup=perlTranslation
syn region perlTranslation matchgroup=perlMatchStartEnd start=+/+ end=+/[xosmigecd]*+ contained contains=@perlInterpSQ
syn region perlTranslation matchgroup=perlMatchStartEnd start=+#+ end=+#[xosmige]*+ contained contains=@perlInterpSQ
syn region perlTranslation matchgroup=perlMatchStartEnd start=+\[+ end=+\][xosmige]*+ contained contains=@perlInterpSQ


" The => operator forces a bareword to the left of it to be interpreted as
" a string
syn match perlString "\<\I\i*\s*=>"me=e-2

" Strings and q, qq, qw and qr expressions
syn region perlStringUnexpanded matchgroup=perlStringStartEnd start="'" end="'" contains=@perlInterpSQ
syn region perlString matchgroup=perlStringStartEnd start=+"+  end=+"+ contains=@perlInterpDQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q#+ end=+#+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q|+ end=+|+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q(+ end=+)+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q{+ end=+}+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q/+ end=+/+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q[qx]#+ end=+#+ contains=@perlInterpDQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q[qx]|+ end=+|+ contains=@perlInterpDQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q[qx](+ end=+)+ contains=@perlInterpDQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q[qx]{+ end=+}+ contains=@perlInterpDQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<q[qx]/+ end=+/+ contains=@perlInterpDQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qw#+  end=+#+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qw|+  end=+|+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qw(+  end=+)+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qw{+  end=+}+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qw/+  end=+/+ contains=@perlInterpSQ
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qr#+  end=+#+ contains=@perlInterpMatch
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qr|+  end=+|+ contains=@perlInterpMatch
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qr(+  end=+)+ contains=@perlInterpMatch
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qr{+  end=+}+ contains=@perlInterpMatch
syn region perlQQ matchgroup=perlStringStartEnd start=+\<qr/+  end=+/+ contains=@perlInterpMatch

" Constructs such as print <<EOF [...] EOF
"
syn region  perlUntilEOF start=+<<EOF+hs=s+2 start=+<<"EOF"+hs=s+2 end="^EOF$" contains=@perlInterpDQ
"syn region  perlUntilEOF start=+<<"EOF"+hs=s+2 end="^EOF$" contains=@perlInterpDQ
syn region  perlUntilEOF start=+<<""+hs=s+2 end="^$" contains=@perlInterpDQ,perlNotEmptyLine
syn region  perlUntilEOF start=+<<'EOF'+hs=s+2 end="^EOF$" contains=@perlInterpSQ
syn region  perlUntilEOF start=+<<''+hs=s+2 end="^$" contains=@perlInterpSQ,perlNotEmptyLine

" Class declarations
"
syn match perlPackageDecl "^\s*package\>[^;]*" contains=perlStatementPackage
syn keyword perlStatementPackage package contained

" Functions
"       sub [name] [(prototype)] {
"
syn region perlFunction start="\s*sub\s\+"rs=s end="[;{]"he=e-1 contains=perlStatementSub,perlFunctionPrototype,perlFunctionPRef,perlFunctionName,perlComment
syn keyword perlStatementSub sub contained

syn match perlFunctionPrototype "([^)]*)" contained
if exists("perl_want_scope_in_variables")
   syn match perlFunctionPRef "\h\w*::" contained
   syn match perlFunctionName "\h\w*[^:]" contained
else
   syn match perlFunctionName "\h[[:alnum:]_:]*" contained
endif


" All other # are comments, except ^#!
syn match  perlComment   "#.*" contains=perlTodo
syn match  perlSharpBang "^#!.*"

" Formats
syn region perlFormat matchgroup=perlStatementIOFunc start="^\s*format\s\+\k\+\s*=\s*$"rs=s+6 end="^\s*\.\s*$" contains=perlFormatName,perlFormatField,perlVarPlain
syn match perlFormatName "format\s\+\k\+\s*="lc=7,me=e-1 contained
syn match perlFormatField "[@^][|<>~]\+\(\.\.\.\)\=" contained
syn match perlFormatField "[@^]#[#.]*" contained
syn match perlFormatField "@\*" contained
syn match perlFormatField "@[^A-Za-z_|<>~#*]"me=e-1 contained
syn match perlFormatField "@$" contained


if !exists("did_perl_syntax_inits")
  let did_perl_syntax_inits = 1

  hi link perlSharpBang         PreProc
  hi link perlControl           PreProc
  hi link perlInclude           Include
  hi link perlSpecial           Special
  hi link perlString            String
  hi link perlCharacter         Character
  hi link perlNumber            Number
  hi link perlType              Type
  hi link perlIdentifier        Identifier
  hi link perlLabel             Label
  hi link perlStatement         Statement
  hi link perlConditional       Conditional
  hi link perlRepeat            Repeat
  hi link perlOperator          Operator
  hi link perlFunction          Function
  hi link perlComment           Comment
  hi link perlTodo              Todo
  hi link perlList              perlStatement
  hi link perlMisc              perlStatement
  hi link perlVarPlain          perlIdentifier
  hi link perlVarSimpleMember   perlIdentifier
  hi link perlVarSimpleMemberName	perlString
  hi link perlVarNotInMatches   perlIdentifier
  hi link perlVarSlash          perlIdentifier
  hi link perlQQ                perlString
  hi link perlUntilEOF          perlString
  hi link perlStringUnexpanded  perlString
  hi link perlSubstitution      perlString
  hi link perlTranslation       perlString
  hi link perlMatch             perlString
  hi link perlMatchStartEnd     perlStatement
  if exists("perl_string_as_statement")
     hi link perlStringStartEnd    perlStatement
  else
     hi link perlStringStartEnd    perlString
  endif
  hi link perlFormatName        perlIdentifier
  hi link perlFormatField       perlString
  hi link perlPackageDecl       perlType
  hi link perlStorageClass      perlType
  hi link perlPackageRef        perlType
  hi link perlStatementPackage  perlStatement
  hi link perlStatementSub      perlStatement
  hi link perlStatementStorage  perlStatement
  hi link perlStatementControl  perlStatement
  hi link perlStatementScalar   perlStatement
  hi link perlStatementRegexp   perlStatement
  hi link perlStatementNumeric  perlStatement
  hi link perlStatementList     perlStatement
  hi link perlStatementHash     perlStatement
  hi link perlStatementIOfunc   perlStatement
  hi link perlStatementVector   perlStatement
  hi link perlStatementFiles    perlStatement
  hi link perlStatementFlow     perlStatement
  hi link perlStatementScope    perlStatement
  hi link perlStatementInclude  perlStatement
  hi link perlStatementProc     perlStatement
  hi link perlStatementSocket   perlStatement
  hi link perlStatementIPC      perlStatement
  hi link perlStatementNetwork  perlStatement
  hi link perlStatementPword    perlStatement
  hi link perlStatementTime     perlStatement
  hi link perlStatementMisc     perlStatement
  hi link perlFunctionName      perlIdentifier
  hi link perlFunctionPRef      perlType
  hi link perlPOD               perlComment
  hi link perlShellCommand      perlString
  hi link perlSpecialAscii      perlSpecial
  hi link perlSpecialDollar     perlSpecial
  hi link perlSpecialString     perlSpecial
  hi link perlSpecialStringU    perlSpecial
  hi link perlSpecialMatch      perlSpecial
  hi link perlSpecialBEOM       perlSpecial

  " Possible errors
  hi link perlNotEmptyLine      Error
  hi link perlElseIfError       Error
endif

" Syncing to speed up processing
"
if !exists("perl_no_sync_on_sub")
  syn sync match perlSync grouphere NONE "^\s*package\s"
  syn sync match perlSync grouphere perlFunction "^\s*sub\s"
  syn sync match perlSync grouphere NONE "^}"
endif

if !exists("perl_no_sync_on_global_var")
  syn sync match perlSync grouphere NONE "^$\I[[:alnum:]_:]+\s*=\s*{"
  syn sync match perlSync grouphere NONE "^[@%]\I[[:alnum:]_:]+\s*=\s*("
endif

if exists("perl_sync_dist")
  execute "syn sync maxlines=" . perl_sync_dist
else
  syn sync maxlines=100
endif

syn sync match perlSyncPOD grouphere perlPOD "^=pod"
syn sync match perlSyncPOD grouphere perlPOD "^=head"
syn sync match perlSyncPOD grouphere perlPOD "^=item"
syn sync match perlSyncPOD grouphere NONE "^=cut"

let b:current_syntax = "perl"

" vim: ts=8
