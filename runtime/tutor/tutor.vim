" Vim tutor support file
" Author: Eduardo F. Amatria <eferna1@platea.pntic.mec.es>
" Last Change:	2002 Mar 19

" This small source file is used for detecting if a translation of the
" tutor file exist, i.e., a tutor.xx file, where xx is the language.
" If the translation does not exist, or no extension is given,
" it defaults to the english version.

" It is invoked by the vimtutor shell script.

" 1. Build the extension of the file, if any:
let s:ext = ""
if strlen($xx) > 1
  let s:ext = "." . $xx
elseif exists("v:lang") && v:lang != "C"
  let s:ext = "." . strpart(v:lang, 0, 2)
elseif strlen($LANG) > 0 && $LANG != "C"
  let s:ext = "." . strpart($LANG, 0, 2)
endif

" The japanese tutor is available in two encodings, guess which one to use
" The "sjis" one is actually "cp932", it doesn't matter for this text.
if s:ext =~? '\.ja'
  if &enc =~ "euc"
    let s:ext = s:ext . ".euc"
  else
    let s:ext = s:ext . ".sjis"
  endif
endif

" The Chinese tutor is available in two encodings, guess which one to use
" This segment is from the above lines and modified by
" Mendel L Chan <beos@turbolinux.com.cn> for Chinese vim tutorial
if s:ext =~? '\.zh'
  if &enc =~ 'big5\|cp950'
    let s:ext = s:ext . ".big5"
  else
    let s:ext = s:ext . ".euc"
  endif
endif

" The Polish tutor is available in two encodings, guess which one to use
if s:ext =~? '\.pl' && &enc =~ 1250
  let s:ext = s:ext . ".cp1250"
endif

if s:ext =~? '\.en'
  let s:ext = ""
endif

" 2. Build the name of the file:
let s:tutorfile = "/tutor/tutor"
let s:tutorxx = $VIMRUNTIME . s:tutorfile . s:ext

" 3. Finding the file:
if filereadable(s:tutorxx)
  let $TUTOR = s:tutorxx
else
  let $TUTOR = $VIMRUNTIME . s:tutorfile
  echo "The file " . s:tutorxx . " does not exist.\n"
  echo "Copying English version: " . $TUTOR
  4sleep
endif

" 4. Making the copy and exiting Vim:
e $TUTOR
wq! $TUTORCOPY
