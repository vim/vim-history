" Vim syntax file
" Language:	php PHP 3/4
" Maintainer:	Lutz Eymers <ixtab@polzin.com>
" URL:		http://www-public.rz.uni-duesseldorf.de/~eymers/vim/syntax
" Email:	Subject: send syntax_vim.tgz
" Last Change:	2000 Dec 17
"
" Options	php_sql_query = 1  for SQL syntax highligthing inside strings
"		php_minlines = x  to sync at least x lines backwards
"		php_baselib = 1  for highlighting baselib functions
"		php_parentError = 1  for highligthing parent error
"		php_oldStyle = 1  for using old colorstyle

" Remove any old syntax stuff hanging around
syn clear


if !exists("main_syntax")
  let main_syntax = 'php'
endif

runtime syntax/html.vim
syn cluster htmlPreproc add=phpRegion

if exists( "php_sql_query")
  syn include @phpSql <sfile>:p:h/sql.vim
endif
syn cluster phpSql remove=sqlString,sqlComment

syn case match

" Env Variables
syn keyword	phpEnvVar	SERVER_SOFTWARE SERVER_NAME SERVER_URL GATEWAY_INTERFACE	contained
syn keyword	phpEnvVar	SERVER_PROTOCOL SERVER_PORT REQUEST_METHOD PATH_INFO	contained
syn keyword	phpEnvVar	PATH_TRANSLATED SCRIPT_NAME QUERY_STRING REMOTE_HOST	contained
syn keyword	phpEnvVar	REMOTE_ADDR AUTH_TYPE REMOTE_USER CONTEN_TYPE	contained
syn keyword	phpEnvVar	CONTENT_LENGTH HTTPS HTTPS_KEYSIZE HTTPS_SECRETKEYSIZE	contained
syn keyword	phpEnvVar	HTTP_ACCECT HTTP_USER_AGENT HTTP_IF_MODIFIED_SINCE	contained
syn keyword	phpEnvVar	HTTP_FROM HTTP_REFERER	contained
syn keyword	phpEnvVar	PHP_SELF PHP_AUTH_TYPE PHP_AUTH_USER PHP_AUTH_PW	contained

" Internal Variables
syn keyword	phpIntVar GLOBALS HTTP_GET_VARS HTTP_POST_VARS HTTP_COOKIE_VARS	contained
syn keyword	phpIntVar PHP_ERRMSG PHP_SELF	contained

syn case ignore

" Comment
syn region	phpComment	start="/\*" skip="?>" end="\*/"	contained contains=phpTodo,phpNoEnd
syn match	phpComment	"#.*$"	contained contains=phpTodo,phpNoEnd
syn match	phpComment	"//.*$"	contained contains=phpTodo,phpNoEnd

