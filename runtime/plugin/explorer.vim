"=============================================================================
" File : explorer.vim
" Author : M A Aziz Ahmed (aziz@123india.com)
" Last update : Thu Nov 02 2000
" Version : 2.0
"-----------------------------------------------------------------------------
" This file implements a file explorer. Latest version available at:
" http://www.freespeech.org/aziz/vim/
"-----------------------------------------------------------------------------
" Just type <Leader>e to launch the file explorer (this file should have been
" sourced) in a separate window. Type <Leader>s to split the current window and
" launch explorer there. If the current buffer is modified, the window is
" anyway split (irrespective of <Leader>e or <Leader>s).
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

if (!exists("g:explShowDotFiles"))
  let g:explShowDotFiles=0
endif


if exists("loaded_explorer") || &cp
  finish
endif
let loaded_explorer=1

if !hasmapto('<Plug>ExplorerInitiate0')
  nmap <unique> <Leader>e <Plug>ExplorerInitiate0
endif
if !hasmapto('<Plug>ExplorerInitiate1')
  nmap <unique> <Leader>s <Plug>ExplorerInitiate1
endif

nnoremap <unique> <script> <Plug>ExplorerInitiate0 :call <SID>Initiate(0)<cr>
nnoremap <unique> <script> <Plug>ExplorerInitiate1 :call <SID>Initiate(1)<cr>

function! <SID>Initiate(split, ...)
  if (expand("%:p:t")=="_fileExplorer.tmp")
    echo "Already in file explorer"
  else
    let s:oldCh=&ch
    let &ch=2
    if (a:0==0)
      call <SID>InitializeDirName("")
    else
      call <SID>InitializeDirName(a:1)
    endif
    if ((&modified==1) || (a:split==1))
      if (g:explVertical==0)
        "exec g:explWinSize.'sp /_fileExplorer.tmp'
        exec 'sp /_fileExplorer.tmp'
      else
        "exec g:explWinSize.'vsp /_fileExplorer.tmp'
        exec 'vsp /_fileExplorer.tmp'
      endif
      let b:splitWindow=1
    else
      e /_fileExplorer.tmp
      let b:splitWindow=0
    endif
    call <SID>SyntaxFile()
    call <SID>ProcessFile(g:currDir)
  endif
endfunction

function! <SID>InitializeDirName(dirName)
  if (a:dirName=="")
    if (exists("&bsdir"))
      if (&bsdir=="buffer")
        let startDir=expand("%:p:h")
      elseif ((!exists("g:currDir")) || (&bsdir=="current"))
        let startDir=getcwd()
      else
        let startDir=expand(g:currDir)
      endif
    elseif (!exists("g:currDir"))
      let startDir=getcwd()
    else
      let startDir=expand(g:currDir)
    endif
  else
    let startDir = a:dirName
  endif
  let g:currDir=(substitute(startDir,"\\","/","g"))."/"
  " In case the ending / was already a part of getcwd(), two //s would appear
  " at the end of g:currDir. So remove one of them
  let g:currDir=substitute(g:currDir,"//$","/","g")
  let g:currDir=substitute(g:currDir,"/\./","/","g")
endfunction

