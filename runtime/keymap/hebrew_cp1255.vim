" codepage 1255 on Windows is equal to ISO 8859-8
source <sfile>:p:h/hebrew_iso-8859-8.vim
" add in the vowel points, known as 'niqud', which are present in CP1255:
" Note: there are several 'standards' for placement of the vowel points, and I
" ignored all of them.  Since we can input English by simply pressing Ctrl-^
" and eliminating this map, I saw no reason to try to use exotic key
" locations.  If you don't like it, that's just too bad :-)  Write your own map!
" TODO: Add cantillation marks (only for utf?)
loadkeymap
A	<Char-192>	" sheva
S	<Char-193>	" hataf segol
D	<Char-194>	" hataf patah
F	<Char-195>	" hataf qamats
G	<Char-196>	" hiriq
H	<Char-197>	" tsere
J	<Char-198>	" segol
K	<Char-199>	" patah
L	<Char-200>	" qamats
Z	<Char-201>	" holam
X	<Char-203>	" qubuts
C	<Char-204>	" dagesh
V	<Char-205>	" meteg
B	<Char-206>	" maqaf
N	<Char-209>	" shin-dot
M	<Char-210>	" sin-dot
Q	<Char-211>	" sof-pasuq