" Function names
syn keyword	phpFunctions  require include	contained
syn keyword	phpFunctions  apache_lookup_uri apache_note getallheaders virtual	contained
syn keyword	phpFunctions  bcadd bccomp bcdiv bcmod bcmul bcpow bcscale bcsqrt bcsub	contained
syn keyword	phpFunctions  array array_count_values array_flip array_keys array_merge array_pad array_pop array_push array_reverse array_shift array_slice array_splice array_unshift array_values array_walk arsort asort compact count current each end extract in_array key krsort ksort list next pos prev range reset rsort shuffle sizeof sort uasort uksort usort	contained
syn keyword	phpFunctions  aspell_new aspell_check aspell_check_raw aspell_suggest	contained
syn keyword	phpFunctions  jdtogregorian gregoriantojd jdtojulian juliantojd jdtojewish jewishtojd jdtofrench frenchtojd jdmonthname jddayofweek easter_date easter_days unixtojd jdtounix	contained
syn keyword	phpFunctions  com_load com_invoke com_propget com_get com_propput com_propset com_set	contained
syn keyword	phpFunctions  get_class_methods get_class_vars get_object_vars method_exists	contained
syn keyword	phpFunctions  cpdf_global_set_document_limits cpdf_set_creator cpdf_set_title cpdf_set_subject cpdf_set_keywords cpdf_open cpdf_close cpdf_page_init cpdf_finalize_page cpdf_finalize cpdf_output_buffer cpdf_save_to_file cpdf_set_current_page cpdf_begin_text cpdf_end_text cpdf_show cpdf_show_xy cpdf_text cpdf_set_font cpdf_set_leading cpdf_set_text_rendering cpdf_set_horiz_scaling cpdf_set_text_rise cpdf_set_text_matrix cpdf_set_text_pos cpdf_set_char_spacing cpdf_set_word_spacing cpdf_continue_text cpdf_stringwidth cpdf_save cpdf_restore cpdf_translate cpdf_scale cpdf_rotate cpdf_setflat cpdf_setlinejoin cpdf_setlinecap cpdf_setmiterlimit cpdf_setlinewidth cpdf_setdash cpdf_newpath cpdf_moveto cpdf_rmoveto cpdf_curveto cpdf_lineto cpdf_rlineto cpdf_circle cpdf_arc cpdf_rect cpdf_closepath cpdf_stroke cpdf_closepath_stroke cpdf_fill cpdf_fill_stroke cpdf_closepath_fill_stroke cpdf_clip cpdf_setgray_fill cpdf_setgray_stroke cpdf_setgray cpdf_setrgbcolor_fill cpdf_setrgbcolor_stroke cpdf_setrgbcolor cpdf_add_outline cpdf_set_page_animation cpdf_import_jpeg cpdf_place_inline_image cpdf_add_annotation	contained
syn keyword	phpFunctions  cybercash_encr cybercash_decr cybercash_base64_encode cybercash_base64_decode	contained
syn keyword	phpFunctions  xmldoc xmldocfile xmltree	contained
syn keyword	phpFunctions  gzclose gzeof gzfile gzgetc gzgets gzgetss gzopen gzpassthru gzputs gzread gzrewind gzseek gztell gzwrite readgzfile gzcompress gzuncompress	contained
syn keyword	phpFunctions  dba_close dba_delete dba_exists dba_fetch dba_firstkey dba_insert dba_nextkey dba_popen dba_open dba_optimize dba_replace dba_sync	contained
syn keyword	phpFunctions  checkdate date getdate gettimeofday gmdate gmmktime gmstrftime localtime microtime mktime strftime time strtotime	contained
syn keyword	phpFunctions  dbase_create dbase_open dbase_close dbase_pack dbase_add_record dbase_replace_record dbase_delete_record dbase_get_record dbase_get_record_with_names dbase_numfields dbase_numrecords	contained
syn keyword	phpFunctions  dbmopen dbmclose dbmexists dbmfetch dbminsert dbmreplace dbmdelete dbmfirstkey dbmnextkey dblist	contained
syn keyword	phpFunctions  chdir closedir opendir readdir rewinddir	contained
syn keyword	phpFunctions  dl	contained
syn keyword	phpFunctions  mcrypt_get_cipher_name mcrypt_get_block_size mcrypt_get_key_size mcrypt_create_iv mcrypt_cbc mcrypt_cfb mcrypt_ecb mcrypt_ofb	contained
syn keyword	phpFunctions  filepro filepro_fieldname filepro_fieldtype filepro_fieldwidth filepro_retrieve filepro_fieldcount filepro_rowcount	contained
syn keyword	phpFunctions  basename chgrp chmod chown clearstatcache copy delete dirname diskfreespace fclose feof fgetc fgetcsv fgets fgetss file file_exists fileatime filectime filegroup fileinode filemtime fileowner fileperms filesize filetype flock fopen fpassthru fputs fread fseek ftell ftruncate fwrite set_file_buffer is_dir is_executable is_file is_link is_readable is_writeable link linkinfo mkdir pclose popen readfile readlink rename rewind rmdir stat lstat symlink tempnam touch umask unlink	contained
syn keyword	phpFunctions  fdf_open fdf_close fdf_create fdf_save fdf_get_value fdf_set_value fdf_next_field_name fdf_set_ap fdf_set_status fdf_get_status fdf_set_file fdf_get_file	contained
syn keyword	phpFunctions  ftp_connect ftp_login ftp_pwd ftp_cdup ftp_chdir ftp_mkdir ftp_rmdir ftp_nlist ftp_rawlist ftp_systype ftp_pasv ftp_get ftp_fget ftp_put ftp_fput ftp_size ftp_mdtm ftp_rename ftp_delete ftp_site ftp_quit	contained
syn keyword	phpFunctions  bindtextdomain dcgettext dgettext gettext textdomain	contained
syn keyword	phpFunctions  mhash_get_hash_name mhash_get_block_size mhash_count mhash	contained
syn keyword	phpFunctions  header setcookie	contained
syn keyword	phpFunctions  hw_array2objrec hw_children hw_childrenobj hw_close hw_connect hw_cp hw_deleteobject hw_docbyanchor hw_docbyanchorobj hw_documentattributes hw_documentbodytag hw_documentcontent hw_documentsetcontent hw_documentsize hw_errormsg hw_edittext hw_error hw_free_document hw_getparents hw_getparentsobj hw_getchildcoll hw_getchildcollobj hw_getremote hw_getremotechildren hw_getsrcbydestobj hw_getobject hw_getandlock hw_gettext hw_getobjectbyquery hw_getobjectbyqueryobj hw_getobjectbyquerycoll hw_getobjectbyquerycollobj hw_getchilddoccoll hw_getchilddoccollobj hw_getanchors hw_getanchorsobj hw_mv hw_identify hw_incollections hw_info hw_inscoll hw_insdoc hw_insertdocument hw_insertobject hw_mapid hw_modifyobject hw_new_document hw_objrec2array hw_outputdocument hw_pconnect hw_pipedocument hw_root hw_unlock hw_who hw_getusername	contained
syn keyword	phpFunctions  getimagesize imagearc imagechar imagecharup imagecolorallocate imagecolordeallocate imagecolorat imagecolorclosest imagecolorexact imagecolorresolve imagegammacorrect imagecolorset imagecolorsforindex imagecolorstotal imagecolortransparent imagecopy imagecopyresized imagecreate imagecreatefromgif imagecreatefromjpeg imagecreatefrompng imagedashedline imagedestroy imagefill imagefilledpolygon imagefilledrectangle imagefilltoborder imagefontheight imagefontwidth imagegif imagepng imagejpeg imageinterlace imageline imageloadfont imagepolygon imagepsbbox imagepsencodefont imagepsfreefont imagepsloadfont imagepsextendfont imagepsslantfont imagepstext imagerectangle imagesetpixel imagestring imagestringup imagesx imagesy imagettfbbox imagettftext	contained
syn keyword	phpFunctions  imap_append imap_base64 imap_body imap_check imap_close imap_createmailbox imap_delete imap_deletemailbox imap_expunge imap_fetchbody imap_fetchstructure imap_header imap_rfc822_parse_headers imap_headers imap_listmailbox imap_getmailboxes imap_listsubscribed imap_getsubscribed imap_mail_copy imap_mail_move imap_num_msg imap_num_recent imap_open imap_ping imap_renamemailbox imap_reopen imap_subscribe imap_undelete imap_unsubscribe imap_qprint imap_8bit imap_binary imap_scanmailbox imap_mailboxmsginfo imap_rfc822_write_address imap_rfc822_parse_adrlist imap_setflag_full imap_clearflag_full imap_sort imap_fetchheader imap_uid imap_msgno imap_search imap_last_error imap_errors imap_alerts imap_status imap_utf7_decode imap_utf7_encode imap_utf8 imap_fetch_overview imap_mime_header_decode imap_mail_compose imap_mail	contained
syn keyword	phpFunctions  ifx_connect ifx_pconnect ifx_close ifx_query ifx_prepare ifx_do ifx_error ifx_errormsg ifx_affected_rows ifx_getsqlca ifx_fetch_row ifx_htmltbl_result ifx_fieldtypes ifx_fieldproperties ifx_num_fields ifx_num_rows ifx_free_result ifx_create_char ifx_free_char ifx_update_char ifx_get_char ifx_create_blob ifx_copy_blob ifx_free_blob ifx_get_blob ifx_update_blob ifx_blobinfile_mode ifx_textasvarchar ifx_byteasvarchar ifx_nullformat ifxus_create_slob ifxus_free_slob ifxus_close_slob ifxus_open_slob ifxus_tell_slob ifxus_seek_slob ifxus_read_slob ifxus_write_slob	contained
syn keyword	phpFunctions  ibase_connect ibase_pconnect ibase_close ibase_query ibase_fetch_row ibase_fetch_object ibase_free_result ibase_prepare ibase_execute ibase_free_query ibase_timefmt ibase_num_fields	contained
syn keyword	phpFunctions  ldap_add ldap_mod_add ldap_mod_del ldap_mod_replace ldap_bind ldap_close ldap_connect ldap_count_entries ldap_delete ldap_dn2ufn ldap_explode_dn ldap_first_attribute ldap_first_entry ldap_free_result ldap_get_attributes ldap_get_dn ldap_get_entries ldap_get_values ldap_get_values_len ldap_list ldap_modify ldap_next_attribute ldap_next_entry ldap_read ldap_search ldap_unbind ldap_err2str ldap_errno ldap_error	contained
syn keyword	phpFunctions  mail	contained
syn keyword	phpFunctions  abs acos asin atan atan2 base_convert bindec ceil cos decbin dechex decoct deg2rad exp floor getrandmax hexdec log log10 max min mt_rand mt_srand mt_getrandmax number_format octdec pi pow rad2deg rand round sin sqrt srand tan	contained
syn keyword	phpFunctions  mcal_open mcal_close mcal_fetch_event mcal_list_events mcal_append_event mcal_store_event mcal_delete_event mcal_snooze mcal_list_alarms mcal_event_init mcal_event_set_category mcal_event_set_title mcal_event_set_description mcal_event_set_start mcal_event_set_end mcal_event_set_alarm mcal_event_set_class mcal_is_leap_year mcal_days_in_month mcal_date_valid mcal_time_valid mcal_day_of_week mcal_day_of_year mcal_date_compare mcal_next_recurrence mcal_event_set_recur_none mcal_event_set_recur_daily mcal_event_set_recur_weekly mcal_event_set_recur_monthly_mday mcal_event_set_recur_monthly_wday mcal_event_set_recur_yearly mcal_fetch_current_stream_event	contained
syn keyword	phpFunctions  mssql_close mssql_connect mssql_data_seek mssql_fetch_array mssql_fetch_field mssql_fetch_object mssql_fetch_row mssql_field_length mssql_field_name mssql_field_seek mssql_field_type mssql_free_result mssql_get_last_message mssql_min_error_severity mssql_min_message_severity mssql_num_fields mssql_num_rows mssql_pconnect mssql_query mssql_result mssql_select_db	contained
syn keyword	phpFunctions  connection_aborted connection_status connection_timeout define defined die eval exit func_get_arg func_get_args func_num_args function_exists get_browser ignore_user_abort iptcparse leak pack register_shutdown_function serialize sleep uniqid unpack unserialize usleep	contained
syn keyword	phpFunctions  msql msql_affected_rows msql_close msql_connect msql_create_db msql_createdb msql_data_seek msql_dbname msql_drop_db msql_dropdb msql_error msql_fetch_array msql_fetch_field msql_fetch_object msql_fetch_row msql_fieldname msql_field_seek msql_fieldtable msql_fieldtype msql_fieldflags msql_fieldlen msql_free_result msql_freeresult msql_list_fields msql_listfields msql_list_dbs msql_listdbs msql_list_tables msql_listtables msql_num_fields msql_num_rows msql_numfields msql_numrows msql_pconnect msql_query msql_regcase msql_result msql_select_db msql_selectdb msql_tablename	contained
syn keyword	phpFunctions  mysql_affected_rows mysql_change_user mysql_close mysql_connect mysql_create_db mysql_data_seek mysql_db_query mysql_drop_db mysql_errno mysql_error mysql_fetch_array mysql_fetch_field mysql_fetch_lengths mysql_fetch_object mysql_fetch_row mysql_field_name mysql_field_seek mysql_field_table mysql_field_type mysql_field_flags mysql_field_len mysql_free_result mysql_insert_id mysql_list_fields mysql_list_dbs mysql_list_tables mysql_num_fields mysql_num_rows mysql_pconnect mysql_query mysql_result mysql_select_db mysql_tablename	contained
syn keyword	phpFunctions  checkdnsrr closelog debugger_off debugger_on fsockopen gethostbyaddr gethostbyname gethostbynamel getmxrr getprotobyname getprotobynumber getservbyname getservbyport openlog pfsockopen set_socket_blocking syslog	contained
syn keyword	phpFunctions  yp_get_default_domain yp_order yp_master yp_match yp_first yp_next	contained
syn keyword	phpFunctions  odbc_autocommit odbc_binmode odbc_close odbc_close_all odbc_commit odbc_connect odbc_cursor odbc_do odbc_exec odbc_execute odbc_fetch_into odbc_fetch_row odbc_field_name odbc_field_num odbc_field_type odbc_field_len odbc_field_precision odbc_field_scale odbc_free_result odbc_longreadlen odbc_num_fields odbc_pconnect odbc_prepare odbc_num_rows odbc_result odbc_result_all odbc_rollback odbc_setoption odbc_tables odbc_tableprivileges odbc_columns odbc_columnprivileges odbc_gettypeinfo odbc_primarykeys odbc_foreignkeys odbc_procedures odbc_procedurecolumns odbc_specialcolumns odbc_statistics	contained
syn keyword	phpFunctions  ora_bind ora_close ora_columnname ora_columntype ora_commit ora_commitoff ora_commiton ora_error ora_errorcode ora_exec ora_fetch ora_getcolumn ora_logoff ora_logon ora_open ora_parse ora_rollback	contained
syn keyword	phpFunctions  ocidefinebyname ocibindbyname ocilogon ociplogon ocinlogon ocilogoff ociexecute ocicommit ocirollback ocinewdescriptor ocirowcount ocinumcols ociresult ocifetch ocifetchinto ocifetchstatement ocicolumnisnull ocicolumnsize ociserverversion ocistatementtype ocinewcursor ocifreestatement ocifreecursor ocicolumnname ocicolumntype ociparse ocierror ociinternaldebug	contained
syn keyword	phpFunctions  pdf_get_info pdf_set_info pdf_open pdf_close pdf_begin_page pdf_end_page pdf_show pdf_show_boxed pdf_show_xy pdf_set_font pdf_set_leading pdf_set_parameter pdf_get_parameter pdf_set_value pdf_get_value pdf_set_text_rendering pdf_set_horiz_scaling pdf_set_text_rise pdf_set_text_matrix pdf_set_text_pos pdf_set_char_spacing pdf_set_word_spacing pdf_skew pdf_continue_text pdf_stringwidth pdf_save pdf_restore pdf_translate pdf_scale pdf_rotate pdf_setflat pdf_setlinejoin pdf_setlinecap pdf_setmiterlimit pdf_setlinewidth pdf_setdash pdf_moveto pdf_curveto pdf_lineto pdf_circle pdf_arc pdf_rect pdf_closepath pdf_stroke pdf_closepath_stroke pdf_fill pdf_fill_stroke pdf_closepath_fill_stroke pdf_endpath pdf_clip pdf_setgray_fill pdf_setgray_stroke pdf_setgray pdf_setrgbcolor_fill pdf_setrgbcolor_stroke pdf_setrgbcolor pdf_add_outline pdf_set_transition pdf_set_duration pdf_open_gif pdf_open_png pdf_open_memory_image pdf_open_jpeg pdf_close_image pdf_place_image pdf_put_image pdf_execute_image pdf_add_annotation pdf_set_border_style pdf_set_border_color pdf_set_border_dash	contained
syn keyword	phpFunctions  preg_match preg_match_all preg_replace preg_split preg_quote preg_grep	contained
syn keyword	phpFunctions  error_log error_reporting extension_loaded getenv get_cfg_var get_current_user get_magic_quotes_gpc get_magic_quotes_runtime getlastmod getmyinode getmypid getmyuid getrusage phpinfo phpversion php_logo_guid putenv set_magic_quotes_runtime set_time_limit zend_logo_guid	contained
syn keyword	phpFunctions  posix_kill posix_getpid posix_getppid posix_getuid posix_geteuid posix_getgid posix_getegid posix_setuid posix_setgid posix_getgroups posix_getlogin posix_getpgrp posix_setsid posix_setpgid posix_getpgid posix_getsid posix_uname posix_times posix_ctermid posix_ttyname posix_isatty posix_getcwd posix_mkfifo posix_getgrnam posix_getgrgid posix_getpwnam posix_getpwuid posix_getrlimit	contained
syn keyword	phpFunctions  pg_close pg_cmdtuples pg_connect pg_dbname pg_errormessage pg_exec pg_fetch_array pg_fetch_object pg_fetch_row pg_fieldisnull pg_fieldname pg_fieldnum pg_fieldprtlen pg_fieldsize pg_fieldtype pg_freeresult pg_getlastoid pg_host pg_loclose pg_locreate pg_loexport pg_loimport pg_loopen pg_loread pg_loreadall pg_lounlink pg_lowrite pg_numfields pg_numrows pg_options pg_pconnect pg_port pg_result pg_trace pg_tty pg_untrace	contained
syn keyword	phpFunctions  escapeshellcmd exec passthru system	contained
syn keyword	phpFunctions  recode_string recode recode_file	contained
syn keyword	phpFunctions  ereg ereg_replace eregi eregi_replace split sql_regcase	contained
syn keyword	phpFunctions  sem_get sem_acquire sem_release shm_attach shm_detach shm_remove shm_put_var shm_get_var shm_remove_var	contained
syn keyword	phpFunctions  session_start session_destroy session_name session_module_name session_save_path session_id session_register session_unregister session_unset session_is_registered session_get_cookie_params session_set_cookie_params session_decode session_encode	contained
syn keyword	phpFunctions  snmpget snmpset snmpwalk snmpwalkoid snmp_get_quick_print snmp_set_quick_print	contained
syn keyword	phpFunctions  addcslashes addslashes bin2hex chop chr chunk_split convert_cyr_string count_chars crypt echo explode flush get_html_translation_table get_meta_tags htmlentities htmlspecialchars implode join levenshtein ltrim md5 metaphone nl2br ord parse_str print printf quoted_printable_decode quotemeta rawurldecode rawurlencode setlocale similar_text soundex sprintf strcasecmp strchr strcmp strcspn strip_tags stripcslashes stripslashes stristr strlen strpos strrchr str_repeat strrev strrpos strspn strstr strtok strtolower strtoupper str_replace strtr substr substr_replace trim ucfirst ucwords	contained
syn keyword	phpFunctions  swf_openfile swf_closefile swf_labelframe swf_showframe swf_setframe swf_getframe swf_mulcolor swf_addcolor swf_placeobject swf_modifyobject swf_removeobject swf_nextid swf_startdoaction swf_actiongotoframe swf_actiongeturl swf_actionnextframe swf_actionprevframe swf_actionplay swf_actionstop swf_actiontogglequality swf_actionwaitforframe swf_actionsettarget swf_actiongotolabel swf_enddoaction swf_defineline swf_definerect swf_definepoly swf_startshape swf_shapelinesold swf_shapefilloff swf_shapefillsolid swf_shapefillbitmapclip swf_shapefillbitmaptile swf_shapemoveto swf_shapelineto swf_shapecurveto swf_shapecurveto3 swf_shapearc swf_endshape swf_definefont swf_setfont swf_fontsize swf_fontslant swf_fonttracking swf_getfontinfo swf_definetext swf_textwidth swf_definebitmap swf_getbitmapinfo swf_startsymbol swf_endsymbol swf_startbutton swf_addbuttonrecord swf_oncondition swf_endbutton swf_viewport swf_ortho swf_ortho2 swf_perspective swf_polarview swf_lookat swf_pushmatrix swf_popmatrix swf_scale swf_translate swf_rotate swf_posround	contained
syn keyword	phpFunctions  sybase_affected_rows sybase_close sybase_connect sybase_data_seek sybase_fetch_array sybase_fetch_field sybase_fetch_object sybase_fetch_row sybase_field_seek sybase_free_result sybase_num_fields sybase_num_rows sybase_pconnect sybase_query sybase_result sybase_select_db	contained
syn keyword	phpFunctions  base64_decode base64_encode parse_url urldecode urlencode	contained
syn keyword	phpFunctions  call_user_func doubleval empty gettype intval is_array is_bool is_double is_float is_int is_integer is_long is_numeric is_object is_real is_resource is_string isset print_r settype strval unset var_dump	contained
syn keyword	phpFunctions  vm_adduser vm_addalias vm_passwd vm_delalias vm_deluser	contained
syn keyword	phpFunctions  wddx_serialize_value wddx_serialize_vars wddx_packet_start wddx_packet_end wddx_add_vars wddx_deserialize	contained
syn keyword	phpFunctions  xml_parser_create xml_set_object xml_set_element_handler xml_set_character_data_handler xml_set_processing_instruction_handler xml_set_default_handler xml_set_unparsed_entity_decl_handler xml_set_notation_decl_handler xml_set_external_entity_ref_handler xml_parse xml_get_error_code xml_error_string xml_get_current_line_number xml_get_current_column_number xml_get_current_byte_index xml_parser_free xml_parser_set_option xml_parser_get_option utf8_decode utf8_encode	contained
syn keyword	phpFunctions  include	contained

