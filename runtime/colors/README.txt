README.txt for color scheme files

These files are used for the ":colorscheme" command.  They appear in the
Edit/Color Scheme menu in the GUI.


Hints for writing a color scheme file:

You can use ":highlight clear" to reset everything to the defaults, and then
change the groups that you want differently.  This also will work for groups
that are added in later versions of Vim.

You can use ":highlight" to find out the current colors.  Exception: the
ctermfg and ctermbg values are numbers, which are only valid for the current
terminal.  Use the color names instead.  See ":help cterm-colors".

The default color settings can be found in the source file src/syntax.c.
Search for "highlight_init".

If you think you have a color scheme that is good enough to be used by others,
please check the following items:

- Does it work in a color terminal as well as in the GUI?
- Is 'background' appropriately set to "light" or "dark"?
- Try setting 'hlsearch' and searching for a pattern, is the match easy to
  spot?
- Split a window with ":split" and ":vsplit".  Are the status lines and
  vertical separators clearly visible?
- In the GUI, is it easy to find the cursor, also in a file with lots of
  syntax highlighting?
- Do not use hard coded escape sequences, these will not work in other
  terminals.  Always use color names or #RRGGBB for the GUI.
