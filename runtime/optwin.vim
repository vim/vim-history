" These commands create the option window.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Jan 02

" If there already is an option window, jump to that one.
if bufwinnr("option-window") > 0
  let s:thiswin = winnr()
  while 1
    if @% == "option-window"
      finish
    endif
    exe "norm! \<C-W>w"
    if s:thiswin == winnr()
      break
    endif
  endwhile
endif

" Make sure the '<' flag is not included in 'cpoptions', otherwise <CR> would
" not be recognized.  See ":help 'cpoptions'".
let s:cpo_save = &cpo
set cpo&vim

" function to be called when <CR> is hit in the option-window
fun! <SID>CR()

  " If on a continued comment line, go back to the first comment line
  let lnum = line(".")
  let line = getline(lnum)
  while line[0] == "\t"
    let lnum = lnum - 1
    let line = getline(lnum)
  endwhile

  " <CR> on a "set" line executes the option line
  if match(line, "^set ") >= 0

    " For a local option: go to the previous window
    " If this is a help window, go to the window below it
    let thiswin = winnr()
    let local = <SID>Find(lnum)
    if local >= 0
      exe line
      call <SID>Update(lnum, line, local, thiswin)
    endif

  " <CR> on a "option" line shows help for that option
  elseif match(line, "^[a-z]") >= 0
    let name = substitute(line, '\([^\t]*\).*', '\1', "")
    exe "help '" . name . "'"

  " <CR> on an index line jumps to the group
  elseif match(line, '^ \=[0-9]') >= 0
    exe "norm! /" . line . "\<CR>zt"
  endif
endfun

" function to be called when <Space> is hit in the option-window
fun! <SID>Space()

  let lnum = line(".")
  let line = getline(lnum)

  " <Space> on a "set" line refreshes the option line
  if match(line, "^set ") >= 0

    " For a local option: go to the previous window
    " If this is a help window, go to the window below it
    let thiswin = winnr()
    let local = <SID>Find(lnum)
    if local >= 0
      call <SID>Update(lnum, line, local, thiswin)
    endif

  endif
endfun

" find the window in which the option applies
" returns 0 for global option, 1 for local option, -1 for error
fun! <SID>Find(lnum)
    if getline(a:lnum - 1) =~ "(local to"
      let local = 1
      let thiswin = winnr()
      exe "norm! \<C-W>p"
      if exists("b:current_syntax") && b:current_syntax == "help"
        exe "norm! \<C-W>j"
        if winnr() == thiswin
          exe "norm! \<C-W>j"
        endif
      endif
    else
      let local = 0
    endif
    if local && (winnr() == thiswin || (exists("b:current_syntax")
	\ && b:current_syntax == "help"))
      echo "Don't know in which window"
      let local = -1
    endif
    return local
endfun

" Update a "set" line in the option window
fun! <SID>Update(lnum, line, local, thiswin)
  " get the new value of the option and update the option window line
  if match(a:line, "=") >= 0
    let name = substitute(a:line, '^set \([^=]*\)=.*', '\1', "")
  else
    let name = substitute(a:line, '^set \(no\)\=\([a-z]*\).*', '\2', "")
  endif
  exe "let val = substitute(&" . name . ', "[ \\t\\\\\"|]", "\\\\\\0", "g")'
  if a:local
    exe "norm! " . a:thiswin . "\<C-W>w"
  endif
  if match(a:line, "=") >= 0 || (val != "0" && val != "1")
    call setline(a:lnum, "set " . name . "=" . val)
  else
    if val
      call setline(a:lnum, "set " . name . "\tno" . name)
    else
      call setline(a:lnum, "set no" . name . "\t" . name)
    endif
  endif
  set nomodified
endfun

" Reset 'title' and 'icon' to make it work faster.
let s:old_title = &title
let s:old_icon = &icon
let s:old_sc = &sc
let s:old_ru = &ru
set notitle noicon nosc noru

" If the current window is a help window, try finding a non-help window.
" Relies on syntax highlighting to be switched on.
let s:thiswin = winnr()
while exists("b:current_syntax") && b:current_syntax == "help"
  exe "norm! \<C-W>w"
  if s:thiswin == winnr()
    break
  endif
endwhile

" Open the window
new option-window
set ts=15 tw=0

" Insert help and a "set" command for each option.
call append(0, '" Each "set" line shows the current value of an option (on the left).')
call append(1, '" Hit <CR> on a "set" line to execute it.')
call append(2, '"            A boolean option will be toggled.')
call append(3, '"            For other options you can edit the value.')
call append(4, '" Hit <CR> on a help line to open a help window on this option.')
call append(5, '" Hit <CR> on an index line to jump there.')
call append(6, '" Hit <Space> on a "set" line to refresh it.')

