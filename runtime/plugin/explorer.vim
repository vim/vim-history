"=============================================================================
" File : explorer.vim
" Author : M A Aziz Ahmed (aziz@123india.com)
" Last update : Thu Dec 07 2000
" Version : 2.0
"-----------------------------------------------------------------------------
" This file implements a file explorer. Latest version available at:
" http://www.freespeech.org/aziz/vim/
"-----------------------------------------------------------------------------
" Just edit a directory or type :Explore to launch the file explorer (this
" file should have been sourced) in a separate window. Type :Sexplore to split
" the current window and launch explorer there. If the current buffer is
" modified, the window is anyway split (irrespective of <Leader>e or
" <Leader>s).
" It is also possible to delete files and rename files within explorer.
" The directory which explorer uses by default is determined by the 'browsedir'
" option.
"-----------------------------------------------------------------------------
" Updates from last version (1.1) :
" 1. No corruption of registers (either named or unnamed)
" 2. Possible to edit a file in a new window.
" 3. The help is only one line with an option for detailed help
" 4. Works as a plugin for Vim6.0+
"=============================================================================

" Has this already been loaded?
if exists("loaded_explorer")
  finish
endif
let loaded_explorer=1


"Default settings :
if (!exists("g:explVertical"))
  let g:explVertical=0
endif

if (!exists("g:explWinSize"))
  let g:explWinSize=15
endif

if (!exists("g:explDetailedHelp"))
  let g:explDetailedHelp=0
endif

if (!exists("g:explShowFileSize"))
  let g:explShowFileSize=0
endif

if (!exists("g:explShowFileDate"))
  let g:explShowFileDate=0
endif

if (!exists("g:explDateFormat"))
  let g:explDateFormat="%d %b %Y %H:%M"
endif

if (!exists("g:explHideFiles"))
  let g:explHideFiles='^\.,\.zip$'
endif

if !exists(':Explore')
  command Explore :call s:StartExplorer(0)
endif
if !exists(':Sexplore')
  command Sexplore :call s:StartExplorer(1)
endif

" Start the explorer using the preferences from the global variables
"
function! s:StartExplorer(split)
  let startcmd = "edit"
  if (a:split) 
    let startcmd = g:explWinSize . "new ."
    if (g:explVertical != 0)
      let startcmd = "vertical " . startcmd
    endif
  else
    let startcmd = "edit ."
  endif
  execute startcmd
endfunction

