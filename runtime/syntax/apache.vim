" Vim syntax file
" This is a GENERATED FILE. Please always refer to source file at the URI below.
" Language: Apache configuration (httpd.conf, srm.conf, access.conf, .htaccess)
" Maintainer: David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change: 2002 Mar 03
" URI: http://physics.muni.cz/~yeti/download/apache.vim


" Setup
if version >= 600
  if exists("b:current_syntax")
    finish
  endif
else
  syntax clear
endif

if version >= 600
  command -nargs=1 SetIsk setlocal iskeyword=<args>
else
  command -nargs=1 SetIsk set iskeyword=<args>
endif
SetIsk @,48-57,-,+,_
delcommand SetIsk

syn case ignore

" Base constructs
syn match apacheComment "^\s*#.*$" contains=apacheFixme
syn case match
syn keyword apacheFixme FIXME TODO XXX NOT
syn case ignore
syn match apacheAnything "\s[^>]*" contained
syn match apacheError "\w\+" contained
syn region apacheString start=+"+ end=+"+ skip=+\\\\\|\\\"+

" Core
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

" Modules
syn match apacheAllowDeny "Allow\s\+from.*$" contains=apacheAllowDenyValue,apacheComment
syn match apacheAllowDeny "Deny\s\+from.*$" contains=apacheAllowDenyValue,apacheComment
syn keyword apacheAllowDenyValue All None contained
syn match apacheOrder "^\s*Order\s.*$" contains=apacheOrderValue,apacheComment
syn keyword apacheOrderValue Deny Allow contained
syn keyword apacheDeclaration Action Script
syn keyword apacheDeclaration Alias AliasMatch Redirect RedirectMatch RedirectTemp RedirectPermanent ScriptAlias ScriptAliasMatch
syn keyword apacheOption permanent temp seeother gone
syn keyword apacheDeclaration AuthGroupFile AuthUserFile AuthAuthoritative
syn keyword apacheDeclaration Anonymous Anonymous_Authoritative Anonymous_LogEmail Anonymous_MustGiveEmail Anonymous_NoUserID Anonymous_VerifyEmail
syn keyword apacheDeclaration AuthDBGroupFile AuthDBUserFile AuthDBAuthoritative
syn keyword apacheDeclaration AuthDBMGroupFile AuthDBMUserFile AuthDBMAuthoritative
syn keyword apacheDeclaration AuthDigestFile AuthDigestGroupFile AuthDigestQop AuthDigestNonceLifetime AuthDigestNonceFormat AuthDigestNcCheck AuthDigestAlgorithm AuthDigestDomain
syn keyword apacheOption none auth auth-int MD5 MD5-sess
syn keyword apacheDeclaration AddAlt AddAltByEncoding AddAltByType AddDescription AddIcon AddIconByEncoding AddIconByType DefaultIcon FancyIndexing HeaderName IndexIgnore IndexOptions IndexOrderDefault ReadmeName
syn keyword apacheOption DescriptionWidth FancyIndexing FoldersFirst IconHeight IconsAreLinks IconWidth NameWidth ScanHTMLTitles SuppressColumnSorting SuppressDescription SuppressHTMLPreamble SuppressLastModified SuppressSize
syn keyword apacheOption Ascending Descending Name Date Size Description
syn keyword apacheDeclaration BrowserMatch BrowserMatchNoCase
syn keyword apacheDeclaration MetaFiles MetaDir MetaSuffix
syn keyword apacheDeclaration ScriptLog ScriptLogLength ScriptLogBuffer
syn keyword apacheDeclaration Define
syn keyword apacheDeclaration AuthDigestFile
syn keyword apacheDeclaration DirectoryIndex
syn keyword apacheDeclaration PassEnv SetEnv UnsetEnv
syn keyword apacheDeclaration Example
syn keyword apacheDeclaration ExpiresActive ExpiresByType ExpiresDefault
syn keyword apacheDeclaration Header
syn keyword apacheOption set unset append add
syn keyword apacheDeclaration ImapMenu ImapDefault ImapBase
syn keyword apacheOption none formatted semiformatted unformatted
syn keyword apacheOption nocontent
syn keyword apacheDeclaration XBitHack
syn keyword apacheOption on off full
syn keyword apacheDeclaration AddModuleInfo
syn keyword apacheDeclaration ISAPIReadAheadBuffer ISAPILogNotSupported ISAPIAppendLogToErrors ISAPIAppendLogToQuery
syn keyword apacheDeclaration AgentLog
syn keyword apacheDeclaration CookieLog CustomLog LogFormat TransferLog
syn keyword apacheDeclaration RefererIgnore RefererLog
syn keyword apacheDeclaration AddCharset AddEncoding AddHandler AddLanguage AddType DefaultLanguage ForceType RemoveEncoding RemoveHandler RemoveType SetHandler TypesConfig
syn keyword apacheDeclaration MimeMagicFile
syn keyword apacheDeclaration MMapFile
syn keyword apacheDeclaration CacheNegotiatedDocs LanguagePriority
syn keyword apacheDeclaration PerlModule PerlRequire PerlTaintCheck PerlWarn
syn keyword apacheDeclaration PerlSetVar PerlSetEnv PerlPassEnv PerlSetupEnv
syn keyword apacheDeclaration PerlInitHandler PerlPostReadRequestHandler PerlHeaderParserHandler
syn keyword apacheDeclaration PerlTransHandler PerlAccessHandler PerlAuthenHandler PerlAuthzHandler
syn keyword apacheDeclaration PerlTypeHandler PerlFixupHandler PerlHandler PerlLogHandler
syn keyword apacheDeclaration PerlCleanupHandler PerlChildInitHandler PerlChildExitHandler
syn keyword apacheDeclaration PerlRestartHandler PerlDispatchHandler
syn keyword apacheDeclaration PerlFreshRestart PerlSendHeader
syn keyword apacheDeclaration php_value php_flag php_admin_value php_admin_flag
syn keyword apacheDeclaration ProxyRequests ProxyRemote ProxyPass ProxyPassReverse ProxyBlock AllowCONNECT ProxyReceiveBufferSize NoProxy ProxyDomain ProxyVia CacheRoot CacheSize CacheMaxExpire CacheDefaultExpire CacheLastModifiedFactor CacheGcInterval CacheDirLevels CacheDirLength CacheForceCompletion NoCache
syn keyword apacheOption block
syn keyword apacheDeclaration RewriteEngine RewriteOptions RewriteLog RewriteLogLevel RewriteLock RewriteMap RewriteBase RewriteCond RewriteRule
syn keyword apacheOption inherit
syn keyword apacheDeclaration RoamingAlias
syn keyword apacheDeclaration BrowserMatch BrowserMatchNoCase SetEnvIf SetEnvIfNoCase
syn keyword apacheDeclaration LoadFile LoadModule
syn keyword apacheDeclaration CheckSpelling
syn keyword apacheDeclaration SSLPassPhraseDialog SSLMutex SSLRandomSeed SSLSessionCache SSLSessionCacheTimeout SSLEngine SSLProtocol SSLCipherSuite SSLCertificateFile SSLCertificateKeyFile SSLCertificateChainFile SSLCACertificatePath SSLCACertificateFile SSLCARevocationPath SSLCARevocationFile SSLVerifyClient SSLVerifyDepth SSLLog SSLLogLevel SSLOptions SSLRequireSSL SSLRequire
syn keyword apacheOption StdEnvVars CompatEnvVars ExportCertData FakeBasicAuth StrictRequire OptRenegotiate
syn keyword apacheOption +StdEnvVars +CompatEnvVars +ExportCertData +FakeBasicAuth +StrictRequire +OptRenegotiate
syn keyword apacheOption -StdEnvVars -CompatEnvVars -ExportCertData -FakeBasicAuth -StrictRequire -OptRenegotiate
syn keyword apacheOption builtin sem
syn match apacheOption "\(file\|exec\|egd\|dbm\|shm\):"
syn keyword apacheOption SSLv2 SSLv3 TLSv1 -SSLv2 -SSLv3 -TLSv1 +SSLv2 +SSLv3 +TLSv1
syn keyword apacheOption optional require optional_no_ca
syn keyword apacheDeclaration ExtendedStatus
syn keyword apacheDeclaration UserDir
syn keyword apacheDeclaration CookieExpires CookieName CookieTracking
syn keyword apacheDeclaration VirtualDocumentRoot VirtualDocumentRootIP VirtualScriptAlias VirtualScriptAliasIP

" Define the default highlighting
if version >= 508 || !exists("did_apache_syntax_inits")
  if version < 508
    let did_apache_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink apacheAllowOverride		apacheDeclaration
  HiLink apacheAllowOverrideValue	apacheOption
  HiLink apacheAuthType			apacheDeclaration
  HiLink apacheAuthTypeValue		apacheOption
  HiLink apacheOptionOption		apacheOption
  HiLink apacheDeclaration		Function
  HiLink apacheAnything			apacheOption
  HiLink apacheOption			Number
  HiLink apacheComment			Comment
  HiLink apacheFixme			Todo
  HiLink apacheLimitSectionKeyword	apacheLimitSection
  HiLink apacheLimitSection		apacheSection
  HiLink apacheSection			Label
  HiLink apacheMethodOption		Type
  HiLink apacheAllowDeny		Include
  HiLink apacheAllowDenyValue		Identifier
  HiLink apacheOrder			Special
  HiLink apacheOrderValue		String
  HiLink apacheString			Number
  HiLink apacheError			Error

  delcommand HiLink
endif

let b:current_syntax = "apache"
