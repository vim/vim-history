Language files for Vim

Translated menus
----------------

The "$VIMRUNTIME/menu.vim" file will search for a menu translation file.  This
depends on the value of $LANG:

	menu_ $LANG .vim

(without the spaces)
The file name must be lower case.

For example, to use the Big5 (Taiwan) menus on MS-Windows the $LANG will be

	Chinese(Taiwan)_Taiwan.950

and use the menu translation file:

	$VIMRUNTIME/lang/menu_Chinese(Taiwan)_Taiwan.950.vim

On Unix you should set $LANG, depending on your shell:

	csh/tcsh:	setenv LANG "zh_TW.Big5"
	sh/bash/ksh:	export LANG="zh_TW.Big5"

and the menu translation file is:

	$VIMRUNTIME/lang/menu_zh_tw.big5.vim

The menu translation file should set the "did_menu_trans" variable so that Vim
will stop searching for another file.


Translated messages
-------------------

This requires doing "make install".  It will compile the portable files
"src/po/*.po" into system-dependend ".mo" files and place them in the right
place.
