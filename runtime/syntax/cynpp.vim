" Vim syntax file
" Language:	Cyn++
" Maintainer:	Phil Derrick <phild@cynapps.com>
" Last change:	2000 Dec 17
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


" Remove any old syntax stuff hanging around
syn clear

" Read the Cynlib syntax to start with - this includes the C++ syntax
runtime syntax/cynlib.vim

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