" These functions are called often below.  Keep them fast!
fun! <SID>BinOption(name)
  exe "norm! \<C-W>p"
  exe "let val = &" . a:name
  exe "norm! \<C-W>p"
  call append("$", substitute(substitute("set " . val . a:name . "\t" .
	\!val . a:name, "0", "no", ""), "1", "", ""))
endfun

fun! <SID>BinOptionL(name, val)
  call append("$", substitute(substitute("set " . a:val . a:name . "\t" .
	\!a:val . a:name, "0", "no", ""), "1", "", ""))
endfun

fun! <SID>Option(name)
  exe "norm! \<C-W>p"
  exe "let val = substitute(&" . a:name . ', "[ \\t\\\\\"|]", "\\\\\\0", "g")'
  exe "norm! \<C-W>p"
  call append("$", "set " . a:name . "=" . val)
endfun

fun! <SID>OptionL(name, val)
  call append("$", "set " . a:name . "=" . substitute(a:val, "[ \\t\\\\\"|]",
	\"\\\\\\0", "g"))
endfun

let s:idx = 1
let s:lnum = line("$")
call append("$", "")

fun! <SID>Header(text)
  let line = s:idx . " " . a:text
  if s:idx < 10
    let line = " " . line
  endif
  call append("$", "")
  call append("$", line)
  call append("$", "")
  call append(s:lnum, line)
  let s:idx = s:idx + 1
  let s:lnum = s:lnum + 1
endfun

" Restore the previous value of 'cpoptions' here, it's used below.
let &cpo = s:cpo_save

" List of all options, organized by function.
" The text should be sufficient to know what the option is used for.

call <SID>Header("important")
call append("$", "compatible\tbehave very Vi compatible (not advisable)")
call <SID>BinOptionL("cp", &cp)
call append("$", "cpoptions\tlist of flags to specify Vi compatibility")
call <SID>OptionL("cpo", &cpo)
call append("$", "insertmode\tuse Insert mode as the default mode")
call <SID>BinOptionL("im", &im)
call append("$", "paste\tpaste mode, insert typed text literally")
call <SID>BinOptionL("paste", &paste)
call append("$", "pastetoggle\tkey sequence to toggle paste mode")
call <SID>OptionL("pt", &pt)
call append("$", "helpfile\tname of the main help file")
call <SID>OptionL("hf", &hf)


call <SID>Header("moving around, searching and patterns")
call append("$", "whichwrap\tlist of flags specifying which commands wrap to another line")
call append("$", "\t(local to window)")
call <SID>Option("ww")
call append("$", "startofline\tmany jump commands move the cursor to the first non-blank")
call append("$", "\tcharacter of a line")
call <SID>BinOptionL("sol", &sol)
call append("$", "paragraphs\tnroff macro names that separate paragraphs")
call <SID>OptionL("para", &para)
call append("$", "sections\tnroff macro names that separate sections")
call <SID>OptionL("sect", &sect)
call append("$", "path\tlist of directory names used for file searching")
call <SID>OptionL("pa", &pa)
call append("$", "wrapscan\tsearch commands wrap around the end of the buffer")
call <SID>BinOptionL("ws", &ws)
call append("$", "incsearch\tshow match for partly typed search command")
call <SID>BinOptionL("is", &is)
call append("$", "magic\tchange the way backslashes are used in search patterns")
call <SID>BinOptionL("magic", &magic)
call append("$", "ignorecase\tignore case when using a search pattern")
call <SID>BinOptionL("ic", &ic)
call append("$", "smartcase\toverride 'ignorecase' when pattern has upper case characters")
call <SID>BinOptionL("scs", &scs)
call append("$", "define\tpattern for a macro definition line")
call <SID>OptionL("def", &def)
call append("$", "include\tpattern for an include-file line")
call <SID>OptionL("inc", &inc)


call <SID>Header("tags")
call append("$", "tagbsearch\tuse binary searching in tags files")
call <SID>BinOptionL("tbs", &tbs)
call append("$", "taglength\tnumber of significant characters in a tag name or zero")
call append("$", "set tl=" . &tl)
call append("$", "tags\tlist of file names to search for tags")
call <SID>OptionL("tag", &tag)
call append("$", "tagrelative\tfile names in a tags file are relative to the tags file")
call <SID>BinOptionL("tr", &tr)
call append("$", "showfulltag\twhen completing tags in Insert mode show more info")
call <SID>BinOptionL("sft", &sft)
if has("cscope")
  call append("$", "cscopeprg\tcommand for executing cscope")
  call <SID>OptionL("csprg", &csprg)
  call append("$", "cscopetag\tuse cscope for tag commands")
  call <SID>BinOptionL("cst", &cst)
  call append("$", "cscopetagorder\t0 or 1; the order in which \":cstag\" performs a search")
  call append("$", "set csto=" . &csto)
  call append("$", "cscopeverbose\tgive messages when adding a cscope database")
  call <SID>BinOptionL("csverb", &csverb)
