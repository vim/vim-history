" Vim syntax file
" Language:	Selects between various assemblers
" Maintainer:	C. Laurence Gonsalves <clgonsal@kami.com>
" Last change:	1998 July 22
" URL:		http://www.cryogen.com/clgonsal/vim/syntax/asmselect.vim

" make sure b:asmsyntax exists
if !exists("b:asmsyntax")
    let b:asmsyntax = ""
endif

if b:asmsyntax == ""
    " see if file contains any asmsyntax=foo overrides. If so, change
    " b:asmsyntax appropriately
    let b:head = " ".getline(1)." ".getline(2)." ".getline(3)." ".getline(4)." ".getline(5)." "
    if b:head =~ '\sasmsyntax=\S\+\s'
        let b:asmsyntax = substitute(b:head, '.*\sasmsyntax=\(\S\+\)\s.*','\1', "")
    endif
    unlet b:head
endif

" if b:asmsyntax still isn't set, default to asmsyntax or GNU
if b:asmsyntax == ""
    if exists("asmsyntax")
	let b:asmsyntax = asmsyntax
    else
	let b:asmsyntax = "asm"
    endif
endif

exe "so $VIM/syntax/" . b:asmsyntax . ".vim"
