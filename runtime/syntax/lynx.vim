" Lynx syntax file
" Filename:     lynx.vim
" Language:     Lynx configuration file ( lynx.cfg )
" Maintainer:   Doug Kearns <djkea2@mugca.cc.monash.edu.au>
" URL:          http://mugca.cc.monash.edu.au/~djkea2/vim/syntax/lynx.vim
" Last Change:  2001 Sep 24

" TODO: more intelligent and complete argument highlighting

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn match   lynxComment    "^#.*$" contains=lynxTodo

syn keyword lynxTodo       TODO NOTE FIXME XXX contained

syn match   lynxDelimiter  ":" contained nextgroup=lynxBoolean,lynxNumber

syn case ignore
syn keyword lynxBoolean    TRUE FALSE contained
syn case match

syn match   lynxNumber     "-\=\<\d\+\>" contained

syn case ignore
syn match   lynxOption "^\s*ACCEPT_ALL_COOKIES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ALERTSECS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ALWAYS_RESUBMIT_POSTS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ALWAYS_TRUSTED_EXEC" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ASSUME_CHARSET" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ASSUMED_COLOR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ASSUMED_DOC_CHARSET_CHOICE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ASSUME_LOCAL_CHARSET" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ASSUME_UNREC_CHARSET" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*AUTO_UNCACHE_DIRLISTS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*BIBP_BIBHOST" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*BIBP_GLOBAL_SERVER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*BLOCK_MULTI_BOOKMARKS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*BOLD_H1" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*BOLD_HEADERS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*BOLD_NAME_ANCHORS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CASE_SENSITIVE_ALWAYS_ON" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CHARACTER_SET" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CHARSETS_DIRECTORY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CHARSET_SWITCH_RULES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CHECKMAIL" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COLLAPSE_BR_TAGS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COLOR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CONNECT_TIMEOUT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_ACCEPT_DOMAINS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_FILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_LOOSE_INVALID_DOMAINS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_QUERY_INVALID_DOMAINS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_REJECT_DOMAINS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_SAVE_FILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*COOKIE_STRICT_INVALID_DOMAINS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CSO_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*CSWING_PATH" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_BOOKMARK_FILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_CACHE_SIZE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_EDITOR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_INDEX_FILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_KEYPAD_MODE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_KEYPAD_MODE_IS_NUMBERS_AS_ARROWS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_USER_MODE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DEFAULT_VIRTUAL_MEMORY_SIZE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DIRED_MENU" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DISPLAY_CHARSET_CHOICE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*DOWNLOADER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*EMACS_KEYS_ALWAYS_ON" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ENABLE_LYNXRC" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*ENABLE_SCROLLBACK" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*EXTERNAL" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FINGER_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FOCUS_WINDOW" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FORCE_8BIT_TOUPPER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FORCE_EMPTY_HREFLESS_A" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FORCE_SSL_COOKIES_SECURE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FORMS_OPTIONS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FTP_PASSIVE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*FTP_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*GLOBAL_EXTENSION_MAP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*GLOBAL_MAILCAP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*GOPHER_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*GOTOBUFFER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HELPFILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HIDDEN_LINK_MARKER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HISTORICAL_COMMENTS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HTMLSRC_ATTRNAME_XFORM" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HTMLSRC_TAGNAME_XFORM" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HTTP_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*HTTPS_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*INCLUDE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*INFOSECS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*JUMPBUFFER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*JUMPFILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*JUMP_PROMPT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*JUSTIFY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*JUSTIFY_MAX_VOID_PERCENT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*KEYBOARD_LAYOUT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*KEYMAP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LEFTARROW_IN_TEXTFIELD_PROMPT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LIST_FORMAT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LIST_NEWS_DATES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LIST_NEWS_NUMBERS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LOCAL_DOMAIN" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LOCAL_EXECUTION_LINKS_ALWAYS_ON" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LOCAL_EXECUTION_LINKS_ON_BUT_NOT_REMOTE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LOCALHOST_ALIAS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LYNXCGI_DOCUMENT_ROOT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LYNXCGI_ENVIRONMENT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LYNX_HOST_NAME" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*LYNX_SIG_FILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MAIL_ADRS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MAIL_SYSTEM_ERROR_LOGGING" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MAKE_LINKS_FOR_ALL_IMAGES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MAKE_PSEUDO_ALTS_FOR_INLINES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MESSAGESECS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MINIMAL_COMMENTS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*MULTI_BOOKMARK_SUPPORT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NCR_IN_BOOKMARKS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NEWS_CHUNK_SIZE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NEWS_MAX_CHUNK" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NEWS_POSTING" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NEWSPOST_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NEWS_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NEWSREPLY_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NNTP_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NNTPSERVER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_DOT_FILES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_FILE_REFERER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_FORCED_CORE_DUMP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_FROM_HEADER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_ISMAP_IF_USEMAP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NONRESTARTING_SIGWINCH" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_REFERER_HEADER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*NO_TABLE_CENTER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*OUTGOING_MAIL_CHARSET" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PARTIAL" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PARTIAL_THRES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PERSISTENT_COOKIES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PERSONAL_EXTENSION_MAP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PERSONAL_MAILCAP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PREFERRED_CHARSET" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PREFERRED_LANGUAGE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PREPEND_BASE_TO_SOURCE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PREPEND_CHARSET_TO_SOURCE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PRETTYSRC" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PRETTYSRC_SPEC" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PRETTYSRC_VIEW_NO_ANCHOR_NUMBERING" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*PRINTER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*QUIT_DEFAULT_YES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*REFERER_WITH_QUERY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*REUSE_TEMPFILES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*RULE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*RULESFILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SAVE_SPACE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SCAN_FOR_BURIED_NEWS_REFS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SCROLLBAR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SCROLLBAR_ARROW" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SEEK_FRAG_AREA_IN_CUR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SEEK_FRAG_MAP_IN_CUR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SET_COOKIES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SHOW_CURSOR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SHOW_KB_RATE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SNEWSPOST_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SNEWS_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SNEWSREPLY_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SOFT_DQUOTES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SOURCE_CACHE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SOURCE_CACHE_FOR_ABORTED" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*STARTFILE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*STRIP_DOTDOT_URLS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SUBSTITUTE_UNDERSCORES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SUFFIX" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SUFFIX_ORDER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SYSTEM_EDITOR" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SYSTEM_MAIL" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*SYSTEM_MAIL_FLAGS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*TAGSOUP" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*TEXTFIELDS_NEED_ACTIVATION" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*TIMEOUT" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*TRIM_INPUT_FIELDS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*TRUSTED_EXEC" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*TRUSTED_LYNXCGI" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*UPLOADER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*URL_DOMAIN_PREFIXES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*URL_DOMAIN_SUFFIXES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*USE_FIXED_RECORDS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*USE_MOUSE" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*USE_SELECT_POPUPS" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*VERBOSE_IMAGES" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*VIEWER" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*VI_KEYS_ALWAYS_ON" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*WAIS_PROXY" nextgroup=lynxDelimiter
syn match   lynxOption "^\s*XLOADIMAGE_COMMAND" nextgroup=lynxDelimiter
syn case match

" NOTE: set this if you want the cfg2html.pl formatting directives to be highlighted
if exists("lynx_formatting_directives")
  syn match lynxFormatDir  "^\.\(h1\|h2\)\s.*$"
  syn match lynxFormatDir  "^\.\(ex\|nf\)\(\s\+\d\+\)\=$"
  syn match lynxFormatDir  "^\.fi$"
endif

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_lynx_syn_inits")
  if version < 508
    let did_lynx_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink lynxBoolean    Boolean
  HiLink lynxComment    Comment
  HiLink lynxDelimiter  Special
  HiLink lynxFormatDir  Special
  HiLink lynxNumber     Number
  HiLink lynxOption     Identifier
  HiLink lynxTodo       Todo

  delcommand HiLink
endif

let b:current_syntax = "lynx"
