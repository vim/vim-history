DOCS = vim_ami.txt vim_arch.txt vim_diff.txt vim_digr.txt vim_dos.txt \
	vim_help.txt vim_idx.txt vim_mac.txt vim_w32.txt vim_ref.txt \
	vim_tips.txt vim_unix.txt vim_win.txt vim_gui.txt vim_40.txt \
	vim_kcc.txt vim_rlh.txt vim_menu.txt vim_os2.txt vim_mint.txt

all: vim_tags vim.man xxd.man ctags.man

vim_tags: doctags $(DOCS)
	./doctags $(DOCS) | sort >vim_tags
	uniq -d -2 vim_tags

doctags: doctags.c
	cc doctags.c -o doctags

vim.man: vim.1
	nroff -man vim.1 | sed -e s/.//g > vim.man

ctags.man: ctags.1
	nroff -man ctags.1 | sed -e s/.//g > ctags.man

xxd.man: xxd.1
	nroff -man xxd.1 | sed -e s/.//g > xxd.man

clean:
	-rm doctags
