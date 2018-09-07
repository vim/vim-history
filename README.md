Vim-history
-----------

This repository contains the old source code of Vim. It basically consists of two parts:

* Source Commits from the old [CVS repository][1] at the [sourceforge project page][2]  
  This contains the history for the releases 5.5 to 6.4 and have been created by using `git cvsimport` (see [documentation][3]).

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
git filter-branch 8f6182bf9f905bfac0fee..HEAD
```

Release History
---------------

The following table summarizes the previous release dates according to several sources.

Data has been taken from the

 * [Vim FAQ][4]
 * a youtube video of [Brams presentation on Vim][5] (1:37)
 * Wikipedia article on [Vim][6]
 * Date of the release tags in the CVS repository (now available in the [vim-ancient repository][7])
 * Date of the release tag in the current [Vim repository][8]
 * Announcements in the mailinglist [vim-announce][9]


Version  | [vim-faq][10]  | [video][5]  | [Wikipedia][6] | [Tags current][8], [Tags old][7] | [Announcements][9]
|--------|----------|-----------|--------------|------------|-----------------
8.1  | 17th May, 2018  | | May 18 | May 17 | May 17
8.0  | 12th September, 2016  | Sept 12  | Sept 12 | Sept 12 | Sept 12
7.4  | 10th August, 2013  | | Aug 10 | Aug 10 | Aug 10
7.3  | 15th August, 2010  | | Aug 15 | Aug 15 | Aug 15
7.2  | 9th August, 2008  | | Aug 9 | Aug 9 | Aug 9
7.1  | 12th May, 2007  | | May 12 | May 12 | May 12
7.0  | 8th May, 2006  | May 8  | May 7 | May 7 | May 8
6.4  | 15th October, 2005  | | Oct 15 | Oct 15 | Oct 15
6.3  | 8th June 2004  | | June 7 | June 8 | June 8
6.2  | 1st June 2003  | | June 1 | June 2 | June 1
6.1  | 24th March 2002  | | Mar 24 | Mar 25 | Mar 24
6.0  | 27th September, 2001  | Sep 27  | Sep 26 | Sep 27 | Sep 27
5.8  | 31st May, 2001  | | May 31 | June 5 | May 31
5.7  | 24th June, 2000  | | June 24 | June 24 | June 24
5.6  | 16th January, 2000  | | June 16 | Jan 17 | June 16
5.5  | 21st September, 1999  | | Sep 19 | | Sep 21
5.4  | 26th July, 1999  | | July 25 | | July 26
5.3  | 31st August, 1998  | | Aug 31 | | Aug 31
5.2  | 24th August, 1998  | | **Apr 27** \[1\] | | Aug 24
5.1  | 7th April, 1998  | | Apr 06 | | Apr 7
5.0  | 19th February, 1998  | Feb 28  | Feb 19 | | Feb 19
4.6  | 13th March, 1997  | | | | Mar 13
4.5  | 17th October, 1996  | | | | 
4.2  | 5th July, 1996  | | | | 
4.0  | 21st May, 1996  | May 29  | May 29 | | 
3.0  | 16th August, 1994  | Aug 12 | Aug 12 | | 
2.0  | 21st December, 1993  | Dec 21  | Dec 14 | | 
1.27  | 23rd April, 1993  | Apr 23  | | | 
1.17  | 21st April, 1992  | | | | 
1.14  | 2nd November, 1991  | Nov 2 | | | 

\[1\]: I think it is an error in the wikipedia page and very unlikely, that Vim 5.2 was released just 20 days after 5.1


Merge the whole history
-----------------------

If you want to see the whole history of Vim in your one local repository, try this:

```bash
git clone https://github.com/vim/vim.git
cd vim
git fetch https://github.com/vim/vim-history.git --tags
git replace --graft 071d4279d6ab81b7187b48f3a0fc61e587b6db6c 6336d50dfb7ddf71d44d55f57e9d5e3e35a2bf52
```


Other repositories
------------------

* [vim-ancient][7]  
  This includes the history from v5.5 to v6.4.  
  This was converted from the old [CVS][1] repository at sourceforge.

* [official repository][8]  
  This includes the history from v7.0 to the latest release.

[1]: https://sourceforge.net/p/vim/cvs/
[2]: https://sourceforge.net/p/vim/
[3]: https://sourceforge.net/p/forge/documentation/CVS/
[4]: https://github.com/chrisbra/vim_faq
[5]: https://www.youtube.com/watch?v=ayc_qpB-93o
[6]: https://en.wikipedia.org/wiki/Vim_(text_editor)#History
[7]: https://bitbucket.org/vim-mirror/vim-ancient
[8]: https://github.com/vim/vim
[9]: https://groups.yahoo.com/neo/groups/vimannounce/info
[10]: http://vimhelp.appspot.com/vim_faq.txt.html#faq-3.1
