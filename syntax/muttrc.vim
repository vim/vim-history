" Vim syntax file
" Language:	Mutt setup files
" Maintainer:	Preben "Peppe" Guldberg (c928400@student.dtu.dk)
" Last change:	Mon Feb  9 11:26:52 1998

" This file is for mutt 0.88. It is best used for version 0.81 to 0.88
" Got a newer version of mutt?
" 	See: 	http://www.student.dtu.dk/~c928400/vim

" Remove any old syntax stuff hanging around
syn clear

" String variables
syn keyword muttrcIdentString	alias_file alternates attach_sep attribution charset date_format
syn keyword muttrcIdentString	decode_format dsn_notify dsn_return editor escape forw_format
syn keyword muttrcIdentString	hdr_format hostname in_reply_to indent_str ispell local_sig
syn keyword muttrcIdentString	local_site locale mailcap_path mask mbox_type msg_format pager
syn keyword muttrcIdentString	pager_format pipe_sep pop_host pop_pass pop_user post_indent_str
syn keyword muttrcIdentString	postponed print_cmd quote_regexp realname record remote_sig
syn keyword muttrcIdentString	reply_regexp sendmail sendmail_bounce shell signature
syn keyword muttrcIdentString	simple_search sort sort_aux sort_browser spoolfile status_chars
syn keyword muttrcIdentString	status_format tmpdir to_chars url_regexp visual web_browser
syn match   muttrcIdentString	"\<folder\|mbox\>"

" Number variables
syn keyword muttrcIdentNumber	history pager_context pager_index_lines pop_port
syn keyword muttrcIdentNumber	read_inc references timeout write_inc

