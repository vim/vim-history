" Vim syntax file
" Language:	php3 PHP 3.0
" Maintainer:	Lutz Eymers <ixtab@polzin.com>
" URL:		http://www-public.rz.uni-duesseldorf.de/~eymers/vim/syntax
" Email:        Subject: send syntax_vim.tgz
" Last change:	1999 Jun 14
"
" Options       php3_sql_query = 1 for SQL syntax highligthing inside strings
"               php3_minlines = x     to sync at least x lines backwards

" Remove any old syntax stuff hanging around
syn clear


if !exists("main_syntax")
  let main_syntax = 'php3'
endif

so <sfile>:p:h/html.vim
syn cluster htmlPreproc add=php3RegionInsideHtmlTags

if exists( "php3_sql_query")
  if php3_sql_query == 1
    syn include @php3Sql <sfile>:p:h/sql.vim
  endif
endif
syn cluster php3Sql remove=sqlString,sqlComment

syn case match

" Env Variables
syn keyword php3EnvVar SERVER_SOFTWARE SERVER_NAME SERVER_URL GATEWAY_INTERFACE   contained
syn keyword php3EnvVar SERVER_PROTOCOL SERVER_PORT REQUEST_METHOD PATH_INFO  contained
syn keyword php3EnvVar PATH_TRANSLATED SCRIPT_NAME QUERY_STRING REMOTE_HOST contained
syn keyword php3EnvVar REMOTE_ADDR AUTH_TYPE REMOTE_USER CONTEN_TYPE  contained
syn keyword php3EnvVar CONTENT_LENGTH HTTPS HTTPS_KEYSIZE HTTPS_SECRETKEYSIZE  contained
syn keyword php3EnvVar HTTP_ACCECT HTTP_USER_AGENT HTTP_IF_MODIFIED_SINCE  contained
syn keyword php3EnvVar HTTP_FROM HTTP_REFERER contained

" Internal Variables
syn keyword php3IntVar GLOBALS contained
syn case ignore
syn keyword php3IntVar GLOBALS php_errmsg php_self contained

" Comment
syn region php3Comment		start="/\*" skip="?>" end="\*/"  contained contains=php3Todo
syn match php3Comment		"#.*$"  contained contains=php3Todo
syn match php3Comment		"//.*$"  contained contains=php3Todo

