" Vim syntax file
" Language: Apache httpd.conf, srm.conf, access.conf, .htaccess
" Maintainer: David Necas (Yeti) <yeti@physics.muni.cz>
" Last Change: 2001 Feb 04
"              (synchronized with Apache 1.3.14 documentation)
" URL: http://physics.muni.cz/~yeti/download/apache.vim
" TODO: see particular FIXME's scattered thorough the file

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" SECTION: basic setup
syn case ignore
set iskeyword=a-z,A-Z,48-57,-,+,_
syn match   apacheComment       "^\s*#.*$"
syn match apacheAnything "\s[^>]*" contained
syn match apacheError "\w\+" contained
syn region apacheString start=+"+ end=+"+ skip=+\\\"+
syn sync ccomment

" SECTION: core
syn keyword apacheDeclaration AccessConfig AccessFileName AddDefaultCharset AddModule AuthName AuthType BindAddress BS2000Account ClearModuleList ContentDigest CoreDumpDirectory DefaultType DocumentRoot ErrorDocument ErrorLog Group HostNameLookups IdentityCheck Include KeepAlive KeepAliveTimeout LimitRequestBody LimitRequestFields LimitRequestFieldsize LimitRequestLine Listen ListenBacklog LockFile LogLevel MaxClients MaxKeepAliveRequests MaxRequestsPerChild MaxSpareServers MinSpareServers NameVirtualHost Options PidFile Port require ResourceConfig RLimitCPU RLimitMEM RLimitNPROC Satisfy ScoreBoardFile ScriptInterpreterSource SendBufferSize ServerAdmin ServerAlias ServerName ServerPath ServerRoot ServerSignature ServerTokens ServerType StartServers ThreadsPerChild ThreadStackSize TimeOut UseCanonicalName User
syn keyword apacheOption Any All On Off Double EMail DNS Min Minimal OS Prod ProductOnly Full
syn keyword apacheOption emerg alert crit error warn notice info debug
syn keyword apacheOption registry script inetd standalone
syn keyword apacheOptionOption ExecCGI FollowSymLinks Includes IncludesNoExec Indexes MultiViews SymLinksIfOwnerMatch
syn keyword apacheOptionOption +ExecCGI +FollowSymLinks +Includes +IncludesNoExec +Indexes +MultiViews +SymLinksIfOwnerMatch
syn keyword apacheOptionOption -ExecCGI -FollowSymLinks -Includes -IncludesNoExec -Indexes -MultiViews -SymLinksIfOwnerMatch
syn keyword apacheOption user group valid-user
syn case match
syn keyword apacheMethodOption GET POST PUT DELETE CONNECT OPTIONS TRACE PATCH PROPFIND PROPPATCH MKCOL COPY MOVE LOCK UNLOCK contained
syn case ignore
syn match apacheSection "<\/\=\(Directory\|DirectoryMatch\|Files\|FilesMatch\|IfModule\|IfDefine\|Location\|LocationMatch\|VirtualHost\)\+.*>" contains=apacheAnything
syn match apacheLimitSection "<\/\=\(Limit\|LimitExcept\)\+.*>" contains=apacheLimitSectionKeyword,apacheMethodOption,apacheError
syn keyword apacheLimitSectionKeyword Limit LimitExcept contained
syn match apacheAuthType "AuthType\s.*$" contains=apacheAuthTypeValue
syn keyword apacheAuthTypeValue Basic Digest
syn match apacheAllowOverride "AllowOverride\s.*$" contains=apacheAllowOverrideValue,apacheComment
syn keyword apacheAllowOverrideValue AuthConfig FileInfo Indexes Limit Options contained

