" Vim syntax file
" Language:	Perl
" Maintainer:	Sonia Heimann <niania@netsurf.org>
" Last change:	1997 Dec 16

" Remove any old syntax stuff hanging around
syn clear

"
" Comments
"

" First, treat the the #!/bin/perl
syn match  perlSharpBang         "^#!.\+$"
" All other # are comment, when at the beginning of a line or after a space
" (avoid m## case ...)
syn match  perlComment         "^#\([^!].*\)\=$\|[ \t]#.*" contains=perlTodo

"
" POD documentation
"

" POD starts with ^=head and ends with =cut
" Set the variable "perl_embedded_pod" to use embedded POD syntax file.

if !exists("perl_embedded_pod")
  syntax region perlPOD start=+^=head+ end=+=cut+
  syn sync match perlSyncPOD grouphere perlPOD "^=head"
  syn sync match perlSyncPOD groupthere NONE "^=cut"
else
  let b:embedded_pod = 1
  source <sfile>:p:h/pod.vim
  unlet b:embedded_pod

  syn region perlEmbeddedPod start="^=pod" start="^=head1" matchgroup=podCommand end="^=cut" contains=podCommand,podVerbatimLine,podSpecial,podFormat
  syn sync match perlEmbeddedPod grouphere perlEmbeddedPod "^=pod"
  syn sync match perlEmbeddedPod grouphere perlEmbeddedPod "^=head1"
  syn sync match perlEmbeddedPod groupthere NONE "^=cut"
endif

"
" Includes
"
syn region  perlInclude     start=+^[ \t]*\<\(use\|require\)\>+ end=+;+he=e-1


"
" All keywords
"
syn keyword perlLabel                   case default
syn keyword perlConditional             if elsif unless else switch eq ne gt lt ge le cmp not and or xor
syn keyword perlRepeat                  while for foreach do until
syn keyword perlOperator                defined undef and or not bless
syn keyword perlControl                 BEGIN END

syn keyword perlStatementStorageClass   my local
syn keyword perlStatementControl        goto return last next continue redo
syn keyword perlStatementScalar         chomp chop chr crypt index lc lcfirst length ord pack reverse rindex sprintf substr uc ucfirst
syn keyword perlStatementRegexp         pos quotemeta split study
syn keyword perlStatementNumeric        abs atan2 cos exp hex int log oct rand sin sqrt srand
syn keyword perlStatementList           splice unshift shift push pop split join reverse grep map qw sort unpack
syn keyword perlStatementHash           each exists keys values
syn keyword perlStatementIOfunc         binmode close closedir dbmclose dbmopen die eof fileno flock format getc print printf read readdir rewinddir seek seekdir select syscall sysopen sysread syswrite tell telldir truncate warn write
syn keyword perlStatementFixedlength    pack vec
syn keyword perlStatementFiles          chdir chmod chown chroot fcntl glob ioctl link lstat mkdir open opendir readlink rename rmdir stat symlink umask unlink utime
syn keyword perlStatementFlow           caller die dump eval exit wantarray
syn keyword perlStatementScope          import
syn keyword perlStatementProc           alarm exec fork getpgrp getppid getpriority kill pipe qx setpgrp setpriority sleep system times wait waitpid
syn keyword perlStatementSocket         accept bind connect getpeername getsockname getsockopt listen recv send setsockopt shutdown socket socketpair
syn keyword perlStatementIPC            msgctl msgget msgrcv msgsnd semctl semget semop shmctl shmget shmread shmwrite
syn keyword perlStatementNetwork        endprotoent endservent gethostbyaddr gethostbyname gethostent getnetbyaddr getnetbyname getnetent getprotobyname getprotobynumber getprotoent getservbyname getservbyport getservent sethostent setnetent setprotoent setservent
syn keyword perlStatementTime           gmtime localtime time times
syn keyword perlStatementMisc           print warn formline reset scalar new delete STDIN STDOUT STDERR

syn keyword perlTodo contained TODO TBD

"
" Perl Identifiers.
"

" Plain identifier:
"   Scalar identifier: $foo
"   Array identifier: @foo
"   Array Length: $#foo
"   Hash identifier: %foo
"   Function identifier: &foo
"   Reference dereferences: @$foo, %$$foo, &$foo, $$foo, ...

" We do not process complex things such as @{${"foo"}}. Too complicated, and
" too slow. And what is after the -> is *not* considered as part of the
" variable - there again, too complicated and too slow.

