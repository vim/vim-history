" Vim syntax file
" Language:	none; used to see highlighting
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Feb 15

" Written by Ronald Schild <rs@dps.de>
" to see your current highlight settings: do ":so <thisfile>"

" save current options
let save_more = &more
let save_lz   = &lz
let save_ws   = &ws
let save_bk   = &bk
let save_wb   = &wb

set nomore lz ws nobk nowb
set sbr=\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ 
" (24 * "\ ")

" print current highlight settings into temporary file
let file = tempname()
exe "redir! >" . file
highlight
redir END

" edit temporary file
exe "edit " . file
set wrap ts=24 linebreak

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

" remove temporary file
let result = delete(file)

" remove 'hls' highlighing
g/^asdfasdf$/p

" restore previous options
let &more = save_more
let &lz   = save_lz
let &ws   = save_ws
let &bk   = save_bk
let &wb   = save_wb
