" Vim syntax file
" Language:	Slrn setup file
" Maintainer:	Preben "Peppe" Guldberg <c928400@student.dtu.dk>
" Last Change:	21st Sep 2001

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn keyword slrnrcTodo		contained Todo

" in some places white space is illegal
syn match slrnrcSpaceError	contained "\s"

syn match slrnrcNumber		contained "-\=\<\d\+\>"
syn match slrnrcNumber		contained +'[^']\+'+

syn match slrnrcSpecKey		contained +\(\\[e"']\|\^[^'"]\)+

syn match  slrnrcKey		contained "\S\+"	contains=slrnrcSpecKey
syn region slrnrcKey		contained start=+"+ skip=+\\"+ end=+"+ oneline contains=slrnrcSpecKey
syn region slrnrcKey		contained start=+'+ skip=+\\'+ end=+'+ oneline contains=slrnrcSpecKey

syn match slrnrcSpecChar	contained +'+
syn match slrnrcSpecChar	contained +\\[n"]+
syn match slrnrcSpecChar	contained "%[dfmnrs%]"

syn match  slrnrcString		contained /[^ \t%"']\+/	contains=slrnrcSpecChar
syn region slrnrcString		contained start=+"+ skip=+\\"+ end=+"+ oneline contains=slrnrcSpecChar

syn match slrnSlangPreCondit	"^#ifn\=\(def\>\|false\>\|true\>\|\$\)"
syn match slrnSlangPreCondit	"^#\(elif\|else\|endif\)\>"

syn match slrnrcComment		"%.*$"	contains=slrnrcTodo

syn keyword slrnrcVarInt	contained abort_unmodified_edits auto_mark_article_as_read beep
syn keyword slrnrcVarInt	contained broken_xref cc_followup check_new_groups color_by_score
syn keyword slrnrcVarInt	contained confirm_actions display_cursor_bar drop_bogus_groups
syn keyword slrnrcVarInt	contained editor_uses_mime_charset emphasized_text_mask
syn keyword slrnrcVarInt	contained emphasized_text_mode fold_headers
syn keyword slrnrcVarInt	contained followup_strip_signature force_authentication
syn keyword slrnrcVarInt	contained generate_date_header generate_email_from
syn keyword slrnrcVarInt	contained generate_message_id grouplens_port hide_pgpsignature
syn keyword slrnrcVarInt	contained hide_signature hide_verbatim_marks
syn keyword slrnrcVarInt	contained highlight_unread_subjects highlight_urls
syn keyword slrnrcVarInt	contained ignore_signature kill_score lines_per_update
syn keyword slrnrcVarInt	contained mail_editor_is_mua max_low_score min_high_score mouse
syn keyword slrnrcVarInt	contained netiquette_warnings new_subject_breaks_threads
syn keyword slrnrcVarInt	contained no_backups prefer_head process_verbatim_marks
syn keyword slrnrcVarInt	contained query_next_article query_next_group
syn keyword slrnrcVarInt	contained query_read_group_cutoff read_active reject_long_lines
syn keyword slrnrcVarInt	contained scroll_by_page show_article show_thread_subject
syn keyword slrnrcVarInt	contained simulate_graphic_chars smart_quote sorting_method
syn keyword slrnrcVarInt	contained spoiler_char spoiler_display_mode spool_check_up_on_nov
syn keyword slrnrcVarInt	contained uncollapse_threads unsubscribe_new_groups use_blink
syn keyword slrnrcVarInt	contained use_color use_flow_control use_grouplens
syn keyword slrnrcVarInt	contained use_header_numbers use_inews use_localtime use_metamail
syn keyword slrnrcVarInt	contained use_mime use_slrnpull use_tilde use_tmpdir use_uudeview
syn keyword slrnrcVarInt	contained warn_followup_to wrap_flags wrap_method
syn keyword slrnrcVarInt	contained write_newsrc_flags

" Listed for removal
syn keyword slrnrcVarInt	contained author_display display_author_realname display_score
syn keyword slrnrcVarInt	contained group_dsc_start_column process_verbatum_marks
syn keyword slrnrcVarInt	contained prompt_next_group query_reconnect show_descriptions
syn keyword slrnrcVarInt	contained use_xgtitle

" match as a "string" too
syn region  slrnrcVarIntStr	contained matchgroup=slrnrcVarInt start=+"+ end=+"+ oneline contains=slrnrcVarInt,slrnrcSpaceError

syn keyword slrnrcVarStr	contained Xbrowser art_help_line art_status_line
syn keyword slrnrcVarStr	contained cc_followup_string cc_post_string charset custom_headers
syn keyword slrnrcVarStr	contained decode_directory editor_command failed_posts_file
syn keyword slrnrcVarStr	contained followup_custom_headers followup_date_format
syn keyword slrnrcVarStr	contained followup_string group_help_line group_status_line
syn keyword slrnrcVarStr	contained grouplens_host grouplens_pseudoname header_help_line
syn keyword slrnrcVarStr	contained header_status_line hostname inews_program
syn keyword slrnrcVarStr	contained macro_directory mail_editor_command metamail_command
syn keyword slrnrcVarStr	contained mime_charset non_Xbrowser organization
syn keyword slrnrcVarStr	contained overview_date_format post_editor_command post_object
syn keyword slrnrcVarStr	contained postpone_directory printer_name quote_string realname
syn keyword slrnrcVarStr	contained reply_custom_headers reply_string replyto save_directory
syn keyword slrnrcVarStr	contained save_posts save_replies score_editor_command scorefile
syn keyword slrnrcVarStr	contained sendmail_command server_object signature signoff_string
syn keyword slrnrcVarStr	contained spool_active_file spool_activetimes_file spool_inn_root
syn keyword slrnrcVarStr	contained spool_newsgroups_file spool_nov_file spool_nov_root
syn keyword slrnrcVarStr	contained spool_root supersedes_custom_headers top_status_line
syn keyword slrnrcVarStr	contained username
" Listed for removal
syn keyword slrnrcVarStr	contained followup
" match as a "string" too
syn region  slrnrcVarStrStr	contained matchgroup=slrnrcVarStr start=+"+ end=+"+ oneline contains=slrnrcVarStr,slrnrcSpaceError

" various commands
syn region slrnrcCmdLine	matchgroup=slrnrcCmd start="\(unsetkey\|setkey\|server\|color\|mono\|set\|nnrpaccess\|ignore_quotes\|strip_re_regexp\|strip_sig_regexp\|strip_was_regexp\|autobaud\|grouplens_add\|interpret\|include\|header_display_format\|group_display_format\|visible_headers\|compatible_charsets\|posting_host\)" end="$" oneline contains=slrnrc\(String\|Comment\)
" Listed for removal
syn region slrnrcCmdLine	matchgroup=slrnrcCmd start="\(hostname\|username\|replyto\|organization\|scorefile\|signature\|realname\|followup\|cc_followup_string\|quote_string\|decode_directory\|editor_command\)" end="$" oneline contains=slrnrc\(String\|Comment\)

" setting variables
syn keyword slrnrcSet		contained set
syn match   slrnrcSetStr	"^\s*set\s\+\S\+" skipwhite nextgroup=slrnrcString contains=slrnrcSet,slrnrcVarStr\(Str\)\=
syn match   slrnrcSetInt	contained "^\s*set\s\+\S\+" contains=slrnrcSet,slrnrcVarInt\(Str\)\=
syn match   slrnrcSetIntLine	"^\s*set\s\+\S\+\s\+\(-\=\d\+\>\|'[^']\+'\)" contains=slrnrcSetInt,slrnrcNumber,slrnrcVarInt

" color definitions
syn keyword slrnrcColorObj	contained article author boldtext box cursor date description
syn keyword slrnrcColorObj	contained error frame from_myself group grouplens_display
syn keyword slrnrcColorObj	contained header_name header_number headers high_score italicstext
syn keyword slrnrcColorObj	contained menu menu_press neg_score normal pos_score pgpsignature
syn keyword slrnrcColorObj	contained quotes response_char selection signature status subject
syn keyword slrnrcColorObj	contained thread_number tilde tree underlinetext unread_subject
syn keyword slrnrcColorObj	contained url verbatim
syn match   slrnrcColorObj	contained "\<quotes\o\>"

syn region  slrnrcColorObjStr	contained matchgroup=slrnrcColorObj start=+"+ end=+"+ oneline contains=slrnrcColorObj,slrnrcSpaceError
syn keyword slrnrcColorVal	contained black blue green cyan red magenta brown lightgray gray
syn keyword slrnrcColorVal	contained brightblue brightgreen brightcyan brightred
syn keyword slrnrcColorVal	contained brightmagenta yellow white
syn region  slrnrcColorValStr	contained matchgroup=slrnrcColorVal start=+"+ end=+"+ oneline contains=slrnrcColorVal,slrnrcSpaceError
" mathcing a function with three arguments
syn keyword slrnrcColor		contained color
syn match   slrnrcColorInit	contained "^\s*color\s\+\S\+" skipwhite nextgroup=slrnrcColorVal\(Str\)\= contains=slrnrcColor\(Obj\|ObjStr\)\=
syn match   slrnrcColorLine	"^\s*color\s\+\S\+\s\+\S\+" skipwhite nextgroup=slrnrcColorVal\(Str\)\= contains=slrnrcColor\(Init\|Val\|ValStr\)

" mono settings
syn keyword slrnrcMonoVal	contained blink bold none reverse underline
syn region  slrnrcMonoValStr	contained matchgroup=slrnrcMonoVal start=+"+ end=+"+ oneline contains=slrnrcMonoVal,slrnrcSpaceError
" color object is inherited
" mono needs at least one argument
syn keyword slrnrcMono		contained mono
syn match   slrnrcMonoInit	contained "^\s*mono\s\+\S\+" contains=slrnrcMono,slrnrcColorObj\(Str\)\=
syn match   slrnrcMonoLine	"^\s*mono\s\+\S\+\s\+\S.*" contains=slrnrcMono\(Init\|Val\|ValStr\),slrnrcComment

" Functions in article mode
syn keyword slrnrcFunArt	contained article_bob article_eob article_left article_line_down
syn keyword slrnrcFunArt	contained article_line_up article_page_down article_page_up
syn keyword slrnrcFunArt	contained article_right article_search author_search_backward
syn keyword slrnrcFunArt	contained author_search_forward browse_url cancel catchup
syn keyword slrnrcFunArt	contained catchup_all create_score decode delete delete_thread
syn keyword slrnrcFunArt	contained digit_arg enlarge_article_window evaluate_cmd
syn keyword slrnrcFunArt	contained exchange_mark expunge fast_quit followup forward
syn keyword slrnrcFunArt	contained forward_digest get_children_headers get_parent_header
syn keyword slrnrcFunArt	contained grouplens_rate_article goto_article goto_last_read
syn keyword slrnrcFunArt	contained header_bob header_eob header_line_down header_line_up
syn keyword slrnrcFunArt	contained header_page_down header_page_up help hide_article
syn keyword slrnrcFunArt	contained locate_article mark_spot next next_high_score
syn keyword slrnrcFunArt	contained next_same_subject pipe post post_postponed previous
syn keyword slrnrcFunArt	contained print quit redraw repeat_last_key reply save
syn keyword slrnrcFunArt	contained show_spoilers shrink_article_window skip_quotes
syn keyword slrnrcFunArt	contained skip_to_next_group skip_to_previous_group
syn keyword slrnrcFunArt	contained subject_search_backward subject_search_forward supersede
syn keyword slrnrcFunArt	contained suspend tag_header toggle_collapse_threads
syn keyword slrnrcFunArt	contained toggle_header_formats toggle_header_tag toggle_headers
syn keyword slrnrcFunArt	contained toggle_pgpsignature toggle_quotes toggle_rot13
syn keyword slrnrcFunArt	contained toggle_signature toggle_sort toggle_verbatim_marks
syn keyword slrnrcFunArt	contained uncatchup uncatchup_all undelete untag_headers
syn keyword slrnrcFunArt	contained wrap_article zoom_article_window
" Listed for removal
syn keyword slrnrcFunArt	contained art_bob art_eob art_xpunge article_linedn article_lineup
syn keyword slrnrcFunArt	contained article_pagedn article_pageup down enlarge_window
syn keyword slrnrcFunArt	contained goto_beginning goto_end left locate_header_by_msgid
syn keyword slrnrcFunArt	contained pagedn pageup pipe_article prev print_article right
syn keyword slrnrcFunArt	contained scroll_dn scroll_up shrink_window skip_to_prev_group
syn keyword slrnrcFunArt	contained toggle_show_author up

" Functions in group mode

syn keyword slrnrcFunGroup	contained add_group bob catchup digit_arg eob evaluate_cmd
syn keyword slrnrcFunGroup	contained group_search group_search_backward group_search_forward
syn keyword slrnrcFunGroup	contained help line_down line_up move_group page_down page_up post
syn keyword slrnrcFunGroup	contained post_postponed quit redraw refresh_groups
syn keyword slrnrcFunGroup	contained repeat_last_key save_newsrc select_group subscribe
syn keyword slrnrcFunGroup	contained suspend toggle_group_formats toggle_hidden
syn keyword slrnrcFunGroup	contained toggle_list_all toggle_scoring transpose_groups
syn keyword slrnrcFunGroup	contained uncatchup unsubscribe
" Listed for removal
syn keyword slrnrcFunGroup	contained down group_bob group_eob pagedown pageup
syn keyword slrnrcFunGroup	contained toggle_group_display uncatch_up up

" Functions in readline mode (actually from slang's slrline.c)
syn keyword slrnrcFunRead	contained bdel bol del deleol down enter eol left
syn keyword slrnrcFunRead	contained quoted_insert right trim up

" binding keys
syn keyword slrnrcSetkeyObj	contained article group readline
syn region  slrnrcSetkeyObjStr	contained matchgroup=slrnrcSetkeyObj start=+"+ end=+"+ oneline contains=slrnrcSetkeyObj
syn match   slrnrcSetkeyArt	contained '\("\=\)\<article\>\1\s\+\S\+' skipwhite nextgroup=slrnrcKey contains=slrnrcSetKeyObj\(Str\)\=,slrnrcFunArt
syn match   slrnrcSetkeyGroup	contained '\("\=\)\<group\>\1\s\+\S\+' skipwhite nextgroup=slrnrcKey contains=slrnrcSetKeyObj\(Str\)\=,slrnrcFunGroup
syn match   slrnrcSetkeyRead	contained '\("\=\)\<readline\>\1\s\+\S\+' skipwhite nextgroup=slrnrcKey contains=slrnrcSetKeyObj\(Str\)\=,slrnrcFunRead
syn match   slrnrcSetkey	"^\s*setkey\>" skipwhite nextgroup=slrnrcSetkeyArt,slrnrcSetkeyGroup,slrnrcSetkeyRead

" unbinding keys
syn match   slrnrcUnsetkey	'^\s*unsetkey\s\+\("\)\=\(article\|group\|readline\)\>\1' skipwhite nextgroup=slrnrcKey contains=slrnrcSetkeyObj\(Str\)\=

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_slrnrc_syntax_inits")
  if version < 508
    let did_slrnrc_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink slrnrcTodo		Todo
  HiLink slrnrcSpaceError	Error
  HiLink slrnrcNumber		Number
  HiLink slrnrcSpecKey		SpecialChar
  HiLink slrnrcKey		String
  HiLink slrnrcSpecChar		SpecialChar
  HiLink slrnrcString		String
  HiLink slrnSlangPreCondit	Special
  HiLink slrnrcComment		Comment
  HiLink slrnrcVarInt		Identifier
  HiLink slrnrcVarStr		Identifier
  HiLink slrnrcCmd		slrnrcSet
  HiLink slrnrcSet		Operator
  HiLink slrnrcColor		Keyword
  HiLink slrnrcColorObj		Identifier
  HiLink slrnrcColorVal		String
  HiLink slrnrcMono		Keyword
  HiLink slrnrcMonoObj		Identifier
  HiLink slrnrcMonoVal		String
  HiLink slrnrcFunArt		Macro
  HiLink slrnrcFunGroup		Macro
  HiLink slrnrcFunRead		Macro
  HiLink slrnrcSetkeyObj	Identifier
  HiLink slrnrcSetkey		Keyword
  HiLink slrnrcUnsetkey		slrnrcSetkey

  "HiLink slrnrcObsolete	Special

  delcommand HiLink
endif

let b:current_syntax = "slrnrc"

"EOF	vim: ts=8 noet tw=120 sw=8 sts=0
