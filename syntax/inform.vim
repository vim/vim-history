" Vim syntax file
" Language: Inform
" Maintainer: Stephen Thomas (stephent@isltd.insignia.com)
" Last change: 1997 Nov 28

" Remove any old syntax stuff hanging around
syntax clear

" A bunch of useful Inform keywords.  First, case insensitive stuff

syntax case ignore

syntax keyword informDefine Constant

syntax keyword informType Array Attribute Class Global Nearby
syntax keyword informType Object Property

syntax keyword informInclude Import Include Link Replace System_file

syntax keyword informPreCondit End Ifdef Ifndef Iftrue Iffalse Ifv3 Ifv5

syntax keyword informPreProc Abbreviate Default Extend Fake_action Lowstring
syntax keyword informPreProc Message Release Serial Statusline Stub Switches
syntax keyword informPreProc Trace Verb Zcharacter

" Now the case sensitive stuff.

syntax case match

" Assembly language mnemonics must be preceded by a '@'.

syntax match informAsm "@\s*je"
syntax match informAsm "@\s*jl"
syntax match informAsm "@\s*jg"
syntax match informAsm "@\s*dec_chk"
syntax match informAsm "@\s*inc_chk"
syntax match informAsm "@\s*jin"
syntax match informAsm "@\s*test"
syntax match informAsm "@\s*or"
syntax match informAsm "@\s*and"
syntax match informAsm "@\s*test_attr"
syntax match informAsm "@\s*set_attr"
syntax match informAsm "@\s*clear_attr"
syntax match informAsm "@\s*store"
syntax match informAsm "@\s*insert_obj"
syntax match informAsm "@\s*loadw"
syntax match informAsm "@\s*loadb"
syntax match informAsm "@\s*get_prop"
syntax match informAsm "@\s*get_prop_addr"
syntax match informAsm "@\s*get_next_prop"
syntax match informAsm "@\s*add"
syntax match informAsm "@\s*sub"
syntax match informAsm "@\s*mul"
syntax match informAsm "@\s*div"
syntax match informAsm "@\s*mod"
syntax match informAsm "@\s*call"
syntax match informAsm "@\s*storew"
syntax match informAsm "@\s*storeb"
syntax match informAsm "@\s*put_prop"
syntax match informAsm "@\s*sread"
syntax match informAsm "@\s*print_char"
syntax match informAsm "@\s*print_num"
syntax match informAsm "@\s*random"
syntax match informAsm "@\s*push"
syntax match informAsm "@\s*pull"
syntax match informAsm "@\s*split_window"
syntax match informAsm "@\s*set_window"
syntax match informAsm "@\s*output_stream"
syntax match informAsm "@\s*input_stream"
syntax match informAsm "@\s*sound_effect"
syntax match informAsm "@\s*jz"
syntax match informAsm "@\s*get_sibling"
syntax match informAsm "@\s*get_child"
syntax match informAsm "@\s*get_parent"
syntax match informAsm "@\s*get_prop_len"
syntax match informAsm "@\s*inc"
syntax match informAsm "@\s*dec"
syntax match informAsm "@\s*print_addr"
syntax match informAsm "@\s*remove_obj"
syntax match informAsm "@\s*print_obj"
syntax match informAsm "@\s*ret"
syntax match informAsm "@\s*jump"
syntax match informAsm "@\s*print_paddr"
syntax match informAsm "@\s*load"
syntax match informAsm "@\s*not"
syntax match informAsm "@\s*rtrue"
syntax match informAsm "@\s*rfalse"
syntax match informAsm "@\s*print"
syntax match informAsm "@\s*print_ret"
syntax match informAsm "@\s*nop"
syntax match informAsm "@\s*save"
syntax match informAsm "@\s*restore"
syntax match informAsm "@\s*restart"
syntax match informAsm "@\s*ret_popped"
syntax match informAsm "@\s*pop"
syntax match informAsm "@\s*quit"
syntax match informAsm "@\s*new_line"
syntax match informAsm "@\s*show_status"
syntax match informAsm "@\s*verify"
syntax match informAsm "@\s*call_2s"
syntax match informAsm "@\s*call_vs"
syntax match informAsm "@\s*aread"
syntax match informAsm "@\s*call_vs2"
syntax match informAsm "@\s*erase_window"
syntax match informAsm "@\s*erase_line"
syntax match informAsm "@\s*set_cursor"
syntax match informAsm "@\s*get_cursor"
syntax match informAsm "@\s*set_text_style"
syntax match informAsm "@\s*buffer_mode"
syntax match informAsm "@\s*read_char"
syntax match informAsm "@\s*scan_table"
syntax match informAsm "@\s*call_1s"
syntax match informAsm "@\s*call_2n"
syntax match informAsm "@\s*set_colour"
syntax match informAsm "@\s*throw"
syntax match informAsm "@\s*call_vn"
syntax match informAsm "@\s*call_vn2"
syntax match informAsm "@\s*tokenise"
syntax match informAsm "@\s*encode_text"
syntax match informAsm "@\s*copy_table"
syntax match informAsm "@\s*print_table"
syntax match informAsm "@\s*check_arg_count"
syntax match informAsm "@\s*call_1n"
syntax match informAsm "@\s*catch"
syntax match informAsm "@\s*piracy"
syntax match informAsm "@\s*log_shift"
syntax match informAsm "@\s*art_shift"
syntax match informAsm "@\s*set_font"
syntax match informAsm "@\s*save_undo"
syntax match informAsm "@\s*restore_undo"
syntax match informAsm "@\s*draw_picture"
syntax match informAsm "@\s*picture_data"
syntax match informAsm "@\s*erase_picture"
syntax match informAsm "@\s*set_margins"
syntax match informAsm "@\s*move_window"
syntax match informAsm "@\s*window_size"
syntax match informAsm "@\s*window_style"
syntax match informAsm "@\s*get_wind_prop"
syntax match informAsm "@\s*scroll_window"
syntax match informAsm "@\s*pop_stack"
syntax match informAsm "@\s*read_mouse"
syntax match informAsm "@\s*mouse_window"
syntax match informAsm "@\s*push_stack"
syntax match informAsm "@\s*put_wind_prop"
syntax match informAsm "@\s*print_form"
syntax match informAsm "@\s*make_menu"
syntax match informAsm "@\s*picture_table"