" Special variables first ($^A, ...)
syn match  perlVarPlain "$^[A-Z]"
" Special variables, continued ($|, $', ...)
syn match  perlVarPlain "$[\\\"\[\]'&`+*.,;=%~^:!@$<>(0-9-]"
" These variables are not recognized within matches.
syn match perlVarNotInMatches "$[|)]"
" This variable is not recognized within matches delimited by //.
syn match perlVarSlash "$/"

" And plain identifiers

syn match perlPackageRef "[a-zA-Z_][a-zA-Z0-9_]*\>\('\|::\)" contained

" To highlight packages in variables as a scope reference - i.e. in $pack::var,
" pack:: is a scope, just set "perl_want_scope_in_variables"
" If you *want* complex things like @{${"foo"}} to be processed,
" just set the variable "perl_extended_vars"...

if exists("perl_want_scope_in_variables")
  syn match perlVarPlain   "\(\$#\|\$\+\|@\$*\|%\$*\|\&\$*\)[a-zA-Z_][a-zA-Z0-9_]*\(\(::\|'\)[a-zA-Z_][a-zA-Z0-9_]*\)*\>" contains=perlPackageRef nextgroup=perlVarMember
else
  syn match perlVarPlain   "\(\$#\|\$\+\|@\$*\|%\$*\|\&\$*\)[a-zA-Z_][a-zA-Z0-9_]*\(\(::\|'\)[a-zA-Z_][a-zA-Z0-9_]*\)*\>" nextgroup=perlVarMember
endif

if exists("perl_extended_vars")
  syn region perlVarPlain start="[@%\$]{" skip="\\}" end="}" contains=perlVarPlain,perlVarNotInMatches,perlVarSlash nextgroup=perlVarMember
  syn region perlVarMember start="\(->\)\={" skip="\\}" end="}" contained contains=perlVarPlain,perlVarNotInMatches,perlVarSlash nextgroup=perlVarMember
  syn region perlVarMember start="\(->\)\=\[" skip="\\]" end="]" contained contains=perlVarPlain,perlVarNotInMatches,perlVarSlash nextgroup=perlVarMember
endif

"
" String and Character constants
"

" Highlight special characters (those which have a backslash) differently
syn match   perlSpecial           contained "\\[0-9][0-9][0-9]\|\\."
" "" String may contain variables
syn match   perlCharacter         "'[^\\]'"
syn match   perlSpecialCharacter  "'\\.'"
syn match   perlSpecialCharacter  "'\\[0-9][0-9][0-9]'"

" Strings
syn region  perlString            start=+"+  skip=+\\\\\|\\"+  end=+"+ contains=perlSpecial,perlVarPlain,perlVarNotInMatches,perlVarSlash
" '' Strings may not contain anything
syn region  perlStringUnexpanded  start=+'+  skip=+\\\\\|\\"+  end=+'+ contains=perlSpecial
syn region  perlStringUnexpanded  start="qw("hs=s+2 skip="\\\\\|\\)" end=")" contains=perlSpecial

" Shell commands
syn region  perlShellCommand            start=+`+  skip=+\\\\\|\\"+  end=+`+ contains=perlSpecial,perlVarPlain

" Numbers
syn match  perlNumber          "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"

