" Vim syntax file
" Language:	GSP - GNU Server Pages (v. 0.86)
" Maintainer:	Nathaniel Harward nharward@yahoo.com
" Last Changed: Dec. 7, 2000
" Filenames:    *.gsp
" URL:          http://www.constructicon.com/~nharward/vim/syntax/gsp.vim
"
" Notes:      * This is a hack of jsp.vim and is not quite complete; if you do
"               wierd stuff and put in-line java in HTML tags or tag args it
"               may not look very good, but otherwise it should be fine
"
"             * By default the backticks around in-line java blocks are
"               highlighted as HTML errors so that they are not so hard to
"               find -- if you don't like this take out the
"               'matchgroup=htmlError' part on the gspInLine line or change it
"               to something else

" Clear existing syntax
syn clear

if !exists("main_syntax")
  let main_syntax = 'gsp'
endif

" Source HTML syntax
runtime syntax/html.vim

" Next syntax items are case-sensitive
syn case match

" Include Java syntax
syn include @gspJava <sfile>:p:h/java.vim

" Add <java> as an HTML tag name along with its args
syn keyword htmlTagName contained java
syn keyword htmlArg     contained type file page

" Redefine some HTML things to include (and highlight) gspInLine code in
" places where it's likely to be found
syn region htmlString contained start=+"+ end=+"+ contains=htmlSpecialChar,javaScriptExpression,@htmlPreproc,gspInLine
syn region htmlString contained start=+'+ end=+'+ contains=htmlSpecialChar,javaScriptExpression,@htmlPreproc,gspInLine
syn match  htmlValue  contained "=[\t ]*[^'" \t>][^ \t>]*"hs=s+1 contains=javaScriptExpression,@htmlPreproc,gspInLine
syn region htmlEndTag           start=+</+    end=+>+ contains=htmlTagN,htmlTagError,gspInLine
syn region htmlTag              start=+<[^/]+ end=+>+ contains=htmlTagN,htmlString,htmlArg,htmlValue,htmlTagError,htmlEvent,htmlCssDefinition,@htmlPreproc,@htmlArgCluster,gspInLine
syn match  htmlTagN   contained +<\s*[-a-zA-Z0-9]\++hs=s+1 contains=htmlTagName,htmlSpecialTagName,@htmlTagNameCluster,gspInLine
syn match  htmlTagN   contained +</\s*[-a-zA-Z0-9]\++hs=s+2 contains=htmlTagName,htmlSpecialTagName,@htmlTagNameCluster,gspInLine

" Define the java code blocks
syn region  gspJavaBlock start="<java\>[^>]*\>" end="</java>"me=e-7 contains=@gspJava,htmlTag
syn region  gspInLine    matchgroup=htmlError start="`" end="`" contains=@gspJava

let b:current_syntax = "gsp"

if main_syntax == 'gsp'
  unlet main_syntax
endif
