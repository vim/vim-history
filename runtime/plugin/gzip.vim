" Vim plugin for editing compressed files.
" Maintainer: Bram Moolenaar <Bram@vim.org>
" Last Change: 2001 Sep 20

" Exit quickly when:
" - this plugin was already loaded
" - when 'compatible' is set
" - some autocommands are already taking care of compressed files
if exists("loaded_gzip") || &cp || exists("#BufReadPre#*.gz")
  finish
endif
let loaded_gzip = 1

augroup gzip
  " Remove all gzip autocommands
  au!

  " Enable editing of gzipped files
  " set binary mode before reading the file
  " use "gzip -d", gunzip isn't always available
  autocmd BufReadPre,FileReadPre	*.gz,*.bz2,*.Z setlocal bin
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
augroup END

" Function to check that executing "cmd [-f]" works.
" The result is cached in s:have_"cmd" for speed.
fun s:check(cmd)
  let name = substitute(a:cmd, '\(\S*\).*', '\1', '')
  if !exists("s:have_" . name)
    let e = executable(name)
    if e < 0
      let r = system(name . " --version")
      let e = (r !~ "not found" && r != "")
    endif
    exe "let s:have_" . name . "=" . e
  endif
  exe "return s:have_" . name
endfun

" After reading compressed file: Uncompress text in buffer with "cmd"
fun s:read(cmd)
  " don't do anything if the cmd is not supported
  if !s:check(a:cmd)
    return
  endif
  " make 'patchmode' empty, we don't want a copy of the written file
  let pm_save = &pm
  set pm=
  " set 'modifiable'
  let ma_save = &ma
  setlocal ma
  " when filtering the whole buffer, it will become empty
  let empty = line("'[") == 1 && line("']") == line("$")
  let tmp = tempname()
  let tmpe = tmp . "." . expand("<afile>:e")
  " write the just read lines to a temp file "'[,']w tmp.gz"
  execute "silent '[,']w " . tmpe
  " uncompress the temp file: call system("gzip -d tmp.gz")
  call system(a:cmd . " " . tmpe)
  " delete the compressed lines
  '[,']d
  " read in the uncompressed lines "'[-1r tmp"
  setlocal nobin
  execute "silent '[-1r " . tmp
  " if buffer became empty, delete trailing blank line
  if empty
    silent $delete
    1
  endif
  " delete the temp file and the used buffers
  call delete(tmp)
  silent! exe "bwipe " . tmp
  silent! exe "bwipe " . tmpe
  let &pm = pm_save
  let &l:ma = ma_save
  " When uncompressed the whole buffer, do autocommands
  if empty
    execute ":silent! doau BufReadPost " . expand("%:r")
  endif
endfun

" After writing compressed file: Compress written file with "cmd"
fun s:write(cmd)
  " don't do anything if the cmd is not supported
  if s:check(a:cmd)
    " Rename to a weird name to avoid the risk of overwriting another file
    let nm = expand("<afile>")
    let nmt = expand("<afile>:p:h") . "/X~=@l9q5"
    if rename(nm, nmt) == 0
      call system(a:cmd . " " . nmt)
      call rename(nmt . "." . expand("<afile>:e"), nm)
    endif
  endif
endfun

" Before appending to compressed file: Uncompress file with "cmd"
fun s:appre(cmd)
  " don't do anything if the cmd is not supported
  if s:check(a:cmd)
    " Rename to a weird name to avoid the risk of overwriting another file
    let nm = expand("<afile>")
    let nmt = expand("<afile>:p:h") . "/X~=@l9q5"
    let nmte = nmt . "." . expand("<afile>:e")
    if rename(nm, nmte) == 0
      if &patchmode != "" && getfsize(nm . &patchmode) == -1
	" Create patchmode file by creating the decompressed file new
	call system(a:cmd . " -c " . nmte . " > " . nmt)
	call rename(nmte, nm . &patchmode)
      else
	call system(a:cmd . " " . nmte)
      endif
      call rename(nmt, nm)
    endif
  endif
endfun

" vim: set sw=2 :
