" Vim syntax file
" Language:	none; used to see highlighting
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Mar 7

" Written by Ronald Schild <rs@dps.de>
" to see your current highlight settings: do ":so $VIM/syntax/hitest.vim"

" save current options
let save_more   = &more
let save_lz     = &lz
let save_ws     = &ws
let save_bk     = &bk
let save_wb     = &wb
let save_report = &report
let save_shm    = &shm
let save_hid    = &hid

set nomore lz ws nobk nowb report=9999 shm=aostW hidden

" print current highlight settings into temporary file
let file = tempname()
exe "redir! >" . file
highlight
redir END

" edit temporary file
exe "edit " . file
set ts=16 sw=16 noet fo=t ai
let &tw=&co

" remove color settings (not needed here)
g!/links to/s/\s.*$//

" move linked groups to the end of file
g/links to/m$

" move linked group names to the matching preferred groups
g/links to/normal mzdEWWW#A p'zD

" delete empty lines
g/^ *$/d

" precede syntax command (one <tab> between & and &)
%s/^[^ ]*/syn keyword &	&/
1i
syn clear
.
w
so %

" remove syntax commands again
1d
%s/^syn keyword //

" pretty formatting
g/.*/normal Wi	AAx
g/^\S/j

" remove 'hls' highlighting
s/^//

set nomod

" remove temporary file
let result = delete(file)

" restore previous options
let &more   = save_more
let &lz     = save_lz
let &ws     = save_ws
let &bk     = save_bk
let &wb     = save_wb
let &report = save_report
let &shm    = save_shm
let &hid    = save_hid

" remove variables
unlet save_more
unlet save_lz
unlet save_ws
unlet save_bk
unlet save_wb
unlet save_report
unlet save_shm
unlet save_hid
unlet file
unlet result
