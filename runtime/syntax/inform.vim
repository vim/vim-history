" Vim syntax file
" Language:	Inform
" Maintainer:	Stephen Thomas (stephent@insignia.com)
" Last Change:	2001 Apr 20

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" A bunch of useful Inform keywords.  First, case insensitive stuff

syn case ignore

syn keyword informDefine Constant

syn keyword informType Array Attribute Class Global Nearby
syn keyword informType Object Property String Routine

syn keyword informInclude Import Include Link Replace System_file

syn keyword informPreCondit End Endif Ifdef Ifndef Iftrue Iffalse Ifv3 Ifv5

syn keyword informPreProc Abbreviate Default Fake_action Lowstring
syn keyword informPreProc Message Release Serial Statusline Stub Switches
syn keyword informPreProc Trace Zcharacter

syn keyword informGramPreProc contained Verb Extend

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
  syn keyword informLibObj thedark selfobj player location second actor
  syn keyword informLibObj noun

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
  syn keyword informLibRoutine AfterLife AfterPrompt Amusing BeforeParsing
  syn keyword informLibRoutine ChooseObjects DarkToDark DeathMessage
  syn keyword informLibRoutine GamePostRoutine GamePreRoutine Initialise
  syn keyword informLibRoutine InScope LookRoutine NewRoom ParseNoun
  syn keyword informLibRoutine ParseNumber ParserError PrintRank PrintVerb
  syn keyword informLibRoutine PrintTaskName TimePasses UnknownVerb

  syn keyword informLibAction1a Quit Restart Restore Verify Save ScriptOn
  syn keyword informLibAction1a ScriptOff Pronouns Score Fullscore LMode1
  syn keyword informLibAction1a LMode2 LMode3 NotifyOn NotifyOff Version
  syn keyword informLibAction1a Places Objects

  hi def link informLibAction1a informLibAction

  syn keyword informLibAction1b TraceOn TraceOff TraceLevel ActionsOn
  syn keyword informLibAction1b ActionsOff RoutinesOn RoutinesOff TimersOn
  syn keyword informLibAction1b TimersOff CommandsOn CommandsOff
  syn keyword informLibAction1b CommandsRead Predictable XPurloin XAbstract
  syn keyword informLibAction1b XTree Scope Goto Gonear

  hi def link informLibAction1b informLibAction

  syn keyword informLibAction2 Inv InvTall InvWide Take Drop Remove PutOn
  syn keyword informLibAction2 Insert Transfer Empty Enter Exit GetOff Go
  syn keyword informLibAction2 Goin Look Examine Search Give Show Unlock
  syn keyword informLibAction2 Lock SwitchOn SwitchOff Open Close Disrobe
  syn keyword informLibAction2 Wear Eat

  hi def link informLibAction2 informLibAction

  syn keyword informLibAction3 Yes No Burn Pray Wake WakeOther Consult Kiss
  syn keyword informLibAction3 Think Smell Listen Taste Touch Dig Cut Jump
  syn keyword informLibAction3 JumpOver Tie Drink Fill Sorry Strong Mild
  syn keyword informLibAction3 Attack Swim Swing Blow Rub Set SetTo
  syn keyword informLibAction3 WaveHands Wave Pull Push PushDir Turn
  syn keyword informLibAction3 Squeeze LookUnder ThrowAt Tell Answer Buy
  syn keyword informLibAction3 Ask AskFor Sing Climb Wait Sleep

  hi def link informLibAction3 informLibAction

  syn keyword informLibActionFake LetGo Receive ThrownAt Order TheSame
  syn keyword informLibActionFake PluralFound Miscellany Prompt

  hi def link informLibActionFake informLibAction

  syn keyword informLibVariable keep_silent deadflag action special_number
  syn keyword informLibVariable consult_from consult_words etype verb_num
  syn keyword informLibVariable verb_word the_time real_location c_style
  syn keyword informLibVariable parser_one parser_two listing_together wn
  syn keyword informLibVariable parser_action scope_stage scope_reason
  syn keyword informLibVariable action_to_be menu_item item_name item_width
  syn keyword informLibVariable lm_o lm_n inventory_style task_scores

  syn keyword informLibConst AMUSING_PROVIDED DEBUG Headline MAX_CARRIED
  syn keyword informLibConst MAX_SCORE MAX_TIMERS NO_PLACES NUMBER_TASKS
  syn keyword informLibConst OBJECT_SCORE ROOM_SCORE SACK_OBJECT Story
  syn keyword informLibConst TASKS_PROVIDED WITHOUT_DIRECTIONS
  syn keyword informLibConst NEWLINE_BIT INDENT_BIT FULLINV_BIT ENGLISH_BIT
  syn keyword informLibConst RECURSE_BIT ALWAYS_BIT TERSE_BIT PARTINV_BIT
  syn keyword informLibConst DEFART_BIT WORKFLAG_BIT ISARE_BIT CONCEAL_BIT
  syn keyword informLibConst PARSING_REASON TALKING_REASON EACHTURN_REASON
  syn keyword informLibConst REACT_BEFORE_REASON REACT_AFTER_REASON
  syn keyword informLibConst TESTSCOPE_REASON LOOPOVERSCOPE_REASON
  syn keyword informLibConst STUCK_PE UPTO_PE NUMBER_PE CANTSEE_PE TOOLIT_PE
  syn keyword informLibConst NOTHELD_PE MULTI_PE MMULTI_PE VAGUE_PE EXCEPT_PE
  syn keyword informLibConst ANIMA_PE VERB_PE SCENERY_PE ITGONE_PE
  syn keyword informLibConst JUNKAFTER_PE TOOFEW_PE NOTHING_PE ASKSCOPE_PE
