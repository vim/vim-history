" Wget syntax file
" Filename:     wget.vim
" Language:     Wget configuration file ( /etc/wgetrc ~/.wgetrc )
" Maintainer:   Doug Kearns <djkea2@mugca.cc.monash.edu.au>
" URL:          http://mugca.cc.monash.edu.au/~djkea2/vim/syntax/wget.vim
" Last Change:  2001 Sep 05

" TODO: all commands are actually underscore-insensitive as well as
"       case-insensitive, though they are normally named as listed below

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn match   wgetComment    "^\s*#.*$" contains=wgetTodo

syn keyword wgetTodo       TODO NOTE FIXME XXX contained

syn match   wgetAssignment "^\s*\h\+\s*=\s*.*$" contains=wgetCommand,wgetAssignmentOperator,wgetString,wgetBoolean,wgetNumber,wgetValue,wgetQuota

syn match   wgetAssignmentOperator "=" contained

syn region  wgetString     start=+"+ skip=+\\\\\|\\"+ end=+"+ contained oneline
syn region  wgetString     start=+'+ skip=+\\\\\|\\'+ end=+'+ contained oneline

syn case ignore
syn keyword wgetBoolean    on off always never 1 0 contained
syn case match

syn match   wgetNumber     "\<\d\+\|inf\>"    contained

syn match   wgetQuota      "\<\d\+[kKmM]\?\>" contained

syn case ignore
syn keyword wgetValue      default binary mega giga micro contained
syn case match

syn case ignore
syn match wgetCommand      "^\s*accept" contained
syn match wgetCommand      "^\s*add_hostdir" contained
syn match wgetCommand      "^\s*background" contained
syn match wgetCommand      "^\s*backup_converted" contained
syn match wgetCommand      "^\s*base" contained
syn match wgetCommand      "^\s*bind_address" contained
syn match wgetCommand      "^\s*cache" contained
syn match wgetCommand      "^\s*continue" contained
syn match wgetCommand      "^\s*convert_links" contained
syn match wgetCommand      "^\s*cookies" contained
syn match wgetCommand      "^\s*cut_dirs" contained
syn match wgetCommand      "^\s*debug" contained
syn match wgetCommand      "^\s*delete_after" contained
syn match wgetCommand      "^\s*dir_prefix" contained
syn match wgetCommand      "^\s*dirstruct" contained
syn match wgetCommand      "^\s*domains" contained
syn match wgetCommand      "^\s*dot_bytes" contained
syn match wgetCommand      "^\s*dots_in_line" contained
syn match wgetCommand      "^\s*dot_spacing" contained
syn match wgetCommand      "^\s*dot_style" contained
syn match wgetCommand      "^\s*exclude_directories" contained
syn match wgetCommand      "^\s*exclude_domains" contained
syn match wgetCommand      "^\s*follow_ftp" contained
syn match wgetCommand      "^\s*follow_tags" contained
syn match wgetCommand      "^\s*force_html" contained
syn match wgetCommand      "^\s*ftp_proxy" contained
syn match wgetCommand      "^\s*glob" contained
syn match wgetCommand      "^\s*header" contained
syn match wgetCommand      "^\s*html_extension" contained
syn match wgetCommand      "^\s*http_keep_alive" contained
syn match wgetCommand      "^\s*http_passwd" contained
syn match wgetCommand      "^\s*http_proxy" contained
syn match wgetCommand      "^\s*https_proxy" contained
syn match wgetCommand      "^\s*http_user" contained
syn match wgetCommand      "^\s*ignore_length" contained
syn match wgetCommand      "^\s*ignore_tags" contained
syn match wgetCommand      "^\s*include_directories" contained
syn match wgetCommand      "^\s*input" contained
syn match wgetCommand      "^\s*kill_longer" contained
syn match wgetCommand      "^\s*load_cookies" contained
syn match wgetCommand      "^\s*logfile" contained
syn match wgetCommand      "^\s*login" contained
syn match wgetCommand      "^\s*mirror" contained
syn match wgetCommand      "^\s*netrc" contained
syn match wgetCommand      "^\s*noclobber" contained
syn match wgetCommand      "^\s*no_parent" contained
syn match wgetCommand      "^\s*no_proxy" contained
syn match wgetCommand      "^\s*output_document" contained
syn match wgetCommand      "^\s*page_requisites" contained
syn match wgetCommand      "^\s*passive_ftp" contained
syn match wgetCommand      "^\s*passwd" contained
syn match wgetCommand      "^\s*proxy_passwd" contained
syn match wgetCommand      "^\s*proxy_user" contained
syn match wgetCommand      "^\s*quiet" contained
syn match wgetCommand      "^\s*quota" contained
syn match wgetCommand      "^\s*reclevel" contained
syn match wgetCommand      "^\s*recursive" contained
syn match wgetCommand      "^\s*referer" contained
syn match wgetCommand      "^\s*reject" contained
syn match wgetCommand      "^\s*relative_only" contained
syn match wgetCommand      "^\s*remove_listing" contained
syn match wgetCommand      "^\s*retr_symlinks" contained
syn match wgetCommand      "^\s*robots" contained
syn match wgetCommand      "^\s*save_cookies" contained
syn match wgetCommand      "^\s*server_response" contained
syn match wgetCommand      "^\s*simple_host_check" contained
syn match wgetCommand      "^\s*span_hosts" contained
syn match wgetCommand      "^\s*sslcertfile" contained
syn match wgetCommand      "^\s*sslcertkey" contained
syn match wgetCommand      "^\s*timeout" contained
syn match wgetCommand      "^\s*timestamping" contained
syn match wgetCommand      "^\s*tries" contained
syn match wgetCommand      "^\s*use_proxy" contained
syn match wgetCommand      "^\s*verbose" contained
syn match wgetCommand      "^\s*wait" contained
syn match wgetCommand      "^\s*waitretry" contained
syn case match

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_wget_syn_inits")
  if version < 508
    let did_wget_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink wgetAssignmentOperator Special
  HiLink wgetBoolean            Boolean
  HiLink wgetCommand            Identifier
  HiLink wgetComment            Comment
  HiLink wgetNumber             Number
  HiLink wgetQuota              Number
  HiLink wgetString             String
  HiLink wgetTodo               Todo
  HiLink wgetValue              Constant

  delcommand HiLink
endif

let b:current_syntax = "wget"
