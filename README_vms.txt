README_vms.txt for version 5.7a of Vim: Vi IMproved.

This file explains the installation of Vim on VMS systems.
See "README.txt" in the runtime archive for information about Vim.


Most information can be found in the on-line documentation.  Use ":help vms"
inside Vim.  Or get the runtime files and read runtime/doc/os_vms.txt to find
out how to install the runtime files.

To compile Vim yourself you need three archives:
  vim-5.7-rt.tar.gz	runtime files
  vim-5.7-src.tar.gz	source files
  vim-5.7-extra.tar.gz	extra source files

Compilation is recommended, because it will make sure that the correct
libraries will be used for your specific system.  However, you might not be
able to compile Vim.

To use the binary version, you need these archives:
  vim-5.7-vms.tar.gz	executables
  vim-5.7-rt.tar.gz	runtime files

The binary archive contains two files: vim.exe and xxd.exe.  These are
executables compiled on OpenVMS v6.2.  The executables have been made by Chip
Campbell <cec@gryphon.gsfc.nasa.gov>.  ctags.exe is missing, because it could
not be compiled without errors.
