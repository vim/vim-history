" Vim indent file
" Language: PoV-Ray Scene Description Language
" Maintainer: David Necas (Yeti) <yeti@physics.muni.cz>
" Last Change: 2002-03-10
" URI: http://physics.muni.cz/~yeti/download/indent/pov.vim

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

" Some preliminary settings
setlocal nolisp " Make sure lisp indenting doesn't supersede us

setlocal indentexpr=GetPoVRayIndent(v:lnum)
setlocal indentkeys+==else,=end

" Only define the function once.
if exists("*GetPoVRayIndent")
  finish
endif

function GetPoVRayIndent(lnum)
  " Search backwards for the frist non-empty line.
  let plnum = prevnonblank(v:lnum - 1)

  " Start indenting from zero
  if plnum == 0
    return 0
  endif

  let plind = indent(plnum)

  " If we are inside a comment (may be nested in obscure ways), give up
  " FIXME: it's possible to have indent on, but syntax off!
  if synIDattr(synIDtrans(synID(line("."), col("."), 1)), "name") == "Comment"
    return -1
  endif

  let ich = 0
  let chch = 0
  " Indenting less:
  if getline(v:lnum) =~ '^\s*#\s*\(end\|else\)\>'
    let ich = ich - &sw
    let chch = 1
  endif

  if getline(v:lnum) =~ '^\s*}'
    let ich = ich - &sw
    let chch = 1
  endif

  " Indenting more:
  if getline(plnum) =~ '{\s*\(//.*\)\=$'
    let ich = ich + &sw
    let chch = 1
  endif

  if getline(plnum) =~ '^\s*#\s*\(if\|ifdef\|ifndef\|switch\|while\|macro\|else\)\>'
    let ich = ich + &sw
    let chch = 1
  endif

  " Otherwise let the user do what (s)he wants
  if chch == 0
    return -1
  endif

  return plind+ich < 0 ? 0 : plind+ich
endfunction