endif


call <SID>Header("displaying text")
call append("$", "scroll\tnumber of lines to scroll for CTRL-U and CTRL-D")
call append("$", "\t(local to window)")
call <SID>Option("scr")
call append("$", "scrolloff\tnumber of screen lines to show around the cursor")
call append("$", "set so=" . &so)
call append("$", "wrap\tlong lines wrap")
call <SID>BinOptionL("wrap", &wrap)
call append("$", "linebreak\twrap long lines at a character in 'breakat'")
call append("$", "\t(local to window)")
call <SID>BinOption("lbr")
call append("$", "breakat\twhich characters might cause a line break")
call <SID>OptionL("brk", &brk)
call append("$", "showbreak\tstring to put before wrapped screen lines")
call <SID>OptionL("sbr", &sbr)
call append("$", "sidescroll\tminimal number of columns to scroll horizontally")
call append("$", "set ss=" . &ss)
call append("$", "display\twhen \"lastline\": show the last line even if it doesn't fit")
call <SID>OptionL("dy", &dy)
call append("$", "cmdheight\tnumber of lines used for the command-line")
call append("$", "set ch=" . &ch)
call append("$", "columns\twidth of the display")
call append("$", "set co=" . &co)
call append("$", "lines\tnumber of lines in the display")
call append("$", "set lines=" . &lines)
call append("$", "lazyredraw\tdon't redraw while executing macros")
call <SID>BinOptionL("lz", &lz)
call append("$", "writedelay\tdelay in msec for each char written to the display")
call append("$", "\t(for debugging)")
call append("$", "set wd=" . &wd)
call append("$", "list\tshow <Tab> as ^I and end-of-line as $")
call append("$", "\t(local to window)")
call <SID>BinOption("list")
call append("$", "listchars\tlist of strings used for list mode")
call <SID>OptionL("lcs", &lcs)
call append("$", "number\tshow the line number for each line")
call append("$", "\t(local to window)")
call <SID>BinOption("nu")


call <SID>Header("syntax and highlighting")
call append("$", "background\t\"dark\" or \"light\"; the background color brightness")
call <SID>OptionL("bg", &bg)
if has("autocmd")
  call append("$", "filetype\ttype of file; triggers the FileType event when set")
  call append("$", "\t(local to buffer)")
  call <SID>Option("ft")
endif
if has("syntax")
  call append("$", "syntax\tname of syntax highlighting used")
  call append("$", "\t(local to buffer)")
  call <SID>Option("syn")
endif
call append("$", "highlight\twhich highlighting to use for various occasions")
call <SID>OptionL("hl", &hl)
call append("$", "hlsearch\thighlight all matches for the current search pattern")
call <SID>BinOptionL("hls", &hls)


call <SID>Header("multiple windows")
call append("$", "laststatus\t0, 1 or 2; when to use a status line for the last window")
call append("$", "set ls=" . &ls)
if has("statusline")
  call append("$", "statusline\talternate format to be used for a status line")
  call <SID>OptionL("stl", &stl)
endif
call append("$", "equalalways\tmake all windows the same size when adding/removing windows")
call <SID>BinOptionL("ea", &ea)
call append("$", "winheight\tminimal number of lines used for the current window")
call append("$", "set wh=" . &wh)
call append("$", "winminheight\tminimal number of lines used for any window")
call append("$", "set wmh=" . &wmh)
call append("$", "helpheight\tinitial height of the help window")
call append("$", "set hh=" . &hh)
call append("$", "previewheight\tdefault height for the preview window")
call append("$", "set pvh=" . &pvh)
call append("$", "hidden\tdon't unload a buffer when no longer shown in a window")
call <SID>BinOptionL("hid", &hid)
call append("$", "switchbuf\t\"useopen\" and/or \"split\"; which window to use when jumping")
call append("$", "\tto a buffer")
call <SID>OptionL("swb", &swb)
call append("$", "splitbelow\ta new window is put below the current one")
call <SID>BinOptionL("sb", &sb)
if has("scrollbind")
  call append("$", "scrollbind\tthis window scrolls together with other bound windows")
  call append("$", "\t(local to window)")
  call <SID>BinOption("scb")
  call append("$", "scrollopt\t\"ver\", \"hor\" and/or \"jump\"; list of options for 'scrollbind'")
  call <SID>OptionL("sbo", &sbo)
endif