function! <SID>ProcessFile(fileName, ...)
  if ((isdirectory(a:fileName)) || (a:fileName==g:currDir."../"))
    "Delete all lines
    1,$d _
    let oldRep=&report
    set report=1000
    if (a:fileName==g:currDir."../")
      let g:currDir=substitute(g:currDir,"/[^/]*/$","/","")
    else
      let g:currDir=a:fileName
    endif
    let s:verboseDisplay=0
    call <SID>AddHeader(g:explDetailedHelp)
    call <SID>DisplayFiles(g:currDir)
    normal zz
    if (isdirectory(@#))
      " Delete the previous buffer if the explorer was launched by means of
      " editing a directory
      bd! #
    else
      echo "Loaded contents of ".g:currDir
    endif
    let &report=oldRep
  elseif (filereadable(a:fileName))
    let newWindow=0
    let openCmd="e! "
    if (a:0 > 0)
      if (a:1==1)
        let newWindow=1
        if (g:explVertical==0)
          sp
          exec 'resize '.g:explWinSize
          exec "norm \<c-w>p"
        else
          vsp
          exec 'vertical resize '.g:explWinSize
          exec "norm \<c-w>p"
        endif
      endif
    endif
    if (filereadable(@#))
      exec("e! ".escape(@#, ' %'))
    endif
    exec("e! ".escape(a:fileName, ' %'))
    if (newWindow==0)
      call <SID>CloseExplorer()
    endif
  endif
  let &modified=0
endfunction

function! <SID>GetFileName()
  if (s:verboseDisplay==0)
    return g:currDir.getline(".")
  else
    let firstField=substitute(getline("."), '^\(.\{'.s:maxFileLen.'}\).*$',
                             \'\1', 'g')
    return g:currDir.substitute(firstField, '\s*$', '', '')
  endif
endfunction

function! <SID>AddHeader(detailed)
    let save_f=@f
    if (a:detailed==1)
      " Give a very brief help
      let @f="\" <enter> : open file or directory\n"
      let @f=@f."\" - : go up one level      c : change directory\n"
      let @f=@f."\" r : rename file          d : delete file\n"
      let @f=@f."\" q : quit file explorer   s : set this dir to current directory\n"
      let @f=@f."\" i : increase verbosity   e : edit file in new window\n"
    else
      let @f="\"Press h for detailed help\n"
    endif
    let @f=@f."\"---------------------------------------------------\n"
    let @f=@f.". ".g:currDir."\n"
    let @f=@f."\"---------------------------------------------------\n"
    " Add parent directory
    let @f=@f."../\n"
    put! f
    /\.\.\//+ d _
    let @f=save_f
endfunction

function! <SID>DisplayFiles(dir)
  let save_f=@f
  let @f=glob(a:dir."*")
  " Removing dot files if they are a part of @f. In Unix for example, dot files
  " are not included, but in Windows they are!
  let @f=substitute(@f, "[^\n]*[\\\\/]\\.[^\\\\/\n]*\n", '', 'g')

  " Now see what the user wants :
  if (g:explShowDotFiles==1)
    let @f=@f.glob(a:dir.".*")
  endif

  if (@f!="")
    normal mt
    put f
    let s:maxFileLen=0
    .,$g/^/call <SID>MarkDirs()
    normal `t
    call <SID>ShowFileSizes(g:explShowFileSize)
    call <SID>ShowDates(g:explShowFileDate)
  endif
  let @f=save_f
endfunction

function! <SID>MarkDirs()
  let oldRep=&report
  set report=1000
  "Remove slashes if added
  s;/$;;e  
  "Removes all the leading slashes and adds slashes at the end of directories
  s;^.*\\\([^\\]*\)$;\1;e
  s;^.*/\([^/]*\)$;\1;e
  "normal ^
  if (isdirectory(<SID>GetFileName()))
    s;$;/;
    let fileLen=strlen(getline("."))
  else
    " Move the file at the end so that directories appear first
    let fileLen=strlen(getline("."))
    m$
  endif
  if (fileLen > s:maxFileLen)
    let s:maxFileLen=fileLen
  endif
  let &report=oldRep
endfunction

function! <SID>ShowFileSizes(enable)
  if (a:enable==1)
    normal mt
    if (s:verboseDisplay==1)
      "Check if date field exists
      /^\.\.\//,$g/^/let fileSize=getfsize(<SID>GetFileName()) |
          \exec "norm $".(9-strlen(fileSize))."a \<esc>" |
          \exec 's/$/'.fileSize.'/'
    else
      /^\.\.\//,$g/^/let fileSize=getfsize(<SID>GetFileName()) |
          \exec "norm $".(s:maxFileLen-strlen(getline("."))+2+9-strlen(fileSize))."a \<esc>" |
          \exec 's/$/'.fileSize.'/'
    endif
    let s:verboseDisplay=s:verboseDisplay+1
    set nomodified
    normal `t
  endif
endfunction

function! <SID>ShowDates(enable)
  if (a:enable==1)
    normal mt
      if (s:verboseDisplay==1)
        "If file size field exists
        /^\.\.\//,$g/^/exec 's/$/ '.escape(strftime(g:explDateFormat, getftime(<SID>GetFileName())), '/').'/'
      else
        /^\.\.\//,$g/^/let fileTime=getftime(<SID>GetFileName()) |
                       \exec "norm $".(s:maxFileLen-strlen(getline("."))+2)."a \<esc>" |
                       \exec 's/$/ '.escape(strftime(g:explDateFormat, fileTime), '/').'/'

      endif
      let s:verboseDisplay=s:verboseDisplay+1
      set nomodified
    normal `t
  endif
endfunction

function! <SID>DeleteFile() range
  let oldRep = &report
  let &report = 1000

  let filesDeleted = 0
  let stopDel = 0
  let delAll = 0
  let currLine = a:firstline
  let lastLine = a:lastline
  while ((currLine <= lastLine) && (stopDel==0))
    exec(currLine)
    let fileName=<SID>GetFileName()
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

function! <SID>RenameFile()
  let fileName=<SID>GetFileName()
  if (isdirectory(fileName))
    echo "Directory renaming not supported yet"
  elseif (filereadable(fileName))
    let altName=input("Rename ".fileName." to : ")
    echo " "
    let success=rename(fileName, g:currDir.altName)
    if (success!=0)
      echo "Cannot rename ".fileName. " to ".altName
    else
      echo "Renamed ".fileName." to ".altName
      let oldRep=&report
      set report=1000
      exec("s/^\\S*$/".altName."/")
      let &report=oldRep
    endif
  endif
  let &modified=0
endfunction

function! <SID>GotoDir(dummy, dirName)
  if (isdirectory(expand(a:dirName)))
    " Guess the complete path
    if (isdirectory(expand(getcwd()."/".a:dirName)))
      let dirpath=getcwd()."/".a:dirName
    else
      let dirpath=expand(a:dirName)
    endif
    call <SID>InitializeDirName(dirpath)
    call <SID>ProcessFile(g:currDir)
  else
    echo a:dirName." : No such directory"
  endif
endfunction

function! <SID>CloseExplorer()
  bd! /_fileExplorer.tmp
  if (exists("s:oldCh"))
    let &ch=s:oldCh
  endif
endfunction

function! <SID>Back2PrevFile()
  if ((@#!="") && (@#!="_fileExplorer.tmp") && (b:splitWindow==0) && 
        \(isdirectory(@#)==0))
    exec("e #")
  endif
  call <SID>CloseExplorer()
endfunction

function! <SID>SyntaxFile()
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
      
function! <SID>EditDir(fileName)
  if (isdirectory(a:fileName))
    " Do some processing if the path is relative..
    let completePath=expand("%:p")
    call <SID>Initiate(0, completePath)
  elseif ((expand("%")=="") && (bufloaded(".")==1))
    " This is a workaround for a vim bug in Windows. When one tries to edit   
    " :e .
    " expand("%") *sometimes* returns a blank string
    call <SID>Initiate(0, getcwd())
  endif
endfunction

function! <SID>ExpandHelp()
  1,/^\.\.\//-d
  call <SID>AddHeader(1)
  ?^\.\.\/?
  let &modified=0
endfunction

function! <SID>IncrVerbosity()
  if (s:verboseDisplay==0)
    call <SID>ShowFileSizes(1)
  elseif (s:verboseDisplay==1)
    if ((g:explShowFileSize==0) && (g:explShowFileDate==1))
      call <SID>ShowFileSizes(1)
    else
      call <SID>ShowDates(1)
    endif
  endif
endfunction

augroup fileExplorer
  au!
  au BufEnter _fileExplorer.tmp let s:oldSwap=&swapfile | set noswapfile
  au BufLeave _fileExplorer.tmp let &swapfile=s:oldSwap
  au BufEnter _fileExplorer.tmp let s:oldCpo=&cpo | set cpo&vim
  au BufLeave _fileExplorer.tmp let &cpo=s:oldCpo
  au BufEnter _fileExplorer.tmp nm <cr> :call <SID>ProcessFile(<SID>GetFileName())<cr>
  au BufLeave _fileExplorer.tmp nun <cr>
  au BufEnter _fileExplorer.tmp nm - :call <SID>ProcessFile(g:currDir."../")<cr>
  au BufLeave _fileExplorer.tmp nun -
  au BufEnter _fileExplorer.tmp nm e :call <SID>ProcessFile(<SID>GetFileName(), 1)<cr>
  au BufLeave _fileExplorer.tmp nun e
  au BufEnter _fileExplorer.tmp nm h :call <SID>ExpandHelp()<cr>
  au BufLeave _fileExplorer.tmp nun h
  au BufEnter _fileExplorer.tmp nm c :ChangeDirectory to: 
  au BufLeave _fileExplorer.tmp nun c
  au BufEnter _fileExplorer.tmp nm r :call <SID>RenameFile()<cr>
  au BufLeave _fileExplorer.tmp nun r
  au BufEnter _fileExplorer.tmp nm d :. call <SID>DeleteFile()<cr>
  au BufLeave _fileExplorer.tmp nun d
  au BufEnter _fileExplorer.tmp vm d :call <SID>DeleteFile()<cr>
  au BufLeave _fileExplorer.tmp vun d
  au BufEnter _fileExplorer.tmp nm q :call <SID>Back2PrevFile()<cr>
  au BufLeave _fileExplorer.tmp nun q
  au BufEnter _fileExplorer.tmp nm i :call <SID>IncrVerbosity()<cr>
  au BufLeave _fileExplorer.tmp nun i
  au BufEnter _fileExplorer.tmp nm s :exec ("cd ".escape(g:currDir,' '))<cr>
  au BufLeave _fileExplorer.tmp nun s
  au BufEnter _fileExplorer.tmp command! -nargs=+ -complete=dir ChangeDirectory call <SID>GotoDir(<f-args>)
  au BufLeave _fileExplorer.tmp delcommand ChangeDirectory
  au BufEnter * nested call <SID>EditDir(expand("%"))
augroup end
