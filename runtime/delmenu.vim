" This Vim script deletes all the menus, so that they can be redefined.
" Warning: This also deletes all menus defined by the user!
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Mar 21

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

" vim: set sw=2 :
