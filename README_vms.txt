README_vms.txt for version 6.0ac of Vim: Vi IMproved.

This file explains the installation of Vim on VMS systems.
See "README.txt" in the runtime archive for information about Vim.


Most information can be found in the on-line documentation.  Use ":help vms"
inside Vim.  Or get the runtime files and read runtime/doc/os_vms.txt to find
out how to install the runtime files.

To compile Vim yourself you need three archives:
  vim-6.0-rt.tar.gz	runtime files
  vim-6.0-src.tar.gz	source files
  vim-6.0-extra.tar.gz	extra source files

Compilation is recommended, because it will make sure that the correct
libraries will be used for your specific system.  However, you might not be
able to compile Vim.

To use the binary version, you need one of these archives:

  vim-60exe-alpha-gui.zip	Alpha GUI executables
  vim-60exe-alpha-term.zip	Alpha console executables
  vim-60exe-vax-gui.zip		VAX GUI executables
  vim-60exe-vax-term.zip	VAX console executables

and of course
  vim-6.0-rt.tar.gz		runtime files

The binary archives contain: vim.exe, ctags.exe, xxd.exe, mms_vim.exe files,
but there are also prepared "deploy ready" archives:

vim-60-alpha.zip		GUI and console executables with runtime and 
				help files for Alpha systems
vim-60-vax.zip			GUI and console executables with runtime and 
				help files for VAX systems

These executables and up to date patches for OpenVMS system are downloadable
from http://www.polarfox.com/vim/
