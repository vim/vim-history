This directory contains vim scripts for specific languages.

syntax.vim	Contains autocommands to load a language file when a certain
		file name (extension) is used.  And sets up the Syntax menu
		for the GUI.

nosyntax.vim	Undo the loading of syntax.vim.

scripts.vim	Contains checks for the file contents, to recognize some
		languages and load their script file.



A few rules for language files:
- The name of the file is the same as the start of the group names used.
- Start with a "syntax clear".
- Does NOT do anything that is a user preference.
- Should avoid using specific colors.  Use the standard highlight groups
  whenever possible.  Don't forget that some people use a different background
  color, or have only eight colors available.


A few special files:

2html.vim	Converts any highlighted file to HTML (GUI only).
colortest.vim	Check for color names and actual color on screen.
hitest.vim	View the current highlight settings.


For further info see ":help syntax" in Vim.
