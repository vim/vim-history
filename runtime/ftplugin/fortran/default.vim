" Vim settings file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	0.22
" Last Change:	2000 Dec 14
" Maintainer:	Ajit J. Thakkar <ajit@unb.ca>; <http://www.unb.ca/chem/ajit/>
" For the latest version of this file, see <http://www.unb.ca/chem/ajit/vim.htm>

" Only do these settings when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't do other file type settings for this buffer
let b:did_ftplugin = 1

" Determine whether this is a fixed or free format source file
" if this hasn't been done yet
if !exists("b:fortran_fixed_source")
  if exists("fortran_free_source")
    let b:fortran_fixed_source = 0
  else
    let b:fortran_fixed_source = 1
    let s:ln=1
    while s:ln < 25
      let s:test = strpart(getline(s:ln),0,5)
      if s:test[0] !~ '[Cc*#]' && s:test !~ '^\s*!' && s:test =~ '[^ 0-9\t]'
	let b:fortran_fixed_source = 0
	break
      endif
      let s:ln = s:ln + 1
    endwhile
  endif
endif

" Set comments and textwidth according to source type
if (b:fortran_fixed_source == 1)
  setlocal comments=:!,:*,:C
  " Fixed format requires a textwidth of 72 for code
  setlocal tw=72
  " If you need to add "&" on continued lines so that the code is
  " compatible with both free and fixed format, then you should do so
  " in column 73 and uncomment the next line
  " setlocal tw=73
else
  setlocal comments=:!
  " Free format allows a textwidth of 132 for code but 80 is more usual
  setlocal tw=80
endif

" Tabs are not a good idea in Fortran so the default is to expand tabs
if !exists("fortran_have_tabs")
  setlocal expandtab
endif

" Set 'formatoptions' to break comment and text lines but allow long lines
setlocal fo+=tcql

" vim:sw=2