endif

" Now the case sensitive stuff.

syntax case match

syn keyword informSysFunc child children elder indirect parent random
syn keyword informSysFunc sibling younger youngest metaclass

syn keyword informSysConst adjectives_table actions_table classes_table
syn keyword informSysConst identifiers_table preactions_table version_number
syn keyword informSysConst largest_object strings_offset code_offset
syn keyword informSysConst dict_par1 dict_par2 dict_par3
syn keyword informSysConst actual_largest_object static_memory_offset
syn keyword informSysConst array_names_offset readable_memory_offset
syn keyword informSysConst cpv__start cpv__end ipv__start ipv__end
syn keyword informSysConst array__start array__end lowest_attribute_number
syn keyword informSysConst highest_attribute_number attribute_names_array
syn keyword informSysConst lowest_property_number highest_property_number
syn keyword informSysConst property_names_array lowest_action_number
syn keyword informSysConst highest_action_number action_names_array
syn keyword informSysConst lowest_fake_action_number highest_fake_action_number
syn keyword informSysConst fake_action_names_array lowest_routine_number
syn keyword informSysConst highest_routine_number routines_array
syn keyword informSysConst routine_names_array routine_flags_array
syn keyword informSysConst lowest_global_number highest_global_number globals_array
syn keyword informSysConst global_names_array global_flags_array
syn keyword informSysConst lowest_array_number highest_array_number arrays_array
syn keyword informSysConst array_names_array array_flags_array lowest_constant_number
syn keyword informSysConst highest_constant_number constants_array constant_names_array
syn keyword informSysConst lowest_class_number highest_class_number class_objects_array
syn keyword informSysConst lowest_object_number highest_object_number

syn keyword informConditional default else if switch

syn keyword informRepeat break continue do for objectloop until while

syn keyword informStatement box font give inversion jump move new_line
syn keyword informStatement print print_ret quit read remove restore return
syn keyword informStatement rfalse rtrue save spaces string style

syn keyword informOperator roman reverse bold underline fixed on off to
syn keyword informOperator near from

syn keyword informKeyword dictionary symbols objects verbs assembly
syn keyword informKeyword expressions lines tokens linker on off alias long
syn keyword informKeyword additive score time string table data initial
syn keyword informKeyword initstr with private has class error fatalerror
syn keyword informKeyword warning self

syn keyword informMetaAttrib remaining create destroy recreate copy call

syn keyword informPredicate contained has hasnt in notin ofclass or
syn keyword informPredicate contained provides

syn keyword informGrammar contained noun held multi multiheld multiexcept
syn keyword informGrammar contained multiinside creature special number
syn keyword informGrammar contained scope topic reverse meta only replace
syn keyword informGrammar contained first last

syn keyword informTodo contained TODO

" Assembly language mnemonics must be preceded by a '@'.

syn match informAsmContainer "@\s*\k*" contains=informAsm