" Function names
syn keyword php3Functions  Abs contained
syn keyword php3Functions  ada_afetch ada_autocommit ada_close ada_commit contained
syn keyword php3Functions  ada_connect ada_exec ada_fetchrow ada_fieldname contained
syn keyword php3Functions  ada_fieldnum ada_fieldtype ada_freeresult ada_numfields contained
syn keyword php3Functions  ada_numrows ada_result ada_resultall ada_rollback contained
syn keyword php3Functions  array arsort asort count current each end key ksort contained
syn keyword php3Functions  list next pos prev reset rsort sizeof sort contained
syn keyword php3Functions  bcadd bccomp bcdiv bcmod bcmul bcpow bcscale bcsqrt contained
syn keyword php3Functions  bcsub contained
syn keyword php3Functions  JDToGregorian GregorianToJD JDToJulian JulianToJD contained
syn keyword php3Functions  JDToJewish JewishToJD JDToFrench FrenchToJD contained
syn keyword php3Functions  JDMonthName JDDayOfWeek contained
syn keyword php3Functions  checkdate date getdate gmdate mktime time microtime contained
syn keyword php3Functions  set_time_limit contained
syn keyword php3Functions  dbase_create dbase_open dbase_close dbase_pack contained
syn keyword php3Functions  dbase_add_record dbase_delete_record dbase_get_record contained
syn keyword php3Functions  dbase_numfields dbase_numrecords contained
syn keyword php3Functions  dbmopen dbmclose dbmexists dbmfetch dbminsert contained
syn keyword php3Functions  dbmreplace dbmdelete dbmfirstkey dbmnextkey dbmlist contained
syn keyword php3Functions  chdir dir closedir opendir readdir rewindir contained
syn keyword php3Functions  dl contained
syn keyword php3Functions  escapeshellcmd exec system passthru virtual contained
syn keyword php3Functions  filepro filepro_fieldname filepro_fieldtype contained
syn keyword php3Functions  filepro_fieldwidth filepro_retrieve contained
syn keyword php3Functions  filepro_fieldcount filepro_rowcount contained
syn keyword php3Functions  basename chgrp chmod chown clearstatcache copy contained
syn keyword php3Functions  dirname fclose feof fgetc fgets fegtss file contained
syn keyword php3Functions  file_exists fileatume filectime filegroup fileinode contained
syn keyword php3Functions  filemtime fileowner fileperms filesize filetype contained
syn keyword php3Functions  fileumask fopen fpassthru fputs fseek ftell contained
syn keyword php3Functions  is_dir is_executable is_file is_link is_readable contained
syn keyword php3Functions  is_writeable link linkinfo mkdir pclose popen contained
syn keyword php3Functions  readfile readlink rename rewind rmdir stat contained
syn keyword php3Functions  symlink tempnam touch umask unlink contained
syn keyword php3Functions  getallheaders header setcookie contained
syn keyword php3Functions  GetImageSize ImageArc ImageChar ImageCharUp contained
syn keyword php3Functions  ImageColorAllocate ImageColorTransparent  contained
syn keyword php3Functions  ImageCopyResized ImageCreate ImageCreateFromGif contained
syn keyword php3Functions  ImageDashedLine ImageDestroy ImageFill contained
syn keyword php3Functions  ImageFilledPolygon ImageFilledRectangle contained
syn keyword php3Functions  ImageFillToBorder ImageFontHeight ImageFontWidth contained
syn keyword php3Functions  ImageGif ImageInterlace ImageLine ImageLoadFont contained
syn keyword php3Functions  ImagePolygon ImageRectangle ImageSetPixel contained
syn keyword php3Functions  ImageString ImageStringUp ImageSX ImageSY contained
syn keyword php3Functions  ImageTTFText ImageColorAt ImageColorClosest contained
syn keyword php3Functions  ImageColorExact ImageColorSet ImageColorsForIndex contained
syn keyword php3Functions  ImageColorsTotal contained
syn keyword php3Functions  imap_append imap_base64 imap_body imap_check contained
syn keyword php3Functions  imap_close imap_createmailbox imap_delete contained
syn keyword php3Functions  imap_deletemailbox imap_expunge imap_fetchbody contained
syn keyword php3Functions  imap_fetchstructure imap_header imap_headerinfo contained
syn keyword php3Functions  imap_headers imap_listmailbox imap_listsubscribed contained
syn keyword php3Functions  imap_mail_copy imap_mail_move imap_num_msg contained
syn keyword php3Functions  imap_num_recent imap_open imap_ping contained
syn keyword php3Functions  imap_renamemailbox map_reopen imap_subscribe contained
syn keyword php3Functions  imap_undelete imap_unsubscribe imap_qprint contained
syn keyword php3Functions  imap_8bit contained
syn keyword php3Functions  error_log error_reporting getenv get_cfg_var contained
syn keyword php3Functions  get_current_user getlastmod getmyinode getmypid contained
syn keyword php3Functions  getmyuid phpinfo phpversion putenv contained
syn keyword php3Functions  ldap_add ldap_bind ldap_close ldap_connect contained
syn keyword php3Functions  ldap_count_entries ldap_delete ldap_dn2ufn contained
syn keyword php3Functions  ldap_first_attribute ldap_first_entry contained
syn keyword php3Functions  ldap_free_entry ldap_free_result ldap_get_attributes contained
syn keyword php3Functions  ldap_get_dn ldap_get_entries ldap_get_values contained
syn keyword php3Functions  ldap_list ldap_modify ldap_next_attribute contained
syn keyword php3Functions  ldap_next_entry ldap_read ldap_search contained
syn keyword php3Functions  ldap_unbind contained
syn keyword php3Functions  mail contained
syn keyword php3Functions  Abs Acos Asin Atan BinDec Ceil Cos DecBin DecHex contained
syn keyword php3Functions  DecOct Exp Floor getrandmax HexDec Log Log10 contained
syn keyword php3Functions  max min OctDec pi pow rand round Sin Sqrt srand contained
syn keyword php3Functions  Tan contained
syn keyword php3Functions  sleep usleep uniqid leak contained
syn keyword php3Functions  msql msql_close msql_connect msql_create_db contained
syn keyword php3Functions  msql_createdb msql_data_seek msql_dbname contained
syn keyword php3Functions  msql_drop_db msql_dropdb msql_error contained
syn keyword php3Functions  msql_fetch_array msql_fetch_field msql_fetch_object contained
syn keyword php3Functions  msql_fetch_row msql_fieldname msql_field_seek contained
syn keyword php3Functions  msql_fieldtable msql_fieldtype msql_fieldflags contained
syn keyword php3Functions  msql_fieldlen msql_free_result msql_freeresult contained
syn keyword php3Functions  msql_list_fields msql_listfields msql_list_dbs contained
syn keyword php3Functions  msql_listdbs msql_list_tables msql_listtables contained
syn keyword php3Functions  msql_num_fields msql_num_rows msql_numfields contained
syn keyword php3Functions  msql_numrows msql_pconnect msql_query contained
syn keyword php3Functions  msql_regcase msql_result msql_select_db contained
syn keyword php3Functions  msql_selectdb msql_tablename contained
syn keyword php3Functions  mysql_affected_rows mysql_close mysql_connect contained
syn keyword php3Functions  mysql_create_db mysql_data_seek mysql_dbname contained
syn keyword php3Functions  mysql_db_query mysql_drop_db mysql_errno contained
syn keyword php3Functions  mysql_error mysql_fetch_array mysql_fetch_field contained
syn keyword php3Functions  mysql_fetch_lengths mysql_fetch_object contained
syn keyword php3Functions  mysql_fetch_row mysql_field_name mysql_field_seek contained
syn keyword php3Functions  mysql_field_table mysql_field_type mysql_field_flags contained
syn keyword php3Functions  mysql_field_len mysql_free_result mysql_insert_id contained
syn keyword php3Functions  mysql_list_dbs mysql_list_tables mysql_num_fields contained
syn keyword php3Functions  mysql_num_rows mysql_pconnect mysql_query contained
syn keyword php3Functions  mysql_result mysql_select_db mysql_tablename contained
syn keyword php3Functions  sybase_close sybase_connect sybase_data_seek contained
syn keyword php3Functions  sybase_fetch_array sybase_fetch_field contained
syn keyword php3Functions  sybase_fetch_object sybase_fetch_row sybase_field_seek contained
syn keyword php3Functions  sybase_num_fields sybase_num_rows sybase_pconnect contained
syn keyword php3Functions  sybase_query sybase_result sybase_select_db contained
syn keyword php3Functions  fsockopen gethostbyaddr gethostbyname openlog contained
syn keyword php3Functions  syslog closelog debugger_on debugger_off contained
syn keyword php3Functions  odbc_autocommit odbc_binmode odbc_close contained
syn keyword php3Functions  odbc_close_all odbc_commit odbc_connect contained
syn keyword php3Functions  odbc_cursor odbc_do odbc_exec odbc_execute contained
syn keyword php3Functions  odbc_fetch_into odbc_fetch_row odbc_field_name contained
syn keyword php3Functions  odbc_field_num odbc_field_type odbc_free_result contained
syn keyword php3Functions  odbc_longreadlen odbc_num_fields odbc_pconnect contained
syn keyword php3Functions  odbc_prepare odbc_num_rows odbc_result contained
syn keyword php3Functions  odbc_result_all odbc_rollback contained
syn keyword php3Functions  Ora_Close Ora_ColumnName Ora_ColumnType contained
syn keyword php3Functions  Ora_Commit Ora_CommitOff Ora_CommitOn Ora_Error contained
syn keyword php3Functions  Ora_ErrorCode Ora_Exec Ora_Fetch Ora_GetColumn contained
syn keyword php3Functions  Ora_Logoff Ora_Logon Ora_Open Ora_Parse contained
syn keyword php3Functions  Ora_Rollback contained
syn keyword php3Functions  Ora_Close Ora_ColumnName Ora_ColumnType Ora_Commit contained
syn keyword php3Functions  Ora_CommitOff Ora_CommitOn Ora_Error Ora_ErrorCode contained
syn keyword php3Functions  Ora_Exec Ora_Fetch Ora_GetColumn Ora_Logoff contained
syn keyword php3Functions  Ora_Logon Ora_Open Ora_Parse Ora_Rollback contained
syn keyword php3Functions  pg_Close pg_cmdTuples pg_Connect pg_DBname contained
syn keyword php3Functions  pg_Fetch_Array pg_Fetch_Object pg_Fetch_Row contained
syn keyword php3Functions  pg_ErrorMessage pg_Exec pg_FieldIsNull pg_FieldName contained
syn keyword php3Functions  pg_FieldNum pg_FieldPrtLen pg_FieldSize contained
syn keyword php3Functions  pg_FreeResult pg_GetLastOid pg_Host pg_loclose contained
syn keyword php3Functions  pg_locreate pg_loopen pg_loread pg_loreadall contained
syn keyword php3Functions  pg_lounlink pg_lowrite pg_NumFields pg_NumRows contained
syn keyword php3Functions  pg_Options pg_pConnect pg_Port pg_Result contained
syn keyword php3Functions  pg_tty contained
syn keyword php3Functions  ereg ereg_replace eregi eregi_replace split contained
syn keyword php3Functions  sql_regcase contained
syn keyword php3Functions  solid_close solid_connect solid_exec contained
syn keyword php3Functions  solid_fetchrow solid_fieldname solid_fieldnum contained
syn keyword php3Functions  solid_freeresult solid_numfields solid_numrows contained
syn keyword php3Functions  solid_result contained
syn keyword php3Functions  snmpget snmpwalk contained
syn keyword php3Functions  AddSlashes Chop Chr crypt echo explode flush contained
syn keyword php3Functions  htmlspecialchars htmlentities implode join contained
syn keyword php3Functions  nl2br Ord print printf QuoteMeta rawurldecode contained
syn keyword php3Functions  rawurlencode setlocale sprintf strchr StripSlashes contained
syn keyword php3Functions  strlen strtok strrchr strrev strstr contained
syn keyword php3Functions  strtolower strtoupper strtr substr ucfirst contained
syn keyword php3Functions  md5 soundex parse_str contained
syn keyword php3Functions  parse_url urldecode urlencode base64_encode contained
syn keyword php3Functions  base64_decode contained
syn keyword php3Functions  gettype intval doubleval strval is_array contained
syn keyword php3Functions  is_double is_integer is_long is_object is_real contained
syn keyword php3Functions  is_string isset settype empty eval contained
syn keyword php3Functions  die contained

