" Vim filetype plugin file
" Language:	Fortran90 (and Fortran95, Fortran77, F and elf90)
" Version:	0.1
" Last Change:	2000 Oct 15
" Maintainer:	Ajit J. Thakkar <ajit@unb.ca>; <http://www.unb.ca/chem/ajit/>
" For the latest version of this file, see <http://www.unb.ca/chem/ajit/vim.htm>

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

"Determine whether this is a fixed or free format source file
let b:fortran_fixed_source = 1
let b:ln=1
while b:ln < 5
  let b:test = strpart(getline(b:ln),0,5)
  if b:test[0] !~ '[Cc*]' && b:test !~ '^\s*!' && b:test =~ '[^ 0-9\t]'
    let b:fortran_fixed_source = 0
    break
  endif
  let b:ln = b:ln + 1
endwhile
unlet b:ln b:test

"Set comments according to source type
if (b:fortran_fixed_source == 1)
  setlocal comments=:!,:*,:C
else
  setlocal comments=:!
endif

" Set 'formatoptions' to break comment lines but not other lines,
" and insert the comment leader when hitting <CR> or using "o".
setlocal fo-=t fo+=croql

" vim:sw=2
