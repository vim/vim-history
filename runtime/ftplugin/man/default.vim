" Vim filetype plugin file
" Language:	man
" Maintainer:	Sung-Hyun Nam <namsh@kldp.org>
" Last Change:	2000 Oct 25

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

    nnoremap <buffer> <c-]> :call <SID>PreGetPage(v:count)<CR>
    nnoremap <buffer> <c-t> :call <SID>PopPage()<CR>
  endif

endif

if !exists(":Man")
  com -nargs=1 Man call <SID>GetPage(<q-args>)
endif

" Define functions only once.
if !exists("s:man_tag_depth")

let s:man_tag_depth = 0

if $OSTYPE =~ "solaris"
  let s:man_sect_arg = "-s"
  let s:man_find_arg = "-l"
else
  let s:man_sect_arg = ""
  let s:man_find_arg = "-w"
endif

func <SID>PreGetPage(cnt)
  if a:cnt == 0
    let old_isk = &iskeyword
    setl iskeyword+=(,)
    let str = expand("<cword>")
    let &iskeyword = old_isk
    let page = substitute(str, '\(\k\+\).*', '\1', '')
    let sect = substitute(str, '\(\k\+\)(\([^()]*\)).*', '\2', '')
    if match(sect, '^[0-9 ]\+$') == -1
      let sect = ""
    endif
    if sect == page
      let sect = ""
    endif
  else
    let sect = a:cnt
    let page = expand("<cword>")
  endif
  call <SID>GetPage(sect, page)
endfunc

func <SID>GetPage(...)
  if a:0 >= 2
    let sect = a:1
    let page = a:2
  elseif a:0 >= 1
    let sect = ""
    let page = a:1
  else
    return
  endif
  if sect != ""
    let where = system("/usr/bin/man ".s:man_find_arg." ".sect." ".page)
    if where !~ "^/"
      let sect = ""
    endif
  endif
  let where = system("/usr/bin/man ".s:man_find_arg." ".sect." ".page)
  if where !~ "^/"
    echo "\nCannot find a '".page."'."
    return
  endif
  exec "let s:man_tag_buf_".s:man_tag_depth." = ".bufnr("%")
  exec "let s:man_tag_lin_".s:man_tag_depth." = ".line(".")
  exec "let s:man_tag_col_".s:man_tag_depth." = ".col(".")
  let s:man_tag_depth = s:man_tag_depth + 1

  " Use an existing "man" window if it exists, otherwise open a new one.
  if &filetype != "man"
    let thiswin = winnr()
    exe "norm! \<C-W>b"
    if winnr() == 1
      new
    else
      exe "norm! " . thiswin . "\<C-W>w"
      while 1
	if &filetype == "man"
	  break
	endif
	exe "norm! \<C-W>w"
	if thiswin == winnr()
	  new
	  break
	endif
      endwhile
    endif
  endif
  exec "edit $HOME/".page

  exec "norm 1GdG"
  exec "r!/usr/bin/man ".s:man_sect_arg." ".sect." ".page." | col -b"
  " Is it OK?  It's for remove blank or message line.
  if getline(1) =~ "^\s*$"
    exec "norm 2G/^[^\s]\<cr>kd1G"
  endif
  setl ft=man nomod
endfunc

func <SID>PopPage()
  if s:man_tag_depth > 0
    let s:man_tag_depth = s:man_tag_depth - 1
    exec "let s:man_tag_buf=s:man_tag_buf_".s:man_tag_depth
    exec "let s:man_tag_lin=s:man_tag_lin_".s:man_tag_depth
    exec "let s:man_tag_col=s:man_tag_col_".s:man_tag_depth
    exec s:man_tag_buf."b"
    exec s:man_tag_lin
    exec "norm ".s:man_tag_col."|"
    exec "unlet s:man_tag_buf_".s:man_tag_depth
    exec "unlet s:man_tag_lin_".s:man_tag_depth
    exec "unlet s:man_tag_col_".s:man_tag_depth
    unlet s:man_tag_buf s:man_tag_lin s:man_tag_col
  endif
endfunc

endif

" vim: set sw=2:
