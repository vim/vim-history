" Vim syntax file
" Language: Inform
" Maintainer: Stephen Thomas (stephent@isltd.insignia.com)
" Last change: 1998 March 27th

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful Inform keywords.  First, case insensitive stuff

syn case ignore

syn keyword informDefine Constant

syn keyword informType Array Attribute Class Global Nearby
syn keyword informType Object Property

syn keyword informInclude Import Include Link Replace System_file

syn keyword informPreCondit End Ifdef Ifndef Iftrue Iffalse Ifv3 Ifv5

syn keyword informPreProc Abbreviate Default Extend Fake_action Lowstring
syn keyword informPreProc Message Release Serial Statusline Stub Switches
syn keyword informPreProc Trace Verb Zcharacter

" Now the case sensitive stuff.

syntax case match

" Assembly language mnemonics must be preceded by a '@'.

syn match informAsm "@\s*je"
syn match informAsm "@\s*jl"
syn match informAsm "@\s*jg"
syn match informAsm "@\s*dec_chk"
syn match informAsm "@\s*inc_chk"
syn match informAsm "@\s*jin"
syn match informAsm "@\s*test"
syn match informAsm "@\s*or"
syn match informAsm "@\s*and"
syn match informAsm "@\s*test_attr"
syn match informAsm "@\s*set_attr"
syn match informAsm "@\s*clear_attr"
syn match informAsm "@\s*store"
syn match informAsm "@\s*insert_obj"
syn match informAsm "@\s*loadw"
syn match informAsm "@\s*loadb"
syn match informAsm "@\s*get_prop"
syn match informAsm "@\s*get_prop_addr"
syn match informAsm "@\s*get_next_prop"
syn match informAsm "@\s*add"
syn match informAsm "@\s*sub"
syn match informAsm "@\s*mul"
syn match informAsm "@\s*div"
syn match informAsm "@\s*mod"
syn match informAsm "@\s*call"
syn match informAsm "@\s*storew"
syn match informAsm "@\s*storeb"
syn match informAsm "@\s*put_prop"
syn match informAsm "@\s*sread"
syn match informAsm "@\s*print_char"
syn match informAsm "@\s*print_num"
syn match informAsm "@\s*random"
syn match informAsm "@\s*push"
syn match informAsm "@\s*pull"
syn match informAsm "@\s*split_window"
syn match informAsm "@\s*set_window"
syn match informAsm "@\s*output_stream"
syn match informAsm "@\s*input_stream"
syn match informAsm "@\s*sound_effect"
syn match informAsm "@\s*jz"
syn match informAsm "@\s*get_sibling"
syn match informAsm "@\s*get_child"
syn match informAsm "@\s*get_parent"
syn match informAsm "@\s*get_prop_len"
syn match informAsm "@\s*inc"
syn match informAsm "@\s*dec"
syn match informAsm "@\s*print_addr"
syn match informAsm "@\s*remove_obj"
syn match informAsm "@\s*print_obj"
syn match informAsm "@\s*ret"
syn match informAsm "@\s*jump"
syn match informAsm "@\s*print_paddr"
syn match informAsm "@\s*load"
syn match informAsm "@\s*not"
syn match informAsm "@\s*rtrue"
syn match informAsm "@\s*rfalse"
syn match informAsm "@\s*print"
syn match informAsm "@\s*print_ret"
syn match informAsm "@\s*nop"
syn match informAsm "@\s*save"
syn match informAsm "@\s*restore"
syn match informAsm "@\s*restart"
syn match informAsm "@\s*ret_popped"
syn match informAsm "@\s*pop"
syn match informAsm "@\s*quit"
syn match informAsm "@\s*new_line"
syn match informAsm "@\s*show_status"
syn match informAsm "@\s*verify"
syn match informAsm "@\s*call_2s"
syn match informAsm "@\s*call_vs"
syn match informAsm "@\s*aread"
syn match informAsm "@\s*call_vs2"
syn match informAsm "@\s*erase_window"
syn match informAsm "@\s*erase_line"
syn match informAsm "@\s*set_cursor"
syn match informAsm "@\s*get_cursor"
syn match informAsm "@\s*set_text_style"
syn match informAsm "@\s*buffer_mode"
syn match informAsm "@\s*read_char"
syn match informAsm "@\s*scan_table"
syn match informAsm "@\s*call_1s"
syn match informAsm "@\s*call_2n"
syn match informAsm "@\s*set_colour"
syn match informAsm "@\s*throw"
syn match informAsm "@\s*call_vn"
syn match informAsm "@\s*call_vn2"
syn match informAsm "@\s*tokenise"
syn match informAsm "@\s*encode_text"
syn match informAsm "@\s*copy_table"
syn match informAsm "@\s*print_table"
syn match informAsm "@\s*check_arg_count"
syn match informAsm "@\s*call_1n"
syn match informAsm "@\s*catch"
syn match informAsm "@\s*piracy"
syn match informAsm "@\s*log_shift"
syn match informAsm "@\s*art_shift"
syn match informAsm "@\s*set_font"
syn match informAsm "@\s*save_undo"
syn match informAsm "@\s*restore_undo"
syn match informAsm "@\s*draw_picture"
syn match informAsm "@\s*picture_data"
syn match informAsm "@\s*erase_picture"
syn match informAsm "@\s*set_margins"
syn match informAsm "@\s*move_window"
syn match informAsm "@\s*window_size"
syn match informAsm "@\s*window_style"
syn match informAsm "@\s*get_wind_prop"
syn match informAsm "@\s*scroll_window"
syn match informAsm "@\s*pop_stack"
syn match informAsm "@\s*read_mouse"
syn match informAsm "@\s*mouse_window"
syn match informAsm "@\s*push_stack"
syn match informAsm "@\s*put_wind_prop"
syn match informAsm "@\s*print_form"
syn match informAsm "@\s*make_menu"
syn match informAsm "@\s*picture_table"

syn keyword informPredicate contained has hasnt in notin ofclass or
syn keyword informPredicate contained provides

syn keyword informPrintSpec contained char string address name a an the The
syn keyword informPrintSpec contained property object

syn keyword informSysFunc child children elder indirect parent random
syn keyword informSysFunc sibling younger youngest metaclass

syn keyword informSysConst adjectives_table actions_table classes_table
syn keyword informSysConst identifiers_table preactions_table version_number
syn keyword informSysConst largest_object strings_offset code_offset
syn keyword informSysConst dict_par1 dict_par2 dict_par3

syn keyword informConditional default else if switch

syn keyword informRepeat break continue do for objectloop until while

syn keyword informStatement box font give inversion jump move new_line
syn keyword informStatement print print_ret quit read remove restore return
syn keyword informStatement rfalse rtrue save spaces string style

syn keyword informOperator number roman reverse bold
syn keyword informOperator underline fixed on off to
syn keyword informOperator near from

syn keyword informKeyword dictionary symbols objects verbs assembly
syn keyword informKeyword expressions lines tokens linker on off alias long
syn keyword informKeyword additive score time noun held multi multiheld
syn keyword informKeyword multiexcept multiinside creature special number
syn keyword informKeyword scope topic reverse meta only replace first last
syn keyword informKeyword string table data initial initstr with private has
syn keyword informKeyword class error fatalerror warning self

syn keyword informMetaAttrib remaining create destroy recreate copy call

