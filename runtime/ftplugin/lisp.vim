" Vim filetype plugin
" Language:      Lisp
" Maintainer:    Dorai Sitaram <ds26@gte.com>
" URL:		 http://www.ccs.neu.edu/~dorai/vimplugins/vimplugins.html
" Last Change:   Apr 5, 2003

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

setl autoindent
setl comments=:;
setl define=^\\s*(def[-a-z]*
setl formatoptions-=t
setl isident+=+,-,*,%,<,=,>,:,$,?,!,@-@,94
setl lisp

let &l:iskeyword = &isident
setl iskeyword+=/
