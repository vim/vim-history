" Vim filetype plugin file
" Language:	man
" Maintainer:	Sung-Hyun Nam <namsh@kldp.org>
" Last Change:	2000 Oct 21

" To make the ":Man" command available before editing a manual page, source
" this script from your startup vimrc file.

" If 'filetype' isn't "man", we must have been called to only define ":Man".
if &filetype == "man"

  " Only do this when not done yet for this buffer
  if exists("b:did_ftplugin")
    finish
  endif
  let b:did_ftplugin = 1

  " allow dot in manual page name.
  setlocal iskeyword+=\.

  " Add mappings, unless the user didn't want this.
  if !exists("no_plugin_maps") && !exists("no_man_maps")
    if !hasmapto('<Plug>ManBS')
      nmap <buffer> <LocalLeader>h <Plug>ManBS
    endif
    nnoremap <buffer> <Plug>ManBS :%s/.\b//g<CR>:set nomod<CR>''

    nnoremap <buffer> <c-]> :call <SID>GetPage(expand("<cword>"))<CR>
    nnoremap <buffer> <c-t> :call <SID>PopPage()<CR>
  endif

endif

if !exists(":Man")
  com -nargs=1 Man call <SID>GetPage(<q-args>)
endif

" Define functions only once.
if !exists("s:man_tag_depth")

let s:man_tag_depth = 0

func <SID>GetPage(f)
  exec "let s:man_tag_".s:man_tag_depth."_buf = ".bufnr("%")
  exec "let s:man_tag_".s:man_tag_depth."_line = ".line(".")
  exec "let s:man_tag_".s:man_tag_depth."_col = ".col(".")
  let s:man_tag_depth = s:man_tag_depth + 1
  exec "new $HOME/".a:f
  only
  exec "r!/usr/bin/man ".a:f." | col -b"
  " Is it OK?  It's for remove blank or message line.
  exec "norm 2G/^[^\s]\<CR>kd1G"
  setl ft=man nomod
endfunc

func <SID>PopPage()
  if s:man_tag_depth > 0
    let s:man_tag_depth = s:man_tag_depth - 1
    exec "let s:man_tag_buf=s:man_tag_".s:man_tag_depth."_buf"
    exec "let s:man_tag_line=s:man_tag_".s:man_tag_depth."_line"
    exec "let s:man_tag_col=s:man_tag_".s:man_tag_depth."_col"
    exec s:man_tag_buf."b"
    exec s:man_tag_line
    exec "norm ".s:man_tag_col."|"
    exec "unlet s:man_tag_".s:man_tag_depth."_buf"
    exec "unlet s:man_tag_".s:man_tag_depth."_line"
    exec "unlet s:man_tag_".s:man_tag_depth."_col"
    unlet s:man_tag_buf s:man_tag_line s:man_tag_col
  endif
endfunc

endif

" vim: set sw=2:
