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
  * <http://ftp.vim.org/pub/vim/extra/>  
    (v5.0 - v5.5)

  Those older commits are not the TRUE history.
  (E.g. commit dates are faked. They are based on the dates from `src/version.c` or `src/version.h`.)

Initially those two parts were two separate repositories. However they have been merged into a single repository using
```bash
git replace --graft 686757e4d2a46c8ab55c08c7a0ccd v5.5
git filter-branch v5.5..HEAD
```

Some tags were updated [manually][11].


Release History
---------------

The following table summarizes the previous release dates according to several sources.

Data has been taken from the

 * [Vim FAQ][4]
 * a youtube video of [Brams presentation on Vim][5] (1:37)
 * Wikipedia article on [Vim][6]
 * Announcements in the mailinglist [vim-announce][9]
 * Announcements in the [comp.editors][12] news group
 * Date of the release tags in the CVS repository (now available in the [vim-ancient repository][7])
 * Date of the release tag in the current [Vim repository][8]
 * Source code (the date written in `src/version.h` or `src/version.c`)  E.g.
   ```
   $ git show v8.1.0000:src/version.h | grep '#define VIM_VERSION_LONG\>'
   $ git show v4.6:src/version.c | grep '\*longVersion'
   ```

Version | [vim-faq][10] | [video][5] | [Wikipedia][6] | [Announcements][9],<br/>[comp.editors][12] | [Tags current][8],<br/>[Tags old][7] | Source code
|----|----------------------|--------|------------------|--------------------|---------|-------------
8.2  | 12th, December, 2019 |        |                  | [Dec 12][68]       | Dec 12  | [Dec 12][69]
8.1  | 17th May, 2018       |        | May 18           | [May 17][38]       | May 17  | [May 17][39]
8.0  | 12th September, 2016 | Sep 12 | Sep 12           | [Sep 12][37]       | Sep 12  | [Sep 12][40]
7.4  | 10th August, 2013    |        | Aug 10           | [Aug 10][36]       | Aug 10  | [Aug 10][41]
7.3  | 15th August, 2010    |        | Aug 15           | [Aug 15][35]       | Aug 15  | [Aug 15][42]
7.2  | 9th August, 2008     |        | Aug 9            | [Aug 9][34]        | Aug 9   | [Aug 9][43]
7.1  | 12th May, 2007       |        | May 12           | [May 12][33]       | May 12  | [May 12][44]
7.0  | 8th May, 2006        | May 8  | May 7            | [May 8][32]        | May 7   | [May 7][45]
6.4  | 15th October, 2005   |        | Oct 15           | [Oct 15][31]       | Oct 15  | [Oct 15][46]
6.3  | 8th June 2004        |        | June 7           | [June 8][30]       | June 8  | [June 7][47]
6.2  | 1st June 2003        |        | June 1           | [June 1][29]       | June 2  | [Jun 1][48]
6.1  | 24th March 2002      |        | Mar 24           | [Mar 24][28]       | Mar 25  | [Mar 24][49]
6.0  | 27th September, 2001 | Sep 27 | Sep 26           | [Sep 27][27]       | Sep 27  | [Sep 26][50]
5.8  | 31st May, 2001       |        | May 31           | [May 31][26]       | June 5  | [May 31][51]
5.7  | 24th June, 2000      |        | June 24          | [June 24][25]      | June 24 | [Jun 24][52]
5.6  | 16th January, 2000   |        | Jan 16           | [Jan 16][24]       | Jan 17  | [Jan 16][53]
5.5  | 21st September, 1999 |        | Sep 19           | [Sep 21][23]       |         | [Sep 19][54]
5.4  | 26th July, 1999      |        | July 25          | [July 26][22]      |         | [Jul 25][55]
5.3  | 31st August, 1998    |        | Aug 31           | [Aug 31][21]       |         | [Aug 30][56]
5.2  | 24th August, 1998    |        | **Apr 27** \[1\] | [Aug 24][20]       |         | [Aug 23][57]
5.1  | 7th April, 1998      |        | Apr 6            | [Apr 7][19]        |         | [Apr 7][58]
5.0  | 19th February, 1998  | Feb 28 | Feb 19           | [Feb 19][18]       |         | [Feb 19][59]
4.6  | 13th March, 1997     |        |                  | [Mar 13][17]       |         | [Mar 13][60]
4.5  | 17th October, 1996   |        |                  | [Oct 17][13]       |         | [Oct 12][61]
4.2  | 5th July, 1996       |        |                  | [Jun 21][14]       |         | [June 17][62]
4.0  | 21st May, 1996       | May 29 | May 29           |                    |         | [May 29][63]
3.0  | 16th August, 1994    | Aug 12 | Aug 12           | [Sep 7][15] \[2\]  |         | [Aug 12][64]
2.0  | 21st December, 1993  | Dec 21 | Dec 14           | [Dec 31][16] \[2\] |         | [Dec 14][65]
1.27 | 23rd April, 1993     | Apr 23 |                  |                    |         | [April 6][66]
1.24 |                      |        |                  |                    |         | [5 Jan 1993][67]
1.17 | 21st April, 1992     |        |                  |                    |         | 
1.14 | 2nd November, 1991   | Nov 2  |                  |                    |         | 

