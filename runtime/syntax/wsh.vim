" Vim syntax file
" Language:	Windows Scripting Host
" Maintainer:	Paul Moore <gustav@morpheus.demon.co.uk>
" Last Change:	01 Oct 2000

" This reuses the XML, VB and JavaScript syntax files. While VB is not
" VBScript, it's close enough for us. No attempt is made to handle
" other languages.
" Send comments, suggestions and requests to the maintainer.

source <sfile>:p:h/xml.vim

syn case ignore
syn include @wshVBScript <sfile>:p:h/vb.vim
syn include @wshJavaScript <sfile>:p:h/javascript.vim
syn region wshVBScript matchgroup=xmlTag start="<script[^>]*VBScript[^>]*>" end="</script>" contains=@wshVBScript
syn region wshJavaScript matchgroup=xmlTag start="<script[^>]*J\(ava\)\=Script[^>]*>" end="</script>" contains=@wshJavaScript

" No type-specific highlighting -- it's all reused from other files

let b:current_syntax = "wsh"
