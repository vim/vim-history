" Vim settings file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	0.3
" Last Change:	2001 Jul 28
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

" Set commentstring for foldmethod=marker
setlocal cms=!%s

" Tabs are not a good idea in Fortran so the default is to expand tabs
if !exists("fortran_have_tabs")
  setlocal expandtab
endif

" Set 'formatoptions' to break comment and text lines but allow long lines
setlocal fo+=tcql

" Define patterns for the matchit plugin
if !exists("b:match_words")
  let s:notend = '\%(\<end\s\+\)\@<!'
  let s:notselect = '\%(\<select\s\+\)\@<!'
  let s:notelse = '\%(\<end\s\+\|\<else\s\+\)\@<!'
  let b:match_ignorecase = 1
  let b:match_words =
    \ '\<select\s*case\>:' . s:notselect. '\<case\>:\<end\s*select\>,' .
    \ s:notelse . '\<if\s*(.\+)\s*then\>:' .
    \ '\<else\s*\%(if\s*(.\+)\s*then\)\=\>:\<end\s*if\>,'.
    \ 'do\s\+\(\d\+\):\%(^\s*\)\@<=\1\s,'.
    \ s:notend . '\<do\>:\<end\s*do\>,'.
    \ s:notelse . '\<where\>:\<elsewhere\>:\<end\s*where\>,'.
    \ s:notend . '\<type\s*[^(]:\<end\s*type\>,'.
    \ s:notend . '\<subroutine\>:\<end\s*subroutine\>,'.
    \ s:notend . '\<function\>:\<end\s*function\>,'.
    \ s:notend . '\<module\>:\<end\s*module\>,'.
    \ s:notend . '\<program\>:\<end\s*program\>'
endif

" vim:sw=2
