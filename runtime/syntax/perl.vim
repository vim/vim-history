" Vim syntax file
" Language:	Perl
" Maintainer:	Nick Hibma <n_hibma@webweaving.org>
" Last Change:	2001 Apr 20
" Location:	http://www.etla.net/~n_hibma/vim/syntax/perl.vim
"
" Please download most recent version first before mailing
" any comments.
" See also the file perl.vim.regression.pl to check whether your
" modifications work in the most odd cases
"	http://www.etla.net/~n_hibma/vim/syntax/perl.vim.regression.pl
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
" unlet perl_fold

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" POD starts with ^=<word> and ends with ^=cut

if exists("perl_include_pod")
  " Include a while extra syntax file
  syn include @Pod <sfile>:p:h/pod.vim
  unlet b:current_syntax
  syn region perlPOD start="^=[a-z]" end="^=cut" contains=@Pod,perlTodo keepend
else
  " Use only the bare minimum of rules
  syn region perlPOD start="^=[a-z]" end="^=cut"
endif


" All keywords
"
syn keyword perlConditional		if elsif unless switch eq ne gt lt ge le cmp not and or xor
syn keyword perlConditional		else nextgroup=perlElseIfError skipwhite skipnl skipempty
syn keyword perlRepeat			while for foreach do until
syn keyword perlOperator		defined undef and or not bless ref
syn keyword perlControl			BEGIN END

syn keyword perlStatementStorage	my local our
syn keyword perlStatementControl	goto return last next continue redo
syn keyword perlStatementScalar		chomp chop chr crypt index lc lcfirst length ord pack reverse rindex sprintf substr uc ucfirst
syn keyword perlStatementRegexp		pos quotemeta split study
syn keyword perlStatementNumeric	abs atan2 cos exp hex int log oct rand sin sqrt srand
syn keyword perlStatementList		splice unshift shift push pop split join reverse grep map sort unpack
syn keyword perlStatementHash		each exists keys values
syn keyword perlStatementIOfunc		carp confess croak dbmclose dbmopen die syscall
syn keyword perlStatementFiledesc	binmode close closedir eof fileno getc lstat print printf readdir rewinddir select stat tell telldir write nextgroup=perlFiledescStatementNocomma
syn keyword perlStatementFiledesc	fcntl flock ioctl open opendir read seek seekdir sysopen sysread sysseek syswrite truncate nextgroup=perlFiledescStatementComma
syn keyword perlStatementVector		pack vec
syn keyword perlStatementFiles		chdir chmod chown chroot glob link mkdir readlink rename rmdir symlink umask unlink utime
syn match   perlStatementFiles		"-[rwxoRWXOezsfdlpSbctugkTBMAC]\>"
syn keyword perlStatementFlow		caller die dump eval exit wantarray
syn keyword perlStatementInclude	require
syn match   perlStatementInclude	"\(use\|no\)\s\+\(integer\>\|strict\>\|lib\>\|sigtrap\>\|subs\>\|vars\>\|warnings\>\|utf8\>\|byte\>\)\="
syn keyword perlStatementScope		import
syn keyword perlStatementProc		alarm exec fork getpgrp getppid getpriority kill pipe setpgrp setpriority sleep system times wait waitpid
syn keyword perlStatementSocket		accept bind connect getpeername getsockname getsockopt listen recv send setsockopt shutdown socket socketpair
syn keyword perlStatementIPC		msgctl msgget msgrcv msgsnd semctl semget semop shmctl shmget shmread shmwrite
syn keyword perlStatementNetwork	endprotoent endservent gethostbyaddr gethostbyname gethostent getnetbyaddr getnetbyname getnetent getprotobyname getprotobynumber getprotoent getservbyname getservbyport getservent sethostent setnetent setprotoent setservent
syn keyword perlStatementPword		getpwuid getpwnam getpwent setpwent endpwent getgrent getgrgid
syn keyword perlStatementTime		gmtime localtime time times

syn keyword perlStatementMisc		warn formline reset scalar new delete

syn keyword perlTodo			TODO TBD FIXME XXX contained

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
syn match  perlVarPlain		 "$^[ADEFHILMOPSTWX]\="
syn match  perlVarPlain		 "$[\\\"\[\]'&`+*.,;=%~!?@$<>(0-9-]"
" Same as above, but avoids confusion in $::foo (equivalent to $main::foo)
syn match  perlVarPlain		 "$:[^:]"
" These variables are not recognized within matches.
syn match  perlVarNotInMatches	 "$[|)]"
" This variable is not recognized within matches delimited by m//.
syn match  perlVarSlash		 "$/"

