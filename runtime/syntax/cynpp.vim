" Vim syntax file
" Language:	Cyn++
" Maintainer:	Phil Derrick <phild@cynapps.com>
" Last change:	2001 Jan 15
" URL http://www.derrickp.freeserve.co.uk/vim/syntax/cynpp.vim
"
" Language Information
"
"               Cyn++ (cynpp) is a macro language around the Cynlib
"               class library. The aim of this is to allow an HDL-like 
"               syntax without the verbosity of defining and inheriting 
"               the C++ classes.
"
"               Cynlib is a library of C++ classes to allow hardware
"               modelling in C++. Combined with a simulation kernel, 
"               the compiled and linked executable forms a hardware 
"               simulation of the described design.
"
"               Further information can be found from www.cynapps.com


" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Read the Cynlib syntax to start with - this includes the C++ syntax
runtime! syntax/cynlib.vim
unlet b:current_syntax

" Cyn++ extensions

syn keyword     cynppMacro      Always EndAlways 
syn keyword     cynppMacro      Module EndModule
syn keyword     cynppMacro      Initial EndInitial
syn keyword     cynppMacro      Posedge Negedge Changed
syn keyword     cynppMacro      At 
syn keyword     cynppMacro      Thread EndThread
syn keyword     cynppMacro      Instantiate

" The default highlighting.
hi def link cynppMacro  Statement

let b:current_syntax = "cynpp"

