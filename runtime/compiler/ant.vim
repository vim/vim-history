" Vim Compiler File
" Compiler:	ant
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Mon, 27 Aug 2001 23:59:46 +0200

if exists("current_compiler")
    finish
endif
let current_compiler = "ant"

setlocal makeprg=ant

" first  line:
"     ant with jikes +E, which assumes  the following
"     two property lines in your 'build.xml':
"
"	  <property name = "build.compiler"	  value = "jikes"/>
"	  <property name = "build.compiler.emacs" value = "true"/>
"
" second line:
"     ant with javac
"
setlocal errorformat=\ %#[javac]\ %#%f:%l:%c:%*\\d:%*\\d:\ %t%[%^:]%#:%m,
    \%A\ %#[javac]\ %f:%l:\ %m,%-Z\ %#[javac]\ %p^,%-C%.%#