call <SID>Header("terminal")
call append("$", "term\tname of the used terminal")
call <SID>OptionL("term", &term)
call append("$", "ttytype\talias for 'term'")
call <SID>OptionL("tty", &tty)
call append("$", "ttybuiltin\tcheck built-in termcaps first")
call <SID>BinOptionL("tbi", &tbi)
call append("$", "ttyfast\tterminal connection is fast")
call <SID>BinOptionL("tf", &tf)
call append("$", "weirdinvert\tterminal that requires extra redrawing")
call <SID>BinOptionL("wiv", &wiv)
call append("$", "esckeys\trecognize keys that start with <Esc> in Insert mode")
call <SID>BinOptionL("ek", &ek)
call append("$", "scrolljump\tminimal number of lines to scroll at a time")
call append("$", "set sj=" . &sj)
call append("$", "ttyscroll\tmaximum number of lines to use scrolling instead of redrawing")
call append("$", "set tsl=" . &tsl)
if has("gui") || has("msdos") || has("win32")
  call append("$", "guicursor\tspecifies what the cursor looks like in different modes")
  call <SID>OptionL("gcr", &gcr)
endif
if has("title")
  let &title = s:old_title
  call append("$", "title\tshow info in the window title")
  call <SID>BinOptionL("title", &title)
  set notitle
  call append("$", "titlelen\tpercentage of 'columns' used for the window title")
  call append("$", "set titlelen=" . &titlelen)
  call append("$", "titlestring\twhen not empty, string to be used for the window title")
  call <SID>OptionL("titlestring", &titlestring)
  call append("$", "titleold\tstring to restore the title to when exiting Vim")
  call <SID>OptionL("titleold", &titleold)
  let &icon = s:old_icon
  call append("$", "icon\tset the text of the icon for this window")
  call <SID>BinOptionL("icon", &icon)
  set noicon
  call append("$", "iconstring\twhen not empty, text for the icon of this window")
  call <SID>OptionL("iconstring", &iconstring)
endif
if has("win32")
  call append("$", "restorescreen\trestore the screen contents when exiting Vim")
  call <SID>BinOptionL("rs", &rs)
endif


call <SID>Header("using the mouse")
call append("$", "mouse\tlist of flags for using the mouse")
call <SID>OptionL("mouse", &mouse)
if has("gui")
  call append("$", "mousefocus\tthe window with the mouse pointer becomes the current one")
  call <SID>BinOptionL("mousef", &mousef)
  call append("$", "mousehide\thide the mouse pointer while typing")
  call <SID>BinOptionL("mh", &mh)
endif
call append("$", "mousemodel\t\"extend\", \"popup\" or \"popup_setpos\"; what the right")
call append("$", "\tmouse button is used for")
call <SID>OptionL("mousem", &mousem)
call append("$", "mousetime\tmaximum time in msec to recognize a double-click")
call append("$", "set mouset=" . &mouset)
call append("$", "ttymouse\t\"xterm\", \"xterm2\", \"dec\" or \"netterm\"; type of mouse")
call <SID>OptionL("ttym", &ttym)


if has("gui")
  call <SID>Header("GUI")
  call append("$", "guifont\tlist of font names to be used in the GUI")
  call <SID>OptionL("gfn", &gfn)
  call append("$", "guioptions\tlist of flags that specify how the GUI works")
  call <SID>OptionL("go", &go)
  if has("gui_gtk")
    call append("$", "toolbar\t\"icons\", \"text\" and/or \"tooltips\"; how to show the toolbar")
    call <SID>OptionL("tb", &tb)
    call append("$", "guiheadroom\troom (in pixels) left above/below the window")
    call append("$", "set ghr=" . &ghr)
  endif
  call append("$", "guipty\tuse a pseudo-tty for I/O to external commands")
  call <SID>BinOptionL("guipty", &guipty)
  if has("xfontset")
    call append("$", "guifontset\tpair of fonts to be used, for multibyte editing")
    call <SID>OptionL("gfs", &gfs)
  endif
  if has("browse")
    call append("$", "browsedir\t\"last\", \"buffer\" or \"current\": which directory used for the file browser")
    call <SID>OptionL("bsdir", &bsdir)
  endif
  if has("winaltkeys")
    call append("$", "winaltkeys\t\"no\", \"yes\" or \"menu\"; how to use the ALT key")
    call <SID>OptionL("wak", &wak)
  endif
endif


call <SID>Header("messages and info")
call append("$", "terse\tadd 's' flag in 'shortmess' (don't show search message)")
call <SID>BinOptionL("terse", &terse)
call append("$", "shortmess\tlist of flags to make messages shorter")
call <SID>OptionL("shm", &shm)
call append("$", "showcmd\tshow (partial) command keys in the status line")
let &sc = s:old_sc
call <SID>BinOptionL("sc", &sc)
set nosc
call append("$", "showmode\tdisplay the current mode in the status line")
call <SID>BinOptionL("smd", &smd)
call append("$", "ruler\tshow cursor position below each window")
let &ru = s:old_ru
call <SID>BinOptionL("ru", &ru)
set noru
if has("statusline")
  call append("$", "rulerformat\talternate format to be used for the ruler")
  call <SID>OptionL("ruf", &ruf)
