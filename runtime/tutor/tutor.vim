" Vim tutor support file
" Author: Eduardo F. Amatria <eferna1@platea.pntic.mec.es>
" Last Change:	2001 Mar 21

" This small source file is used for detecting if a translation of the
" tutor file exist, i.e., a tutor.xx file, where xx is the language.
" If the translation does not exist, or no extension is given,
" it defaults to the english version.

" It is invoked by the vimtutor shell script.

" 1. Build the extension of the file, if any:
let s:ext = ""
if strlen($xx) == 0 && exists("v:lang") && v:lang != "C"
  let s:ext = "." . strpart(v:lang, 0, 2)
else
  if strlen($xx) > 1
    let s:ext = "." . $xx
  endif
endif
if s:ext =~ ".en"
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
  5sleep
endif

" 4. Making the copy and exiting Vim: 
e $TUTOR
wq! $TUTORCOPY
