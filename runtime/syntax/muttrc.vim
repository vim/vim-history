" Vim syntax file
" Language:	Mutt setup files
" Maintainer:	Preben "Peppe" Guldberg <c928400@student.dtu.dk>
" Last Change:	22nd Sep 2001

" This file covers mutt version 1.2.5i

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Set the keyword characters
if version < 600
  set isk=@,48-57,_,-
else
  setlocal isk=@,48-57,_,-
endif

syn match muttrcComment		"^#.*$"
syn match muttrcComment		"[^\\]#.*$"lc=1

" Escape sequences (back-tick and pipe goes here too)
syn match muttrcEscape		+\\[#tnr"'Cc]+
syn match muttrcEscape		+[`|]+

" The variables takes the following arguments
syn match  muttrcString		"=\s*[^ #"']\+"lc=1 contains=muttrcEscape
syn region muttrcString		start=+"+ms=e skip=+\\"+ end=+"+ contains=muttrcEscape,muttrcSet,muttrcCommand
syn region muttrcString		start=+'+ms=e skip=+\\'+ end=+'+ contains=muttrcEscape,muttrcSet,muttrcCommand

syn match muttrcSpecial		+\(['"]\)!\1+

" Numbers and Quadoptions may be surrounded by " or '
syn match muttrcNumber		/=\s*\d\+/lc=1
syn match muttrcNumber		/"=\s*\d\+"/lc=2
syn match muttrcNumber		/'=\s*\d\+'/lc=2
syn match muttrcQuadopt		+=\s*\(ask-\)\=\(yes\|no\)+lc=1
syn match muttrcQuadopt		+"=\s*\(ask-\)\=\(yes\|no\)"+lc=2
syn match muttrcQuadopt		+'=\s*\(ask-\)\=\(yes\|no\)'+lc=2

" Now catch some email addresses and headers (purified version from mail.vim)
syn match muttrcEmail		"[a-zA-Z0-9._-]\+@[a-zA-Z0-9./-]\+"
syn match muttrcHeader		"\<\(From\|To\|Cc\|Bcc\|Reply-To\|Subject\)\>:\="
syn match muttrcHeader		"\<\(Return-Path\|Received\|Date\|Replied\|Attach\)\>:\="

syn match   muttrcKeySpecial	contained +\(\\[Cc'"]\|\^\|\\[01]\d\{2}\)+
syn match   muttrcKey		contained "\S\+"			contains=muttrcKeySpecial
syn region  muttrcKey		contained start=+"+ skip=+\\"+ end=+"+	contains=muttrcKeySpecial
syn region  muttrcKey		contained start=+'+ skip=+\\'+ end=+'+	contains=muttrcKeySpecial
syn match   muttrcKeyName	contained "\<f\(\d\|10\)\>"
syn match   muttrcKeyName	contained "\\[trne]"
syn match   muttrcKeyName	contained "\(<BackSpace>\|<Delete>\|<Down>\|<End>\|<Enter>\|<Esc>\)"
syn match   muttrcKeyName	contained "\(<Home>\|<Insert>\|<Left>\|<PageDown>\|<PageUp>\)"
syn match   muttrcKeyName	contained "\(<Return>\|<Right>\|<Space>\|<Tab>\|<Up>\)"

syn keyword muttrcVarBool	contained allow_8bit arrow_cursor ascii_chars askbcc askcc
syn keyword muttrcVarBool	contained attach_split auto_tag autoedit beep beep_new
syn keyword muttrcVarBool	contained bounce_delivered check_new collapse_unread confirmappend
syn keyword muttrcVarBool	contained confirmcreate delete_untag edit_hdrs edit_headers
syn keyword muttrcVarBool	contained encode_from envelope_from fast_reply fcc_attach
syn keyword muttrcVarBool	contained fcc_clear followup_to force_name forw_decode
syn keyword muttrcVarBool	contained forw_decrypt forw_quote forward_decode forward_decrypt
syn keyword muttrcVarBool	contained forward_quote hdrs header help hidden_host
syn keyword muttrcVarBool	contained ignore_list_reply_to imap_force_ssl imap_list_subscribed
syn keyword muttrcVarBool	contained imap_passive imap_servernoise implicit_autoview
syn keyword muttrcVarBool	contained mailcap_sanitize mark_old markers menu_scroll meta_key
syn keyword muttrcVarBool	contained metoo mh_purge mime_forward_decode pager_stop
syn keyword muttrcVarBool	contained pgp_autoencrypt pgp_autosign pgp_long_ids
syn keyword muttrcVarBool	contained pgp_replyencrypt pgp_replysign pgp_replysignencrypted
syn keyword muttrcVarBool	contained pgp_retainable_sigs pgp_show_unusable pgp_strict_enc
syn keyword muttrcVarBool	contained pipe_decode pipe_split pop_delete pop_last print_decode
syn keyword muttrcVarBool	contained prompt_after read_only reply_self resolve reverse_alias
syn keyword muttrcVarBool	contained reverse_name reverse_realname rfc2047_parameters
syn keyword muttrcVarBool	contained save_address save_empty save_name score sig_dashes
syn keyword muttrcVarBool	contained smart_wrap sort_re ssl_use_sslv2 ssl_use_sslv3
syn keyword muttrcVarBool	contained ssl_use_tlsv1 ssl_usesystemcerts status_on_top
syn keyword muttrcVarBool	contained strict_threads suspend thorough_search tilde
syn keyword muttrcVarBool	contained uncollapse_jump use_8bitmime use_domain use_from
syn keyword muttrcVarBool	contained user_agent wait_key weed wrap_search write_bcc

syn keyword muttrcVarBool	contained noallow_8bit noarrow_cursor noascii_chars noaskbcc
syn keyword muttrcVarBool	contained noaskcc noattach_split noauto_tag noautoedit nobeep
syn keyword muttrcVarBool	contained nobeep_new nobounce_delivered nocheck_new
syn keyword muttrcVarBool	contained nocollapse_unread noconfirmappend noconfirmcreate
syn keyword muttrcVarBool	contained nodelete_untag noedit_hdrs noedit_headers noencode_from
syn keyword muttrcVarBool	contained noenvelope_from nofast_reply nofcc_attach nofcc_clear
syn keyword muttrcVarBool	contained nofollowup_to noforce_name noforw_decode noforw_decrypt
syn keyword muttrcVarBool	contained noforw_quote noforward_decode noforward_decrypt
syn keyword muttrcVarBool	contained noforward_quote nohdrs noheader nohelp nohidden_host
syn keyword muttrcVarBool	contained noignore_list_reply_to noimap_force_ssl
syn keyword muttrcVarBool	contained noimap_list_subscribed noimap_passive noimap_servernoise
syn keyword muttrcVarBool	contained noimplicit_autoview nomailcap_sanitize nomark_old
syn keyword muttrcVarBool	contained nomarkers nomenu_scroll nometa_key nometoo nomh_purge
syn keyword muttrcVarBool	contained nomime_forward_decode nopager_stop nopgp_autoencrypt
syn keyword muttrcVarBool	contained nopgp_autosign nopgp_long_ids nopgp_replyencrypt
syn keyword muttrcVarBool	contained nopgp_replysign nopgp_replysignencrypted
syn keyword muttrcVarBool	contained nopgp_retainable_sigs nopgp_show_unusable
syn keyword muttrcVarBool	contained nopgp_strict_enc nopipe_decode nopipe_split nopop_delete
syn keyword muttrcVarBool	contained nopop_last noprint_decode noprompt_after noread_only
syn keyword muttrcVarBool	contained noreply_self noresolve noreverse_alias noreverse_name
syn keyword muttrcVarBool	contained noreverse_realname norfc2047_parameters nosave_address
syn keyword muttrcVarBool	contained nosave_empty nosave_name noscore nosig_dashes
syn keyword muttrcVarBool	contained nosmart_wrap nosort_re nossl_use_sslv2 nossl_use_sslv3
syn keyword muttrcVarBool	contained nossl_use_tlsv1 nossl_usesystemcerts nostatus_on_top
syn keyword muttrcVarBool	contained nostrict_threads nosuspend nothorough_search notilde
syn keyword muttrcVarBool	contained nouncollapse_jump nouse_8bitmime nouse_domain nouse_from
syn keyword muttrcVarBool	contained nouser_agent nowait_key noweed nowrap_search nowrite_bcc

syn keyword muttrcVarBool	contained invallow_8bit invarrow_cursor invascii_chars invaskbcc
syn keyword muttrcVarBool	contained invaskcc invattach_split invauto_tag invautoedit invbeep
syn keyword muttrcVarBool	contained invbeep_new invbounce_delivered invcheck_new
syn keyword muttrcVarBool	contained invcollapse_unread invconfirmappend invconfirmcreate
syn keyword muttrcVarBool	contained invdelete_untag invedit_hdrs invedit_headers
syn keyword muttrcVarBool	contained invencode_from invenvelope_from invfast_reply
syn keyword muttrcVarBool	contained invfcc_attach invfcc_clear invfollowup_to invforce_name
syn keyword muttrcVarBool	contained invforw_decode invforw_decrypt invforw_quote
syn keyword muttrcVarBool	contained invforward_decode invforward_decrypt invforward_quote
syn keyword muttrcVarBool	contained invhdrs invheader invhelp invhidden_host
syn keyword muttrcVarBool	contained invignore_list_reply_to invimap_force_ssl
syn keyword muttrcVarBool	contained invimap_list_subscribed invimap_passive
syn keyword muttrcVarBool	contained invimap_servernoise invimplicit_autoview
syn keyword muttrcVarBool	contained invmailcap_sanitize invmark_old invmarkers
syn keyword muttrcVarBool	contained invmenu_scroll invmeta_key invmetoo invmh_purge
syn keyword muttrcVarBool	contained invmime_forward_decode invpager_stop invpgp_autoencrypt
syn keyword muttrcVarBool	contained invpgp_autosign invpgp_long_ids invpgp_replyencrypt
syn keyword muttrcVarBool	contained invpgp_replysign invpgp_replysignencrypted
syn keyword muttrcVarBool	contained invpgp_retainable_sigs invpgp_show_unusable
syn keyword muttrcVarBool	contained invpgp_strict_enc invpipe_decode invpipe_split
syn keyword muttrcVarBool	contained invpop_delete invpop_last invprint_decode
syn keyword muttrcVarBool	contained invprompt_after invread_only invreply_self invresolve
syn keyword muttrcVarBool	contained invreverse_alias invreverse_name invreverse_realname
syn keyword muttrcVarBool	contained invrfc2047_parameters invsave_address invsave_empty
syn keyword muttrcVarBool	contained invsave_name invscore invsig_dashes invsmart_wrap
syn keyword muttrcVarBool	contained invsort_re invssl_use_sslv2 invssl_use_sslv3
syn keyword muttrcVarBool	contained invssl_use_tlsv1 invssl_usesystemcerts invstatus_on_top
syn keyword muttrcVarBool	contained invstrict_threads invsuspend invthorough_search invtilde
syn keyword muttrcVarBool	contained invuncollapse_jump invuse_8bitmime invuse_domain
syn keyword muttrcVarBool	contained invuse_from invuser_agent invwait_key invweed
syn keyword muttrcVarBool	contained invwrap_search invwrite_bcc

syn keyword muttrcVarQuad	contained abort_nosubject abort_unmodified copy delete
syn keyword muttrcVarQuad	contained honor_followup_to include mime_forward mime_forward_rest
syn keyword muttrcVarQuad	contained mime_fwd move pgp_create_traditional pgp_verify_sig
syn keyword muttrcVarQuad	contained postpone print quit recall reply_to

syn keyword muttrcVarQuad	contained noabort_nosubject noabort_unmodified nocopy nodelete
syn keyword muttrcVarQuad	contained nohonor_followup_to noinclude nomime_forward
syn keyword muttrcVarQuad	contained nomime_forward_rest nomime_fwd nomove
syn keyword muttrcVarQuad	contained nopgp_create_traditional nopgp_verify_sig nopostpone
syn keyword muttrcVarQuad	contained noprint noquit norecall noreply_to

syn keyword muttrcVarQuad	contained invabort_nosubject invabort_unmodified invcopy invdelete
syn keyword muttrcVarQuad	contained invhonor_followup_to invinclude invmime_forward
syn keyword muttrcVarQuad	contained invmime_forward_rest invmime_fwd invmove
syn keyword muttrcVarQuad	contained invpgp_create_traditional invpgp_verify_sig invpostpone
syn keyword muttrcVarQuad	contained invprint invquit invrecall invreply_to

syn keyword muttrcVarNum	contained history imap_checkinterval mail_check pager_context
syn keyword muttrcVarNum	contained pager_index_lines pgp_timeout pop_port read_inc
syn keyword muttrcVarNum	contained score_threshold_delete score_threshold_flag
syn keyword muttrcVarNum	contained score_threshold_read sendmail_wait timeout write_inc

syn keyword muttrcVarStr	contained alias_file alias_format alternates attach_format
syn keyword muttrcVarStr	contained attach_sep attribution certificate_file charset
syn keyword muttrcVarStr	contained date_format default_hook dotlock_program dsn_notify
syn keyword muttrcVarStr	contained dsn_return editor entropy_file escape folder
syn keyword muttrcVarStr	contained folder_format forw_format forward_format from gecos_mask
syn keyword muttrcVarStr	contained hdr_format hostname imap_cramkey imap_home_namespace
syn keyword muttrcVarStr	contained imap_pass imap_preconnect imap_user in_reply_to
syn keyword muttrcVarStr	contained indent_str indent_string index_format ispell locale
syn keyword muttrcVarStr	contained mailcap_path mask mbox mbox_type message_format
syn keyword muttrcVarStr	contained mix_entry_format mixmaster msg_format pager pager_format
syn keyword muttrcVarStr	contained pgp_clearsign_command pgp_decode_command
syn keyword muttrcVarStr	contained pgp_decrypt_command pgp_encrypt_only_command
syn keyword muttrcVarStr	contained pgp_encrypt_sign_command pgp_entry_format
syn keyword muttrcVarStr	contained pgp_export_command pgp_getkeys_command
syn keyword muttrcVarStr	contained pgp_import_command pgp_list_pubring_command
syn keyword muttrcVarStr	contained pgp_list_secring_command pgp_sign_as pgp_sign_command
syn keyword muttrcVarStr	contained pgp_sign_micalg pgp_sort_keys pgp_verify_command
syn keyword muttrcVarStr	contained pgp_verify_key_command pipe_sep pop_host pop_pass
syn keyword muttrcVarStr	contained pop_user post_indent_str post_indent_string postponed
syn keyword muttrcVarStr	contained print_cmd print_command query_command quote_regexp
syn keyword muttrcVarStr	contained realname record reply_regexp send_charset sendmail shell
syn keyword muttrcVarStr	contained signature simple_search smileys sort sort_alias sort_aux
syn keyword muttrcVarStr	contained sort_browser spoolfile status_chars status_format tmpdir
syn keyword muttrcVarStr	contained to_chars visual

syn keyword muttrcMenu		contained alias attach browser compose editor index pager postpone
syn keyword muttrcMenu		contained pgp mix query generic

syn keyword muttrcCommand	auto_view alternative_order charset-hook uncolor exec
syn keyword muttrcCommand	fcc-hook fcc-save-hook folder-hook hdr_order ignore
syn keyword muttrcCommand	lists mailboxes mbox-hook my_hdr pgp-hook push reset
syn keyword muttrcCommand	save-hook score send-hook source subscribe toggle
syn keyword muttrcCommand	unalias unhdr_order unhook unignore unlists unmono
syn keyword muttrcCommand	unmy_hdr unscore unsubscribe

syn keyword muttrcSet		set     skipwhite nextgroup=muttrcVar.*
syn keyword muttrcUnset		unset   skipwhite nextgroup=muttrcVar.*

syn keyword muttrcBind		contained bind		skipwhite nextgroup=muttrcMenu
syn match   muttrcBindLine	"^\s*bind\s\+\S\+"	skipwhite nextgroup=muttrcKey\(Name\)\= contains=muttrcBind

syn keyword muttrcMacro		contained macro		skipwhite nextgroup=muttrcMenu
syn match   muttrcMacroLine	"^\s*macro\s\+\S\+"	skipwhite nextgroup=muttrcKey\(Name\)\= contains=muttrcMacro

syn keyword muttrcAlias		contained alias
syn match   muttrcAliasLine	"^\s*alias\s\+\S\+" contains=muttrcAlias

" Colour definitions takes object, foreground and background arguments (regexps excluded).
syn keyword muttrcColorField	contained attachment body bold error hdrdefault header index
syn keyword muttrcColorField	contained indicator markers message normal quoted search signature
syn keyword muttrcColorField	contained status tilde tree underline
syn match   muttrcColorField	contained "\<quoted\d\=\>"
syn keyword muttrcColorFG	contained black blue cyan default green magenta red white yellow
syn keyword muttrcColorFG	contained brightblue brightcyan brightdefault brightgreen
syn keyword muttrcColorFG	contained brightmagenta brightred brightwhite brightyellow
syn match   muttrcColorFG	contained "\<\(bright\)\=color\d\{1,2}\>"
syn keyword muttrcColorBG	contained black blue cyan default green magenta red white yellow
syn match   muttrcColorBG	contained "\<color\d\{1,2}\>"
" Now for the match
syn keyword muttrcColor		contained color			skipwhite nextgroup=muttrcColorField
syn match   muttrcColorInit	contained "^\s*color\s\+\S\+"	skipwhite nextgroup=muttrcColorFG contains=muttrcColor
syn match   muttrcColorLine	"^\s*color\s\+\S\+\s\+\S"	skipwhite nextgroup=muttrcColorBG contains=muttrcColorInit

" Mono are almost like color (ojects inherited from color)
syn keyword muttrcMonoAttrib	contained bold none normal reverse standout underline
syn keyword muttrcMono		contained mono		skipwhite nextgroup=muttrcColorField
syn match   muttrcMonoLine	"^\s*mono\s\+\S\+"	skipwhite nextgroup=muttrcMonoAttrib contains=muttrcMono

" obsolete
syn keyword muttrcKeyName	contained backspace delete down end enter home insert left pagedown
syn keyword muttrcKeyName	contained pageup return right up
syn keyword muttrcVarBool	contained confirmfiles confirmfolders hold no_hdrs pgp_replypgp
syn keyword muttrcVarBool	contained point_new noconfirmfiles noconfirmfolders nohold nono_hdrs
syn keyword muttrcVarBool	contained nopgp_replypgp nopoint_new invconfirmfiles
syn keyword muttrcVarBool	contained invconfirmfolders invhold invno_hdrs invpgp_replypgp
syn keyword muttrcVarBool	contained invpoint_new attach_split edit_hdrs forw_decode forw_quote
syn keyword muttrcVarBool	contained mime_fwd
syn keyword muttrcVarNum	contained references
syn keyword muttrcVarQuad	contained verify_sig
syn keyword muttrcVarStr	contained local_sig local_site pgp pgp_pubring pgp_secring
syn keyword muttrcVarStr	contained pgp_version remote_sig thread_chars url_regexp web_browser
syn keyword muttrcVarStr	contained decode_format pgp_v3 pgp_v3_language pgp_v3_pubring
syn keyword muttrcVarStr	contained pgp_v3_secring sendmail_bounce
syn keyword muttrcMenu		contained url
syn keyword muttrcCommand	alternates localsite unlocalsite

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_muttrc_syntax_inits")
  if version < 508
    let did_muttrc_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink muttrcComment		Comment
  HiLink muttrcEscape		SpecialChar
  HiLink muttrcString		String
  HiLink muttrcSpecial		Special
  HiLink muttrcNumber		Number
  HiLink muttrcQuadopt		Boolean
  HiLink muttrcEmail		Special
  HiLink muttrcHeader		Type
  HiLink muttrcKeySpecial	SpecialChar
  HiLink muttrcKey		Type
  HiLink muttrcKeyName		Macro
  HiLink muttrcVarBool		Identifier
  HiLink muttrcVarQuad		Identifier
  HiLink muttrcVarNum		Identifier
  HiLink muttrcVarStr		Identifier
  HiLink muttrcMenu		Identifier
  HiLink muttrcCommand		Keyword
  HiLink muttrcSet		muttrcCommand
  HiLink muttrcUnset		muttrcCommand
  HiLink muttrcBind		muttrcCommand
  HiLink muttrcMacro		muttrcCommand
  HiLink muttrcAlias		muttrcCommand
  HiLink muttrcAliasLine	Identifier
  HiLink muttrcColorField	Identifier
  HiLink muttrcColorFG		String
  HiLink muttrcColorBG		muttrcColorFG
  HiLink muttrcColor		muttrcCommand
  HiLink muttrcMonoAttrib	muttrcColorFG
  HiLink muttrcMono		muttrcCommand

  delcommand HiLink
endif

let b:current_syntax = "muttrc"

"EOF	vim: ts=8 noet tw=100 sw=8 sts=0
