:" use this script to create the file "bugreport.txt", which contains
:" information about the environment of a possible bug in Vim.
:" e.g., ":so $VIM/bugreport.vim" inside Vim, or "vim -s $VIM/bugreport.vim".
:let more_save = &more
:set nomore
:if has("unix")
:  !echo "uname -a" >bugreport.txt
:  !uname -a >>bugreport.txt
:endif
:redir >>bugreport.txt
:version
:set all
:set termcap
:au
:echo "--- Normal mode mappings ---"
:map
:echo "--- Insert mode mappings ---"
:map!
:echo "--- Abbreviations ---"
:ab
:echo "--- Variables ---"
:let
:redir END
:let &more = more_save