" Identifier
syn match  php3Identifier "$\{1,}[a-zA-Z_][a-zA-Z0-9_]*" contained contains=php3EnvVar,php3IntVar
syn match  php3Identifier "${"me=e-1,he=e-1 contained

" Conditional
syn keyword php3Conditional  if else elseif endif switch endswitch contained

" Repeat
syn keyword php3Repeat  do for while endwhile contained

" Repeat
syn keyword php3Label  case default switch  contained

" Statement
syn keyword php3Statement  break return continue exit contained

" Keyword
syn keyword php3Keyword var contained

" Structure
syn keyword php3Structure class extends contained

" StorageClass
syn keyword php3StorageClass global static contained

" Operator
syn match php3Operator  "[-=+%^&|*!.~?:]" contained
syn match php3Operator  "[-+*/%^&|.]=" contained
syn match php3Operator  "/[^*/]"me=e-1 contained
syn match php3Operator  "/$" contained
syn match php3Operator  "&&\|\<and\>" contained
syn match php3Operator  "||\|\<x\=or\>" contained
syn match php3Relation  "[!=<>]=" contained
syn match php3Relation  "[<>]" contained

" Include
syn keyword php3Include  include require contained

" Define
syn keyword php3Define  Function cfunction new contained

" Boolean
syn keyword php3Boolean true false contained

