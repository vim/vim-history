" Vim syntax file
" Language:     Apache httpd.conf, srm.conf, access.conf
" Maintainer:   Allan Kelly <allan@fruitloaf.co.uk>
" Version:      5.6(synched), Sat Jan 22 14:28:42 GMT 2000
"               5.6.1(minor revisions) Mon Jan 24 08:43:34 GMT 2000
syn clear

syn match      apacheComment       "^\s*#.*$"
syn keyword apachedeclaration ResourceConfig AccessConfig LoadModule
syn keyword apachedeclaration ClearModuleList AddModule DocumentRoot
syn keyword apachedeclaration UserDir DirectoryIndex FancyIndexing
syn keyword apachedeclaration AddIconByEncoding AddIconByType AddIcon
syn keyword apachedeclaration DefaultIcon ReadmeName HeaderName IndexIgnore
syn keyword apachedeclaration AccessFileName TypesConfig DefaultType AddEncoding 
syn keyword apachedeclaration AddLanguage LanguagePriority Alias ScriptAlias 
syn keyword apachedeclaration AddType AddHandler BrowserMatch Options
syn keyword apachedeclaration AllowOverride SetHandler ErrorDocument ServerType 
syn keyword apachedeclaration Port HostnameLookups User Group ServerAdmin ServerRoot 
syn keyword apachedeclaration ServerRoot Listen ErrorLog LogLevel LogFormat 
syn keyword apachedeclaration CustomLog PidFile ScoreBoardFile LockFile 
syn keyword apachedeclaration ServerName UseCanonicalName Timeout KeepAlive 
syn keyword apachedeclaration MaxKeepAliveRequests KeepAliveTimeout 
syn keyword apachedeclaration MinSpareServers MaxSpareServers StartServers 
syn keyword apachedeclaration MaxClients MaxRequestsPerChild ProxyRequests 
syn keyword apachedeclaration CacheRoot CacheSize CacheGcInterval CacheMaxExpire
syn keyword apachedeclaration CacheLastModifiedFactor CacheDefaultExpire NoCache 
syn keyword apachedeclaration Include 

syn keyword apacheOptions Indexes Includes FollowSymLinks None All all
syn keyword apacheOptions ExecCGI MultiViews

syn match apachesection "<\/*Directory.*>$" contains=apachestring
syn match apachesection "<\/*Location.*>$" contains=apachestring
syn match apachestring "\s\+[a-zA-Z0-9./_]\+" contained

syn match apacheallowDeny "allow\s\+from.*$" contains=apacheallowDenyVal
syn match apacheallowDeny "deny\s\+from.*$" contains=apacheallowDenyVal
syn keyword apacheallowDenyVal all none contained

syn match apacheOrder "^order.*$" contains=apacheOrderValue
syn keyword apacheOrderValue deny allow contained

" The default highlighting.
hi def link apachedeclaration       Function
hi def link apacheOptions           Number
hi def link apacheComment           Comment
hi def link apachesection           Label
hi def link apacheallowDeny         Include
hi def link apacheallowDenyVal      Identifier
hi def link apacheOrder             Special
hi def link apacheOrderValue        String
hi def link apachestring            Number

let b:current_syntax = "apache"
