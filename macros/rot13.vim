" Macro to filter the whole buffer for rot13 coding
" Ra'ul Segura Acevedo 970626 (1997 Jun 26)
" first @ -> @A and a -> @a
" then n -> a, at this point there are no "n"'s in the buffer, so
" we can use "n" as temp_char to translate the other ones.
" finally we translate the @a -> n and @N -> @

map gr Go@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:se noic|%s/@/@A/g|%s/a/@a/g|%s/n/a/g|%s/b/n/g|%s/o/b/g|%s/n/o/g|%s/c/n/g|%s/p/c/g|%s/n/p/g|%s/d/n/g|%s/q/d/g|%s/n/q/g|%s/e/n/g|%s/r/e/g|%s/n/r/g|%s/f/n/g|%s/s/f/g|%s/n/s/g|%s/g/n/g|%s/t/g/g|%s/n/t/g|%s/h/n/g|%s/u/h/g|%s/n/u/g|%s/i/n/g|%s/v/i/g|%s/n/v/g|%s/j/n/g|%s/w/j/g|%s/n/w/g|%s/k/n/g|%s/x/k/g|%s/n/x/g|%s/l/n/g|%s/y/l/g|%s/n/y/g|%s/m/n/g|%s/z/m/g|%s/n/z/g|%s/A/n/g|%s/N/A/g|%s/n/N/g|%s/B/n/g|%s/O/B/g|%s/n/O/g|%s/C/n/g|%s/P/C/g|%s/n/P/g|%s/D/n/g|%s/Q/D/g|%s/n/Q/g|%s/E/n/g|%s/R/E/g|%s/n/R/g|%s/F/n/g|%s/S/F/g|%s/n/S/g|%s/G/n/g|%s/T/G/g|%s/n/T/g|%s/H/n/g|%s/U/H/g|%s/n/U/g|%s/I/n/g|%s/V/I/g|%s/n/V/g|%s/J/n/g|%s/W/J/g|%s/n/W/g|%s/K/n/g|%s/X/K/g|%s/n/X/g|%s/L/n/g|%s/Y/L/g|%s/n/Y/g|%s/M/n/g|%s/Z/M/g|%s/n/Z/g|%s/@a/n/g|%s/@N/@/g<CR>Gdd
