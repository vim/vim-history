" An example for a .vimrc file

version 4.0		" avoid warning for wrong version

set bs=2		" allow backspacing over everything in insert mode
set ai			" always set autoindenting on
set tw=78		" always limit the width of text to 78
set backup		" keep a backup file
set viminfo='20,\"50	" read/write a .viminfo file, don't store more
			" than 50 lines of registers

" When starting to edit a file:
"   For *.c and *.h files set formatting of comments and set C-indenting on
"   For other files switch it off
"   Don't change the sequence, it's important that the line with * comes first.
autocmd BufRead * set formatoptions=tcql nocindent comments&
autocmd BufRead *.c,*.h set formatoptions=croql cindent comments=sr:/*,mb:*,el:*/,://

" Enable editing of gzipped files
"    read: set binary mode before reading the file
"          uncompress text in buffer after reading
"   write: compress file after writing
"  append: uncompress file, append, compress file
autocmd BufReadPre,FileReadPre      *.gz set bin
autocmd BufReadPost,FileReadPost    *.gz '[,']!gunzip
autocmd BufReadPost,FileReadPost    *.gz set nobin

autocmd BufWritePost,FileWritePost  *.gz !mv <afile> <afile>:r
autocmd BufWritePost,FileWritePost  *.gz !gzip <afile>:r

autocmd FileAppendPre		    *.gz !gunzip <afile>
autocmd FileAppendPre		    *.gz !mv <afile>:r <afile>
autocmd FileAppendPost		    *.gz !mv <afile> <afile>:r
autocmd FileAppendPost		    *.gz !gzip <afile>:r