" Constructs such as print <<EOF [...] EOF
syn region perlUntilEOF start=+<<\(["`]\=\)EOF\1+hs=s+2 end=+^EOF$+ contains=perlSpecial,perlVarPlain,perlVarNotInMatches,perlVarSlash
syn region perlUntilEOF start=+<<'EOF'+hs=s+2 end=+^EOF$+ contains=perlSpecial
" When vim supports it, try to use something as
" syntax region perlUntilEOF start=+<<\(["`]\=\)\([a-zA-Z]\+\)\1+s+2 end=+^\2$+
" to allow any keyword, not just EOF. The \2 in the end pattern refers of
" course to the second group in the start pattern.

"
" Perl regexps, second version, thanks to Michael Firestone
"

" any qq## expression
syn match perlQQ "\<q[qxw]\=\([^a-zA-Z0-9_ \t\n]\).*[^\\]\(\\\\\)*\1" contains=perlVarPlain, perlVarSlash
" Any m## match
syn match perlMatchAny "\<m\([^a-zA-Z0-9_ \t\n]\).*[^\\]\(\\\\\)*\1[xosmige]*" contains=perlVarPlain,perlVarSlash
" Plain m// match
syn match perlMatchAny "\<m/.*[^\\]\(\\\\\)*/[xosmige]*" contains=perlVarPlain
" Any s### substitute
" s/// is handled separately, since it can't contain $/ as a variable.
syn match perlSubstitute "\<s\([^a-zA-Z0-9_ \t\n]\).\{-}[^\\]\(\\\\\)*\1.\{-}[^\\]\(\\\\\)*\1[xosmige]*" contains=perlVarPlain,perlVarSlash
syn match perlSubstitute "\<\(s\|y\|tr\)/.\{-}[^\\]\(\\\\\)*/.\{-}[^\\]\(\\\\\)*/[xosmige]*" contains=perlVarPlain
" Note that the above rules don't match substitutions with empty
" replacement texts (like s/deleteme//); these rules cover those.
syn match perlSubstitute "\<s\([^a-zA-Z0-9_ \t\n]\).\{-}[^\\]\(\\\\\)*\1\1[xosmige]*" contains=perlVarPlain,perlVarSlash
syn match perlSubstitute "\<\(s\|y\|tr\)/.\{-}[^\\]\(\\\\\)*//[xosmige]*" contains=perlVarPlain
" The classical // construct
syn match perlMatch "/\(\\/\|[^/]\)*[^\\]\(\\\\\)*/[xosmige]*" contains=perlVarPlain

syn match perlClassDecl		"^[ \t]*package\>[^;]*"

syn match  perlLineSkip     "\\$"

"
" Functions
"

" find ^sub foo { . Only highlight foo.
" Function is sub foo { , sub foo( , sub foo;
" Sneaky:  Instead of terminating the region at a '(' (the start of a
" prototype), contain the prototype in the region and give it no
" highlighting; that way, things inside the prototype that look like
" variables won't be highlighted as variables.
syn region perlFunction start=+^[ \t]*sub[ \t]\++hs=e+1 end=+[;{]+me=s-1 contains=perlFunctionPrototype
syn match perlFunctionPrototype "([^)]*)" contained

if !exists("did_perl_syntax_inits")
  let did_perl_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link perlSharpBang  PreProc
  hi link perlLabel		Label
  hi link perlConditional	Conditional
  hi link perlRepeat		Repeat
  hi link perlOperator		Operator
  hi link perlList		perlStatement
  hi link perlMisc		perlStatement
  hi link perlVarPlain	perlIdentifier
  hi link perlVarMember	perlIdentifier
  hi link perlVarNotInMatches	perlIdentifier
  hi link perlVarSlash	perlIdentifier
  hi link perlQQ	perlString
  hi link perlUntilEOF	perlString
  hi link perlStringUnexpanded	perlString
  hi link perlCharacter		Character
  hi link perlSpecialCharacter	perlSpecial
  hi link perlMatchAny perlMatch
  hi link perlSubstitute perlMatch
  " I happen to prefer having matches and substitutions highlighted; if you
  " agree, set the variable "perl_highlight_matches".
  if exists("perl_highlight_matches")
    hi link perlMatch	perlString
  endif
  hi link perlNumber		Number
  hi link perlClassDecl		Typedef
  hi link perlStorageClass	perlType
  hi link perlPackageRef perlType
  hi link perlInclude		Include
  hi link perlControl		PreProc
  hi link perlStatementStorageClass perlStatement
  hi link perlStatementControl      perlStatement
  hi link perlStatementScalar       perlStatement
  hi link perlStatementRegexp       perlStatement
  hi link perlStatementNumeric      perlStatement
  hi link perlStatementList         perlStatement
  hi link perlStatementHash         perlStatement
  hi link perlStatementIOfunc       perlStatement
  hi link perlStatementFixedlength  perlStatement
  hi link perlStatementFiles        perlStatement
  hi link perlStatementFlow         perlStatement
  hi link perlStatementScope        perlStatement
  hi link perlStatementProc         perlStatement
  hi link perlStatementSocket       perlStatement
  hi link perlStatementIPC          perlStatement
  hi link perlStatementNetwork      perlStatement
  hi link perlStatementTime         perlStatement
  hi link perlStatementMisc         perlStatement
  hi link  perlStatement		Statement
  hi link  perlType		Type
  hi link  perlString		String
  hi link  perlPOD		    perlComment
  hi link  perlShellCommand     Special
  hi link  perlComment		Comment
  hi link  perlSpecial		Special
  hi link  perlTodo		Todo
  hi link  perlFunction		Function
  hi link  perlIdentifier	Identifier
endif

let b:current_syntax = "perl"

" vim: ts=8