" And plain identifiers
syn match  perlPackageRef	 "\(\h\w*\)\=\(::\|'\)\I"me=e-1 contained

" To highlight packages in variables as a scope reference - i.e. in $pack::var,
" pack:: is a scope, just set "perl_want_scope_in_variables"
" If you *want* complex things like @{${"foo"}} to be processed,
" just set the variable "perl_extended_vars"...

" FIXME value between {} should be marked as string. is treated as such by Perl.
" At the moment it is marked as something greyish instead of read. Probably todo
" with transparency. Or maybe we should handle the bare word in that case. or make it into

if exists("perl_want_scope_in_variables")
  syn match  perlVarPlain	"\\\=\([@%$]\|\$#\)\$*\(\I\i*\)\=\(\(::\|'\)\I\i*\)*\>" contains=perlPackageRef nextgroup=perlVarMember,perlVarSimpleMember
  syn match  perlFunctionName	"\\\=\&\$*\(\I\i*\)\=\(\(::\|'\)\I\i*\)*\>" contains=perlPackageRef nextgroup=perlVarMember,perlVarSimpleMember
else
  syn match  perlVarPlain	"\\\=\([@%$]\|\$#\)\$*\(\I\i*\)\=\(\(::\|'\)\I\i*\)*\>" nextgroup=perlVarMember,perlVarSimpleMember
  syn match  perlFunctionName	"\\\=\&\$*\(\I\i*\)\=\(\(::\|'\)\I\i*\)*\>" nextgroup=perlVarMember,perlVarSimpleMember
endif

if exists("perl_extended_vars")
  syn cluster perlExpr		contains=perlStatementScalar,perlStatementRegexp,perlStatementNumeric,perlStatementList,perlStatementHash,perlStatementFiles,perlStatementTime,perlStatementMisc,perlVarPlain,perlVarNotInMatches,perlVarSlash,perlVarBlock,perlShellCommand,perlNumber,perlStringUnexpanded,perlString,perlQQ
  syn region perlVarBlock	matchgroup=perlVarPlain start="\($#\|[@%\$]\){" skip="\\}" end="}" contains=@perlExpr nextgroup=perlVarMember,perlVarSimpleMember
  syn match  perlVarPlain	"\\\=\(\$#\|[@%&$]\)\$*{\I\i*}" nextgroup=perlVarMember,perlVarSimpleMember
  syn region perlVarMember	matchgroup=perlVarPlain start="\(->\)\={" skip="\\}" end="}" contained contains=@perlExpr nextgroup=perlVarMember,perlVarSimpleMember
  syn match  perlVarSimpleMember	"\(->\)\={\I\i*}" nextgroup=perlVarMember,perlVarSimpleMember contains=perlVarSimpleMemberName contained
  syn match  perlVarSimpleMemberName	"\I\i*" contained
  syn region perlVarMember	matchgroup=perlVarPlain start="\(->\)\=\[" skip="\\]" end="]" contained contains=@perlExpr nextgroup=perlVarMember,perlVarSimpleMember
endif

" File Descriptors
syn match  perlFiledescRead	"[<]\h\w\+[>]"

syn match  perlFiledescStatementComma	"\s*(\=\s*\h\w*\>\s*," transparent contained contains=perlFiledescStatement
syn match  perlFiledescStatementNocomma	"\s*(\=\s*\h\w*\>\s\+[^,]"me=e-1 transparent contained contains=perlFiledescStatement

syn match  perlFiledescStatement	"\h\w\+" contained

" Special characters in strings and matches
syn match  perlSpecialString	"\\\(\d\+\|[xX]\x\+\|c\u\|.\)" contained
syn match  perlSpecialStringU	"\\['\\]" contained
syn match  perlSpecialMatch	"{\d\(,\d\)\=}" contained
syn match  perlSpecialMatch	"\[\(\]\|-\)\=[^\[\]]*\(\[\|\-\)\=\]" contained
syn match  perlSpecialMatch	"[+*()?.]" contained
syn match  perlSpecialMatch	"(?[#:=!]" contained
syn match  perlSpecialMatch	"(?[imsx]\+)" contained
" FIXME the line below does not work. It should mark end of line and
" begin of line as perlSpecial.
" syn match perlSpecialBEOM    "^\^\|\$$" contained

