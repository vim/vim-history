" LaTeX filetype plugin
" Language:     LaTeX (ft=tex)
" Maintainer:   Benji Fisher, Ph.D. <benji@member.AMS.org>
" Last Change:  2001 Sept 17

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

let s:save_cpo = &cpo
set cpo&vim

" Set 'comments' to format dashed lists in comments
setlocal com=sO:%\ -,mO:%\ \ ,eO:%%,:%

" Allow "[d" to be used to find a macro definition:
" Recognize plain TeX \def as well as LaTeX \newcommand and \renewcommand .
setlocal define=\\\\def\\\\|\\\\\\(re\\)\\=newcommand{

" Tell Vim how to recognize LaTeX \include{foo} and plain \input bar :
setlocal include=\\\\input\\\\|\\\\include{
setlocal includeexpr=TexIncludeExpr()
fun! TexIncludeExpr()
  " On some file systems, "}" is inluded in 'isfname'.  In case the
  " TeX file has \include{fname} (LaTeX only), strip the "}" and
  " any other trailing characters.
  let fname = substitute(v:fname, '}.*', '', '')
  " Now, add ".tex" if there is no other file extension.
  if fname !~ '\.'
    let fname = fname . '.tex'
  endif
  return fname
endfun

" The following lines enable the macros/matchit.vim plugin for
" extended matching with the % key.
if exists("loaded_matchit")
  let b:match_ignorecase = 0
    \ | let b:match_skip = 'r:\\\@<!\%(\\\\\)*%'
    \ | let b:match_words = '(:),\[:],{:},\\(:\\),\\\[:\\],' .
    \ '\\begin\s*\({\a\+\*\=}\):\\end\s*\1'
endif " exists("loaded_matchit")

let &cpo = s:save_cpo