" String
syn region php3StringDouble keepend matchgroup=None start=+"+ skip=+\\\\\|\\"+  end=+"+ contains=php3Identifier,php3SpecialChar,@php3Sql contained
syn region php3StringSingle keepend matchgroup=None start=+'+ skip=+\\\\\|\\'+  end=+'+ contains=php3SpecialChar,@php3Sql contained

" Number
syn match php3Number  "-\=\<\d\+\>" contained

" Float
syn match php3Float  "\(-\=\<\d+\|-\=\)\.\d\+\>" contained

" SpecialChar
syn match php3SpecialChar "\\[abcfnrtyv\\]" contained
syn match php3SpecialChar "\\\d\{3}" contained contains=php3OctalError
syn match php3SpecialChar "\\x[0-9a-fA-F]\{2}" contained

" Error
syn match php3OctalError "[89]" contained
syn match php3ParentError "[)}\]]" contained

" Todo
syn keyword php3Todo TODO Todo todo contained

" Parents
syn cluster php3Inside contains=php3Comment,php3Functions,php3Identifier,php3Conditional,php3Repeat,php3Label,php3Statement,php3Operator,php3Relation,php3StringSingle,php3StringDouble,php3Number,php3Float,php3SpecialChar,php3Parent,php3ParentError,php3Include,php3Keyword,php3IdentifierParent,php3Boolean,php3Structure