endif
call append("$", "report\tthreshold for reporting number of changed lines")
call append("$", "set report=" . &report)
call append("$", "verbose\tthe higher the more messages are given")
call append("$", "set vbs=" . &vbs)
call append("$", "more\tpause listings when the screen is full")
call <SID>BinOptionL("more", &more)
if has("dialog_con") || has("dialog_gui")
  call append("$", "confirm\tstart a dialog when a command fails")
  call <SID>BinOptionL("cf", &cf)
endif
call append("$", "errorbells\tring the bell for error messages")
call <SID>BinOptionL("eb", &eb)
call append("$", "visualbell\tuse a visual bell instead of beeping")
call <SID>BinOptionL("vb", &vb)


call <SID>Header("selecting text")
call append("$", "selection\t\"old\", \"inclusive\" or \"exclusive\"; how selecting text behaves")
call <SID>OptionL("sel", &sel)
call append("$", "selectmode\t\"mouse\", \"key\" and/or \"cmd\"; when to start Select mode")
call append("$", "\tinstead of Visual mode")
call <SID>OptionL("slm", &slm)
if has("clipboard")
  call append("$", "clipboard\t\"unnamed\" to use the * register like unnamed register")
  call append("$", "\t\"autoselect\" to always put selected text on the clipboard")
  call <SID>OptionL("cb", &cb)
endif
call append("$", "keymodel\t\"startsel\" and/or \"stopsel\"; what special keys can do")
call <SID>OptionL("km", &km)


call <SID>Header("editing text")
call append("$", "undolevels\tmaximum number of changes that can be undone")
call append("$", "set ul=" . &ul)
call append("$", "modified\tchanges have been made and not written to a file")
call append("$", "\t(local to buffer)")
call <SID>BinOption("mod")
call append("$", "readonly\tbuffer is not to be written")
call append("$", "\t(local to buffer)")
call <SID>BinOption("ro")
call append("$", "textwidth\tline length above which to break a line")
call append("$", "\t(local to buffer)")
call <SID>Option("tw")
call append("$", "wrapmargin\tmargin from the right in which to break a line")
call append("$", "\t(local to buffer)")
call <SID>Option("wm")
call append("$", "backspace\t0, 1 or 2; what <BS> can do in Insert mode")
call append("$", "set bs=" . &bs)
call append("$", "comments\tdefinition of what comment lines look like")
call append("$", "\t(local to buffer)")
call <SID>Option("com")
call append("$", "formatoptions\tlist of flags that tell how automatic formatting works")
call append("$", "\t(local to buffer)")
call <SID>Option("fo")
if has("insert_expand")
  call append("$", "complete\tspecifies how Insert mode completion works")
  call append("$", "\t(local to buffer)")
  call <SID>Option("cpt")
  call append("$", "dictionary\tlist of dictionary files for keyword completion")
  call <SID>OptionL("dict", &dict)
endif
call append("$", "infercase\tadjust case of a keyword completion match")
call append("$", "\t(local to buffer)")
call <SID>BinOption("inf")
if has("digraphs")
  call append("$", "digraph\tenable entering digraps with c1 <BS> c2")
  call <SID>BinOptionL("dg", &dg)
endif
call append("$", "tildeop\tthe \"~\" command behaves like an operator")
call <SID>BinOptionL("top", &top)
call append("$", "showmatch\tWhen inserting a bracket, briefly jump to its match")
call <SID>BinOptionL("sm", &sm)
call append("$", "matchtime\ttenth of a second to show a match for 'showmatch'")
call append("$", "set mat=" . &mat)
call append("$", "matchpairs\tlist of pairs that match for the \"%\" command")
call append("$", "\t(local to buffer)")
call <SID>Option("mps")
call append("$", "joinspaces\tuse two spaces after '.' when joining a line")
call <SID>BinOptionL("js", &js)
call append("$", "nrformats\t\"octal\" and/or \"hex\"; number formats recognized for")
call append("$", "\tCTRL-A and CTRL-X commands")
call append("$", "\t(local to buffer)")
call <SID>Option("nf")


