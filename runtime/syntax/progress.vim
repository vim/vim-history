" Vim syntax file
" Language: Progress 4GL
" Filename extensions:	*.p (collides with Pascal),
"			*.i (collides with assembler)
"			*.w
" Maintainer:	Philip Uren <philu@computer.org>
" Last update:	Tue Jan  9 09:30:42 EST 2001
" Use the following VIM variables:
" progress_full_syntax	    if defined then vim will recognise
"			    less-used syntax, but beware that there
"			    is a performance hit for using this.
" Notes:	Still plenty of room for improvement.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn sync lines=400
set isk=@,48-57,_,-,

syn case ignore

" End mismatch errors
syn match   ProgressEndError	    "end\."

syn keyword ProgressConditional	if else then when otherwise case repeat while

" Progress Blocks of code
syn region ProgressForBlock transparent matchgroup=ProgressFor start="^[    ]*for\>" matchgroup=ProgressFor end="end\." contains=ALLBUT,ProgressProcedure,ProgressFunction
syn region ProgressDoBlock transparent matchgroup=ProgressDo start="do:" matchgroup=ProgressDo end="end\." contains=ALLBUT,ProgressProcedure,ProgressFunction
syn keyword ProgressFor		each where

" Make those TODO and debugging notes stand out!
syn keyword ProgressTodo	    contained	TODO BUG FIX
syn keyword ProgressDebug	    contained	DEBUG
syn match   ProgressNeedsWork	    contained	"NEED[S]*[  ][	]*WORK"

syn match ProgressProcedure	"^[	]*procedure.*"
syn match ProgressProcedure	"^[	]*end[	][  ]*procedure.*"

syn match ProgressFunction	"^[	]*function.*"
syn match ProgressFunction	"^[	]*end[	][  ]*function.*"

syn match ProgressInclude	"^[	]*[{].*\.i[}]"

syn match ProgressSubstitute	"^[	]*[{].*[^i][}]"
syn match ProgressPreProc	"^[	]*&.*"

syn match ProgressOperator	"[!;|)(:.><+*=-]"

syn match ProgressNumber	"\<\d\+\(u\=l\=\|lu\|f\)\>"

syn region ProgressComment	start="/\*"  end="\*/" contains=ProgressComment,ProgressTodo,ProgressDebug,ProgressNeedsWork

syn region ProgressString   matchgroup=ProgressQuote	start=+"+ end=+"+   skip=+\\"+
syn region ProgressString   matchgroup=ProgressQuote	start=+'+ end=+'+   skip=+\\'+

syn keyword ProgressStatement create parameter menu stream sub-menu delete disable
syn keyword ProgressStatement insert integer leave length locked message page pause publish subscribe
syn keyword ProgressStatement release reposition retry no-apply return-value return row-id run super persistent set assign string today transaction trigger update valid-event valid-handle validate view view-as wait-for widget-handle
syn keyword ProgressStatement apply buffer choose available can-find current-value current-changed define space input output no-undo initial like as character integer variable format and
syn keyword ProgressStatement accum accumulate display update down enable entry export import fill find first get hide

if exists("progress_full_syntax")
    syn keyword ProgressStatement buffer-compare buffer-copy clear compare connect browse database query server server-socket temp-table frame parameter menu rectangle stream sub-menu work-table delete object widget widget-pool disable disconnect call first-of index
    syn keyword ProgressStatement input from through input-output close last-of lc length left-trim lookup matches maximim member minimum modulo quit os-command output process events program-name prompt-for put random readkey
    syn keyword ProgressStatement right-trim round selection-list set assign size slider stop substitute substring time to-rowid trim truncate unload up use userid weekday year open next
    syn keyword ProgressStatement on at begins can-do can-query can-set caps chr connected count-of date day
endif

" The default highlighting.
hi def link ProgressDo		    Repeat
hi def link ProgressFor		    Repeat
hi def link ProgressConditional	    Conditional
hi def link ProgressEndError	    Error
hi def link ProgressDebug		    Debug
hi def link ProgressTodo		    Todo
hi def link ProgressNeedsWork	    Todo
hi def link ProgressComment	    Comment
hi def link ProgressProcedure	    Procedure
hi def link ProgressFunction	    Procedure
hi def link ProgressInclude	    Include
hi def link ProgressSubstitute	    PreProc
hi def link ProgressPreProc	    PreProc
hi def link ProgressOperator	    Operator
hi def link ProgressNumber	    Number
hi def link ProgressString	    String
hi def link ProgressQuote		    Delimiter
hi def link ProgressReserved	    Identifier
hi def link ProgressFunction	    Function
hi def link ProgressStatement	    Statement

let b:current_syntax = "progress"