if exists( "php_baselib" )
  syn keyword	phpBaselib	query next_record num_rows affected_rows nf f p np num_fields haltmsg seek link_id query_id metadata table_names nextid connect halt free register unregister is_registered delete url purl self_url pself_url hidden_session add_query padd_query reimport_get_vars reimport_post_vars reimport_cookie_vars set_container set_tokenname release_token put_headers get_id get_id put_id freeze thaw gc reimport_any_vars start url purl login_if is_authenticated auth_preauth auth_loginform auth_validatelogin auth_refreshlogin auth_registerform auth_doregister start check have_perm permsum perm_invalid	contained
  syn keyword	phpFunctions	page_open page_close sess_load sess_save	contained
endif

" Conditional
syn keyword	phpConditional	if else elseif endif switch endswitch	contained

" Repeat
syn keyword	phpRepeat	do for while endwhile	contained

" Repeat
syn keyword	phpLabel	case default switch	contained

" Statement
syn keyword phpStatement        break return continue exit contained

" Keyword
syn keyword	phpKeyword	var	contained

" Type
syn keyword	phpType	int integer real double float string array object	contained

" Structure
syn keyword	phpStructure	class extends	contained

" StorageClass
syn keyword	phpStorageClass	global static	contained

" Operator
syn match	phpOperator	"[-=+%^&|*!.~?:]"	contained
syn match	phpOperator	"[-+*/%^&|.]="	contained
syn match	phpOperator	"/[^*/]"me=e-1	contained
syn match	phpOperator	"\$"	contained
syn match	phpOperator	"&&\|\<and\>"	contained
syn match	phpOperator	"||\|\<x\=or\>"	contained
syn match	phpRelation	"[!=<>]="	contained
syn match	phpRelation	"[<>]"	contained
syn match	phpMemberSelector	"->"	contained
syn match	phpVarSelector	"\$"	contained