syn keyword informAsm contained je jl jg dec_chk inc_chk jin test or and
syn keyword informAsm contained test_attr set_attr clear_attr store
syn keyword informAsm contained insert_obj loadw loadb get_prop
syn keyword informAsm contained get_prop_addr get_next_prop add sub mul div
syn keyword informAsm contained mod call storew storeb put_prop sread
syn keyword informAsm contained print_char print_num random push pull
syn keyword informAsm contained split_window set_window output_stream
syn keyword informAsm contained input_stream sound_effect jz get_sibling
syn keyword informAsm contained get_child get_parent get_prop_len inc dec
syn keyword informAsm contained print_addr remove_obj print_obj ret jump
syn keyword informAsm contained print_paddr load not rtrue rfalse print
syn keyword informAsm contained print_ret nop save restore restart
syn keyword informAsm contained ret_popped pop quit new_line show_status
syn keyword informAsm contained verify call_2s call_vs aread call_vs2
syn keyword informAsm contained erase_window erase_line set_cursor get_cursor
syn keyword informAsm contained set_text_style buffer_mode read_char
syn keyword informAsm contained scan_table call_1s call_2n set_colour throw
syn keyword informAsm contained call_vn call_vn2 tokenise encode_text
syn keyword informAsm contained copy_table print_table check_arg_count
syn keyword informAsm contained call_1n catch piracy log_shift art_shift
syn keyword informAsm contained set_font save_undo restore_undo draw_picture
syn keyword informAsm contained picture_data erase_picture set_margins
syn keyword informAsm contained move_window window_size window_style
syn keyword informAsm contained get_wind_prop scroll_window pop_stack
syn keyword informAsm contained read_mouse mouse_window push_stack
syn keyword informAsm contained put_wind_prop print_form make_menu
syn keyword informAsm contained picture_table

" Grammar sections.

syn region informGrammarSection matchgroup=informGramPreProc start="\<Verb\|Extend\>" skip=+".*"+ end=";"he=e-1 contains=ALL

" Special character forms.

syn match informBadAccent display contained "@[^{[:digit:]]\D"
syn match informBadAccent display contained "@{[^}]*}"
syn match informAccent display contained "@:[aouAOUeiyEI]"
syn match informAccent display contained "@'[aeiouyAEIOUY]"
syn match informAccent display contained "@`[aeiouAEIOU]"
syn match informAccent display contained "@\^[aeiouAEIOU]"
syn match informAccent display contained "@\~[anoANO]"
syn match informAccent display contained "@/[oO]"
syn match informAccent display contained "@ss\|@<<\|@>>\|@oa\|@oA\|@ae\|@AE\|@cc\|@cC"
syn match informAccent display contained "@th\|@et\|@Th\|@Et\|@LL\|@oe\|@OE\|@!!\|@\?\?"
syn match informAccent display contained "@{\x\{1,4}}"
syn match informBadStrUnicode display contained "@@\D"
syn match informStringUnicode display contained "@@\d\+"
syn match informStringCode display contained "@\d\d"

" String and Character constants.  Ordering is important here.
syn region informString start=+"+ skip=+\\\\+ end=+"+ contains=informAccent,informStringUnicode,informStringCode,informBadAccent,informBadStrUnicode
syn region informDictString start="'" end="'" contains=informAccent,informBadAccent
syn match informBadDictString display "''"
syn match informDictString display "'''"

" Integer numbers: decimal, hexadecimal and binary.
setlocal iskeyword+=$
syn match informNumber display "\<\d\+\>"
syn match informNumber display "\<\$\x\+\>"
syn match informNumber display "\<\$\$[01]\+\>"

" Comments
syn match informComment "!.*" contains=informTodo

" Syncronization
syn sync match informSyncRoutine grouphere NONE "\[\|\]"
syn sync match informSyncRoutine groupthere informGrammarSection "\<Verb\|Extend\>"
syn sync maxlines=500

" The default highlighting.
hi def link informDefine	Define
hi def link informType		Type
hi def link informInclude	Include
hi def link informPreCondit	PreCondit
hi def link informPreProc	PreProc
hi def link informGramPreProc	PreProc
hi def link informAsm		Special
hi def link informPredicate	Operator
hi def link informSysFunc	Identifier
hi def link informSysConst	Identifier
hi def link informConditional	Conditional
hi def link informRepeat	Repeat
hi def link informStatement	Statement
hi def link informOperator	Operator
hi def link informKeyword	Keyword
hi def link informGrammar	Keyword
hi def link informDictString	String
hi def link informNumber	Number
hi def link informError		Error
hi def link informString	String
hi def link informComment	Comment
hi def link informAccent	Special
hi def link informStringUnicode	Special
hi def link informStringCode	Special
hi def link informTodo		Todo
if !exists("inform_highlight_simple")
  hi def link informLibAttrib	Identifier
  hi def link informLibProp	Identifier
  hi def link informLibObj	Identifier
  hi def link informLibRoutine	Identifier
  hi def link informLibVariable	Identifier
  hi def link informLibConst	Identifier
  hi def link informLibAction	Statement
endif
hi def link informBadDictString	informError
hi def link informBadAccent	informError
hi def link informBadStrUnicode	informError

let current_syntax = "inform"

" vim: ts=8
