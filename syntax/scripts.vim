" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1997 August 26

" This file is called by an autocommand for every file that has just been
" loaded into a buffer.  It checks if the first line of the file is recognized
" as a file for which syntax highlighting is supported.  Only do this if there
" was no match with a filename extension.

if !has("syntax_items")

  " Source the user-specified syntax highlighting file
  if exists("myscriptsfile")
    if file_readable(expand(myscriptsfile))
      execute "so " . myscriptsfile
    endif
  endif

endif


if !has("syntax_items")

  " Bourne-like shell scripts: sh ksh bash
  if getline(1) =~ '^#!.*[/\\][bk]\=a\=sh\>'
    if exists("is_bash")
      unlet is_bash
    endif
    if exists("is_kornshell")
      unlet is_kornshell
    endif
    " if bash is sh on your system as on Linux, you may prefer to
    " add the following in your .vimrc file:
    " let bash_is_sh=1
    if exists("bash_is_sh") || getline(1) =~ '^#!.*[/\\]bash\>'
      let is_bash=1
    elseif getline(1) =~ '^#!.*[/\\]ksh\>'
      let is_kornshell=1
    endif
    so <sfile>:p:h/sh.vim

  " csh and tcsh scripts
  elseif getline(1) =~ '^#!.*[/\\]t\=csh\>'
    so <sfile>:p:h/csh.vim

  " Z shell scripts
  elseif getline(1) =~ '^#!.*[/\\]zsh\>'
    so <sfile>:p:h/zsh.vim

  " ELM Mail files
  elseif getline(1) =~ "^From [a-zA-Z][a-zA-Z_0-9\.-]*@.*[12][09][0-9][0-9]$"
    so <sfile>:p:h/mail.vim

  " Expect scripts
  elseif getline(1) =~ '^#!.*[/\\]expect\>'
    so <sfile>:p:h/expect.vim

  " Perl
  elseif getline(1) =~ '^#!.*[/\\][^/\\]*perl[^/\\]*\>'
    so <sfile>:p:h/perl.vim

  " Vim scripts (must have '" vim' as the first line to trigger this)
  elseif getline(1) =~ '^" *[vV]im$'
    so <sfile>:p:h/vim.vim

  " Diff file:
  " - "diff" in first line (context diff)
  " - "--- " in first line and "+++ " in second line (unified diff).
  " - "*** " in first line and "--- " in second line (context diff).
  elseif getline(1) =~ '^diff\>' || (getline(1) =~ '^--- ' && getline(2) =~ '^+++ ') || (getline(1) =~ '^\*\*\* ' && getline(2) =~ '^--- ')
    so <sfile>:p:h/diff.vim

  " PostScript Files (must have %!PS as the first line, like a2ps output)
  elseif getline(1) =~ '^%![ \t]*PS'
    so <sfile>:p:h/postscr.vim

  " Awk scripts
  elseif getline(1) =~ '^#!.*awk\>'
    so <sfile>:p:h/awk.vim

  " AmigaDos scripts
  elseif $TERM == "amiga" && (getline(1) =~ "^;" || getline(1) =~ "^\.[bB][rR][aA]")
    so <sfile>:p:h/amiga.vim

  endif

endif

" vim: ts=8
