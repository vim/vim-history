" autocommands to edit compressed files.
" Maintainer: Bram Moolenaar <Bram@vim.org>

" Exit quickly when already loaded or when 'compatible' is set.
if exists("loaded_gzip") || &cp
  finish
endif
let loaded_gzip = 1

augroup gzip
  " Remove all gzip autocommands
  au!

  " Enable editing of gzipped files
  " set binary mode before reading the file
  " use "gzip -d", gunzip isn't always available
  autocmd BufReadPre,FileReadPre	*.gz,*.bz2,*.Z set bin
  autocmd BufReadPost,FileReadPost	*.gz  call s:read("gzip -d")
  autocmd BufReadPost,FileReadPost	*.bz2 call s:read("bzip2 -d")
  autocmd BufReadPost,FileReadPost	*.Z   call s:read("uncompress")
  autocmd BufWritePost,FileWritePost	*.gz  call s:write("gzip")
  autocmd BufWritePost,FileWritePost	*.bz2 call s:write("bzip2")
  autocmd BufWritePost,FileWritePost	*.Z   call s:write("compress -f")
  autocmd FileAppendPre			*.gz  call s:appre("gzip -d")
  autocmd FileAppendPre			*.bz2 call s:appre("bzip2 -d")
  autocmd FileAppendPre			*.Z   call s:appre("uncompress")
  autocmd FileAppendPost		*.gz  call s:write("gzip")
  autocmd FileAppendPost		*.bz2 call s:write("bzip2")
  autocmd FileAppendPost		*.Z   call s:write("compress -f")

  " Function to check that executing "cmd" works.
  " The result is cached in s:have_"cmd" for speed.
  fun s:check(cmd)
    let name = substitute(a:cmd, '\(\S*\).*', '\1', '')
    if !exists("s:have_" . name)
      let r = system(a:cmd . " --version")
      exe "let s:have_" . name . "=" . (r !~ "not found" && r != "")
    endif
    exe "return s:have_" . name
  endfun

  " After reading compressed file: Uncompress text in buffer with "cmd"
  fun s:read(cmd)
    " don't do anything if the cmd is not supported
    if !s:check(a:cmd)
      return
    endif
    " set 'cmdheight' to two, to avoid the hit-return prompt
    let ch_save = &ch
    set ch=3
    " when filtering the whole buffer, it will become empty
    let empty = line("'[") == 1 && line("']") == line("$")
    let tmp = tempname()
    let tmpe = tmp . "." . expand("<afile>:e")
    " write the just read lines to a temp file "'[,']w tmp.gz"
    execute "'[,']w " . tmpe
    " uncompress the temp file: call system("gzip -d tmp.gz")
    call system(a:cmd . " " . tmpe)
    " delete the compressed lines
    '[,']d
    " read in the uncompressed lines "'[-1r tmp"
    set nobin
    execute "'[-1r " . tmp
    " if buffer became empty, delete trailing blank line
    if empty
      normal Gdd''
    endif
    " delete the temp file
    call delete(tmp)
    let &ch = ch_save
    " When uncompressed the whole buffer, do autocommands
    if empty
      execute ":silent! doau BufReadPost " . expand("%:r")
    endif
  endfun

  " After writing compressed file: Compress written file with "cmd"
  fun s:write(cmd)
    " don't do anything if the cmd is not supported
    if s:check(a:cmd)
      if rename(expand("<afile>"), expand("<afile>:r")) == 0
	call system(a:cmd . " " . expand("<afile>:r"))
      endif
    endif
  endfun

  " Before appending to compressed file: Uncompress file with "cmd"
  fun s:appre(cmd)
    " don't do anything if the cmd is not supported
    if s:check(a:cmd)
      call system(a:cmd . " " . expand("<afile>"))
      call rename(expand("<afile>:r"), expand("<afile>"))
    endif
  endfun

augroup END

" vim: set sw=2 :