call <SID>Header("tabs and indenting")
call append("$", "tabstop\tnumber of spaces a <Tab> in the text stands for")
call append("$", "\t(local to buffer)")
call <SID>Option("ts")
call append("$", "shiftwidth\tnumber of spaces used for each step of (auto)indent")
call append("$", "\t(local to buffer)")
call <SID>Option("sw")
call append("$", "smarttab\ta <Tab> in an indent inserts 'shiftwidth' spaces")
call <SID>BinOptionL("sta", &sta)
call append("$", "softtabstop\tif non-zero, number of spaces to insert for a <Tab>")
call append("$", "\t(local to buffer)")
call <SID>Option("sts")
call append("$", "shiftround\tround to 'shiftwidth' for \"<<\" and \">>\"")
call <SID>BinOptionL("sr", &sr)
call append("$", "expandtab\texpand <Tab> to spaces in Insert mode")
call append("$", "\t(local to buffer)")
call <SID>BinOption("et")
call append("$", "autoindent\tautomatically set the indent of a new line")
call append("$", "\t(local to buffer)")
call <SID>BinOption("ai")
if has("smartindent")
  call append("$", "smartindent\tdo clever autoindenting")
  call append("$", "\t(local to buffer)")
  call <SID>BinOption("si")
endif
if has("cindent")
  call append("$", "cindent\tenable specific indenting for C code")
  call append("$", "\t(local to buffer)")
  call <SID>BinOption("cin")
  call append("$", "cinoptions\toptions for C-indenting")
  call append("$", "\t(local to buffer)")
  call <SID>Option("cino")
  call append("$", "cinkeys\tkeys that trigger C-indenting in Insert mode")
  call append("$", "\t(local to buffer)")
  call <SID>Option("cink")
  call append("$", "cinwords\tlist of words that cause more C-indent")
  call append("$", "\t(local to buffer)")
  call <SID>Option("cinw")
endif
call append("$", "lisp\tenable lisp mode")
call append("$", "\t(local to buffer)")
call <SID>BinOption("lisp")


call <SID>Header("mapping")
call append("$", "maxmapdepth\tmaximum depth of mapping")
call append("$", "set mmd=" . &mmd)
call append("$", "remap\trecognize mappings in mapped keys")
call <SID>BinOptionL("remap", &remap)
call append("$", "timeout\ttime-out halfway a mapping")
call <SID>BinOptionL("to", &to)
call append("$", "ttimeout\ttime-out halfway a key code")
call <SID>BinOptionL("ttimeout", &ttimeout)
call append("$", "timeoutlen\ttime in msec for 'timeout'")
call append("$", "set tm=" . &tm)
call append("$", "ttimeoutlen\ttime in msec for 'ttimeout'")
call append("$", "set ttm=" . &ttm)


call <SID>Header("reading and writing files")
call append("$", "modeline\tenable using settings from modelines when reading a file")
call append("$", "\t(local to buffer)")
call <SID>BinOption("ml")
call append("$", "modelines\tnumber of lines to check for modelines")
call append("$", "set mls=" . &mls)
call append("$", "binary\tbinary file editing")
call append("$", "\t(local to buffer)")
call <SID>BinOption("bin")
call append("$", "endofline\tlast line in the file has an end-of-line")
call append("$", "\t(local to buffer)")
call <SID>BinOption("eol")
call append("$", "fileformat\tend-of-line format: \"dos\", \"unix\" or \"mac\"")
call append("$", "\t(local to buffer)")
call <SID>Option("ff")
call append("$", "fileformats\tlist of file formats to look for when editing a file")
call <SID>OptionL("ffs", &ffs)
call append("$", "textmode\tobsolete, use 'fileformat'")
call append("$", "\t(local to buffer)")
call <SID>BinOption("tx")
call append("$", "textauto\tobsolete, use 'fileformats'")
call <SID>BinOptionL("ta", &ta)
call append("$", "write\twriting files is allowed")
call <SID>BinOptionL("write", &write)
call append("$", "writebackup\twrite a backup file before overwriting")
call <SID>BinOptionL("wb", &wb)
call append("$", "backup\tkeep a backup after overwriting a file")
call <SID>BinOptionL("bk", &bk)
call append("$", "backupdir\tlist of directories to put backup files in")
call <SID>OptionL("bdir", &bdir)
call append("$", "backupext\tfile name extension for the backup file")
call <SID>OptionL("bex", &bex)
call append("$", "autowrite\tautomatically write a file when leaving a modified buffer")
call <SID>BinOptionL("aw", &aw)
call append("$", "writeany\talways write without asking for confirmation")
call <SID>BinOptionL("wa", &wa)
call append("$", "patchmode\tkeep oldest version of a file; specifies file name extension")
call <SID>OptionL("pm", &pm)
if !has("msdos")
  call append("$", "shortname\tuse 8.3 file names")
  call append("$", "\t(local to buffer)")
  call <SID>BinOption("sn")
endif