" SECTION: modules
" mod_access
syn match apacheAllowDeny "Allow\s\+from.*$" contains=apacheAllowDenyValue,apacheComment
syn match apacheAllowDeny "Deny\s\+from.*$" contains=apacheAllowDenyValue,apacheComment
syn keyword apacheAllowDenyValue All None contained
syn match apacheOrder "^\s*Order\s.*$" contains=apacheOrderValue,apacheComment
syn keyword apacheOrderValue Deny Allow contained
" mod_actions
syn keyword apacheDeclaration Action Script
" mod_alias
syn keyword apacheDeclaration Alias AliasMatch Redirect RedirectMatch RedirectTemp RedirectPermanent ScriptAlias ScriptAliasMatch
syn keyword apacheOption permanent temp seeother gone
" mod_asis (no own directives)
" mod_auth
syn keyword apacheDeclaration AuthGroupFile AuthUserFile AuthAuthoritative
" mod_auth_anon
syn keyword apacheDeclaration Anonymous Anonymous_Authoritative Anonymous_LogEmail Anonymous_MustGiveEmail Anonymous_NoUserID Anonymous_VerifyEmail
" mod_auth_db
syn keyword apacheDeclaration AuthDBGroupFile AuthDBUserFile AuthDBAuthoritative
" mod_auth_dbm
syn keyword apacheDeclaration AuthDBMGroupFile AuthDBMUserFile AuthDBMAuthoritative
" mod_auth_digest
syn keyword apacheDeclaration AuthDigestFile AuthDigestGroupFile AuthDigestQop AuthDigestNonceLifetime AuthDigestNonceFormat AuthDigestNcCheck AuthDigestAlgorithm AuthDigestDomain 
syn keyword apacheOption none auth auth-int MD5 MD5-sess
" mod_autoindex
syn keyword apacheDeclaration AddAlt AddAltByEncoding AddAltByType AddDescription AddIcon AddIconByEncoding AddIconByType DefaultIcon FancyIndexing HeaderName IndexIgnore IndexOptions IndexOrderDefault ReadmeName
syn keyword apacheOption DescriptionWidth FancyIndexing FoldersFirst IconHeight IconsAreLinks IconWidth NameWidth ScanHTMLTitles SuppressColumnSorting SuppressDescription SuppressHTMLPreamble SuppressLastModified SuppressSize
syn keyword apacheOption Ascending Descending Name Date Size Description
" mod_browser
syn keyword apacheDeclaration BrowserMatch BrowserMatchNoCase
" mod_cern_meta
syn keyword apacheDeclaration MetaFiles MetaDir MetaSuffix
" mod_cgi
syn keyword apacheDeclaration ScriptLog ScriptLogLength ScriptLogBuffer
" mod_dav
" FXIME: not implemented---don't know anything about this module
" mod_define
" FXIME: we don't highlight user defined variables at all
syn keyword apacheDeclaration Define
" mod_digest
syn keyword apacheDeclaration AuthDigestFile
" mod_dir
syn keyword apacheDeclaration DirectoryIndex
" mod_env
syn keyword apacheDeclaration PassEnv SetEnv UnsetEnv
" mod_example
syn keyword apacheDeclaration Example
" mod_expires
syn keyword apacheDeclaration ExpiresActive ExpiresByType ExpiresDefault
" mod_headers
syn keyword apacheDeclaration Header
syn keyword apacheOption set unset append add
" mod_imap
syn keyword apacheDeclaration ImapMenu ImapDefault ImapBase
syn keyword apacheOption none formatted semiformatted unformatted
syn keyword apacheOption error nocontent map refer URL
" mod_include
syn keyword apacheDeclaration XBitHack
" mod_info
syn keyword apacheDeclaration AddModuleInfo
" mod_isapi
syn keyword apacheDeclaration ISAPIReadAheadBuffer ISAPILogNotSupported ISAPIAppendLogToErrors ISAPIAppendLogToQuery
" mod_log_agent
syn keyword apacheDeclaration AgentLog
" mod_log_config
syn keyword apacheDeclaration CookieLog CustomLog LogFormat TransferLog
" mod_log_referer
syn keyword apacheDeclaration RefererIgnore RefererLog
" mod_mime
syn keyword apacheDeclaration AddCharset AddEncoding AddHandler AddLanguage AddType DefaultLanguage ForceType RemoveEncoding RemoveHandler RemoveType SetHandler TypesConfig
" mod_mime_magic
syn keyword apacheDeclaration MimeMagicFile
" mod_mmap_static
syn keyword apacheDeclaration MMapFile
" mod_negotiation
syn keyword apacheDeclaration CacheNegotiatedDocs LanguagePriority
" mod_perl
" FXIME: not implemented---for proper highligting the whole Perl thing has to
" be embeded making vim incredibily s...l...o...w...
" mod_proxy
syn keyword apacheDeclaration ProxyRequests ProxyRemote ProxyPass ProxyPassReverse ProxyBlock AllowCONNECT ProxyReceiveBufferSize NoProxy ProxyDomain ProxyVia CacheRoot CacheSize CacheMaxExpire CacheDefaultExpire CacheLastModifiedFactor CacheGcInterval CacheDirLevels CacheDirLength CacheForceCompletion NoCache
syn keyword apacheOption block
" mod_put
" FXIME: not implemented---need some doc
" mod_rewrite
" FIXME: rewriting rules are not highlighted because I don't understand them...
syn keyword apacheDeclaration RewriteEngine RewriteOptions RewriteLog RewriteLogLevel RewriteLock RewriteMap RewriteBase RewriteCond RewriteRule
syn keyword apacheOption inherit
" mod_roaming
syn keyword apacheDeclaration RoamingAlias
" mod_setenvif
syn keyword apacheDeclaration BrowserMatch BrowserMatchNoCase SetEnvIf SetEnvIfNoCase
" mod_so
syn keyword apacheDeclaration LoadFile LoadModule
" mod_speling
syn keyword apacheDeclaration CheckSpelling
" mod_ssl
" FIXME: SSLCipherSuite parameters are not properly highlighted
" FIXME: SSLRequire rules are not highlighted
syn keyword apacheDeclaration SSLPassPhraseDialog SSLMutex SSLRandomSeed SSLSessionCache SSLSessionCacheTimeout SSLEngine SSLProtocol SSLCipherSuite SSLCertificateFile SSLCertificateKeyFile SSLCertificateChainFile SSLCACertificatePath SSLCACertificateFile SSLCARevocationPath SSLCARevocationFile SSLVerifyClient SSLVerifyDepth SSLLog SSLLogLevel SSLOptions SSLRequireSSL SSLRequire
syn keyword apacheOption StdEnvVars CompatEnvVars ExportCertData FakeBasicAuth StrictRequire OptRenegotiate
syn keyword apacheOption +StdEnvVars +CompatEnvVars +ExportCertData +FakeBasicAuth +StrictRequire +OptRenegotiate
syn keyword apacheOption -StdEnvVars -CompatEnvVars -ExportCertData -FakeBasicAuth -StrictRequire -OptRenegotiate
syn keyword apacheOption builtin sem
syn match apacheOption "\(file\|exec\|egd\|dbm\|shm\):"
syn keyword apacheOption SSLv2 SSLv3 TLSv1 -SSLv2 -SSLv3 -TLSv1 +SSLv2 +SSLv3 +TLSv1
syn keyword apacheOption optional require optional_no_ca
" mod_status
syn keyword apacheDeclaration ExtendedStatus
" mod_userdir
syn keyword apacheDeclaration UserDir
" mod_usertrack
syn keyword apacheDeclaration CookieExpires CookieName CookieTracking
" mod_vhost_alias
syn keyword apacheDeclaration VirtualDocumentRoot VirtualDocumentRootIP VirtualScriptAlias VirtualScriptAliasIP 

" SECTION: links
" the default highlighting.
hi def link apacheAllowOverride		apacheDeclaration
hi def link apacheAllowOverrideValue	apacheOption
hi def link apacheAuthType		apacheDeclaration
hi def link apacheAuthTypeValue		apacheOption
hi def link apacheOptionOption		apacheOption
hi def link apacheDeclaration		Function
hi def link apacheAnything		apacheOption
hi def link apacheOption		Number
hi def link apacheComment		Comment
hi def link apacheLimitSectionKeyword	apacheLimitSection
hi def link apacheLimitSection		apacheSection
hi def link apacheSection		Label
hi def link apacheMethodOption		Type
hi def link apacheAllowDeny		Include
hi def link apacheAllowDenyValue	Identifier
hi def link apacheOrder			Special
hi def link apacheOrderValue		String
hi def link apacheString		Number
hi def link apacheError			Error

let b:current_syntax = "apache"
