" Vim filetype plugin file
" Language:	Mail
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Oct 15

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

" Don't use modelines in e-mail messages, avoid trojan horses
setlocal nomodeline

" many people recommend keeping e-mail messages 72 chars wide
setlocal tw=72

" Set 'formatoptions' to break text lines,
" and insert the comment leader ">" when hitting <CR> or using "o".
setlocal fo+=tcroql

" Add mappings, unless the user didn't want this.
if !exists("no_plugin_maps") && !exists("no_mail_maps")
  " Quote text by inserting "> "
  if !hasmapto('\\MailQuote')
    vmap <buffer> <LocalLeader>q \\MailQuote
    nmap <buffer> <LocalLeader>q \\MailQuote
  endif
  vmap <buffer> \\MailQuote :s/^/> /<CR>
  nmap <buffer> \\MailQuote :.,$s/^/> /<CR>
endif
