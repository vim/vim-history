" Vim syntax file
" Language:     SKILL
" Maintainer:   Toby Schaffer <jtschaff@eos.ncsu.edu>
" Last change:  1998 Mar 16
" Comments:     SKILL is a Lisp-like programming language for use in EDA
"               tools from Cadence Design Systems. It allows you to have
"               a programming environment within the Cadence environment
"               that gives you access to the complete tool set and design 
"               database.

" Remove any old syntax stuff hanging around
syn clear

syn keyword skillConstants          t nil unbound

" generic reserved words (these are technically functions... :))
syn keyword skillKeywords           prog progn prog1 prog2 lambda nlambda 
syn keyword skillKeywords           defun procedure nprocedure mprocedure
syn keyword skillKeywords           return let 
syn keyword skillConditional        if else then case unless cond when
syn keyword skillRepeat             for forall foreach go while 

" for Diva rules files
syn keyword skillDivaKeywords       ivIf and or null
syn keyword skillDivaFunctions      drcExtractRules lvsRules
syn keyword skillDivaDRCFunctions   drc dubiousData offGrid saveDerived
syn keyword skillDivaDRCFunctions   area enc notch ovlp sep width not_over
syn keyword skillDivaExtFunctions   saveParasitic saveInterconnect
syn keyword skillDivaExtFunctions   saveProperty saveRecognition
syn keyword skillDivaExtFunctions   measureParasitic attachParasitic
syn keyword skillDivaExtFunctions   calculateParameter measureParameter 
syn keyword skillDivaExtFunctions   saveParameter extractDevice extractMOS
syn keyword skillDivaLVSFunctions   permuteDevice

" procedural interface routines

" CDF functions
syn match skillcdfFunctions         "\<cdf[A-Z][a-zA-Z]\+\>"
" graphic editor functions
syn match skillgeFunctions          "\<ge[A-Z][a-zA-Z]\+\>"
" human interface functions
syn match skillhiFunctions          "\<hi[A-Z][a-zA-Z]\+\>"
" layout editor functions
syn match skillleFunctions          "\<le[A-Z][a-zA-Z]\+\>"
" database functions
syn match skilldbFunctions          "\<db[A-Z][a-zA-Z]\+\>"
" design editor functions
syn match skilldeFunctions          "\<de[A-Z][a-zA-Z]\+\>"
" design flow functions
syn match skilldfFunctions          "\<df[A-Z][a-zA-Z]\+\>"
" design management & design data services functions
syn match skillddFunctions          "\<dd[s]\=[A-Z][a-zA-Z]\+\>"
" parameterized cell functions
syn match skillpcFunctions          "\<pc[A-Z][a-zA-Z]\+\>"
" tech file functions
syn match skilltechFunctions        "\<tech[A-Z][a-zA-Z]\+\>"
" Diva geometry functions
syn match skillDivaGeomFunctions    "\<geom[A-Z][a-zA-Z]\+\>"
syn keyword skillDivaGeomFunctions  geomGetNon45 geomGetNon90

" strings
syn region skillString              start=+"+ skip=+\\"+ end=+"+

syn keyword skillTodo contained     TODO FIXME XXX ???
syn keyword skillNote contained     NOTE IMPORTANT

" comments are either C-style or begin with a semicolon
syn region skillComment             start="/\*" end="\*/" contains=skillTodo,skillNote
syn match skillComment              ";.*" contains=skillTodo,skillNote
syn match skillCommentError         "\*/"

syn sync ccomment skillComment minlines=10

if !exists("did_skill_syntax_inits")
    let did_skill_syntax_inits = 1
                                      
    hi link skillConstants          Constant
    hi link skillKeywords           Statement
    hi link skillConditional        Conditional
    hi link skillRepeat             Repeat
    hi link skillDivaKeywords       Statement
    hi link skillDivaFunctions      Function
    hi link skillDivaDRCFunctions   Function
    hi link skillDivaExtFunctions   Function
    hi link skillDivaLVSFunctions   Function
    hi link skillcdfFunctions       Function
    hi link skillgeFunctions        Function
    hi link skillhiFunctions        Function
    hi link skillleFunctions        Function
    hi link skilldbFunctions        Function
    hi link skilldeFunctions        Function
    hi link skilldfFunctions        Function
    hi link skillddFunctions        Function
    hi link skillpcFunctions        Function
    hi link skilltechFunctions      Function
    hi link skillDivaGeomFunctions  Function
    hi link skillString             String
    hi link skillTodo               Todo
    hi link skillNote               Todo
    hi link skillComment            Comment
    hi link skillCommentError       Error
endif

let b:current_syntax = "skill"

" vim: ts=4