call <SID>Header("the swap file")
call append("$", "directory\tlist of directories for the swap file")
call <SID>OptionL("dir", &dir)
call append("$", "swapfile\tuse a swap file for this buffer")
call append("$", "\t(local to buffer)")
call <SID>BinOption("swf")
call append("$", "swapsync\t\"sync\", \"fsync\" or empty; how to flush a swap file to disk")
call <SID>OptionL("sws", &sws)
call append("$", "updatecount\tnumber of characters typed to cause a swap file update")
call append("$", "set uc=" . &uc)
call append("$", "updatetime\ttime in msec after which the swap file will be updated")
call append("$", "set ut=" . &ut)
call append("$", "maxmem\tmaximum amount of memory in Kbyte used for one buffer")
call append("$", "set mm=" . &mm)
call append("$", "maxmemtot\tmaximum amount of memory in Kbyte used for all buffers")
call append("$", "set mmt=" . &mmt)


call <SID>Header("command line editing")
call append("$", "history\thow many command lines are remembered ")
call append("$", "set hi=" . &hi)
call append("$", "wildchar\tkey that triggers command-line expansion")
call append("$", "set wc=" . &wc)
call append("$", "wildcharm\tlike 'wildchar' but can also be used in a mapping")
call append("$", "set wcm=" . &wcm)
call append("$", "wildmode\tspecifies how command line completion works")
call <SID>OptionL("wim", &wim)
call append("$", "suffixes\tlist of file name extensions that have a lower priority")
call <SID>OptionL("su", &su)
if has("wildignore")
  call append("$", "wildignore\tlist of patterns to ignore files for file name completion")
  call <SID>OptionL("wig", &wig)
endif
if has("wildmenu")
  call append("$", "wildmenu\tcommand-line completion shows a list of matches")
  call <SID>BinOptionL("wmnu", &wmnu)
endif


call <SID>Header("executing external commands")
call append("$", "shell\tname of the shell program used for external commands")
call <SID>OptionL("sh", &sh)
if has("amiga")
  call append("$", "shelltype\twhen to use the shell or directly execute a command")
  call append("$", "set st=" . &st)
endif
call append("$", "shellquote\tcharacter(s) to enclose a shell command in")
call <SID>OptionL("shq", &shq)
call append("$", "shellxquote\tlike 'shellquote' but include the redirection")
call <SID>OptionL("sxq", &sxq)
call append("$", "shellcmdflag\targument for 'shell' to execute a command")
call <SID>OptionL("shcf", &shcf)
call append("$", "shellredir\tused to redirect command output to a file")
call <SID>OptionL("srr", &srr)
call append("$", "equalprg\tprogram used for \"=\" command")
call <SID>OptionL("ep", &ep)
call append("$", "formatprg\tprogram used to format lines with \"gq\" command")
call <SID>OptionL("fp", &fp)
call append("$", "keywordprg\tprogram used for the \"K\" command")
call <SID>OptionL("kp", &kp)
call append("$", "warn\twarn when using a shell command and a buffer has changes")
call <SID>BinOptionL("warn", &warn)


if has("quickfix")
  call <SID>Header("running make and jumping to errors")
  call append("$", "errorfile\tname of the file that contains error messages")
  call <SID>OptionL("ef", &ef)
  call append("$", "errorformat\tlist of formats for error messages")
  call <SID>OptionL("efm", &efm)
  call append("$", "makeprg\tprogram used for the \":make\" command")
  call <SID>OptionL("mp", &mp)
  call append("$", "shellpipe\tstring used to put the output of \":make\" in the error file")
  call <SID>OptionL("sp", &sp)
  call append("$", "makeef\tname of the errorfile for the 'makeprg' command")
  call <SID>OptionL("mef", &mef)
  call append("$", "grepprg\tprogram used for the \":grep\" command")
  call <SID>OptionL("gp", &gp)
  call append("$", "grepformat\tlist of formats for output of 'grepprg'")
  call <SID>OptionL("gfm", &gfm)
endif


if has("msdos") || has("osfiletype")
  call <SID>Header("system specific")
  if has("msdos")
    call append("$", "bioskey\tcall the BIOS to get a keyoard character")
    call <SID>BinOptionL("biosk", &biosk)
    call append("$", "conskey\tuse direct console I/O to get a keyboard character")
    call <SID>BinOptionL("consk", &consk)
  endif
  if has("osfiletype")
    call append("$", "osfiletype\tOS-specific information about the type of file")
    call append("$", "\t(local to buffer)")
    call <SID>Option("oft")
  endif
endif


