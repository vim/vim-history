" netrw.vim: Handles the reading and writing of file(s) across a network
"
"  Author:  Charles E. Campbell, Jr. PhD   <cec@NgrOyphSon.gPsfAc.nMasa.gov>
"           scp  support by raf            <raf@comdyn.com.au>
"           http support by Bram Moolenaar <bram@moolenaar.net>
"  Date:    October 26, 2000
"  Version: 2.05
"

" Reading:
" :Nr machine:file                  uses rcp
" :Nr "machine file"                uses ftp with <.netrc>
" :Nr "machine id password file"    uses ftp
" :Nr "ftp://machine/file"          uses ftp  (autodetects <.netrc>)
" :Nr "http://[user@]machine/file"  uses http (wget)
" :Nr "rcp://machine/file"          uses rcp
" :Nr "scp://[user@]machine/file"   uses scp

" Writing:
" :Nw machine:file                  uses rcp
" :Nw "machine file"                uses ftp with <.netrc>
" :Nw "machine id password file"    uses ftp
" :Nw "ftp://machine/file"          uses ftp  (autodetects <.netrc>)
" :Nw "rcp://machine/file"          uses rcp
" :Nw "scp://[user@]machine/file"   uses scp
" http: not supported!

" User And Password Changing:
"  Attempts to use ftp will prompt you for a user-id and a password.
"  These will be saved in b:netrw_uid and b:netrw_passwd
"  Subsequent uses of ftp will re-use those.  If you need to use
"  a different user id and/or password, you'll want to
"  call NetUserPass() first.

"   :call NetUserPass()                 -- will prompt for userid and password
"   :call NetUserPass("uid")            -- will prompt for password
"   :call NetUserPass("uid","password") -- sets global userid and password

"  This version of <netrw.vim> borrows some ideas from Michael Geddes
"  in the "invisible password" input code.
"  But be doers of the word, and not only hearers, deluding your own selves
"  (James1:22 RSV)
" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

" Exit quickly when already loaded or when 'compatible' is set.
if exists("loaded_netrw") || &cp
  finish
endif
let loaded_netrw = 1

" Vimrc Support:
" Auto-detection for ftp://*, rcp://*, scp://*, and http://*
" Should make file transfers across networks transparent.  Currently I haven't
" supported appends.  Hey, gotta leave something for <netrw.vim> version 3!
if version >= 600
  augroup Network
    au!
    au BufReadCmd	ftp://*,rcp://*,scp://*,http://* exe "Nr 0r " . expand("<afile>") | exe "doau BufReadPost " . expand("<afile>")
    au FileReadCmd	ftp://*,rcp://*,scp://*,http://* exe "Nr "    . expand("<afile>") | exe "doau BufReadPost " . expand("<afile>")
    au BufWriteCmd	ftp://*,rcp://*,scp://* exe "Nw "    . expand("<afile>")
  augroup END
endif

" ------------------------------------------------------------------------

" Commands: :Nr and :Nw
:com -nargs=* Nr call NetRead(<f-args>)
:com -range=% -nargs=* Nw <line1>,<line2>call NetWrite(<f-args>)

" ------------------------------------------------------------------------