if !exists("inform_highlight_simple")
  syn keyword informLibAttrib absent animate clothing concealed container
  syn keyword informLibAttrib door edible enterable female general light
  syn keyword informLibAttrib lockable locked male moved neuter on open
  syn keyword informLibAttrib openable pluralname proper scenery scored
  syn keyword informLibAttrib static supporter switchable talkable
  syn keyword informLibAttrib visited workflag worn
  syn match informLibAttrib "\<transparent\>"

  syn keyword informLibProp e_to se_to s_to sw_to w_to nw_to n_to ne_to
  syn keyword informLibProp u_to d_to in_to out_to before after life
  syn keyword informLibProp door_to with_key door_dir invent plural
  syn keyword informLibProp add_to_scope list_together react_before
  syn keyword informLibProp react_after grammar orders initial when_open
  syn keyword informLibProp when_closed when_on when_off description
  syn keyword informLibProp describe article cant_go found_in time_left
  syn keyword informLibProp number time_out daemon each_turn capacity
  syn keyword informLibProp name short_name short_name_indef parse_name
  syn keyword informLibProp articles inside_description

  syn keyword informLibObj e_obj se_obj s_obj sw_obj w_obj nw_obj n_obj
  syn keyword informLibObj ne_obj u_obj d_obj in_obj out_obj compass
  syn keyword informLibObj thedark selfobj player location

  syn keyword informLibRoutine Achieved AddToScope AllowPushDir CDefArt
  syn keyword informLibRoutine ChangeDefault ChangePlayer DefArt DoMenu
  syn keyword informLibRoutine EnglishNumber HasLightSource InDefArt
  syn keyword informLibRoutine Locale LoopOverScope NextWord
  syn keyword informLibRoutine NextWordStopped NounDomain OffersLight
  syn keyword informLibRoutine PlaceInScope PlayerTo PrintShortName
  syn keyword informLibRoutine ScopeWithin SetTime StartDaemon StartTimer
  syn keyword informLibRoutine StopDaemon StopTimer TestScope TryNumber
  syn keyword informLibRoutine UnsignedCompare WordAddress WordLength
  syn keyword informLibRoutine WriteListFrom YesOrNo ZRegion RunRoutines

  syn keyword informLibAction1a Quit Restart Restore Verify Save ScriptOn
  syn keyword informLibAction1a ScriptOff Pronouns Score Fullscore LMode1
  syn keyword informLibAction1a LMode2 LMode3 NotifyOn NotifyOff Version
  syn keyword informLibAction1a Places Objects

  hi link informLibAction1a informLibAction

  syn keyword informLibAction1b TraceOn TraceOff TraceLevel ActionsOn
  syn keyword informLibAction1b ActionsOff RoutinesOn RoutinesOff TimersOn
  syn keyword informLibAction1b TimersOff CommandsOn CommandsOff
  syn keyword informLibAction1b CommandsRead Predictable XPurloin XAbstract
  syn keyword informLibAction1b XTree Scope Goto Gonear

  hi link informLibAction1b informLibAction

  syn keyword informLibAction2 Inv InvTall InvWide Take Drop Remove PutOn
  syn keyword informLibAction2 Insert Transfer Empty Enter Exit GetOff Go
  syn keyword informLibAction2 Goin Look Examine Search Give Show Unlock
  syn keyword informLibAction2 Lock SwitchOn SwitchOff Open Close Disrobe
  syn keyword informLibAction2 Wear Eat

  hi link informLibAction2 informLibAction

  syn keyword informLibAction3 Yes No Burn Pray Wake WakeOther Consult Kiss
  syn keyword informLibAction3 Think Smell Listen Taste Touch Dig Cut Jump
  syn keyword informLibAction3 JumpOver Tie Drink Fill Sorry Strong Mild
  syn keyword informLibAction3 Attack Swim Swing Blow Rub Set SetTo
  syn keyword informLibAction3 WaveHands Wave Pull Push PushDir Turn
  syn keyword informLibAction3 Squeeze LookUnder ThrowAt Answer Buy Ask
  syn keyword informLibAction3 AskFor Sing Climb Wait Sleep

  hi link informLibAction3 informLibAction

  syn keyword informLibActionFake LetGo Receive ThrownAt Order TheSame
  syn keyword informLibActionFake PluralFound Miscellany Prompt

  hi link informLibActionFake informLibAction

  syn keyword informLibVariable second keep_silent deadflag
endif

syn keyword informTodo contained TODO

" Special character forms.

syn match informSpecChar contained "@[''\:c~o^]\|@@[0-9][0-9]*\|@{[0-9]*}\|@.."

" String and Character constants
syn region informString start=+"+ skip=+\\\\+ end=+"+ contains=informSpecChar
syn region informDictString start="'" skip="@'" end="'"

" Catch errors caused by wrong parenthesis
syn region informParen transparent start='(' end=')' contains=ALLBUT,informParenError,informTodo
syn match informParenError ")"
syn match informInParen contained "[{}]"

" Integer numbers: decimal, hexadecimal and binary.
syn match informNumber "\<[0-9]\+\>"
syn match informNumber "\<\$[0-9A-Za-z]\+\>"
syn match informNumber "\<\$\$[01]\+\>"

" Comments
syn match informComment "!.*" contains=informTodo

" Syncronization
syn sync match informSyncRoutine grouphere NONE "\[\|\]"
syn sync maxlines=500

if !exists("did_inform_syntax_inits")
  let did_inform_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link informDefine		Define
  hi link informType		Type
  hi link informInclude		Include
  hi link informPreCondit	PreCondit
  hi link informPreProc		PreProc
  hi link informAsm		Special
  hi link informPredicate	Operator
  hi link informPrintSpec	Operator
  hi link informSysFunc		Identifier
  hi link informSysConst	Identifier
  hi link informConditional	Conditional
  hi link informRepeat		Repeat
  hi link informStatement	Statement
  hi link informOperator	Operator
  hi link informKeyword		Keyword
  hi link informDictString	String
  hi link informNumber		Number
  hi link informError		Error
  hi link informString		String
  hi link informComment		Comment
  hi link informSpecChar	Special
  hi link informTodo		Todo
  if !exists("inform_highlight_simple")
    hi link informLibAttrib	Identifier
    hi link informLibProp	Identifier
    hi link informLibObj	Identifier
    hi link informLibRoutine	Identifier
    hi link informLibVariable	Identifier
    hi link informLibAction	Statement
  endif
  hi link informParenError	informError
  hi link informInParen		informError
endif

let current_syntax = "inform"

" vim: ts=8
