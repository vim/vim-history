" Vim filetype plugin file
" Language:    MS-DOS .bat files
" Maintainer:  Mike Williams <mrw@netcomuk.co.uk>
" Last Change: 24th August 2001

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

" BAT comment formatting
setlocal comments=b:rem,b:REM,b:::
setlocal formatoptions-=t formatoptions+=rol

" Define patterns for the browse file filter
if has("gui_win32") && !exists("b:browsefilter")
  let b:browsefilter = "DOS Batch Files (*.bat, *.cmd)\t*.bat;*.cmd\nAll Files (*.*)\t*.*\n"
endif
