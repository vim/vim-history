" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1999 May 07

" Transform a file into HTML, using the current syntax highlighting.

" function to produce text with HTML codes for attributes and colors
"
" a:attr  contains 'U' for underline, 'I' for italic and 'B' for bold
" a:fg	  foregound color name
" a:bg    background color name
" a:txt   text
"
" the big return statement concatenates:
" - the code to start underline/italic/bold, substituting each 'U', 'I' or 'B'
"   by the same character inside <>
" - the code to start the background color
" - the code to start the foreground color
" - the text, where each '&', '<', '>' and '"' is translated for their special
"   meaning.  A CTRL-L is translated into a page break
" - the code to end the foreground color
" - the code to end the background color
" - the code to end underline/italic/bold, substituting each 'U', 'I' or 'B'
"   by the same character inside </>, in reverse order
function! HTMLPutText(attr, bg, fg, txt)
	let bgs = ""	" code for background color start
	let bge = ""	" code for background color end
	if a:bg != ""
	  let bgs = '<SPAN style="background-color: ' . a:bg . '">'
	  let bge = '</SPAN>'
	endif
	let fgs = ""	" code for foreground color start
	let fge = ""	" code for foreground color end
	if a:fg != ""
	  let fgs = '<FONT color=' . a:fg . ">"
	  let fge = '</FONT>'
	endif
	return substitute(a:attr, '.', '<&>', 'g') . bgs . fgs . substitute(substitute(substitute(substitute(substitute(a:txt, '&', '\&amp;', 'g'), '<', '\&lt;', 'g'), '>', '\&gt;', 'g'), '"', '\&quot;', 'g'), "\x0c", '<HR class=PAGE-BREAK>', 'g') . fge . bge . substitute(a:attr[2] . a:attr[1] . a:attr[0], '.', '</&>', 'g')
endfun

" Set some options to make it work faster.
" Expand tabs in original buffer to get 'tabstop' correctly used.
let old_title = &title
let old_icon = &icon
let old_paste = &paste
let old_et = &et
set notitle noicon paste et

" Split window to create a buffer with the HTML file.
if expand("%") == ""
  new Untitled.html
else
  new %.html
endif
1,$d
set noet
" Find out the background and foreground color.
let bg = synIDattr(highlightID("Normal"), "bg#", "gui")
let fg = synIDattr(highlightID("Normal"), "fg#", "gui")
if bg == ""
   if &background == "dark"
     let bg = "#000000"
     if fg == ""
       let fg = "#FFFFFF"
     endif
   else
     let bg = "#FFFFFF"
     if fg == ""
       let fg = "#000000"
     endif
   endif
endif

" Insert HTML header, with the background color.  Add the foreground color
" only when it is defined.
exe "normal a<HTML>\n<HEAD>\n<TITLE>".expand("%:t")."</TITLE>\n</HEAD>\n<BODY BGcolor=".bg."\e"
if fg != ""
  exe "normal a TEXT=".fg."\e"
endif
exe "normal a>\n<PRE>\n\e"

exe "normal \<C-W>p"

" Some 'constants' for ease of addressing with []
let uline="U"
let bld="B"
let itl="I"

" Loop over all lines in the original text
let end = line("$")
let lnum = 1
while lnum <= end

  " Get the current line, with tabs expanded to spaces when needed
  let line = getline(lnum)
  if match(line, "\t") >= 0
    exe lnum . "retab!"
    let did_retab = 1
    let line = getline(lnum)
  else
    let did_retab = 0
  endif
  let len = strlen(line)
  let new = ""

  " Loop over each character in the line
  let col = 1
  while col <= len
    let startcol = col " The start column for processing text
    let id = synID(lnum, col, 1)
    let col = col + 1
    " Speed loop (it's small - that's the trick)
    " Go along till we find a change in synID
    while col <= len && id == synID(lnum, col, 1) | let col = col + 1 | endwhile

    " output the text with the same synID, with all its attributes
    " The first part turns attributes into  [U][I][B]
    let id = synIDtrans(id)
    let new = new . HTMLPutText(uline[synIDattr(id, "underline", "gui") - 1] . itl[synIDattr(id, "italic", "gui") - 1] . bld[synIDattr(id, "bold", "gui") - 1], synIDattr(id, "bg#", "gui"), synIDattr(id, "fg#", "gui"), strpart(line, startcol - 1, col - startcol))
    if col > len
      break
    endif
  endwhile
  if did_retab
    undo
  endif

  exe "normal \<C-W>pa" . strtrans(new) . "\n\e\<C-W>p"
  let lnum = lnum + 1
  +
endwhile
" Finish with the last line
exe "normal \<C-W>pa</PRE>\n</BODY>\n</HTML>\e"

let &title = old_title
let &icon = old_icon
let &paste = old_paste
exe "normal \<C-W>p"
let &et = old_et
exe "normal \<C-W>p"

" In case they didn't get used
let startcol = 0
let id = 0
unlet uline bld itl lnum end col startcol line len new id
unlet old_title old_icon old_paste old_et did_retab