" Possible errors
"
" Highlight lines with only whitespace (only in blank delimited here documents) as errors
syn match  perlNotEmptyLine	"^\s\+$" contained
" Highlight '} else if (...) {', it should be '} else { if (...) { ' or
" '} elsif (...) {'.
syn keyword perlElseIfError	if contained

" Variable interpolation
"
" These items are interpolated inside "" strings and similar constructs.
syn cluster perlInterpDQ	contains=perlSpecialString,perlVarPlain,perlVarNotInMatches,perlVarSlash,perlVarBlock
" These items are interpolated inside '' strings and similar constructs.
syn cluster perlInterpSQ	contains=perlSpecialStringU
" These items are interpolated inside m// matches and s/// substitutions.
syn cluster perlInterpSlash	contains=perlSpecialString,perlSpecialMatch,perlVarPlain,perlVarBlock,perlSpecialBEOM
" These items are interpolated inside m## matches and s### substitutions.
syn cluster perlInterpMatch	contains=@perlInterpSlash,perlVarSlash

" Shell commands
syn region  perlShellCommand	matchgroup=perlMatchStartEnd start="`" end="`" contains=@perlInterpDQ

" Constants
"
" Numbers
syn match  perlNumber		"[-+]\=\(\<\d[[:digit:]_]*\(L\=\|[eE][\-+]\=\d\+\)\>\|0[xX]\x[[:xdigit:]_]*\>\)"

" Simple version of searches and matches
" caters for m//, m## and m[] (and the !/ variant)
syn region perlMatch		matchgroup=perlMatchStartEnd start=+[m!]/+ end=+/[cgimosx]*+ contains=@perlInterpSlash
syn region perlMatch		matchgroup=perlMatchStartEnd start=+[m!]#+ end=+#[cgimosx]*+ contains=@perlInterpMatch
syn region perlMatch		matchgroup=perlMatchStartEnd start=+[m!]\[+ end=+\][cgimosx]*+ contains=@perlInterpMatch

" A special case for m!!x which allows for comments and extra whitespace in the pattern
syn region perlMatch		matchgroup=perlMatchStartEnd start=+[m!]!+ end=+![cgimosx]*+ contains=@perlInterpSlash,perlComment

