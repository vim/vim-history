" Debian Changelog mode

if exists("Debian_changelog")
  finish
endif
let Debian_changelog = 1

" Helper functions returning various data.
" Returns full name, either from $DEBFULLNAME or debianfullname.
" TODO Is there a way to determine name from anywhere else?
function <SID>FullName()
  if exists("$DEBFULLNAME")
    return $DEBFULLNAME
  elseif exists("g:debianfullname")
    return g:debianfullname
  else
    return "Your Name"
  endif
endfunction

" Returns email address, from $DEBEMAIL, $EMAIL or debianemail.
function <SID>Email()
  if exists("$DEBEMAIL")
    return $DEBEMAIL
  elseif exists("$EMAIL")
    return $EMAIL
  elseif exists("g:debianemail")
    return g:debianfullemail
  else
    return "your@email.address"
  endif
endfunction

" Returns date in RFC822 format. 822-date is in dpkg-dev, so Debian
" developers should have it.
function <SID>Date()
  if executable("822-date")
    return substitute(system("822-date"), "\n", "", "")
  else
    return "Dow, dd Mon yyyy hh:mm:ss"
  endif
endfunction

function <SID>WarnIfNotUnfinalised()
  if match(getline("."), " -- [[:alpha:]][[:alnum:].]")!=-1
    echohl WarningMsg
    echo "The entry has not been unfinalised before editing."
    echohl None
    return 1
  endif
  return 0
endfunction

" These functions implement the menus
function NewVersion()
  " The new entry is unfinalised and shall be changed
  "amenu disable Changelog.New\ Version
  "amenu enable Changelog.Add\ Entry
  "amenu enable Changelog.Close\ Bug
  "amenu disable Changelog.Unfinalise
  "amenu enable Changelog.Finalise
  call append(0, substitute(getline(1),'-\([[:digit:]]\+\))', '-Ü\1)', ''))
  call append(1, "")
  call append(2, "")
  call append(3, " -- ")
  call append(4, "")
  call Distribution("unstable")
  call Urgency("low")
  normal 1G
  call search(")")
  normal h
  normal 
  call setline(1, substitute(getline(1),'-Ü\([[:digit:]]\+\))', '-\1)', ''))
  call AddEntry()
endfunction

function AddEntry()
  normal 1G
  call search("^ -- ")
  normal kk
  call append(".", "  * ")
  normal jjj
  let warn=<SID>WarnIfNotUnfinalised()
  normal kk
  if warn
    echohl MoreMsg
    call input("Hit ENTER")
    echohl None
  endif
  startinsert!
endfunction

function CloseBug()
  normal 1G
  call search("^ -- ")
  let warn=<SID>WarnIfNotUnfinalised()
  normal kk
  call append(".", "  *  (closes: #" . input("Bug number to close: ") . ")")
  normal j^l 
  startinsert
endfunction

function Distribution(dist)
  call setline(1, substitute(getline(1), ") [[:lower:] ]*;", ") " . a:dist . ";", ""))
endfunction

function Urgency(urg)
  call setline(1, substitute(getline(1), "urgency=.*$", "urgency=" . a:urg, ""))
endfunction

function Unfinalise()
  " This means the entry shall be changed
  "amenu disable Changelog.New\ Version
  "amenu enable Changelog.Add\ Entry
  "amenu enable Changelog.Close\ Bug
  "amenu disable Changelog.Unfinalise
  "amenu enable Changelog.Finalise
  normal 1G
  call search("^ -- ")
  call setline(".", " -- ")
endfunction

function Finalise()
  " This means the entry should not be changed anymore
  "amenu enable Changelog.New\ Version
  "amenu disable Changelog.Add\ Entry
  "amenu disable Changelog.Close\ Bug
  "amenu enable Changelog.Unfinalise
  "amenu disable Changelog.Finalise
  normal 1G
  call search("^ -- ")
  call setline(".", " -- " . <SID>FullName() . " <" . <SID>Email() . ">  " . <SID>Date())
endfunction


amenu &Changelog.&New\ Version				:call NewVersion()<CR>
amenu Changelog.&Add\ Entry				:call AddEntry()<CR>
amenu Changelog.&Close\ Bug				:call CloseBug()<CR>
menu Changelog.-sep-					<nul>

amenu Changelog.Set\ &Distribution.&unstable		:call Distribution("unstable")<CR>
amenu Changelog.Set\ Distribution.&frozen		:call Distribution("frozen")<CR>
amenu Changelog.Set\ Distribution.&stable		:call Distribution("stable")<CR>
menu Changelog.Set\ Distribution.-sep-			<nul>
amenu Changelog.Set\ Distribution.frozen\ unstable	:call Distribution("frozen unstable")<CR>
amenu Changelog.Set\ Distribution.stable\ unstable	:call Distribution("stable unstable")<CR>
amenu Changelog.Set\ Distribution.stable\ frozen	:call Distribution("stable frozen")<CR>
amenu Changelog.Set\ Distribution.stable\ frozen\ unstable
    \ :call Distribution("stable frozen unstable")<CR>

amenu Changelog.Set\ &Urgency.&low			:call Urgency("low")<CR>
amenu Changelog.Set\ Urgency.&medium			:call Urgency("medium")<CR>
amenu Changelog.Set\ Urgency.&high			:call Urgency("high")<CR>

menu Changelog.-sep-					<nul>
amenu Changelog.U&nfinalise				:call Unfinalise()<CR>
amenu Changelog.&Finalise				:call Finalise()<CR>
amenu Changelog.Finalise+Save				<nul>

" Start off with a finalised entry
" TODO I had to disable disable because enable in the functions does not work.
"amenu disable Changelog.Add\ Entry
"amenu disable Changelog.Close\ Bug
"amenu disable Changelog.Finalise
amenu disable Changelog.Finalise+Save
