Vim-history
-----------

This repository contains the old source code of Vim. It basically consists of two parts:

* Source Commits from the old [CVS repository][1] at the [sourceforge project page][3]  
  This contains the history for the releases 5.5 to 6.4 and have been created by using `git cvsimport` (see [documentation][4]).

* Commits for the even older releases from releases 1.14 until 5.5  
  Those commits have been created from the tarballs at the Vim FTP site:

  * <http://ftp.vim.org/pub/vim/old/>  
    (v1.14 - v2.8, v4.0 - v4.5)
  * <http://ftp.vim.org/pub/vim/unix/>  
    (v3.0, v4.6, v5.0 - v5.5)

  Those older commits are not the TRUE history.
  (E.g. commit dates are faked. They are based on the dates from `src/version.c` or `src/version.h`.)

Initially those two parts were two separate repositories. However they have been merged into a single repository using
```bash
git replace --graft 686757e4d2a46c8ab55c08c7a0ccd 8f6182bf9f905bfac0fee
```

Other repositories
------------------

* [vim-ancient][2]  
  This includes the history from v5.5 to v6.4.  
  This was converted from the old [CVS][1] repository at sourceforge.

* [official repository][5]  
  This includes the history from v7.0 to the latest release.

[1]: https://sourceforge.net/p/vim/cvs/
[2]: https://bitbucket.org/vim-mirror/vim-ancient
[3]: https://sourceforge.net/p/vim/
[4]: https://sourceforge.net/p/forge/documentation/CVS/
[5]: https://github.com/vim/vim
