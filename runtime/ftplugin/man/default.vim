" Vim filetype plugin file
" Language:	man
" Maintainer:	Sung-Hyun Nam <namsh@kldp.org>
" Last Change:	2000 Oct 15

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

" allow dot in manual page name.
setlocal iskeyword+=\.

" Add mappings, unless the user didn't want this.
if !exists("no_plugin_maps") && !exists("no_man_maps")
  if !hasmapto('\\ManBS')
    nmap <buffer> <Map>h \\ManBS
  endif
  nmap <buffer> \\ManBS :%s/.\b//g<cr>:set nomod<cr>''

  nmap <buffer> <c-]> :call MAN_GetPage(expand("<cword>"))<cr>
  nmap <buffer> <c-t> :call MAN_PopPage()<cr>
endif

com! -nargs=1 Man call MAN_GetPage(<q-args>)

" Define functions only once.
if !exists("g:man_tag_depth")

let g:man_tag_depth = 0

func! MAN_GetPage(f)
  exec "let g:man_tag_".g:man_tag_depth."_buf = ".bufnr("%")
  exec "let g:man_tag_".g:man_tag_depth."_line = ".line(".")
  exec "let g:man_tag_".g:man_tag_depth."_col = ".col(".")
  let g:man_tag_depth = g:man_tag_depth + 1
  exec "new $HOME/".a:f
  only
  exec "r!/usr/bin/man ".a:f." | col -b"
  " Is it OK?  It's for remove blank or message line.
  exec "norm 2G/^[^\s]\<cr>kd1G"
  setl ft=man nomod
endfunc

func! MAN_PopPage()
  if g:man_tag_depth > 0
    let g:man_tag_depth = g:man_tag_depth - 1
    exec "let g:man_tag_buf=g:man_tag_".g:man_tag_depth."_buf"
    exec "let g:man_tag_line=g:man_tag_".g:man_tag_depth."_line"
    exec "let g:man_tag_col=g:man_tag_".g:man_tag_depth."_col"
    exec g:man_tag_buf."b"
    exec g:man_tag_line
    exec "norm ".g:man_tag_col."|"
    exec "unlet g:man_tag_".g:man_tag_depth."_buf"
    exec "unlet g:man_tag_".g:man_tag_depth."_line"
    exec "unlet g:man_tag_".g:man_tag_depth."_col"
    unlet g:man_tag_buf g:man_tag_line g:man_tag_col
  endif
endfunc

endif
