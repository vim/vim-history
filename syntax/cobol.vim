" Vim syntax file
" Language: COBOL
" Maintainer:   Sitaram Chamarty <sitaram@diac.com>
" Last change:  1998 January 29
" For a fully commented version please see http://www.diac.com/~sitaram/vim

" MOST important - else most of the keywords wont work!
set isk=@,48-57,-
" ...the usual :-)
syn clear
" COBOL syntax is case insensitive
syn case ignore

syn match   cobolTodo         "todo" contained    " can appear in comments only
" many legacy sources have junk in columns 1-6
syn match   cobolJunk         "^......"           " must be before cobolComment
syn match   cobolComment      "^......\*.*"hs=s+6 contains=cobolTodo

syn keyword cobolGoTo         GO GOTO
syn keyword cobolCopy         COPY
syn keyword cobolBAD          ALTER ENTER RENAMES

syn keyword cobolCondFlow     ELSE    " IF INVALID END interfere with the region

" the next should really be a list of keywords like END-IF END-READ, etc...
" someday... :-) (TODO)
syn match   cobolCondFlow     "\<END-[A-Z]\+\>"
" cobolWatch: things that are important when trying to understand a program
syn keyword cobolWatch        OCCURS DEPENDING VARYING BINARY COMP REDEFINES
syn keyword cobolWatch        REPLACING RUN
syn match   cobolWatch        "COMP-[123456XN]"
syn keyword cobolEXECs        EXEC END-EXEC

syn match   cobolParas        "^...... \{1,4}[A-Z0-9][^"]\{-}\."hs=s+7
syn match   cobolDecl         "^...... \{1,4}[0-9]\+ "

syn keyword cobolStmts        ACCEPT ADD CLOSE COMPUTE CONTINUE DELETE DISPLAY
syn keyword cobolStmts        DIVIDE EVALUATE EXIT INSPECT MERGE MOVE MULTIPLY
syn keyword cobolStmts        OPEN READ REWIND REWRITE SEARCH SELECT SET SORT
syn keyword cobolStmts        START STOP STRING SUBTRACT WRITE
" TODO: add more.  Eventually, add all "standard" ones.

syn keyword cobolExtras       PERFORM
syn keyword cobolExtras       CALL CANCEL CORR CORRESPONDING GOBACK
syn match   cobolExtras       "EXIT \+PROGRAM"
syn match   cobolExtras       /\<VALUE \+[0-9]\+\./hs=s+6,he=e-1

syn match   cobolString       /".\{-}"/
syn match   cobolString       /'.\{-}'/

" All REGIONS come last...

syn region  cobolCondFlow     contains=ALL start="\<\(IF\|INVALID\|END\|EOP\)\>" skip=/"[^"]\+"/ end="\."

if !exists("did_cobol_syntax_inits")
  let did_cobol_syntax_inits = 1
  " syntax groups to "levels"
  hi link cobolGoTo     cobolSevere
  hi link cobolCopy     cobolSevere
  hi link cobolBAD      cobolSevere

  hi link cobolCondFlow cobolWatchpts
  hi link cobolWatch    cobolWatchpts
  hi link cobolEXECs    cobolWatchpts

  hi link cobolParas    cobolMarginA

  " cobol* highlights -> "standard" (syntax.vim) highlight groups
  hi link cobolComment  Comment
  hi link cobolSevere   Error
  hi link cobolTodo     Todo
  hi link cobolWatchpts Special
  hi link cobolMarginA  Constant
  hi link cobolStmts    Statement
  hi link cobolExtras   Special
  hi link cobolString   Constant
endif

let b:current_syntax = "cobol"

" vim: ts=8
