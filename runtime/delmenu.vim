" This Vim script deletes all the menus, so that they can be redefined.
" Warning: This also deletes all menus defined by the user!
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 May 10

aunmenu *

if exists("did_install_default_menus")
  unlet did_install_default_menus
endif
if exists("did_install_syntax_menu")
  unlet did_install_syntax_menu
endif
if exists("did_menu_trans")
  menutrans clear
  unlet did_menu_trans
endif
if exists("find_help_dialog")
  unlet find_help_dialog
endif

silent! unlet g:menutrans_help_dialog
silent! unlet g:menutrans_path_dialog
silent! unlet g:menutrans_tags_dialog
silent! unlet g:menutrans_textwidth_dialog
silent! unlet g:menutrans_fileformat_dialog
silent! unlet g:menutrans_no_file

" vim: set sw=2 :
