" Vim filetype plugin
" Language:     Ruby
" Maintainer:   Matt Armstrong <matt@lickey.com>
" Last Change:  2001/03/24
" URL: http://www.lickey.com/env/vim/ftplugin/ruby/ruby.vim

" Only do this when not done yet for this buffer
if (exists("b:did_ftplugin"))
    finish
endif
let b:did_ftplugin = 1

" Set 'formatoptions' to break comment lines but not other lines, and insert
" the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" Format comments to be up to 78 characters long
setlocal tw=78

" Set shift width to 2, the Ruby standard
setlocal sw=2
