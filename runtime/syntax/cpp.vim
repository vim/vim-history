" Vim syntax file
" Language:	C++
" Maintainer:	Ken Shan <ccshan@post.harvard.edu>
" Last change:	2000 Dec 17

" Remove any old syntax stuff hanging around
syn clear

" Read the C syntax to start with
runtime syntax/c.vim

" C++ extentions

syn keyword cppStatement	new delete this friend using
syn keyword cppAccess		public protected private
syn keyword cppType		inline virtual explicit export bool wchar_t
syn keyword cppExceptions	throw try catch
syn keyword cppOperator		operator typeid
syn match cppCast		"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*<"me=e-1
syn match cppCast		"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*$"
syn keyword cppStorageClass	mutable
syn keyword cppStructure	class typename template namespace
syn keyword cppNumber	NPOS
syn keyword cppBoolean	true false
if exists("c_gnu")
  " The minimum and maximum operators in GNU C++
  syn match	cppSpcOperator	"[<>]?"
  syn keyword	cppStructure	signature
  syn keyword	cppConstant	__GNUG__
endif


" The minimum and maximum operators in GNU C++

syn match cppMinMax "[<>]?"

" The default highlighting.
hi def link cppAccess	cppStatement
hi def link cppCast	cppStatement
hi def link cppExceptions	cppStatement
hi def link cppOperator	cppStatement
hi def link cppStatement	Statement
hi def link cppType	Type
hi def link cppStorageClass	StorageClass
hi def link cppStructure	Structure
hi def link cppNumber	Number
hi def link cppBoolean	Boolean

let b:current_syntax = "cpp"

" vim: ts=8