" NetRead: Nr:
function! NetRead(...)
" echo "DBG: NetRead(a:1<".a:1.">) {"

 " Get Temporary Filename
 let tmpfile = tempname()

 " Special Exception: if a file is named "0r", then
 "                    "0r" will be used to read the
 "                    following files instead of "r"
 if     a:0 == 0
  let readcmd= "r"
  let ichoice= 0
 elseif a:1 == "0r"
  let readcmd = "0r"
  let ichoice = 2
 else
  let readcmd = "r"
  let ichoice = 1
 endif

 while ichoice <= a:0

  " attempt to repeat with previous host-file-etc
  if exists("b:netrw_lastfile") && a:0 == 0
"   echo "DBG: using b:netrw_lastfile<" . b:netrw_lastfile . ">"
   let choice = b:netrw_lastfile
   let ichoice= ichoice + 1
  else
   exe "let choice= a:" . ichoice
"   echo "DBG: NetRead1: choice<" . choice . ">"

   " Reconstruct Choice if choice starts with '"'
   if match(choice,"?") == 0
    echo "Usage:"
	echo "  :Nr machine:file                       uses rcp"
	echo "  :Nr \"machine file\"                     uses ftp with <.netrc>"
	echo "  :Nr \"machine id password file\"         uses ftp"
	break
   elseif match(choice,"^\"") != -1
"    echo "DBG: reconstructing choice"
    if match(choice,"\"$") != -1
     " case "..."
 	 let choice=strpart(choice,1,strlen(choice)-2)
    else
    "  case "... ... ..."
 	 let choice      = strpart(choice,1,strlen(choice)-1)
     let wholechoice = ""

     while match(choice,"\"$") == -1
      let wholechoice = wholechoice . " " . choice
      let ichoice     = ichoice + 1
      exe "let choice= a:" . ichoice
     endwhile
 	 let choice= strpart(wholechoice,1,strlen(wholechoice)-1) . " " . strpart(choice,0,strlen(choice)-1)
    endif
   endif
  endif
"  echo "DBG: NetRead2: choice<" . choice . ">"
  let ichoice= ichoice + 1

  " Determine method of read (ftp, rcp, etc)
  call NetMethod(choice)

  " Perform Read
  if     b:netrw_method  == 1	" read with rcp
"   echo "DBG: read via rcp (method #1)"
   exe "!rcp " . b:netrw_machine . ":" . b:netrw_fname . " " . tmpfile
   let result = NetGetFile(readcmd, tmpfile)
   let b:netrw_lastfile = choice

  elseif b:netrw_method  == 2		" read with ftp + <.netrc>
"   echo "DBG: read via ftp+.netrc (method #2)\n"
"   echo "DBG: this line gets wiped out"
   exe "norm mzoascii\<cr>get ".b:netrw_fname." ".tmpfile."\<esc>"
   exe "'z+1,.!ftp -i " . b:netrw_machine
   let result = NetGetFile(readcmd, tmpfile)
   let b:netrw_lastfile = choice

  elseif b:netrw_method == 3		" read with ftp + machine, id, passwd, and fname
"   echo "DBG: read via ftp+mipf (method #3)"
"   echo "DBG: this line gets wiped out"
   exe "norm mzouser ".b:netrw_uid." ".b:netrw_passwd."\<cr>ascii\<cr>get ".b:netrw_fname." ".tmpfile."\<esc>"

   if has("win32")
    exe "norm o\<esc>my"
    exe "'z+1,'y-1!ftp -i -n " . b:netrw_machine
	" the ftp on Win95 puts four lines of trash at the end
	" which the following blots out.  Does Win-NT/98/2000 do the same???
	exe "'y-3,'yd"
   elseif filereadable(expand("$HOME/.netrc"))
    " -n, unix : DON'T use <.netrc>, even though it exists
	"     win32: quit being obnoxious about password
    exe "'z+1,.!ftp -i -n " . b:netrw_machine
   else
    " ordinary ftp
    exe "'z+1,.!ftp -i " . b:netrw_machine
   endif
   norm 'z
   let result = NetGetFile(readcmd, tmpfile)

   " save choice/id/password for future use
   let b:netrw_lastfile = choice

  elseif     b:netrw_method  == 4	" read with scp
"   echo "DBG: read via scp (method #4)"
   exe "!scp " . b:netrw_machine . ":" . b:netrw_fname . " " . tmpfile
   let result = NetGetFile(readcmd, tmpfile)
   let b:netrw_lastfile = choice

  elseif     b:netrw_method  == 5	" read with http (wget)
"   echo "DBG: read via http (method #5)"
   exe "!wget http://" . b:netrw_machine . "/" . b:netrw_fname . " -O " . tmpfile
   let result = NetGetFile(readcmd, tmpfile)
   let b:netrw_lastfile = choice

  else " Complain
   echo "***warning*** unable to comply with your request<" . choice . ">"
  endif
 endwhile

 " cleanup
 unlet b:netrw_method
 unlet b:netrw_machine
 unlet b:netrw_fname

" echo "DBG: return NetRead }"
endfunction
" end of NetRead

" Function to read file "fname" with command "readcmd".
" Takes care of deleting the last line when the buffer was emtpy.
" Deletes the file "fname".
func NetGetFile(readcmd, fname)
  let dodel = 0
  if line("$") == 1 && getline(1) == ""
    let dodel = 1
  endif
  exe a:readcmd . " " . a:fname
  if a:readcmd[0] == '0' && dodel && getline("$") == ""
    $d
    1
  endif
  return delete(a:fname)
endfun

" ------------------------------------------------------------------------

" NetWrite: Nw:
function! NetWrite(...) range
" echo "DBG: NetWrite(a:0=".a:0.") {"

 " Get Temporary Filename
 let tmpfile    = tempname()
 let tmpFTPfile = tempname()

 if a:0 == 0
  let ichoice = 0
 else
  let ichoice = 1
 endif

 " write (selected portion of) file to temporary
 exe a:firstline . "," . a:lastline . "w! " . tmpfile

 while ichoice <= a:0

  " attempt to repeat with previous host-file-etc
  if exists("b:netrw_lastfile") && a:0 == 0
"    	 echo "DBG: using b:netrw_lastfile<" . b:netrw_lastfile . ">"
   let choice = b:netrw_lastfile
   let ichoice= ichoice + 1
  else
   exe "let choice= a:" . ichoice

   " Reconstruct Choice if choice starts with '"'
   if match(choice,"?") == 0
    echo "Usage:"
	echo "  :Nw machine:file                       uses rcp"
	echo "  :Nw \"machine file\"                     uses ftp with <.netrc>"
	echo "  :Nw \"machine id password file\"         uses ftp"
	break
   elseif match(choice,"^\"") != -1
    if match(choice,"\"$") != -1
     " case "..."
 	 let choice=strpart(choice,1,strlen(choice)-2)
    else
    "  case "... ... ..."
 	 let choice      = strpart(choice,1,strlen(choice)-1)
     let wholechoice = ""

     while match(choice,"\"$") == -1
      let wholechoice= wholechoice . " " . choice
      let ichoice= ichoice + 1
      exe "let choice= a:" . ichoice
     endwhile
 	let choice= strpart(wholechoice,1,strlen(wholechoice)-1) . " " . strpart(choice,0,strlen(choice)-1)
    endif
   endif
  endif
"  echo "DBG: choice<" . choice . ">"
  let ichoice= ichoice + 1

  " Determine method of read (ftp, rcp, etc)
  call NetMethod(choice)

  " Perform Write
  if     b:netrw_method == 1	" write with rcp
   exe "!rcp " . tmpfile . " " . b:netrw_machine . ":" . b:netrw_fname
   let b:netrw_lastfile = choice

  elseif b:netrw_method == 2	" write with ftp + <.netrc>
   exe "norm mzoascii\<cr>put ".tmpfile." ".b:netrw_fname."\<esc>"
   exe "'z+1,.!ftp -i " . b:netrw_machine
   norm 'z
   let b:netrw_lastfile = choice

  elseif b:netrw_method == 3	" write with ftp + machine, id, passwd, and fname
   if has("win32")
    exe "norm mzouser ".b:netrw_uid." ".b:netrw_passwd."\<cr>ascii\<cr>put ".tmpfile." ".b:netrw_fname."\<esc>"
    exe "'z+1,.!ftp -i -n " . b:netrw_machine
	norm u
   elseif filereadable(expand("$HOME/.netrc"))
    " DON'T use <.netrc>, even though it exists
    exe "norm mzouser ".b:netrw_uid." ".b:netrw_passwd."\<cr>ascii\<cr>put ".tmpfile." ".b:netrw_fname."\<esc>"
    exe "'z+1,.!ftp -i -n " . b:netrw_machine
   else
    " ordinary ftp
    exe "norm mzouser ".b:netrw_uid." ".b:netrw_passwd."\<cr>ascii\<cr>put ".tmpfile." ".b:netrw_fname."\<esc>"
    exe "'z+1,.!ftp -i " . b:netrw_machine
   endif
   " save choice/id/password for future use
   let b:netrw_lastfile = choice
   let b:netrw_uid     = b:netrw_uid

  elseif     b:netrw_method == 4	" write with scp
   exe "!scp " . tmpfile . " " . b:netrw_machine . ":" . b:netrw_fname
   let b:netrw_lastfile = choice

  else " Complain
   echo "***warning*** unable to comply with your request<" . choice . ">"
  endif
 endwhile

 " cleanup
 let result=delete(tmpfile)
 unlet b:netrw_method
 unlet b:netrw_machine
 unlet b:netrw_fname

" echo "DBG: return NetWrite }"
endfunction
" end of NetWrite

" ------------------------------------------------------------------------

" NetMethod:  determine method of transfer
"  method == 1: rcp
"            2: ftp + <.netrc>
"            3: ftp + machine, id, password, and [path]filename
"            4: scp
"            5: http (wget)
fu! NetMethod(choice)  " globals: method machine id passwd fname
" echo "DBG: NetMethod1(a:choice<".a:choice.">) {"

 " initialization
 let b:netrw_method  = 0
 let b:netrw_machine = ""
 let b:netrw_fname   = ""

 " Patterns:
 " mipf  : a:machine a:id password filename  Use ftp
 " mf    : a:machine filename                Use ftp + <.netrc> or b:netrw_uid b:netrw_passwd
 " ftpurm: ftp://host/filename               Use ftp + <.netrc> or b:netrw_uid b:netrw_passwd
 " rcpurm: rcp://host/filename               Use rcp
 " rcphf : host:filename                     Use rcp
 " scpurm: scp://[user@]host/filename        Use scp
 " httpurm: http://[user@]host/filename      Use wget
 let mipf   = '\(\S\+\)\s\+\(\S\+\)\s\+\(\S\+\)\s\+\(\S\+\)'
 let mf     = '\(\S\+\)\s\+\(\S\+\)'
 let ftpurm = 'ftp://\([^/]\{-}\)/\(.*\)$'
 let rcpurm = 'rcp://\([^/]\{-}\)/\(.*\)$'
 let rcphf  = '\(\I\i*\):\(\S\+\)'
 let scpurm = 'scp://\([^/]\{-}\)/\(.*\)$'
 let httpurm= 'http://\([^/]\{-}\)/\(.*\)$'

 " Determine Method
 " rcp://hostname/...path-to-file
 if match(a:choice,rcpurm) == 0
"  echo "DBG: NetMethod: rcp://..."
  let b:netrw_method = 1
  let b:netrw_machine= substitute(a:choice,rcpurm,'\1',"")
  let b:netrw_fname  = substitute(a:choice,rcpurm,'\2',"")

 " scp://user@hostname/...path-to-file
 elseif match(a:choice,scpurm) == 0
"  echo "DBG: NetMethod: scp://..."
  let b:netrw_method = 4
  let b:netrw_machine= substitute(a:choice,scpurm,'\1',"")
  let b:netrw_fname  = substitute(a:choice,scpurm,'\2',"")

 " http://hostname/...path-to-file
 elseif match(a:choice,httpurm) == 0
"  echo "DBG: NetMethod: http://..."
  let b:netrw_method = 5
  let b:netrw_machine= substitute(a:choice,httpurm,'\1',"")
  let b:netrw_fname  = substitute(a:choice,httpurm,'\2',"")

 " ftp://hostname/...path-to-file
 elseif match(a:choice,ftpurm) == 0
"  echo "DBG: NetMethod: ftp://..."
  let b:netrw_machine= substitute(a:choice,ftpurm,'\1',"")
  let b:netrw_fname  = substitute(a:choice,ftpurm,'\2',"")
  if exists("b:netrw_uid") && exists("b:netrw_passwd")
   let b:netrw_method = 3
  else
   if filereadable(expand("$HOME/.netrc"))
	 let b:netrw_method= 2
   else
     if !exists("b:netrw_uid") || b:netrw_uid == ""
       call NetUserPass()
	 elseif !exists("b:netrw_passwd") || b:netrw_passwd == ""
       call NetUserPass(b:netrw_uid)
	 " else just use current b:netrw_uid and b:netrw_passwd
	 endif
	 let b:netrw_method= 3
	endif
  endif

 " Issue an rcp: hostname:filename"
 elseif match(a:choice,rpchf) == 0
"  echo "DBG: NetMethod: (rcp) host:file"
  let b:netrw_method = 1
  let b:netrw_machine= substitute(a:choice,rpchf,'\1',"")
  let b:netrw_fname  = substitute(a:choice,rpchf,'\2',"")
  if has("win32")
   " don't let PCs try <.netrc>
   let b:netrw_method = 3
  endif

 " Issue an ftp : "machine id password [path/]filename"
 elseif match(a:choice,mipf) == 0
"  echo "DBG: NetMethod: (ftp) host id pass file"
  let b:netrw_method  = 3
  let b:netrw_machine = substitute(a:choice,mipf,'\1',"")
  let b:netrw_uid     = substitute(a:choice,mipf,'\2',"")
  let b:netrw_passwd  = substitute(a:choice,mipf,'\3',"")
  let b:netrw_fname   = substitute(a:choice,mipf,'\4',"")

 " Issue an ftp: "hostname [path/]filename"
 elseif match(a:choice,mf) == 0
"  echo "DBG: NetMethod: (ftp) host file"
  if exists("b:netrw_uid") && exists("b:netrw_passwd")
   let b:netrw_method  = 3;
   let b:netrw_machine = substitute(a:choice,mf,'\1',"")
   let b:netrw_fname   = substitute(a:choice,mf,'\2',"")

  elseif filereadable(expand("$HOME/.netrc"))
   let b:netrw_method  = 2
   let b:netrw_machine = substitute(a:choice,mf,'\1',"")
   let b:netrw_fname   = substitute(a:choice,mf,'\2',"")
  endif
 endif

" echo "DBG: NetMethod: a:choice       <".a:choice.">"
" echo "DBG: NetMethod: b:netrw_method <".b:netrw_method.">"
" echo "DBG: NetMethod: b:netrw_machine<".b:netrw_machine.">"
" if exists("b:netrw_uid")		" DBG
"  echo "DBG: NetMethod: b:netrw_uid    <".b:netrw_uid.">"
" endif							" DBG
" if exists("b:netrw_passwd")	" DBG
"  echo "DBG: NetMethod: b:netrw_passwd <".b:netrw_passwd.">"
" endif							" DBG
" echo "DBG: NetMethod: b:netrw_fname  <".b:netrw_fname.">"
" echo "DBG: NetMethod return }"
endfunction
" end of NetMethod
" ------------------------------------------------------------------------

" NetUserPass: set username and password for subsequent ftp transfer
"   Usage:  :call NetUserPass()                 -- will prompt for userid and password
"           :call NetUserPass("uid")            -- will prompt for password
"           :call NetUserPass("uid","password") -- sets global userid and password
fu! NetUserPass(...)

 " get/set userid
 if a:0 == 0
"  echo "DBG: NetUserPass(a:0<".a:0.">) {"
  if !exists("b:netrw_uid") || b:netrw_uid == ""
   let b:netrw_uid= input('Enter username: ')
  endif
 else
"  echo "DBG: NetUserPass(a:1<".a:1.">) {"
  let b:netrw_uid= a:1
 endif

 " get password -- if the user has specified both foreground and
 "                 background colors for the Normal highlighting
 "                 group, the password will be obtained invisibly.
 if a:0 <= 1
"  echo "DBG: a:0=".a:0." case <=1:"
  if !exists("b:netrw_passwd")
   let _ch  = &ch|set ch=2
   let _gfg = synIDattr(hlID("Normal"), 'fg', 'gui')
   let _gbg = synIDattr(hlID("Normal"), 'bg', 'gui')
   let _cfg = synIDattr(hlID("Normal"), 'fg', 'cterm')
   let _cbg = synIDattr(hlID("Normal"), 'bg', 'cterm')
   echo "\nEnter ".b:netrw_uid."'s Password:"

   " HIDE BEGIN
   if has("gui_running") " gui
"    echo "DBG: NetUserPass: _gfg="._gfg
"    echo "DBG: NetUserPass: _gbg="._gbg
    if _gbg != -1 && _gfg != -1
      exe "hi Normal guifg="._gbg
    endif

   else	" not a gui
"    echo "DBG: NetUserPass: _cfg="._cfg
"    echo "DBG: NetUserPass: _cbg="._cbg
    if _cbg != -1 && _cfg != -1
     exe "hi Normal ctermfg="._cbg
    endif
   endif
   " HIDE END

   let b:netrw_passwd= input('')

   " HIDE BEGIN
   " restore Normal highlighting
   if has("gui_running")
     if _gfg != -1 && _gbg != -1
 	 exe "hi Normal guifg="._gfg
 	endif
   else
     if _cfg != -1 && _cbg != -1
 	 exe "hi Normal ctermfg="._cfg
 	endif
   endif
   " HIDE END

   " cleanup:
   call histdel('@',-1)
   let &ch=_ch
  endif
 else
"  echo "DBG: a:0=".a:0." case >1: a:2<".a:2.">"
  " user has also specified the password
  let b:netrw_passwd=a:2
 endif
" echo "DBG: return NetUserPass }"
endfunction
" end NetUserPass

" ------------------------------------------------------------------------
