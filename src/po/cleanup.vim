" Vim script to cleanup a .po file: comment-out fuzzy and empty messages.
" Requires Vim 6.0 (because of multi-line search patterns).
g/^#, fuzzy\nmsgid ""\@!/.+1,/^$/-1s/^/#\~ /
g/^msgstr ""\(\n"\)\@!/?^msgid?,.s/^/#\~ /
