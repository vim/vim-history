" Short script to check if a *.pm file is an XPM or a Perl file
if getline(1) =~ "XPM"
  so <sfile>:p:h/xpm.vim
else
  so <sfile>:p:h/perl.vim
endif
