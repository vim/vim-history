Language files for Vim

Translated menus
----------------

The contents of each menu file is a sequence of lines with "menutrans"
commands.  Read one of the existing files to get an idea of how this works.

More information in the on-line help:

	:help multilang-menus
	:help :menutrans
	:help 'langmenu'
	:help :language

The "$VIMRUNTIME/menu.vim" file will search for a menu translation file.  This
depends on the value of the "v:lang" variable.

	"menu_" . v:lang . ".vim"

When the 'menutrans' option is set, its value will be used instead of v:lang.

The file name is always lower case.  It is the full name as the ":language"
command shows (the LC_MESSAGES value).

For example, to use the Big5 (Taiwan) menus on MS-Windows the $LANG will be

	Chinese(Taiwan)_Taiwan.950

and use the menu translation file:

	$VIMRUNTIME/lang/menu_chinese(taiwan)_taiwan.950.vim

On Unix you should set $LANG, depending on your shell:

	csh/tcsh:	setenv LANG "zh_TW.Big5"
	sh/bash/ksh:	export LANG="zh_TW.Big5"

and the menu translation file is:

	$VIMRUNTIME/lang/menu_zh_tw.big5.vim

The menu translation file should set the "did_menu_trans" variable so that Vim
will not load another file.


Translated messages
-------------------

This requires doing "make install" in the "src" directory.  It will compile
the portable files "src/po/*.po" into binary ".mo" files and place them in the
right directory.
