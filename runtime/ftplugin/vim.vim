" Vim filetype plugin
" Language:	Vim
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Jul 30

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" Set 'comments' to format dashed lists in comments
setlocal com=sO:\"\ -,mO:\"\ \ eO:\"\",:\"

" Format comments to be up to 78 characters long
setlocal tw=78

" Let the matchit plugin know what items can be matched.
if exists("loaded_matchit")
  let b:match_ignorecase=0
  let b:match_words=
	\ '\<fun\%[ction]\>:\<retu\%[rn]\>:\<endf\%[unction]\>,' .
	\ '\<while\>:\<break\>:\<con\%[tinue]\>:\<endw\%[hile]\>,' .
	\ '\<if\>:\<el\%[seif]\>:\<en\%[dif]\>,' .
	\ '\<aug\%[roup]\s\+\%(END\>\)\@!\S:\<aug\%[roup]\s\+END\>,' .
	\ '(:)'
endif