" Boolean variables
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(allow_8bit\|arrow_cursor\|ascii_chars\|askbcc\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(askcc\|attach_split\|autoedit\|auto_tag\|beep\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(check_new\|confirmappend\|confirmcreate\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(edit_hdrs\|fast_reply\|fcc_attach\|force_name\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(forw_decode\|hdrs\|header\|help\|hold\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(ignore_list_reply_to\|mark_old\|markers\|metoo\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(mime_fwd\|pager_stop\|pipe_decode\|pipe_split\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(point_new\|pop_delete\|prompt_after\|read_only\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(resolve\|reverse_alias\|save_empty\|save_name\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(sig_dashes\|smart_wrap\|sort_re\|status_on_top\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(strict_threads\|suspend\|thorough_search\|tilde\)\>"
syn match muttrcIdentBoolean	"\<\(no\|inv\)\=\(use_8bitmime\|use_domain\|use_from\|wait_key\)\>"

" Quadoption variables (noQuadoption is not supported here - eg set nocopy)
syn match muttrcIdentQuadopt	"\<\(no\|inv\)\=\(abort_nosubject\|abort_unmodified\|copy\|delete\)\>"
syn match muttrcIdentQuadopt	"\<\(no\|inv\)\=\(include\|move\|postpone\|print\|recall\|reply_to\)\>"
syn match muttrcIdentQuadopt	"\<\(no\|inv\)\=\(use_mailcap\|verify_sig\)\>"

" The variables takes the following arguments
syn match  muttrcVarString	"=\s*[^ #\t"']\+"lc=1 contains=muttrcEscape
syn region muttrcVarString	start=+=\=\s*"+ms=e skip=+\\"+ end=+"+ oneline contains=muttrcEscape
syn region muttrcVarString	start=+=\=\s*'+ms=e skip=+\\'+ end=+'+ oneline contains=muttrcEscape
" Numbers and Quadoptions may be surrounded by " or '
syn match muttrcVarNumber	/=\s*[0-9]\+/lc=1
syn match muttrcVarNumber	/"=\s*[0-9]\+"/lc=1
syn match muttrcVarNumber	/'=\s*[0-9]\+'/lc=1
syn match muttrcVarQuadopt	+=\s*\(ask-\)\=\(yes\|no\)+lc=1
syn match muttrcVarQuadopt	+"=\s*\(ask-\)\=\(yes\|no\)"+lc=1
syn match muttrcVarQuadopt	+'=\s*\(ask-\)\=\(yes\|no\)'+lc=1

" Escape sequences (back-tick,`, shell-quote goes here too)
syn match muttrcEscape		+\\[#tnr"']+
syn match muttrcEscape		+`+

" Now catch some email addresses and headers (purified version from mail.vim)
syn match muttrcEmail		"[a-zA-Z0-9._-]\+@[a-zA-Z0-9./-]\+"
syn match muttrcHeader		"\(From:\|To:\|Cc:\|Bcc:\|Reply-To:\|Subject:\|Return-Path:\|Received:\|Date:\|Replied:\)"
" For those that prefer to specify without the colon
"syn match    muttrcHeader	"\(From\|To\|Cc\|Bcc\|Reply-To\|Subject\|Return-Path\|Received\|Date\|Replied\)"

" Some statements (a few have function like highlighting just below)
syn keyword muttrcKeyword	set unset
syn keyword muttrcKeyword	auto_view hdr_order ignore unignore lists unlists
syn keyword muttrcKeyword	mailboxes my_hdr unmy_hdr push source toggle unalias

" Various hooks
syn match muttrcHook		"\<\(fcc\|folder\|mbox\|save\|send\)-hook\>"

" Aliases contain a key word: let's show it
syn region muttrcAliasFill	contained transparent matchgroup=muttrcFunc start="\<alias\>" matchgroup=muttrcFuncKey end="\S\+"
syn match  muttrcAliasLine	"alias\s\+\S\+" contains=muttrcAliasFill

" These are convinient to have defined for bind and macro
syn keyword muttrcMap		contained index pager alias attach browser editor compose pgp url

" Bindings have map, key and function.
syn region muttrcBindFill	contained transparent matchgroup=muttrcFunc start="\<bind\>"  skip="\<\(index\|pager\|alias\|attach\|browser\|editor\|compose\|pgp\|url\)\>" matchgroup=muttrcFuncKey end="\S\+" contains=muttrcMap
syn match  muttrcBindLine	"\s*bind\s\+\S\+\s\+\S\+" contains=muttrcBindFill

" Macros are more or less the same
syn region muttrcMacroFill	contained transparent matchgroup=muttrcFunc start="\<macro\>" skip="\<\(index\|pager\|alias\|attach\|browser\|editor\|compose\|pgp\|url\)\>" matchgroup=muttrcFuncKey end="\S\+" contains=muttrcMap
syn match  muttrcMacroLine	"\s*macro\s\+\S\+\s\+\S\+" contains=muttrcMacroFill

" Colour definitions takes object, foreground and background arguments (regexps excluded).
syn keyword muttrcColorObj	contained attachment body bold error header hdrdefault
syn keyword muttrcColorObj	contained indicator markers message search signature
syn keyword muttrcColorObj	contained status tilde tree underline
syn match   muttrcColorObj	"\<quoted[1-9]\=\>"
syn match   muttrcColorFG	contained "\<\(bright\)\=\(white\|black\|green\|magenta\|blue\|cyan\|yellow\|red\|default\)\>"
syn match   muttrcColorFG	contained "\<\(bright\)\=color[0-9]\{1,2}\>"
syn match   muttrcColorBG	contained "\<\(white\|black\|green\|magenta\|blue\|cyan\|yellow\|red\|default\)\>"
syn match   muttrcColorBG	contained "\<color[0-9]\{1,2}\>"
" Now for the total match
syn region  muttrcColorFill	contained matchgroup=muttrcColor start="\<color\>" matchgroup=muttrcColorObj end="\<\(attachment\|body\|bold\|error\|header\|hdrdefault\|indicator\|markers\|message\|normal\|quoted[0-9]\=\|search\|signature\|status\|tilde\|tree\|underline\)\>"
syn match   muttrcColorLine	"\s*color\s\+\S\+\s\+\(bright\)\=\S\+\s\+\S\+" contains=muttrcColor\(Fill\|FG\|BG\)

" Mono are almst like color (ojects ingherited from color)
syn keyword muttrcMonoAttrib	contained none bold underline reverse standout
syn region  muttrcMonoFill	contained matchgroup=muttrcMono start="\<mono\>" matchgroup=muttrcMonoObj end="\<\(attachment\|body\|bold\|error\|header\|hdrdefault\|indicator\|markers\|message\|normal\|quoted[0-9]\=\|search\|signature\|status\|tilde\|tree\|underline\)\>"
syn match   muttrcMonoLine	"\s*mono\s\+\S\+\s\+\S\+" contains=muttrcMono\(Fill\|Attrib\)

" comments are quite easy here: a # that is not preceded by a \
syn match muttrcComment	"^#.*$"
syn match muttrcComment	"[^\\]#.*$"lc=1

if !exists("did_muttrc_syntax_inits")
  let did_muttrc_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link muttrcKeyword		Keyword
  hi link muttrcHook		Keyword
  hi link muttrcIdentString	Identifier
  hi link muttrcIdentNumber	Identifier
  hi link muttrcIdentBoolean	Identifier
  hi link muttrcIdentQuadopt	Identifier
  hi link muttrcVarString	String
  hi link muttrcVarNumber	Number
  hi link muttrcVarQuadopt	Boolean
  hi link muttrcComment		Comment
  hi link muttrcEscape		SpecialChar
  hi link muttrcEmail		String
  hi link muttrcHeader		Type
  hi link muttrcMap		Identifier
  hi link muttrcFunc		Keyword
  hi link muttrcFuncKey		Type
  hi link muttrcColor		Keyword
  hi link muttrcColorObj	Identifier
  hi link muttrcColorFG		String
  hi link muttrcColorBG		String
  hi link muttrcMono		Keyword
  hi link muttrcMonoObj		Identifier
  hi link muttrcMonoAttrib	String
  hi link muttrcStrError	Error
endif

"EOF	vim: ts=8 noet tw=100 sw=8 sts=0
