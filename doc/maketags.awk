BEGIN   {
	t=1;
	}

NR == 1 { print "<HTML>";
	print "<HEAD>";
	print "<A NAME=\"top\"></A>";
	print "<H1>" FILENAME " - html version </H1>";
	print "<TITLE>" FILENAME " - html version </TITLE>";
	print "</HEAD>";
	print "<BODY>";
	print "<PRE>";
	}

	{
	# from htmlchek, metachar.awk, protects special chars.
	gsub(/&/,"\\&amp;");
	gsub(/>/,"\\&gt;");
	gsub(/</,"\\&lt;");
   	gsub(/"/,"\\&quot;");
   	gsub(/%/,"\\&\#37;");
	nf=split($0,tag,"	");
	if ( nf >= 2 )
		{
		tagkey[t]=tag[1];tagref[t]=tag[2];tagnum[t]=NR;
		# print t " " tagkey[t] " " tagref[t];
		t++;
		}
	}
	{
	ntags = split($0,e,"	");
	n2=split(e[2],w,".");
	printf \
 ("<A HREF=\"%s.html\#%s\">\|%s\|</A>	%s\n",w[1],e[1],e[1],e[2]);
	next;
	}

END     {
	topback();
	print "</PRE>\n</BODY>\n\n\n</HTML>";
	# tags sorted with a simple but inefficient sorting algorithm
	# Knuth, Sorting and Searching, p. 81 (Straight insertion sort)
	# there are t-1 tags
	# tagkey is the key
	# tagref is the reference
	for ( j=2; j < t; j++ ) {
		i=j-1; k=tagkey[j]; r=tagref[j]; s=tagnum[j];
		while ( i > 0 ) {
		if ( k >= tagkey[i] ) {
			break;
			}
		tagref[i+1]=tagref[i];
		tagkey[i+1]=tagkey[i];
		tagnum[i+1]=tagnum[i];
		i--;
		if ( i == 0 ) { break; }
		}
	tagkey[i+1]=k;
	tagref[i+1]=r;
	tagnum[i+1]=s;
	#loop++;print "loop number: " loop ", i=" i ", j=" j
	}
	for ( j=1; j < t; j++ ) {
	print tagkey[j] "	" tagref[j] "	line " tagnum[j] >"tags.ref"
	}
	}

# as main we better keep help.txt
# other candidate, intro.txt
function topback () {
	printf("<A HREF=\"\#top\">top</A> - ");
	printf("<A HREF=\"help.html\">back to help</A>\n");
}