call <SID>Header("language specific")
call append("$", "isfname\tspecifies the characters in a file name")
call <SID>OptionL("isf", &isf)
call append("$", "isident\tspecifies the characters in an identifier")
call <SID>OptionL("isi", &isi)
call append("$", "iskeyword\tspecifies the characters in a keyword")
call append("$", "\t(local to buffer)")
call <SID>Option("isk")
call append("$", "isprint\tspecifies printable characters")
call <SID>OptionL("isp", &isp)
if has("rightleft")
  call append("$", "rightleft\tdisplay the buffer right-to-left")
  call append("$", "\t(local to window)")
  call <SID>BinOption("rl")
  call append("$", "revins\tInsert characters backwards")
  call <SID>BinOptionL("ri", &ri)
  call append("$", "allowrevins\tAllow CTRL-_ in Insert and Command-line mode to toggle 'revins'")
  call <SID>BinOptionL("ari", &ari)
  call append("$", "aleph\tthe ASCII code for the first letter of the Hebrew alphabet")
  call append("$", "set al=" . &al)
  call append("$", "hkmap\tuse Hebrew keyboard mapping")
  call <SID>BinOptionL("hk", &hk)
  call append("$", "hkmapp\tuse phonetic Hebrew keyboard mapping")
  call <SID>BinOptionL("hkp", &hkp)
endif
if has("farsi")
  call append("$", "altkeymap\tuse Farsi as the second language when 'revins' is set")
  call <SID>BinOptionL("akm", &akm)
  call append("$", "fkmap\tuse Farsi keyboard mapping")
  call <SID>BinOptionL("fk", &fk)
endif
if has("multi_byte")
  call append("$", "fileencoding\tcharacter encoding of the file: \"ansi\", \"japan\"")
  call append("$", "\t\"korea\", \"prc\" or \"taiwan\"")
  call append("$", "\t(local to buffer)")
  call <SID>Option("fe")
endif
if has("langmap")
  call append("$", "langmap\ttranslate characters for Command mode")
  call <SID>OptionL("lmap", &lmap)
endif


call <SID>Header("various")
if has("autocmd")
  call append("$", "eventignore\tlist of autocommand events which are to be ignored")
  call <SID>OptionL("ei", &ei)
endif
call append("$", "exrc\tenable reading .vimrc/.exrc/.gvimrc in the current directory")
call <SID>BinOptionL("ex", &ex)
call append("$", "secure\tsafer working with script files in the current directory")
call <SID>BinOptionL("secure", &secure)
call append("$", "gdefault\tuse the 'g' flag for \":substitute\"")
call <SID>BinOptionL("gd", &gd)
call append("$", "edcompatible\t'g' and 'c' flags of \":substitute\" toggle")
call <SID>BinOptionL("ed", &ed)
  call append("$", "maxfuncdepth\tmaximum depth of function calls")
  call append("$", "set mfd=" . &mfd)
if has("mksession")
  call append("$", "sessionoptions\tlist of words that specifies what to put in a session file")
  call <SID>OptionL("ssop", &ssop)
endif
if has("viminfo")
  call append("$", "viminfo\tlist that specifies what to write in the viminfo file")
  call <SID>OptionL("vi", &vi)
endif

set cpo&vim

" go to first line
1

" reset 'modified', so that ":q" can be used to close the window
set nomodified

if has("syntax")
  " Use Vim highlighting, with some additional stuff
  set ft=vim
  syn match optwinHeader "^ \=[0-9].*"
  syn match optwinName "^[a-z]*\t" nextgroup=optwinComment
  syn match optwinComment ".*" contained
  syn match optwinComment "^\t.*"
  if !exists("did_optwin_syntax_inits")
    let did_optwin_syntax_inits = 1
    hi link optwinHeader Title
    hi link optwinName Identifier
    hi link optwinComment Comment
  endif
endif

" Install autocommands to enable mappings in option-window
noremap <buffer> <CR> :call <SID>CR()<CR><C-\><C-N>:echo<CR>
inoremap <buffer> <CR> <Esc>:call <SID>CR()<CR>:echo<CR>
noremap <buffer> <Space> :call <SID>Space()<CR>:echo<CR>
inoremap <buffer> <Space> <Esc>:call <SID>Space()<CR>:echo<CR>

" Make the buffer be deleted when the window is closed.
set buftype=nofile bufhidden=delete noswapfile

augroup optwin
  au! BufUnload,BufHidden option-window nested
  	\ call <SID>unload() | delfun <SID>unload
augroup END

fun! <SID>unload()
  delfun <SID>CR
  delfun <SID>Space
  delfun <SID>Find
  delfun <SID>Update
  delfun <SID>Option
  delfun <SID>OptionL
  delfun <SID>BinOption
  delfun <SID>BinOptionL
  delfun <SID>Header
  au! optwin
endfun

" Restore the previous value of 'title' and 'icon'.
let &title = s:old_title
let &icon = s:old_icon
let &ru = s:old_ru
let &sc = s:old_sc
let &cpo = s:cpo_save
unlet s:old_title s:old_icon s:old_ru s:old_sc s:cpo_save s:idx s:lnum
