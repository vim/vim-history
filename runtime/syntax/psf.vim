" Vim syntax file
" Language:	Software Distributor product specification file
"               (POSIX 1387.2-1995).
" Maintainer:	Rex Barzee <barzee@fc.hp.com>
" Last change:	27 Nov 2000

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" PSF files are case sensitive
syn case match

syn keyword psfObject bundle category control_file distribution end
syn keyword psfObject file fileset installed_software product root
syn keyword psfObject subproduct vendor

syn match  psfUnquotString +[^"# 	][^#]*+ contained
syn region psfQuotString   start=+"+ skip=+\\"+ end=+"+ contained

syn match  psfObjTag    "\<[-_+A-Z0-9a-z]\+\(\.[-_+A-Z0-9a-z]\+\)*" contained
syn match  psfAttAbbrev ",\<\(fa\|fr\|[aclqrv]\)\(<\|>\|<=\|>=\|=\|==\)[^,]\+" contained
syn match  psfObjTags   "\<[-_+A-Z0-9a-z]\+\(\.[-_+A-Z0-9a-z]\+\)*\(\s\+\<[-_+A-Z0-9a-z]\+\(\.[-_+A-Z0-9a-z]\+\)*\)*" contained

syn match  psfNumber    "\<\d\+\>" contained
syn match  psfFloat     "\<\d\+\>\(\.\<\d\+\>\)*" contained

syn match  psfLongDate  "\<\d\d\d\d\d\d\d\d\d\d\d\d\.\d\d\>" contained

" Add other possible states here.  Todo: Split state and patch_state.
syn keyword psfState    applied available superseded configured contained

syn keyword psfBoolean  false true contained


"Some of the attributes covered by attUnquotString and attQuotString:
" architecture category_tag control_directory copyright
" create_date description directory file_permissions install_source
" install_type location machine_type mod_date number os_name os_release
" os_version pose_as_os_name pose_as_os_release readme revision
" share_link title vendor_tag
syn region psfAttUnquotString matchgroup=psfAttrib start=~^\s*[^# 	]\+\s\+[^#" 	]~rs=e-1 contains=psfUnquotString,psfComment end=~$~ keepend oneline

syn region psfAttQuotString matchgroup=psfAttrib start=~^\s*[^# 	]\+\s\+"~rs=e-1 contains=psfQuotString,psfComment skip=~\\"~ matchgroup=psfQuotString end=~"~ keepend


" These regions are defined in attempt to do syntax checking for some
" of the attributes.
syn region psfAttTag matchgroup=psfAttrib start="^\s*tag\s\+" contains=psfObjTag,psfComment end="$" keepend oneline

syn region psfAttSpec matchgroup=psfAttrib start="^\s*\(ancestor\|applied_patches\|applied_to\|contents\|corequisites\|exrequisites\|prerequisites\|software_spec\|supersedes\|superseded_by\)\s\+" contains=psfObjTag,psfAttAbbrev,psfComment end="$" keepend

syn region psfAttTags matchgroup=psfAttrib start="^\s*all_filesets\s\+" contains=psfObjTags,psfComment end="$" keepend

syn region psfAttNumber matchgroup=psfAttrib start="^\s*\(compressed_size\|instance_id\|size\)\s\+" contains=psfNumber,psfComment end="$" keepend oneline

syn region psfAttTime matchgroup=psfAttrib start="^\s*\(create_time\|ctime\|mod_time\|mtime\|timestamp\)\s\+" contains=psfNumber,psfComment end="$" keepend oneline

syn region psfAttFloat matchgroup=psfAttrib start="^\s*\(data_model_revision\|layout_version\)\s\+" contains=psfFloat,psfComment end="$" keepend oneline

syn region psfAttLongDate matchgroup=psfAttrib start="^\s*install_date\s\+" contains=psfLongDate,psfComment end="$" keepend oneline

syn region psfAttState matchgroup=psfAttrib start="^\s*\(patch_state\|state\)\s\+" contains=psfState,psfComment end="$" keepend oneline

syn region psfAttBoolean matchgroup=psfAttrib start="^\s*\(is_kernel\|is_locatable\|is_patch\|is_protected\|is_reboot\|is_reference\|is_secure\|is_sparse\)\s\+" contains=psfBoolean,psfComment end="$" keepend oneline

syn match  psfComment "#.*$"

" The default highlighting.
hi def link psfObject       Statement
hi def link psfAttrib       Type
hi def      psfUnquotString NONE
hi def link psfQuotString   String
hi def link psfObjTag       Identifier
hi def link psfAttAbbrev    PreProc
hi def link psfObjTags      Identifier
hi def      psfNumber       NONE
hi def      psfFloat        NONE
hi def      psfLongDate     NONE
hi def      psfState        NONE
hi def      psfBoolean      NONE

" The psfAtt* regions are given no highlighting because the start of
" each region is highlighted as psfAttrib using matchgroup=, and the
" body of each region is highlighted as psfObjTag, psfNumber,
" psfComment, etc.  In other words there is no need to give the
" psfAtt* regions their own highlighting because everything is already
" pretty much covered.
hi def      psfAttUnquotString NONE
hi def      psfAttQuotString   NONE
hi def      psfAttTag          NONE
hi def      psfAttSpec         NONE
hi def      psfAttTags         NONE
hi def      psfAttNumber       NONE
hi def      psfAttTime         NONE
hi def      psfAttFloat        NONE
hi def      psfAttLongDate     NONE
hi def      psfAttState        NONE
hi def      psfAttBoolean      NONE

hi def link psfComment      Comment

" Long descriptions and copyrights confuse the syntax highlighting, so
" force vim to backup at least 100 lines before the top visible line
" looking for a sync location.
syn sync lines=100

let b:current_syntax = "psf"