\[1\]: I think it is an error in the wikipedia page and very unlikely, that Vim 5.2 was released just 20 days after 5.1  
\[2\]: Announced by Darren Hiebert


Merge the whole history
-----------------------

If you want to see the whole history of Vim in your one local repository, try this:

```bash
git clone https://github.com/vim/vim.git
cd vim
git remote add vim-history https://github.com/vim/vim-history.git
git fetch vim-history --tags
git replace --graft v7.0001 vim-6-3-004
```

This virtually connects the tag `v7.0001` on top of the tag `vim-6-3-004`. The development of Vim 7.0 was branched from around Vim 6.3.


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
[11]: https://github.com/chrisbra/vim_faq/issues/8#issuecomment-419605586
[12]: https://groups.google.com/forum/#!forum/comp.editors
[13]: https://groups.google.com/d/msg/comp.editors/myQlVk5RvEQ/5yk_g2GM9EEJ
[14]: https://groups.google.com/d/msg/comp.editors/GjdHci-xBSw/Iq3kSrDJ36QJ
[15]: https://groups.google.com/d/msg/comp.editors/iSbQgMlWweU/bbPMSBcCkUsJ
[16]: https://groups.google.com/d/msg/comp.editors/9snlhebqQiA/KOi0VsDXUzYJ
[17]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/10
[18]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/26
[19]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/30
[20]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/34
[21]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/35
[22]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/49
[23]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/53
[24]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/59
[25]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/69
[26]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/78
[27]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/88
[28]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/97
[29]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/115
[30]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/130
[31]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/144
[32]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/161
[33]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/178
[34]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/190
[35]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/204
[36]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/221
[37]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/242
[38]: https://groups.yahoo.com/neo/groups/vimannounce/conversations/messages/257
[39]: https://github.com/vim/vim/blob/v8.1.0000/src/version.h#L39
[40]: https://github.com/vim/vim/blob/v8.0.0000/src/version.h#L39
[41]: https://github.com/vim/vim/blob/v7.4/src/version.h#L39
[42]: https://github.com/vim/vim/blob/v7.3/src/version.h#L39
[43]: https://github.com/vim/vim/blob/v7.2/src/version.h#L39
[44]: https://github.com/vim/vim/blob/v7.1/src/version.h#L39
[45]: https://github.com/vim/vim/blob/v7.0/src/version.h#L39
[46]: https://github.com/vim/vim-history/blob/vim-6-4/src/version.h#L39
[47]: https://github.com/vim/vim-history/blob/vim-6-3/src/version.h#L39
[48]: https://github.com/vim/vim-history/blob/vim-6-2/src/version.h#L39
[49]: https://github.com/vim/vim-history/blob/vim-6-1/src/version.h#L39
[50]: https://github.com/vim/vim-history/blob/vim-6-0/src/version.h#L39
[51]: https://github.com/vim/vim-history/blob/vim-5-8-000/src/version.h#L37
[52]: https://github.com/vim/vim-history/blob/vim-5-7/src/version.h#L37
[53]: https://github.com/vim/vim-history/blob/vim-5-6/src/version.h#L34
[54]: https://github.com/vim/vim-history/blob/v5.5/src/version.h#L34
[55]: https://github.com/vim/vim-history/blob/v5.4/src/version.h#L34
[56]: https://github.com/vim/vim-history/blob/v5.3/src/version.h#L31
[57]: https://github.com/vim/vim-history/blob/v5.2/src/version.h#L31
[58]: https://github.com/vim/vim-history/blob/v5.1/src/version.h#L31
[59]: https://github.com/vim/vim-history/blob/v5.0/src/version.h#L31
[60]: https://github.com/vim/vim-history/blob/v4.6/src/version.c#L527
[61]: https://github.com/vim/vim-history/blob/v4.5/src/version.c#L342
[62]: https://github.com/vim/vim-history/blob/v4.2/src/version.c#L140
[63]: https://github.com/vim/vim-history/blob/v4.0/src/version.c#L28
[64]: https://github.com/vim/vim-history/blob/v3.0/src/version.c#L277
[65]: https://github.com/vim/vim-history/blob/v2.0/src/version.c#L277
[66]: https://github.com/vim/vim-history/blob/v1.27/src/version.c#L253
[67]: https://github.com/vim/vim-history/blob/v1.24/src/version.c#L200
[68]: https://groups.google.com/d/msg/vim_announce/KPqcD9MYiMk/T11rMwO1AAAJ
[69]: https://github.com/vim/vim/blob/v8.2.0/src/version.h#L39