" Identifier
syn match	phpIdentifier	"$\h\w*"	contained contains=phpEnvVar,phpIntVar,phpVarSelector

" Methoden
syn match	phpMethods	"->\h\w*"	contained contains=phpBaselib,phpMemberSelector

" Include
syn keyword	phpInclude	include require	contained

" Define
syn keyword	phpDefine	Function cfunction new	contained

" Boolean
syn keyword	phpBoolean	true false	contained

" String
syn region	phpStringDouble	keepend matchgroup=None start=+"+ skip=+\\\\\|\\"+  end=+"+ contains=phpIdentifier,phpSpecialChar,@phpSql contained
syn region	phpStringSingle	keepend matchgroup=None start=+'+ skip=+\\\\\|\\'+  end=+'+ contains=phpSpecialChar,@phpSql contained

" Number
syn match phpNumber	"-\=\<\d\+\>"	contained

" Float
syn match phpFloat	"\(-\=\<\d+\|-\=\)\.\d\+\>"	contained

" SpecialChar
syn match phpSpecialChar	"\\[abcfnrtyv\\]"	contained
syn match phpSpecialChar	"\\\d\{3}"	contained contains=phpOctalError
syn match phpSpecialChar	"\\x[0-9a-fA-F]\{2}"	contained

" Error
syn match phpOctalError	"[89]"	contained
syn match phpParentError	"[)}\]]"	contained

