" Vim syntax file
" Language   : sendmail
" Maintainer : Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change: September 29, 1997

" Remove any old syntax stuff hanging around
syn clear

" Comments
syn match smComment	"^#.*$"

" Operators
syn match smOper	"$"

" Definitions, Classes, Files, Options, Precedence, Trusted Users, Mailers
syn match smDefine	"^[CDFPT]."
syn match smDefine	"^O[AaBcdDeFfgHiLmNoQqrSsTtuvxXyYzZ]"
syn match smDefine	"^M[a-zA-Z]\+,"he=e-1

" Header Format  H?list-of-mailer-flags?name: format
syn match smHeaderSep contained "[?:]"
syn match smHeader	"^H\(?[a-zA-Z]\+?\)\=[-a-zA-Z_]\+:" contains=smHeaderSep

" Variables
syn match smVar		"\$[a-z]"

" Rulesets
syn match smRuleset	"^S[0-9]*"

" Rewriting Rules
syn match smRewrite	"^R"			nextgroup=smRewriteLhsToken,smRewriteLhsUser

syn match smRewriteLhsUser	contained "[^\t$]\+"		nextgroup=smRewriteLhsToken,smRewriteLhsSep
syn match smRewriteLhsToken	contained "\(\$[-*+]\|\$[-=][A-Za-z]\)\+"	nextgroup=smRewriteLhsUser,smRewriteLhsSep

syn match smRewriteLhsSep	contained "\t\+"			nextgroup=smRewriteRhsToken,smRewriteRhsUser

syn match smRewriteRhsUser	contained "[^\t$]\+"		nextgroup=smRewriteRhsToken,smRewriteRhsSep
syn match smRewriteRhsToken	contained "\(\$[0-9]\|\$>[0-9]\|\$#\|\$@\|\$:[-_a-zA-Z]\+\|\$[[\]]\|\$@\|\$:\|\$[A-Za-z]\)\+" nextgroup=smRewriteRhsUser,smRewriteRhsSep

syn match smRewriteRhsSep	contained "\t\+"			nextgroup=smRewriteComment,smRewriteRhsSep
syn match smRewriteRhsSep	contained "$"

syn match smRewriteComment	contained "[^\t$]*$"

" Clauses
syn match smClauseError	"\$|\|\$\."
syn match smElse	transparent contained	"\$|"
syn region smClause	oneline	start="\$?." end="\$\." contains=smElse,smClause,smVar

if !exists("did_sm_syntax_inits")
  let did_sm_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link smClause	Special
  hi link smClauseError	Error
  hi link smComment	Comment
  hi link smDefine	Statement
  hi link smHeader	Statement
  hi link smHeaderSep	String
  hi link smRewrite	Statement
  hi link smRewriteComment	Comment
  hi link smRewriteLhsToken	String
  hi link smRewriteLhsUser	Statement
  hi link smRewriteRhsToken	String
  hi link smRuleset	Statement
  hi link smVar		String
  endif

let b:current_syntax = "sm"

" vim: ts=18
