#!/bin/sh
#
# Spell a file & generate the syntax statements necessary
# to highlight in vim.
#

INFILE=$1
SYNTAX_FILE=/tmp/`basename $INFILE`.syntax

/bin/rm -f $SYNTAX_FILE

#
# If just cleaning up the temporary syntax file
# exit.
#
if [ $# -gt 1 -a "$2" = "-r" ]
then
	exit 0
fi

#
# local spellings
#
LOCAL_DICT=${LOCAL_DICT-$HOME/local/lib/local_dict}

if [ -f $LOCAL_DICT ]
then
	SPELL_ARGS="+$LOCAL_DICT"
fi

spell $SPELL_ARGS $INFILE |
nawk '

BEGIN {
	printf "syntax clear\n\n" ;
      }

      {
      	printf "syntax match SpellErrors \"\\<%s\\>\"\n", $0 ;
      }

END   {
	printf "highlight link SpellErrors ErrorMsg\n\n" ;
      }
' > $SYNTAX_FILE

echo $SYNTAX_FILE
