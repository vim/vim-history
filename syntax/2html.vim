" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Jan 12

" Transform a file into HTML, using the current syntax highlighting.
" Does NOT support background colors.

if !has("gui_running")
  echo "This only works in gvim"
else

" Split window to create a buffer with the HTML file.
new %.html
1,$d
let old_title = &title
let old_icon = &icon
let old_paste = &paste
set notitle noicon paste noet

" Find out the background and foreground color.
let bg = synIDattr(highlightID("Normal"), "bg#")
let fg = synIDattr(highlightID("Normal"), "fg#")
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

if fg != ""
  let norm = "<FONT COLOR=" . fg . ">"
else
  let norm = ""
endif

" Insert HTML header.  First find out the background color.
exec "normal a<HTML>\n<HEAD>\n<TITLE>".expand("%:t")."</TITLE>\n</HEAD>\n<BODY BGCOLOR=".bg.">\n<PRE>\n\e"
exec "normal \<C-W>p"

" Loop over all lines in the buffer
let lnum = 1
while lnum <= line("$")
  let col = 1
  let startcol = 1
  let id = 0
  let line = getline(lnum)
  let len = strlen(line)
  let new = ""
  let color = ""
  let bold = ""
  let italic = ""
  let underline = ""

  " Loop over each character in the line, plus just after the last character
  while col <= len + 1

    " Get the syntax ID of the current character
    if col > len
      let new_id = 0
    else
      let new_id = synID(lnum, col, 1)
    endif

    " When finding a different syntax ID, generate the text up to here.
    if new_id != id || col > len
      while startcol < col
        let c = line[startcol - 1]
	" replace special HTML characters
	if c == "<"
	  let new = new . "&lt;"
	elseif c == ">"
	  let new = new . "&gt;"
	elseif c == "&"
	  let new = new . "&amp;"
	elseif c == '"'
	  let new = new . "&quot;"
	elseif c == "\x0C"
	  let new = new . "<HR class=PAGE-BREAK>"
	else
	  let new = new . c
	endif
	let startcol = startcol + 1
      endwhile

      " Stop any highlighting
      if color != ""
        let new = new . "</FONT>"
      endif
      if bold == "1"
        let new = new . "</B>"
      endif
      if italic == "1"
        let new = new . "</I>"
      endif
      if underline == "1"
        let new = new . "</U>"
      endif
    endif

    " If the following text has highlighing, start it
    if new_id != id && col <= len
      let id = new_id
      let underline = synIDattr(synIDtrans(id), "underline")
      if underline == "1"
        let new = new . "<U>"
      endif
      let italic = synIDattr(synIDtrans(id), "italic")
      if italic == "1"
        let new = new . "<I>"
      endif
      let bold = synIDattr(synIDtrans(id), "bold")
      if bold == "1"
        let new = new . "<B>"
      endif
      let color = synIDattr(synIDtrans(id), "fg#")
      if color != ""
	let new = new . "<FONT COLOR=" . color . ">"
      elseif norm != ""
        let new = new . norm
	let color = fg
      endif
    endif
    let col = col + 1
  endwhile

  exec "normal \<C-W>pa" . new . "\n\e\<C-W>p"

  let lnum = lnum + 1
endwhile

" Finish with the last line
exec "normal \<C-W>pa</PRE>\n</BODY>\n</HTML>\e"

let &title = old_title
let &icon = old_icon
let &paste = old_paste

endif " has("gui_running")
