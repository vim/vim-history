let encoding = &enc
if encoding = ''
	let encoding = 'cp1255'
endif

if encoding == 'utf-8'
	source <sfile>:p:h/hebrew_utf-8.vim
elseif encoding == 'cp1255'
	source <sfile>:p:h/hebrew_cp1255.vim
else
	source <sfile>:p:h/hebrew_iso-8859-8.vim
endif
