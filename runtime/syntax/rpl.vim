" Vim syntax file
" Language:            RPL/2
" Version:             0.13.1 against RPL/2 version 4.00pre4e
" Last Change:         2002 Mar 03
" Maintainer:          Joël BERTRAND <rpl2@free.fr>
" For the latest version of this file, see <http://rpl2.free.fr>
" Credits:
" Nothing

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Case sensitive
syntax case match

syntax keyword rplConstant         e i

" Any binary number
syntax match rplBinaryError        "#\s*\S\+b"
syntax match rplBinary             "\<#\>\s*[01]\+b\>"
syntax match rplBinary             "\<#[01]\+b\>"
syntax match rplOctalError         "#\s*\S\+o"
syntax match rplOctal              "\<#\>\s*\o\+o\>"
syntax match rplOctal              "\<#\o\+o\>"
syntax match rplDecimalError       "#\s*\S\+d"
syntax match rplDecimal            "\<#\>\s*\d\+d\>"
syntax match rplDecimal            "\<#\d\+d\>"
syntax match rplHexadecimalError   "#\s*\S\+h"
syntax match rplHexadecimal        "\<#\>\s*\x\+h\>"
syntax match rplHexadecimal        "\<#\x\+h\>"

" Case unsensitive
syntax case ignore

syntax keyword rplControl          abort kill cont halt clmf sst

syntax keyword rplConstant         pi

syntax keyword rplStatement        return last syseval wait type wait
syntax keyword rplStatement        kind
syntax keyword rplStatement        eval use remove
syntax keyword rplStatement        dup dup2 dupn drop drop2 dropn depth
syntax keyword rplStatement        rolld roll pick rot swap over clear
syntax keyword rplStatement        warranty copyright
syntax keyword rplStatement        convert date time mem
syntax match   rplStatement        "\<->num\>"

syntax keyword rplStorage          get geti put puti sto rcl purge
syntax keyword rplStorage          sinv sneg sconj
syntax keyword rplStorage          steq rceq vars
syntax match   rplStorage          "\<sto+\>" "\<sto-\>" "\<sto*\>" "\<sto/\>"
syntax match   rplStorage          "\<->\>"

syntax keyword rplConditional      if iferr then elseif else end
syntax keyword rplConditional      select case default

syntax keyword rplAlgConditional   ift ifte

syntax keyword rplRepeat           do until
syntax keyword rplRepeat           while repeat
syntax keyword rplRepeat           for start next step cycle

syntax keyword rplOperator         and or not xor same
syntax match   rplOperator         "\<==\>" "\<<=\>" "\>=<\>" "\<<\>"
syntax match   rplOperator         "\>>\>" "\>>=\>" "\>=>\>" "\><>\>"

syntax keyword rplBoolean          true false

syntax keyword rplReadWrite        store recall lock wflock unlock
syntax keyword rplReadWrite        open close delete create format
syntax keyword rplReadWrite        rewind backspace
syntax keyword rplReadWrite        write read inquire sync append
syntax keyword rplReadWrite        rewrite suppress seek
syntax keyword rplReadWrite        pr1 print cr prst prstc erase prlcd
syntax keyword rplReadWrite        prvar prusr prmd
syntax keyword rplReadWrite        disp input prompt key
syntax keyword rplReadWrite        cllcd draw redraw drax indep depnd res axes
syntax keyword rplReadWrite        label
syntax keyword rplReadWrite        pmin pmax centr persist title
syntax keyword rplReadWrite        logscale autoscale scale
syntax keyword rplReadWrite        function polar parametric scatter
syntax keyword rplReadWrite        wireframe
syntax match   rplReadWrite        "\<->lcd\>" "\<lcd->\>" "\<*w\>" "\<*h\>"

syntax keyword rplIntrinsic        abs arg conj re im mant xpon
syntax keyword rplIntrinsic        ceil fact fp floor inv ip max min mod
syntax keyword rplIntrinsic        neg relax
syntax keyword rplIntrinsic        sign sq sqrt xroot
syntax keyword rplIntrinsic        cos sin tan tg
syntax keyword rplIntrinsic        acos arccos asin arcsin atan arctg
syntax keyword rplIntrinsic        cosh sinh tanh th
syntax keyword rplIntrinsic        acosh argch asinh argsh atanh argth
syntax keyword rplIntrinsic        log alog ln exp expm ln1
syntax keyword rplIntrinsic        trn con idn rdm rsd
syntax keyword rplIntrinsic        cnrm cross det dot egv egvl gegv glegv
syntax keyword rplIntrinsic        gregv gegvl legv regv rnrm
syntax keyword rplIntrinsic        std fix sci eng rad deg
syntax keyword rplIntrinsic        nrand rand rdz
syntax keyword rplIntrinsic        fft ifft
syntax keyword rplIntrinsic        dec bin oct hex rclf stof sf cf
syntax keyword rplIntrinsic        chr num pos sub size
syntax keyword rplIntrinsic        stws rcws rl rr rlb rrb
syntax keyword rplIntrinsic        sl sr slb srb asr
syntax keyword rplIntrinsic        int der
syntax keyword rplIntrinsic        stos rcls cls drws scls ns
syntax keyword rplIntrinsic        tot mean sdev var maxs mins
syntax keyword rplIntrinsic        psdev pvar pcov
syntax keyword rplIntrinsic        cols corr cov
syntax keyword rplIntrinsic        utpc utpf utpn utpt comb perm
syntax keyword rplIntrinsic        lu lchol plu uchol schur
syntax match   rplIntrinsic        "\<r->c\>" "\<c->r\>" "\<!\>"
syntax match   rplIntrinsic        "\<%\>" "\<%ch\>" "\<%t\>"
syntax match   rplIntrinsic        "\<hms->\>" "\<->hms\>" "\<hms+\>" "\<hms-\>"
syntax match   rplIntrinsic        "\<d->r\>" "\<r->d\>"
syntax match   rplIntrinsic        "\<b->r\>" "\<r->b\>"
syntax match   rplIntrinsic        "\<str->\>" "\<->str\>"
syntax match   rplIntrinsic        "\<array->\>" "\<->array\>"
syntax match   rplIntrinsic        "\<list->\>" "\<->list\>"
syntax match   rplIntrinsic        "\<s+\>" "\<s-\>"

syntax match   rplObsolete         "\<arry->\>" "\<->arry\>"

" Comments
syntax region rplCommentString contained start=+"+ end=+"+ end=+\*/+me=s-1
syntax region rplCommentLine start="//" skip="\\$" end="$" keepend contains=rplCommentString
syntax region rplComment start="/\*" end="\*/" contains=rplCommentString
syntax match rplCommentError   "\*/"

" Catch errors caused by too many right parentheses
syntax region rplParen transparent start="(" end=")" contains=ALLBUT,rplParenError,rplComplex,rplIncluded
syntax match rplParenError ")"

" Catch errors caused by too many right '>>'
syntax region rplSub matchgroup=rplSubDelimitor start="\<<<\>" end="\<>>\>" contains=ALLBUT,rplSubError,rplIncluded
syntax match rplSubError "\<>>\>"

" Catch errors caused by too many right brackets
syntax region rplArray start="\[" end="\]" contains=ALLBUT,rplArrayError
syntax match rplArrayError "\]"

" Catch errors caused by too many right '}'
syntax region rplList start="{" end="}" contains=ALLBUT,rplListError,rplIncluded
syntax match rplListError "}"

" cpp is used by RPL/2
syntax match rplPreProc   "^\s*#\s*\(define\|undef\)\>"
syntax match rplPreCondit "^\s*#\s*\(if\|ifdef\|ifndef\|elif\|else\|endif\)\>"
syntax match rplIncluded contained "\<<\s*\S*\s*>\>"
syntax match rplInclude   "^\s*#\s*include\>\s*["<]" contains=rplIncluded,rplString
syntax match rplExecPath  "^#!.*"

" Any integer
syntax match rplInteger    "\<[-+]\=\d\+\>"

" Floating point number
syntax match rplFloat      "\<[-+]\=\(\d*\)\=[.,]\=\d*\([eE][-+]\=\d\+\)\=\>" contains=ALLBUT,rplPoint,rplSign
syntax match rplPoint      "\<[.,]\>"
syntax match rplSign       "\<[+-]\>"

" Complex number
syntax match rplComplex    "\<([-+]\=\(\d*\)\=[.,]\=\d*\([eE][-+]\=\d\+\)\=\s*[,.]\s*[-+]\=\(\d*\)\=[.,]\=\d*\([eE][-+]\=\d\+\)\=)\>"

syntax region rplString start=+"+ end=+"+ oneline contains=NONE
syntax region rplExpr   start=+'+ end=+'+ oneline contains=rplParen, rplParenError

syntax match rplTab "\t"  transparent

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_rpl_syntax_inits")
  if version < 508
    let did_rpl_syntax_inits = 1
    command -nargs=+ HiLink highlight link <args>
  else
    command -nargs=+ HiLink highlight default link <args>
  endif

  " The default highlighting.

  HiLink rplControl             Statement
  HiLink rplStatement           Statement
  HiLink rplAlgConditional      Conditional
  HiLink rplConditional         Repeat
  HiLink rplRepeat              Repeat
  HiLink rplIntrinsic           Special
  HiLink rplStorage             StorageClass
  HiLink rplReadWrite           rplIntrinsic

  HiLink rplOperator            Operator

  HiLink rplList                Special
  HiLink rplArray               Special
  HiLink rplConstant            Identifier
  HiLink rplExpr                Type

  HiLink rplString              String

  HiLink rplBinary              Boolean
  HiLink rplOctal               Boolean
  HiLink rplDecimal             Boolean
  HiLink rplHexadecimal         Boolean
  HiLink rplInteger             Number
  HiLink rplFloat               Float
  HiLink rplComplex             Float
  HiLink rplBoolean             Identifier

  HiLink rplObsolete            Todo

  HiLink rplPreCondit           PreCondit
  HiLink rplInclude             Include
  HiLink rplIncluded            rplString
  HiLink rplInclude             Include
  HiLink rplExecPath            Include
  HiLink rplPreProc             PreProc
  HiLink rplComment             Comment
  HiLink rplCommentLine         Comment
  HiLink rplCommentString       Comment
  HiLink rplSubDelimitor        rplStorage
  HiLink rplCommentError        Error
  HiLink rplParenError          Error
  HiLink rplSubError            Error
  HiLink rplArrayError          Error
  HiLink rplListError           Error
  HiLink rplTab                 Error
  HiLink rplBinaryError         Error
  HiLink rplOctalError          Error
  HiLink rplDecimalError        Error
  HiLink rplHexadecimalError    Error

  delcommand HiLink
endif

let b:current_syntax = "rpl"

" vim: ts=8 tw=132