syntax keyword informPredicate contained has hasnt in notin ofclass or
syntax keyword informPredicate contained provides

syntax keyword informSysFunc child children elder indirect parent random
syntax keyword informSysFunc sibling younger youngest metaclass

syntax keyword informSysConst adjectives_table actions_table classes_table
syntax keyword informSysConst identifiers_table preactions_table version_number
syntax keyword informSysConst largest_object strings_offset code_offset
syntax keyword informSysConst dict_par1 dict_par2 dict_par3

syntax keyword informConditional default else if switch

syntax keyword informRepeat break continue do for objectloop until while

syntax keyword informStatement box font give inversion jump move new_line
syntax keyword informStatement print print_ret quit read remove restore return
syntax keyword informStatement rfalse rtrue save spaces string style

syntax keyword informOperator char name the a an The number roman reverse bold
syntax keyword informOperator underline fixed on off to address string object
syntax keyword informOperator near from property

syntax keyword informKeyword dictionary symbols objects verbs assembly
syntax keyword informKeyword expressions lines tokens linker on off alias long
syntax keyword informKeyword additive score time noun held multi multiheld
syntax keyword informKeyword multiexcept multiinside creature special number
syntax keyword informKeyword scope topic reverse meta only replace first last
syntax keyword informKeyword string table data initial initstr with private has
syntax keyword informKeyword class error fatalerror warning

syntax keyword informTodo contained TODO

" Special character forms.

syntax match informSpecChar contained "@[''\:c~o^]\|@@[0-9][0-9]*\|@{[0-9]*}\|@.."

" String and Character constants
syntax region informString start=+"+ skip=+\\\\+ end=+"+ contains=informSpecChar
syntax region informDictString start="'" skip="@'" end="'"

" Catch errors caused by wrong parenthesis
syntax region informParen transparent start='(' end=')' contains=ALLBUT,informParenError,informTodo
syntax match informParenError ")"
syntax match informInParen contained "[{}]"

" Integer numbers: decimal, hexadecimal and binary.
syntax match informNumber "\<[0-9]\+\>"
syntax match informNumber "\<\$[0-9A-Za-z]\+\>"
syntax match informNumber "\<\$\$[01]\+\>"

" Comments
syntax match informComment "!.*" contains=informTodo

" Syncronization
syntax sync lines=20

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
  hi link informParenError	informError
  hi link informInParen		informError
endif

let current_syntax = "inform"

" vim: ts=8