" This is the main entry for 'editing' a directory
"
function! s:EditDir()
  " There was some code to workaround a windows problem here that
  " doesn't seem relevant any more.

  " Get out of here right away if this isn't a directory!
  if (!isdirectory(expand("%")))
    return
  endif

  " If directory is already loaded, don't open it again!
  if (line('$') > 1)
    return
  endif
  " Get the complete path to the directory to look at with a slash at
  " the end
  let b:completePath = substitute(expand("%:p"), '\\', '/', 'g')
  "let origdir = getcwd()
  "exe "chdir" escape(b:completePath,' \')
  "let b:completePath = getcwd()
  if (b:completePath !~ '/$')
    let b:completePath = b:completePath . '/'
  endif
  let b:completePath = substitute(b:completePath, '/[^/]*/\.\./', '/', 'g')
  "exe "chdir" escape(origdir,' \')

  " escape special characters for exec commands
  let b:completePathEsc=escape(b:completePath,' \')
  let b:parentDirEsc=substitute(b:completePathEsc, '/[^/]*/$', '/', 'g')

  " 
  if (!exists("s:tempShowFileSize"))
    let s:tempShowFileSize=0
  endif
  if (!exists("s:tempShowFileDate"))
    let s:tempShowFileDate=0
  endif
  call s:SyntaxFile()
  let g:filterFormula=substitute(g:explHideFiles, '\([^\\]\),', '\1\\|', 'g')
  call s:ShowDirectory()

  " Save options
  let b:oldSwap=&swapfile | set noswapfile
  let &swapfile=b:oldSwap
  let b:oldCpo=&cpo | set cpo=
  let &cpo=b:oldCpo

  let b:splitMode=""
  if (g:explVertical==1)
    let b:splitMode="vertical"
  endif
  " Set up mappings for this buffer
  nm <buffer> <cr> :exec ("silent e "  . <SID>GetFileNameEsc())<cr>:call <SID>DelPrevBuffer()<cr>
  nm <buffer> e :exec ("silent e "  . <SID>GetFileNameEsc())<cr>:call <SID>DelPrevBuffer()<cr>
  nm <buffer> -    :exec ("silent e "  . b:parentDirEsc)<cr>:call <SID>DelPrevBuffer()<cr>
  nm <buffer> o    :exec ("silent ".b:splitMode." sp " .
                           \<SID>GetFileNameEsc())<cr><c-w>x
                           \:exec("silent ".b:splitMode." resize ".g:explWinSize)<cr><c-w>p
  nm <buffer> h    :call <SID>ExpandHelp()<cr>
  "nm <buffer> c    :ChangeDirectory to: 
  nnoremap <buffer> a :call <SID>ShowAllFiles()<cr>
  nm <buffer> r    :call <SID>RenameFile()<cr>
  nm <buffer> d    :. call <SID>DeleteFile()<cr>
  vm <buffer> d    :call <SID>DeleteFile()<cr>
  nm <buffer> i    :call <SID>IncrVerbosity()<cr>
  nm <buffer> s    :exec ("cd ".b:completePathEsc)<cr>
  "command! -buffer -nargs=+ -complete=dir ChangeDirectory call s:GotoDir(<f-args>)

endfunction

function! s:DelPrevBuffer()
  if (isdirectory(@#))
    silent bd!#
  endif
endfunction
" Show the header and contents of the directory
"
function! s:ShowDirectory()
  "Delete all lines
  1,$d _
  let oldRep=&report
  set report=1000
  call s:AddHeader(g:explDetailedHelp)
  $ d
  call s:DisplayFiles(b:completePath)
  normal zz
  echo "Loaded contents of ".b:completePath
  let &report=oldRep
  let &modified=0
endfunction

" Extract the file name from the line the cursor is currently on
"
function! s:GetFileNameEsc()
    let n=escape(s:GetFileName(),' \%#')
    let n=substitute(n, '/[^/]*/\.\./', '/', 'g')
    "call message(n)
    return n
endfunction

function! s:GetFileName()
  if ((b:fileSizesShown==0) && (b:fileDatesShown==0))
    return b:completePath.getline(".")
  else
    let firstField=substitute(getline("."), '^\(.\{'.b:maxFileLen.'}\).*$','\1','g')
    return b:completePath.substitute(firstField, '\s*$','','')
  endif
endfunction

" Add the header with help information
"
function! s:AddHeader(detailed)
    let save_f=@f
    exe '1'
    if (a:detailed==1)
      " Give a very brief help
      let @f="\" <enter> : open file or directory\n"
      let @f=@f."\" - : go up one level      a : show all files\n"
      let @f=@f."\" r : rename file          d : delete file\n"
      let @f=@f."\" s : cd to this dir       o : edit file in a new window\n"
      let @f=@f."\" i : increase verbosity   e : edit file in this window\n"
    else
      let @f="\"Press h for detailed help\n"
    endif
    let @f=@f."\"---------------------------------------------------\n"
    let @f=@f.". ".b:completePath."\n"
    let @f=@f."\"---------------------------------------------------\n"
    " Add parent directory
    let @f=@f."../\n"
    put! f
    let @f=save_f
endfunction

"
"
function! s:DisplayFiles(dir)
 
  " save f register
  let save_f=@f

  " Get a list of all the files
  let @f=glob(a:dir."*")

  " Removing dot files if they are a part of @f. In Unix for example, dot files
  " are not included, but in Windows they are!
  let @f=substitute(@f, "[^\n]*[\\\\/]\\.[^\\\\/\n]*\n", '', 'g')

  " Add the dot files now, making sure "." and ".." files are not included!
  let @f=@f.substitute(glob(a:dir.".*"), "[^\n]*".'[\\/]\.[\\/]\='."\n[^\n]*".'\.\.[\\/]\='."[\n]".'\=', '' , '')

  " Clear flags indicating that sizes are shown
  let b:fileSizesShown=0
  let b:fileDatesShown=0

  " Are there any files left after filtering?
  if (@f!="")
    normal mt
    put f
    let b:maxFileLen=0
    0
    /^\.\.\//,$g/^/call s:MarkDirs()
    normal `t
    call s:ShowFileSizes((g:explShowFileSize) || (s:tempShowFileSize))
    call s:ShowFileDates((g:explShowFileDate) || (s:tempShowFileDate))
  endif

  " restore f register
  let @f=save_f

endfunction

"
"
function! s:MarkDirs()
  let oldRep=&report
  set report=1000
  "Remove slashes if added
  s;/$;;e  
  "Removes all the leading slashes and adds slashes at the end of directories
  s;^.*\\\([^\\]*\)$;\1;e
  s;^.*/\([^/]*\)$;\1;e
  "normal ^
  let currLine=getline(".")
  if (isdirectory(s:GetFileName()))
    s;$;/;
    let fileLen=strlen(currLine)+1
  else
    let fileLen=strlen(currLine)
    if ((g:filterFormula!="") && (currLine =~ g:filterFormula))
      " Don't show the file if it is to be filtered.
      d     
    else
      " Move the file at the end so that directories appear first
      m$
    endif
  endif
  if (fileLen > b:maxFileLen)
    let b:maxFileLen=fileLen
  endif
  let &report=oldRep
endfunction

" Show the size for each file
"
function! s:ShowFileSizes(enable)
  if (a:enable==1)
    normal mt
    if (b:fileDatesShown==1)
      "Check if date field exists
      0
      /^\.\.\//,$g/^/let fileSize=getfsize(s:GetFileName()) |
          \exec "norm ".(9-strlen(fileSize))."A \<esc>" |
          \exec 's/$/'.fileSize.'/'
    else
      0
      /^\.\.\//,$g/^/let fileSize=getfsize(s:GetFileName()) |
          \exec "norm ".(b:maxFileLen-strlen(getline("."))+2+9-strlen(fileSize))."A \<esc>" |
          \exec 's/$/'.fileSize.'/'
    endif
    let b:fileSizesShown=1
    let b:maxLenCorrect=1
    set nomodified
    normal `t
  endif
endfunction

" Show the last modified date for each file
"
function! s:ShowFileDates(enable)
  if (a:enable==1)
    normal mt
    if (b:fileSizesShown==1)
      "If file size field exists
      0
      /^\.\.\//,$g/^/exec 's/$/ '.escape(s:FileModDate(s:GetFileName()), '/').'/'
    else
      0
      /^\.\.\//,$g/^/let fn=s:GetFileName() |
                     \exec "norm $".(b:maxFileLen-strlen(getline("."))+2)."a \<esc>" |
                     \exec 's/$/ '.escape(s:FileModDate(s:(fn)), '/').'/'

    endif
    let b:fileDatesShown=1
    let b:maxLenCorrect=1
    set nomodified
    normal `t
  endif
endfunction

" Get the time for a file
"
function! s:FileModDate(name)
  let filetime=getftime(a:name)
  if (filetime > 0)
    return strftime(g:explDateFormat,filetime)
  else
    return ""
  endif
endfunction

" Delete a file
"
function! s:DeleteFile() range
  let oldRep = &report
  let &report = 1000

  let filesDeleted = 0
  let stopDel = 0
  let delAll = 0
  let currLine = a:firstline
  let lastLine = a:lastline
  while ((currLine <= lastLine) && (stopDel==0))
    exec(currLine)
    let fileName=s:GetFileName()
    if (isdirectory(fileName))
      echo fileName." : Directory deletion not supported yet"
      let currLine = currLine + 1
    else
      if (delAll == 0)
        let sure=input("Delete ".fileName."?(y/n/a/q) ")
        if (sure=="a")
          let delAll = 1
        endif
      endif
      if ((sure=="y") || (sure=="a"))
        let success=delete(fileName)
        if (success!=0)
          exec (" ")
          echo "\nCannot delete ".fileName
          let currLine = currLine + 1
        else
          d _
          let filesDeleted = filesDeleted + 1
          let lastLine = lastLine - 1
        endif
      elseif (sure=="q")
        let stopDel = 1
      elseif (sure=="n")
        let currLine = currLine + 1
      endif
    endif
  endwhile
  echo "\n".filesDeleted." files deleted"
  let &report = oldRep
  let &modified=0
endfunction

" Rename a file
"
function! s:RenameFile()
  let fileName=s:GetFileName()
  if (isdirectory(fileName))
    echo "Directory renaming not supported yet"
  elseif (filereadable(fileName))
    let altName=input("Rename ".fileName." to : ")
    echo " "
    if (altName=="")
      return
    endif
    let success=rename(fileName, b:completePath.altName)
    if (success!=0)
      echo "Cannot rename ".fileName. " to ".altName
    else
      echo "Renamed ".fileName." to ".altName
      let oldRep=&report
      set report=1000
      e!
      let &report=oldRep
    endif
  endif
  let &modified=0
endfunction

"
"
"function! s:GotoDir(dummy, dirName)
"  if (isdirectory(expand(a:dirName)))
"    " Guess the complete path
"    "if (isdirectory(expand(getcwd()."/".a:dirName)))
"    "  let dirpath=getcwd()."/".a:dirName
"    "else
"    "  let dirpath=expand(a:dirName)
"    "endif
"    "let b:completePath=dirpath
"    "call s:ShowDirectory()
"    exec("e ".a:dirName)
"  else
"    echo a:dirName." : No such directory"
"  endif
"endfunction

" Set up the syntax highlighting for directory explorer
"
function! s:SyntaxFile()
  if 1 || has("syntax") && exists("syntax_on") && !has("syntax_items")
    syn match browseSynopsis	"^\".*"
    syn match browseDirectory	"[^\"].*/ "
    syn match browseDirectory	"[^\"].*/$"
    syn match browseCurDir	"^\. .*$"
    
    if !exists("g:did_browse_syntax_inits")
      let did_browse_syntax_inits = 1
      hi link browseSynopsis	PreProc
      hi link browseDirectory	Directory
      hi link browseCurDir	Statement
    endif
  endif
endfunction
  
" Show long help
"
function! s:ExpandHelp()
  normal mt
  0
  1, /^\.\.\//- d
  call s:AddHeader(1)
  0
  /^\.\.\// d _
  normal `t
  let &modified=0
endfunction

"
"
function! s:IncrVerbosity()
  if (b:fileSizesShown==0)
    call s:ShowFileSizes(1)
    let s:tempShowFileSize=1
  elseif (b:fileDatesShown==0)
    call s:ShowFileDates(1)
    let s:tempShowFileDate=1
  endif
endfunction

" Show all files
"
function! s:ShowAllFiles()
  let g:filterFormula=""
  call s:ShowDirectory()
endfunction

" Set up the autocommand to allow directories to be edited
"
augroup fileExplorer
  au!
  au BufEnter * call s:EditDir()
augroup end
