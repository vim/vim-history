TRANSLATING VIM MESSAGES

In this directory you will find xx.po files, where "xx" is a language code.
Each file contains the translation of English Vim messages for one language.
The files are in "po" format, used by the gettext package.  Please refer to
the gettext documentation for more information.


WHEN THERE IS A MISTAKE

If you find there is a mistake in one of the translations, please report this
to the maintainer of the translation.  His/her E-mail address is in the
comments at the start of the file.  You can also see this with the ":messages"
command in Vim when the translation is being used.


UPDATING A PO FILE

If you are the maintainer of a .po file, this is how you update the file.  We
will use "xx.po" as an example here, replace "xx" with the name of your
language.

(1) Add new and changed messages from the Vim sources:
	make xx
    This will extract all the strings from Vim and merge them in with the
    existing translations.  Requires the GNU gettext utilities.  Once you do
    this, you MUST do the next three steps!

(2) Translate
    See the gettext documentation on how to do this.  You can also find
    examples in the other po files.
    Search the po file for items that require translation:
    	/fuzzy\|^msgstr ""\(\n"\)\@!
    Remove the "#, fuzzy" line after adding the translation.

    There is one special message:
	msgid "Messages maintainer: Bram Moolenaar <Bram@vim.org>"
    You should include your name and E-mail address instead, for example:
	msgstr "Berichten übersetzt bei: Johannes Zellner <johannes@zellner.org>"

(3) Clean up
    This is very important to make sure the translation works on all systems.
    Comment-out all non-translated strings.  There are two types:
    - items marked with "#, fuzzy"
    - items with an empty msgstr
    You can do this with the cleanup.vim script:
    	:source cleanup.vim
    Background: on Solaris an empty msgstr results in an empty message; GNU
    gettext ignores empty strings and items marked with "#, fuzzy".
 
(4) Check:
	make xx.mo
    This checks for syntax errors.