" Todo
syn keyword	phpTodo TODO Todo todo	contained

syn cluster	phpInside	contains=phpComment,phpFunctions,phpIdentifier,phpConditional,phpRepeat,phpLabel,phpStatement,phpOperator,phpRelation,phpStringSingle,phpStringDouble,phpNumber,phpFloat,phpSpecialChar,phpParent,phpParentError,phpInclude,phpKeyword,phpType,phpIdentifierParent,phpBoolean,phpStructure,phpMethods

syn cluster	phpTop	contains=@phpInside,phpDefine,phpParentError,phpStorageClass

if exists("php_parentError")
  syn region	phpParent	matchgroup=Delimiter start="(" end=")" contained contains=@phpInside
  syn region	phpParent	matchgroup=Delimiter start="{" end="}" contained contains=@phpTop
  syn region	phpParent	matchgroup=Delimiter start="\[" end="\]" contained contains=@phpInside

  syn region	 phpRegion	keepend matchgroup=Delimiter start="<?\(php\)\=" skip=+".\{-}?>.\{-}"\|'.\{-}?>.\{-}'\|/\*.\{-}?>.\{-}\*/+ end="?>" contains=@phpTop
  syn region	 phpRegion	 keepend matchgroup=Delimiter start=+<script language="php">+ skip=+".\{-}</script>.\{-}"\|'.\{-}</script>.\{-}'\|/\*.\{-}</script>.\{-}\*/+ end=+</script>+ contains=@phpTop