" Below some hacks to recognise the // variant. This is virtually impossible to catch in all
" cases as the / is used in so many other ways, but these should be the most obvious ones.
syn region perlMatch		matchgroup=perlMatchStartEnd start=+^split /+lc=5 start=+[^$@%]\<split /+lc=6 start=+^if /+lc=2 start=+[^$@%]if /+lc=3 start=+[!=]\~\s*/+lc=2 start=+[(~]/+lc=1 start=+\.\./+lc=2 start=+\s/[^= \t0-9$@%]+lc=1,me=e-1,rs=e-1 start=+^/+ skip=+\\/+ end=+/[cgimosx]*+ contains=@perlInterpSlash

" Substitutions
" caters for s///, s### and s[][]
" perlMatch is the first part, perlSubstitution* is the substitution part
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s'+ end=+'+me=e-1 contains=@perlInterpSQ nextgroup=perlSubstitutionSQ
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s"+ end=+"+me=e-1 contains=@perlInterpMatch nextgroup=perlSubstitutionDQ
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s/+ end=+/+me=e-1 contains=@perlInterpSlash nextgroup=perlSubstitutionSlash
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s#+ end=+#+me=e-1 contains=@perlInterpMatch nextgroup=perlSubstitutionHash
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s\[+ end=+\]+ contains=@perlInterpMatch nextgroup=perlSubstitutionBracket skipwhite skipempty skipnl
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s{+ end=+}+ contains=@perlInterpMatch nextgroup=perlSubstitutionCurly skipwhite skipempty skipnl
syn region perlSubstitutionSQ		matchgroup=perlMatchStartEnd start=+'+ end=+'[ecgimosx]*+ contained contains=@perlInterpSQ
syn region perlSubstitutionDQ		matchgroup=perlMatchStartEnd start=+"+ end=+"[ecgimosx]*+ contained contains=@perlInterpDQ
syn region perlSubstitutionSlash	matchgroup=perlMatchStartEnd start=+/+ end=+/[ecgimosx]*+ contained contains=@perlInterpDQ
syn region perlSubstitutionHash		matchgroup=perlMatchStartEnd start=+#+ end=+#[ecgimosx]*+ contained contains=@perlInterpDQ
syn region perlSubstitutionBracket	matchgroup=perlMatchStartEnd start=+\[+ end=+\][ecgimosx]*+ contained contains=@perlInterpDQ
syn region perlSubstitutionCurly	matchgroup=perlMatchStartEnd start=+{+ end=+}[ecgimosx]*+ contained contains=@perlInterpDQ

" A special case for m!!x which allows for comments and extra whitespace in the pattern
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<s!+ end=+!+me=e-1 contains=@perlInterpSlash,perlComment nextgroup=perlSubstitutionPling
syn region perlSubstitutionPling	matchgroup=perlMatchStartEnd start=+!+ end=+![ecgimosx]*+ contained contains=@perlInterpDQ

" Substitutions
" caters for tr///, tr### and tr[][]
" perlMatch is the first part, perlTranslation* is the second, translator part.
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)'+ end=+'+me=e-1 contains=@perlInterpSQ nextgroup=perlTranslationSQ
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)"+ end=+"+me=e-1 contains=@perlInterpSQ nextgroup=perlTranslationDQ
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)/+ end=+/+me=e-1 contains=@perlInterpSQ nextgroup=perlTranslationSlash
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)#+ end=+#+me=e-1 contains=@perlInterpSQ nextgroup=perlTranslationHash
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<\(tr\|y\)\[+ end=+\]+ contains=@perlInterpSQ nextgroup=perlTranslationBracket skipwhite skipempty skipnl
syn region perlMatch		matchgroup=perlMatchStartEnd start=+\<\(tr\|y\){+ end=+}+ contains=@perlInterpSQ nextgroup=perlTranslationCurly skipwhite skipempty skipnl
syn region perlTranslationSQ		matchgroup=perlMatchStartEnd start=+'+ end=+'[cds]*+ contained
syn region perlTranslationDQ		matchgroup=perlMatchStartEnd start=+"+ end=+"[cds]*+ contained
syn region perlTranslationSlash		matchgroup=perlMatchStartEnd start=+/+ end=+/[cds]*+ contained
syn region perlTranslationHash		matchgroup=perlMatchStartEnd start=+#+ end=+#[cds]*+ contained
syn region perlTranslationBracket	matchgroup=perlMatchStartEnd start=+\[+ end=+\][cds]*+ contained
syn region perlTranslationCurly		matchgroup=perlMatchStartEnd start=+{+ end=+}[cds]*+ contained


" The => operator forces a bareword to the left of it to be interpreted as
" a string
syn match  perlString "\<\I\i*\s*=>"me=e-2

" Strings and q, qq, qw and qr expressions

" Brackets in qq()
syn region perlBrackets		start=+(+ end=+)+ contained transparent contains=perlBrackets,@perlStringSQ

syn region perlStringUnexpanded	matchgroup=perlStringStartEnd start="'" end="'" contains=@perlInterpSQ
syn region perlString		matchgroup=perlStringStartEnd start=+"+  end=+"+ contains=@perlInterpDQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q#+ end=+#+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q|+ end=+|+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q(+ end=+)+ contains=@perlInterpSQ,perlBrackets
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q{+ end=+}+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q/+ end=+/+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q[qx]#+ end=+#+ contains=@perlInterpDQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q[qx]|+ end=+|+ contains=@perlInterpDQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q[qx](+ end=+)+ contains=@perlInterpDQ,perlBrackets
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q[qx]{+ end=+}+ contains=@perlInterpDQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<q[qx]/+ end=+/+ contains=@perlInterpDQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qw#+  end=+#+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qw|+  end=+|+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qw(+  end=+)+ contains=@perlInterpSQ,perlBrackets
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qw{+  end=+}+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qw/+  end=+/+ contains=@perlInterpSQ
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qr#+  end=+#[imosx]*+ contains=@perlInterpMatch
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qr|+  end=+|[imosx]*+ contains=@perlInterpMatch
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qr(+  end=+)[imosx]*+ contains=@perlInterpMatch
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qr{+  end=+}[imosx]*+ contains=@perlInterpMatch
syn region perlQQ		matchgroup=perlStringStartEnd start=+\<qr/+  end=+/[imosx]*+ contains=@perlInterpSlash

