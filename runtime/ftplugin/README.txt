The ftplugin directory is for Vim plugin scripts that are only used for a
specific filetype.

All files in subdirectories ending in .vim will be sourced by Vim when it
detects the filetype that matches the name of the subdirectory.  Example:

	c/default.vim

Is loaded when the "c" filetype is detected.

This is only done when the ":filetype plugin" command has been used.

See ":help ftplugin" in Vim.
