" Vim syntax file
" Language:	C++
" Maintainer:	Ken Shan <ken@digitas.harvard.edu>
" Last change:	1998 Jan 9

" Remove any old syntax stuff hanging around
syn clear

" Read the C syntax to start with
source <sfile>:p:h/c.vim

" C++ extentions

syn keyword cppStatement	new delete this
syn keyword cppStatement	template operator friend typename
syn keyword cppScopeDecl	public protected private
syn keyword cppType		inline virtual bool
syn keyword cppExceptions	throw try catch
syn match cppCast		"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*<"me=e-1
syn match cppCast		"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*$"
syn keyword cppCast		explicit
syn keyword cppStorageClass	mutable
syn keyword cppNumber	NPOS
syn keyword cppBoolean	true false

syn match cppClassPreDecl "^\s*\(class\|struct\)\s\+[a-zA-Z_][a-zA-Z0-9_:]*\s*;"
syn match cppClassDecl	  "^\s*\(class\|struct\)\s\+[a-zA-Z_][a-zA-Z0-9_:]*"

" Functions ...
syn match cppFunction  "^[a-zA-Z_][a-zA-Z0-9_<>:]*\s*("me=e-1
syn match cppMethod  "^[a-zA-Z_][a-zA-Z0-9_<>:]*::\~\=[a-zA-Z0-9_<>:]\+\s*("me=e-1

syn match cppMethodWrapped contained  "[a-zA-Z_][a-zA-Z0-9_<>:]*::[a-zA-Z0-9_<>:]\+"
syn match cppMethodWrap  "^[a-zA-Z_][a-zA-Z0-9_<>:]*\s\+[a-zA-Z_][a-zA-Z0-9_<>:]*::[a-zA-Z0-9_<>:]\+\s*("me=e-1 contains=cppMethodWrapped

if !exists("did_cpp_syntax_inits")
  let did_cpp_syntax_inits = 1
  hi link cppClassDecl	Typedef
  hi link cppClassPreDecl	cppStatement
  hi link cppScopeDecl	cppStatement
  hi link cppCast	cppStatement
  hi link cppExceptions	cppStatement
  hi link cppMethod	cppFunction
  hi link cppStatement	Statement
  hi link cppType	Type
  hi link cppMethod	Function
  hi link cppFunction	Function
  hi link cppStorageClass	StorageClass
  hi link cppNumber	Number
  hi link cppBoolean	Boolean
endif

let b:current_syntax = "cpp"

" vim: ts=8