" Constructs such as print <<EOF [...] EOF, 'here' documents
"
syn match perlStringStartEnd	"<<EOF"
syn match perlStringStartEnd	"<<\s*\(["']\)\(EOF\)\=\1"
syn match perlUntilEOFstart	"<<EOF.*" nextgroup=perlUntilEOFDQ skipnl transparent
syn match perlUntilEOFstart	"<<\s*\"EOF\".*" nextgroup=perlUntilEOFDQ skipnl transparent
syn match perlUntilEOFstart	"<<\s*'EOF'.*" nextgroup=perlUntilEOFSQ skipnl transparent
syn match perlUntilEOFstart	"<<\s*\"\".*" nextgroup=perlUntilEmptyDQ skipnl transparent
syn match perlUntilEOFstart	"<<\s*''.*" nextgroup=perlUntilEmptySQ skipnl transparent
syn region perlUntilEOFDQ	matchgroup=perlStringStartEnd start=++ end="^EOF$" contains=@perlInterpDQ contained
syn region perlUntilEOFSQ	matchgroup=perlStringStartEnd start=++ end="^EOF$" contains=@perlInterpSQ contained
syn region perlUntilEmptySQ	matchgroup=perlStringStartEnd start=++ end="^$" contains=@perlInterpDQ,perlNotEmptyLine contained
syn region perlUntilEmptyDQ	matchgroup=perlStringStartEnd start=++ end="^$" contains=@perlInterpSQ,perlNotEmptyLine contained

" Class declarations
"
syn match  perlPackageDecl	"^\s*package\>[^;]*" contains=perlStatementPackage
syn keyword perlStatementPackage	package contained

" Functions
"       sub [name] [(prototype)] {
"
syn region perlFunction		start="\s*sub\>" end="[;{]"he=e-1 contains=perlStatementSub,perlFunctionPrototype,perlFunctionPRef,perlFunctionName,perlComment
syn keyword perlStatementSub	sub contained

syn match  perlFunctionPrototype	"([^)]*)" contained
if exists("perl_want_scope_in_variables")
   syn match  perlFunctionPRef	"\h\w*::" contained
   syn match  perlFunctionName	"\h\w*[^:]" contained
else
   syn match  perlFunctionName	"\h[[:alnum:]_:]*" contained
endif


" All other # are comments, except ^#!
syn match  perlComment		"#.*" contains=perlTodo
syn match  perlSharpBang	"^#!.*"

" Formats
syn region perlFormat		matchgroup=perlStatementIOFunc start="^\s*format\s\+\k\+\s*=\s*$"rs=s+6 end="^\s*\.\s*$" contains=perlFormatName,perlFormatField,perlVarPlain
syn match  perlFormatName	"format\s\+\k\+\s*="lc=7,me=e-1 contained
syn match  perlFormatField	"[@^][|<>~]\+\(\.\.\.\)\=" contained
syn match  perlFormatField	"[@^]#[#.]*" contained
syn match  perlFormatField	"@\*" contained
syn match  perlFormatField	"@[^A-Za-z_|<>~#*]"me=e-1 contained
syn match  perlFormatField	"@$" contained

" __END__ and __DATA__ clauses
syntax region perlDATA		start="^__\(DATA\|END\)__$" skip="." end="." contains=perlPOD



"
" Folding

if version >= 600 && exists("perl_fold")
  syn region myFold start="\s*sub\>" end="^}" transparent fold
  syn sync fromstart
  setlocal foldmethod=syntax
endif


" The default highlighting.
hi def link perlSharpBang		PreProc
hi def link perlControl		PreProc
hi def link perlInclude		Include
hi def link perlSpecial		Special
hi def link perlString		String
hi def link perlCharacter		Character
hi def link perlNumber		Number
hi def link perlType		Type
hi def link perlIdentifier	Identifier
hi def link perlLabel		Label
hi def link perlStatement		Statement
hi def link perlConditional	Conditional
hi def link perlRepeat		Repeat
hi def link perlOperator		Operator
hi def link perlFunction		Function
hi def link perlFunctionPrototype	perlFunction
hi def link perlComment		Comment
hi def link perlTodo		Todo
hi def link perlList		perlStatement
hi def link perlMisc		perlStatement
hi def link perlVarPlain		perlIdentifier
hi def link perlFiledescRead	perlIdentifier
hi def link perlFiledescStatement	perlIdentifier
hi def link perlVarSimpleMember	perlIdentifier
hi def link perlVarSimpleMemberName	perlString
hi def link perlVarNotInMatches	perlIdentifier
hi def link perlVarSlash		perlIdentifier
hi def link perlQQ		perlString
hi def link perlUntilEOFDQ	perlString
hi def link perlUntilEOFSQ	perlString
hi def link perlUntilEmptyDQ	perlString
hi def link perlUntilEmptySQ	perlString
hi def link perlUntilEOF		perlString		
hi def link perlStringUnexpanded	perlString
hi def link perlSubstitutionSQ		perlString
hi def link perlSubstitutionDQ		perlString
hi def link perlSubstitutionSlash		perlString
hi def link perlSubstitutionHash		perlString
hi def link perlSubstitutionBracket	perlString
hi def link perlSubstitutionCurly 	perlString
hi def link perlSubstitutionPling		perlString
hi def link perlTranslationSlash		perlString
hi def link perlTranslationHash		perlString
hi def link perlTranslationBracket	perlString
hi def link perlTranslationCurly		perlString
hi def link perlMatch		perlString
hi def link perlMatchStartEnd	perlStatement
if exists("perl_string_as_statement")
  hi def link perlStringStartEnd	perlStatement
else
  hi def link perlStringStartEnd	perlString
endif
hi def link perlFormatName	perlIdentifier
hi def link perlFormatField	perlString
hi def link perlPackageDecl	perlType
hi def link perlStorageClass	perlType
hi def link perlPackageRef	perlType
hi def link perlStatementPackage	perlStatement
hi def link perlStatementSub	perlStatement
hi def link perlStatementStorage	perlStatement
hi def link perlStatementControl	perlStatement
hi def link perlStatementScalar	perlStatement
hi def link perlStatementRegexp	perlStatement
hi def link perlStatementNumeric	perlStatement
hi def link perlStatementList	perlStatement
hi def link perlStatementHash	perlStatement
hi def link perlStatementIOfunc	perlStatement
hi def link perlStatementFiledesc	perlStatement
hi def link perlStatementVector	perlStatement
hi def link perlStatementFiles	perlStatement
hi def link perlStatementFlow	perlStatement
hi def link perlStatementScope	perlStatement
hi def link perlStatementInclude	perlStatement
hi def link perlStatementProc	perlStatement
hi def link perlStatementSocket	perlStatement
hi def link perlStatementIPC	perlStatement
hi def link perlStatementNetwork	perlStatement
hi def link perlStatementPword	perlStatement
hi def link perlStatementTime	perlStatement
hi def link perlStatementMisc	perlStatement
hi def link perlFunctionName	perlIdentifier
hi def link perlFunctionPRef	perlType
hi def link perlPOD		perlComment
hi def link perlShellCommand	perlString
hi def link perlSpecialAscii	perlSpecial
hi def link perlSpecialDollar	perlSpecial
hi def link perlSpecialString	perlSpecial
hi def link perlSpecialStringU	perlSpecial
hi def link perlSpecialMatch	perlSpecial
hi def link perlSpecialBEOM	perlSpecial
hi def link perlDATA		perlComment

hi def link perlBrackets		Error

" Possible errors
hi def link perlNotEmptyLine	Error
hi def link perlElseIfError	Error

" Syncing to speed up processing
"
if !exists("perl_no_sync_on_sub")
  syn sync match perlSync	grouphere NONE "^\s*package\s"
  syn sync match perlSync	grouphere perlFunction "^\s*sub\s"
  syn sync match perlSync	grouphere NONE "^}"
endif

if !exists("perl_no_sync_on_global_var")
  syn sync match perlSync	grouphere NONE "^$\I[[:alnum:]_:]+\s*=\s*{"
  syn sync match perlSync	grouphere NONE "^[@%]\I[[:alnum:]_:]+\s*=\s*("
endif

if exists("perl_sync_dist")
  execute "syn sync maxlines=" . perl_sync_dist
else
  syn sync maxlines=100
endif

syn sync match perlSyncPOD	grouphere perlPOD "^=pod"
syn sync match perlSyncPOD	grouphere perlPOD "^=head"
syn sync match perlSyncPOD	grouphere perlPOD "^=item"
syn sync match perlSyncPOD	grouphere NONE "^=cut"

let b:current_syntax = "perl"

" vim: ts=8
