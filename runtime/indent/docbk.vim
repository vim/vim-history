" Vim indent file
" Language:	    DocBook Documentation Format
" Maintainer:	    Nikolai Weibull <lone-star@home.se>
" URL:		    http://www.pcppopper.org/vim/indent/pcp/docbk/
" Latest Revision:  2003-09-01

" Same as XML indenting for now.
runtime! indent/xml.vim

setlocal indentexpr=XmlIndentGet(v:lnum,0)

" vim: set sts=4 sw=4:
