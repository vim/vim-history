" Vim settings file
" Language:	Mail
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Aug 31

" Only do these settings when not done yet for this buffer
if exists("b:did_settings")
  finish
endif

" Don't do other file type settings for this buffer
let b:did_settings = 1

" Don't use modelines in e-mail messages, avoid trojan horses
setlocal nomodeline

" many people recommend keeping e-mail messages 72 chars wide
setlocal tw=72

" Set 'formatoptions' to break text lines,
" and insert the comment leader ">" when hitting <CR> or using "o".
setlocal fo+=tcroql

" Add mappings, unless the user didn't want this.
if !exists("no_settings_maps") && !exists("no_mail_maps")
  " Quote text by inserting "> "
  vmap <buffer> <Map>q :s/^/> /<CR>
  nmap <buffer> <Map>q :.,$s/^/> /<CR>
endif
