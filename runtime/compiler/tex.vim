" Vim compiler file
" Compiler:     TeX
" Maintainer:   Artem Chuprina <ran@ran.pp.ru>
" Last Change:  2001 Sep 13

if exists("current_compiler")
	finish
endif
" If buffer-local variable 'tex_flavor' exists, it defines TeX flavor,
" otherwize the same for global variable with same name, else it will be LaTeX
if exists("b:tex_flavor")
	let current_compiler = b:tex_flavor
elseif exists("g:tex_flavor")
	let current_compiler = g:tex_flavor
else
	let current_compiler = "latex"
endif

" Values for makeprg and errorformat are taken from vim help, see
" :help errorformat-LaTeX
let &makeprg=current_compiler.' \\nonstopmode \\input\{$*\}'
setlocal errorformat=%E!\ LaTeX\ %trror:\ %m,
	\%E!\ %m,
	\%+WLaTeX\ %.%#Warning:\ %.%#line\ %l%.%#,
	\%+W%.%#\ at\ lines\ %l--%*\\d,
	\%WLaTeX\ %.%#Warning:\ %m,
	\%Cl.%l\ %m,
	\%+C\ \ %m.,
	\%+C%.%#-%.%#,
	\%+C%.%#[]%.%#,
	\%+C[]%.%#,
	\%+C%.%#%[{}\\]%.%#,
	\%+C<%.%#>%.%#,
	\%C\ \ %m,
	\%-GSee\ the\ LaTeX%m,
	\%-GType\ \ H\ <return>%m,
	\%-G\ ...%.%#,
	\%-G%.%#\ (C)\ %.%#,
	\%-G(see\ the\ transcript%.%#),
	\%-G\\s%#,
	\%+O(%f)%r,
	\%+P(%f%r,
	\%+P\ %\\=(%f%r,
	\%+P%*[^()](%f%r,
	\%+P[%\\d%[^()]%#(%f%r,
	\%+Q)%r,
	\%+Q%*[^()])%r,
	\%+Q[%\\d%*[^()])%r