else
  syn match	phpParent	"[({[\]})]"	contained

  syn region	 phpRegion	matchgroup=Delimiter start="<?\(php\)\=" end="?>" contains=@phpTop
  syn region	 phpRegion	matchgroup=Delimiter start=+<script language="php">+ end=+</script>+ contains=@phpTop
endif

" sync
if exists("php_minlines")
  exec "syn sync minlines=" . php_minlines
else
  syn sync minlines=100
endif

" The default highlighting.
hi def link	 phpComment		Comment
hi def link	 phpBoolean		Boolean
hi def link	 phpStorageClass	StorageClass
hi def link	 phpStructure		Structure
hi def link	 phpStringSingle	String
hi def link	 phpStringDouble	String
hi def link	 phpNumber		Number
hi def link	 phpFloat		Float
hi def link	 phpFunctions		Function
hi def link	 phpBaselib		Function
hi def link	 phpRepeat		Repeat
hi def link	 phpConditional		Conditional
hi def link	 phpLabel		Label
hi def link	 phpStatement		Statement
hi def link	 phpKeyword		Statement
hi def link	 phpType		Type
hi def link	 phpInclude		Include
hi def link	 phpDefine		Define
hi def link	 phpSpecialChar		SpecialChar
hi def link	 phpParent		Delimiter
hi def link	 phpParentError		Error
hi def link	 phpOctalError		Error
hi def link	 phpTodo		Todo
hi def link	 phpMemberSelector	Structure
hi def link	 phpNoEnd		Operator
if exists("php_oldStyle")
  hi def phpIntVar guifg=Red ctermfg=DarkRed
  hi def phpEnvVar guifg=Red ctermfg=DarkRed
  hi def phpOperator guifg=SeaGreen ctermfg=DarkGreen
  hi def phpVarSelector guifg=SeaGreen ctermfg=DarkGreen
  hi def phpRelation guifg=SeaGreen ctermfg=DarkGreen
  hi def phpIdentifier guifg=DarkGray ctermfg=Brown
else
  hi def link	phpIntVar		Identifier
  hi def link	phpEnvVar		Identifier
  hi def link	phpOperator		Operator
  hi def link	phpVarSelector		Operator
  hi def link	phpRelation		Operator
  hi def link	phpIdentifier		Identifier
endif

let b:current_syntax = "php"

if main_syntax == 'php'
  unlet main_syntax
endif

" vim: ts=8