syn cluster php3Top contains=@php3Inside,php3Define,php3ParentError,php3StorageClass

syn region php3Parent	matchgroup=Delimiter start="(" end=")" contained contains=@php3Inside
syn region php3Parent	matchgroup=Delimiter start="{" end="}" contained contains=@php3Top
syn region php3Parent	matchgroup=Delimiter start="\[" end="\]" contained contains=@php3Inside

syn region php3Region keepend matchgroup=Delimiter start="<?\(php\)\=" skip=+".\{-}?>.\{-}"\|'.\{-}?>.\{-}'\|/\*.\{-}?>.\{-}\*/+ end="?>" contains=@php3Top
syn region php3Region matchgroup=Delimiter start="<?\(php\)\=" end="?>" contains=@php3Top contained
syn region php3RegionInsideHtmlTags keepend matchgroup=Delimiter start="<?\(php\)\=" skip=+".\{-}?>.\{-}"\|'.\{-}?>.\{-}'\|/\*.\{-}?>.\{-}\*/+ end="?>" contains=@php3Top contained

syn region php3Region keepend matchgroup=Delimiter start=+<script language="php">+ skip=+".\{-}</script>.\{-}"\|'.\{-}</script>.\{-}'\|/\*.\{-}</script>.\{-}\*/+ end=+</script>+ contains=@php3Top
syn region php3Region matchgroup=Delimiter start=+<script language="php">+ end=+</script>+ contains=@php3Top contained
syn region php3RegionInsideHtmlTags keepend matchgroup=Delimiter start=+<script language="php">+ skip=+".\{-}</script>.\{-}"\|'.\{-}</script>.\{-}'\|/\*.\{-}</script>.\{-}\*/+ end=+</script>+ contains=@php3Top contained

" sync
if exists("php3_minlines")
  exec "syn sync minlines=" . php3_minlines
endif

if !exists("did_php3_syntax_inits")
  let did_php3_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link php3Comment                   Comment
  hi link php3Boolean                   Boolean
  hi link php3StorageClass              StorageClass
  hi link php3Structure                 Structure
  hi link php3StringSingle              String
  hi link php3StringDouble              String
  hi link php3Number                    Number
  hi link php3Float                     Float
  hi php3Identifier guifg=DarkGray ctermfg=Brown
  hi link php3Functions                 Function
  hi link php3Repeat                    Repeat
  hi link php3Conditional               Conditional
  hi link php3Label                     Label
  hi link php3Statement                 Statement
  hi link php3Keyword                   Statement
  hi link php3Type                      Type
  hi link php3Include                   Include
  hi link php3Define                    Define
  hi link php3SpecialChar               SpecialChar
  hi link php3ParentError	         Error
  hi link php3OctalError	         Error
  hi link php3Todo                      Todo
  hi php3Relation guifg=SeaGreen ctermfg=DarkGreen
  hi php3Operator guifg=SeaGreen ctermfg=DarkGreen
  hi php3IntVar guifg=Red ctermfg=DarkRed
  hi php3EnvVar guifg=Red ctermfg=DarkRed
endif

let b:current_syntax = "php3"

if main_syntax == 'php3'
  unlet main_syntax
endif

" vim: ts=8
