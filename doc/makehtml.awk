BEGIN   {
	# some initialization variables
	asciiart="no";
	wasset="no";
	lineset=0;
	t=1;u=0;
	while ( getline ti <"tags.ref" > 0 )
	{
	nf=split(ti,tag,"	");
	tagkey[t]=tag[1];tagref[t]=tag[2];
	# print t " " tagkey[t] " " tagref[t];
	t++;
	}
	t--;
	#print t " tags.";
	#exit;
	# there are "t" entries in the tags table
	}
#
# from htmlchek, metachar.awk, protects special chars.
#
/[><&]/ {gsub(/&/,"\\&amp;");gsub(/>/,"\\&gt;");gsub(/</,"\\&lt;")}
#ad hoc code
/^"\|\& / {gsub(/\|/,"\\&\#124;"); }
/ = b / {gsub(/ b /," \\&\#98; "); }
#
# one letter tag
#
/[ 	]\*.\*[ 	]/ {gsub(/\*/,"ZWWZ"); }
#
# isolated "*"
#
/[ 	]\*[ 	]/ {gsub(/ \* /," \\&\#42; ");
                    gsub(/ \*	/," \\&\#42;	");
                    gsub(/	\* /,"	\\&\#42; ");
                    gsub(/	\*	/,"	\\&\#42;	"); }
#
# tag start
#
/[ 	]\*[^ 	]/	{gsub(/ \*/," ZWWZ");gsub(/	\*/,"	ZWWZ");}
/^\*[^ 	]/ 	 {gsub(/^\*/,"ZWWZ");}
#
# tag end
#
/[^ 	]\*$/ 	 {gsub(/\*$/,"ZWWZ");}
/[^ \/	]\*[ 	]/  {gsub(/\*/,"ZWWZ");}
#
# isolated "|"
#
/[ 	]\|[ 	]/ {gsub(/ \| /," \\&\#124; ");
                    gsub(/ \|	/," \\&\#124;	");
                    gsub(/	\| /,"	\\&\#124; ");
                    gsub(/	\|	/,"	\\&\#124;	"); }
/'\|'/ { gsub(/'\|'/,"'\\&\#124;'"); }
/\^V\|/ {gsub(/\^V\|/,"\^V\\&\#124;");}
/ \\\|	/ {gsub(/\|/,"\\&\#124;");}
#
# one letter pipes and "||" false pipe (digraphs)
#
/[ 	]\|.\|[ 	]/ && asciiart == "no" {gsub(/\|/,"YXXY"); }
/^\|.\|[ 	]/ {gsub(/\|/,"YXXY"); }
/\|\|/ {gsub(/\|\|/,"\\&\#124;\\&\#124;"); }
/^shellpipe/ {gsub(/\|/,"\\&\#124;"); }
#
# pipe start
#
/[ 	]\|[^ 	]/ && asciiart == "no"	{gsub(/ \|/," YXXY");
			gsub(/	\|/,"	YXXY");}
/^\|[^ 	]/ 	 {gsub(/^\|/,"YXXY");}
#
# pipe end
#
/[^ 	]\|$/ && asciiart == "no" {gsub(/\|$/,"YXXY");}
/[^ 	]\|[ ,.)\];	]/ && asciiart == "no"  {gsub(/\|/,"YXXY");}
#
# various
#
/'"/ 	{gsub(/'"/,"\\&\#39;\\&\#34;'");}
/"/	{gsub(/"/,"\\&quot;");}
/%/	{gsub(/%/,"\\&\#37;");}

NR == 1 { print "<HTML>";
	print "<HEAD>";
	print "<A NAME=\"top\"></A>";
	print "<H1>" FILENAME " - html version </H1>";
	print "<TITLE>" FILENAME " - html version </TITLE>";
	print "</HEAD>";
	print "<BODY>";
	print "<PRE>";
	nf=split(FILENAME,f,".")
	filename=f[1]".html";
	#print filename;
	#exit;
	}

# set to a low value to test for few lines of text
# NR == 99999 { exit; }

# ignore underlines and tags
substr($0,1,3) == "===" { next; }
substr($0,1,3) == "---" { next; }
substr($0,1,5) == " vim:" { next; }
substr($0,1,4) == "vim:" { next; }

	{
	nstar = split($0,s,"ZWWZ");
	for ( i=2 ; i <= nstar ; i=i+2 ) {
		nbla=split(s[i],blata," ");
		ntabs=split(s[i],tb,"	");
		if ( nbla > 1 || ntabs > 1 ) {
			gsub("ZWWZ","\*");
			nstar = split($0,s,"ZWWZ");
		}
	}
	npipe = split($0,p,"YXXY");
	for ( i=2 ; i <= npipe ; i=i+2 ) {
		nbla=split(p[i],blata," ");
		ntabs=split(p[i],tb,"	");
		if ( nbla > 1 || ntabs > 1 ) {
			gsub("YXXY","\|");
			ntabs = split($0,p,"YXXY");
		}
	}

	}


FILENAME == "gui.txt" && asciiart == "no"  \
          && $0 ~ /\+----/ && $0 ~ /----\+/ {
	asciiart= "yes";
	}

asciiart == "yes" { npipe = 1; }
#	{ print NR " <=> " asciiart; }

#
# line contains  "*"
#
nstar > 2 && npipe < 3 {
	printf("\n\n");
	for ( i=1; i <= nstar ; i=i+2 ) {
		this=s[i];
		put_this();
		ii=i+1;
		nbla = split(s[ii],blata," ");
		if ( ii <= nstar ) {
			if ( nbla == 1 && substr(s[ii],length(s[ii]),1) != " " ) {
			printf("<A NAME=\"\%s\"></A>",s[ii]);
				printf("*<B>%s</B>*",s[ii]);
			} else {
			printf("*%s*",s[ii]);
			}
		}
	}
        printf("\n");
	next;
        }
#
# line contains "|"
#
npipe > 2 && nstar < 3 {
	if  ( npipe%2 == 0 ) {
		for ( i=1; i < npipe ; i++ ) {
			gsub("ZWWZ","*",p[i]);
			printf("%s\|",p[i]);
		}
		printf("%s\n",p[npipe]);
		next;
		}
	for ( i=1; i <= npipe ; i++ )
		{
		if ( i % 2 == 1 ) {
			gsub("ZWWZ","*",p[i]);
			this=p[i];
			put_this();
			}
			else {
			nfn=split(p[i],f,".");
			if ( nfn == 1 || f[2] == "" || f[1] == "" ) {
				find_tag();
				}
				else {
		printf "<A HREF=\"" f[1] ".html\">\|" p[i] "\|</A>";
				}
			}
		}
		printf("\n");
		next;
	}
#
# line contains both "|" and "*"
#
npipe > 2 && nstar > 2 {
	printf("\n\n");
	for ( j=1; j <= nstar ; j=j+2 ) {
		npipe = split(s[j],p,"YXXY");
		if ( npipe > 1 ) {
		this=p[1];
		put_this();
		i=2;find_tag();
		this=p[3];
		put_this();
		} else {
		this=s[j];
		put_this();
		}
		jj=j+1;
		nbla = split(s[jj],blata," ");
		if ( jj <= nstar && nbla == 1 && s[jj] != "" ) {
		printf("<A NAME=\"\%s\"></A>",s[jj]);
			printf("*<B>%s</B>*",s[jj]);
		} else {
			if ( s[jj] != "" ) {
			printf("*%s*",s[jj]);
			}
		}
	}
        printf("\n");
	next;
	}
#
# line contains e-mail address (normalized:  <john.doe@some.place.edu> )
#
$0 ~ /@/ && ( $0 ~ /\.com/ || $0 ~ /\.de/ || $0 ~ /\.nl/ || $0 ~ /\.edu/ ) \
	{
	nemail=split($0,em,"\\&");
	for ( i=1; i <= nemail; i++ ) {
	if ( substr(em[i],1,2) == "lt" ) {
	mailaddr=substr(em[i],4);
	printf("<A HREF=\"mailto:%s\">\&lt;%s\&gt;</A>",mailaddr,mailaddr);
	} else {
	if ( substr(em[i],1,2) == "gt" ) {
	shortstr=substr(em[i],4);
	printf("%s",shortstr);
	} else {
	printf("%s",em[i]);
	}
	}
	}
	#print "*** " NR " " FILENAME " - possible mail ref";
	printf("\n");
	next;
	}
#
# line contains http reference (normalized: <URL:http://xxx.yyy.zzz>
#                                           <URL:ftp://xxx.yyy.zzz> )
#
$0 ~ /URL:/ || $0 ~ /ftp:/ || $0 ~ /http:/ \
	{
	nemail=split($0,em,"\\&");
	for ( i=1; i <= nemail; i++ ) {
	if ( substr(em[i],1,2) == "lt" ) {
	httpref=substr(em[i],8);
	printf("<A HREF=\"%s\">\&lt;%s\&gt;</A>",httpref,httpref);
	} else {
	if ( substr(em[i],1,2) == "gt" ) {
	shortstr=substr(em[i],4);
	printf("%s",shortstr);
	} else {
	printf("%s",em[i]);
	}
	}
	}
	#print "*** " NR " " FILENAME " - possible http ref";
	printf("\n");
	next;
	}
#
# some lines contains just one "almost regular" "*"...
#
nstar == 2  {
	this=s[1];
	put_this();
	printf("*");
	this=s[2];
	put_this();
	printf("\n");
	next;
	}
#
# regular line
#
	{ ntabs = split($0,tb,"	");
	for ( i=1; i < ntabs ; i++) {
		this=tb[i];
		put_this();
		printf("	");
		}
	this=tb[ntabs];
	put_this();
	printf("\n");
	}


asciiart == "yes"  && $0 ~ /\+-\+--/  \
        && $0 ~ "scrollbar" { asciiart = "no"; }

END {
	topback();
	print "</PRE>\n</BODY>\n\n\n</HTML>"; }

# as main we better keep help.txt
# other candidate, intro.txt
# let us leave tags as a list
function topback () {
	if ( FILENAME != "tags" ) {
	if ( FILENAME != "help.txt" ) {
	printf("<A HREF=\"\#top\">top</A> - ");
	printf("<A HREF=\"help.html\">back to help</A>\n");
	} else {
	printf("<A HREF=\"\#top\">top</A>\n");
	}
	}
}

function find_tag() {
	if ( p[i] == "" ) { return; }
	vvvodelta=9999;
	vvodelta=9999;
	vodelta=9999;
	odelta=9999;
	u=int(t/2);done="no";delta=int(u/2+1);
	while ( u <= t && u >= 0 && done=="no" && delta > 0  ) {
	#if ( p[i] == "CTRL-BREAK" ) {
	#print u ", delta=" delta ",odelta="  \
 	#odelta "@" tagkey[u] "@" tagref[u] "@";
	#}
	what=tagkey[u];
	if ( p[i] == what ) {
	        which=tagref[u];
		put_href();
		done="yes";
		}
	if ( p[i]  > what ) 	{ u=int(u+delta);
				if ( u > t ) { u=t; }
				}
		else 	    	{ u=int(u-delta);
				if ( u < 1 ) { u=1; }
				}
	vvvodelta=vvodelta;
	vvodelta=vodelta;
	vodelta=odelta;
	odelta=delta;
	delta=int((delta+1)/2);
	if ( delta <= 0 ) { delta = 1; }
	if ( delta == vvvodelta ) { delta = 0; }
	}
	# if not found, then we have a problem
	if ( done == "no" ) {
        print "============================================" \
		 >>"errors.log"
	print FILENAME ", line " NR ", pointer: >>" p[i] "<<" >>"errors.log"
	print $0 >>"errors.log"
	which="intro.html";
	put_href();
	}
}

function see_tag() {
# ad-hoc code:
if ( atag == "\"--" || atag == "--\"" ) { return; }
if_already();
if ( already == "yes" ) {
	printf("%s",aword);
	return;
	}
allow_one_char="no";
find_tag2();
if ( done == "yes" ) { return; }
rightchar=substr(atag,length(atag),1);
if (    rightchar == "." \
     || rightchar == "," \
     || rightchar == ":" \
     || rightchar == ";" \
     || rightchar == "!" \
     || rightchar == "?" \
     || rightchar == ")" ) {
	atag=substr(atag,1,length(atag)-1);
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	find_tag2();
	if ( done == "yes" ) { printf("%s",rightchar);return; }
	leftchar=substr(atag,1,1);
	lastbut1=substr(atag,length(atag),1);
	if (    leftchar == "'" && lastbut1 == "'"  ) {
		allow_one_char="yes";
		atag=substr(atag,2,length(atag)-2);
		if_already();
		if ( already == "yes" ) {
			printf("%s",aword);
			return;
			}
		printf("%s",leftchar);
		aword=substr(atag,1,length(atag))""lastbut1""rightchar;
		find_tag2();
		if ( done == "yes" ) { printf("%s%s",lastbut1,rightchar);return; }
		}
	}
atag=aword;
leftchar=substr(atag,1,1);
if (    leftchar == "'" && rightchar == "'"  ) {
	allow_one_char="yes";
	atag=substr(atag,2,length(atag)-2);
	if  ( atag == "<" ) { printf(" |%s|%s| ",atag,p[2]); }
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	printf("%s",leftchar);
	find_tag2();
	if ( done == "yes" ) { printf("%s",rightchar);return; }
	printf("%s%s",atag,rightchar);
	return;
	}
last2=substr(atag,length(atag)-1,2);
first2=substr(atag,1,2);
if (    first2 == "('" && last2 == "')"  ) {
	allow_one_char="yes";
	atag=substr(atag,3,length(atag)-4);
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	printf("%s",first2);
	find_tag2();
	if ( done == "yes" ) { printf("%s",last2);return; }
	printf("%s%s",atag,last2);
	return;
	}
if ( last2 == ".)" ) {
	atag=substr(atag,1,length(atag)-2);
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	find_tag2();
	if ( done == "yes" ) { printf("%s",last2);return; }
	printf("%s%s",atag,last2);
	return;
	}
if ( last2 == ")." ) {
	atag=substr(atag,1,length(atag)-2);
	find_tag2();
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	if ( done == "yes" ) { printf("%s",last2);return; }
	printf("%s%s",atag,last2);
	return;
	}
first6=substr(atag,1,6);
last6=substr(atag,length(atag)-5,6);
if ( last6 == atag ) {
	printf("%s",aword);
	return;
	}
last6of7=substr(atag,length(atag)-6,6);
if ( first6 == "&quot;" && last6of7 == "&quot;" && length(atag) > 12 ) {
	allow_one_char="yes";
	atag=substr(atag,7,length(atag)-13);
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	printf("%s",first6);
	find_tag2();
	if ( done == "yes" ) { printf("&quot;%s",rightchar); return; }
	printf("%s&quot;%s",atag,rightchar);
	return;
	}
if ( first6 == "&quot;" && last6 != "&quot;" ) {
	allow_one_char="yes";
	atag=substr(atag,7,length(atag)-6);
	if ( atag == "[" ) { printf("&quot;%s",atag); return; }
	if ( atag == "." ) { printf("&quot;%s",atag); return; }
	if ( atag == ":" ) { printf("&quot;%s",atag); return; }
	if ( atag == "a" ) { printf("&quot;%s",atag); return; }
	if ( atag == "A" ) { printf("&quot;%s",atag); return; }
	if ( atag == "g" ) { printf("&quot;%s",atag); return; }
	if_already();
	if ( already == "yes" ) {
		printf("&quot;%s",atag);
		return;
		}
	printf("%s",first6);
	find_tag2();
	if ( done == "yes" ) { return; }
	printf("%s",atag);
	return;
	}
if ( last6 == "&quot;" && first6 == "&quot;" ) {
	allow_one_char="yes";
	atag=substr(atag,7,length(atag)-12);
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	printf("%s",first6);
	find_tag2();
	if ( done == "yes" ) { printf("%s",last6);return; }
	printf("%s%s",atag,last6);
	return;
	}
last6of7=substr(atag,length(atag)-6,6);
if ( last6of7 == "&quot;" && first6 == "&quot;" ) {
	allow_one_char="yes";
	atag=substr(atag,7,length(atag)-13);
	#printf("\natag=%s,aword=%s\n",atag,aword);
	if_already();
	if ( already == "yes" ) {
		printf("%s",aword);
		return;
		}
	printf("%s",first6);
	find_tag2();
	if ( done == "yes" ) { printf("%s%s",last6of7,rightchar);return; }
	printf("%s%s%s",atag,last6of7,rightchar);
	return;
	}
printf("%s",aword);
}

function find_tag2() {
	vvvodelta=9999;
	vvodelta=9999;
	vodelta=9999;
	odelta=9999;
	done="no";
	nbla = split(atag,blata," ");
	if ( nbla > 1 ) { return; }
	ntags = split(atag,blata,"	");
	if ( ntags > 1 ) { return; }
	# no blanks present in a tag...
	# printf("\natag=%s\n",atag);
	# the one letter list was generated by gawk...
	if 	( ( allow_one_char == "no" ) && \
		(  atag == "!" \
		|| atag == "#" \
		|| atag == "$" \
		|| atag == "%" \
		|| atag == "\&" \
		|| atag == "'" \
		|| atag == "(" \
		|| atag == ")" \
		|| atag == "+" \
		|| atag == "," \
		|| atag == "-" \
		|| atag == "." \
		|| atag == "/" \
		|| atag == "0" \
		|| atag == ":" \
		|| atag == ";" \
		|| atag == "=" \
		|| atag == "?" \
		|| atag == "@" \
		|| atag == "A" \
		|| atag == "C" \
		|| atag == "I" \
		|| atag == "N" \
		|| atag == "X" \
		|| atag == "[" \
		|| atag == "]" \
		|| atag == "^" \
		|| atag == "_" \
		|| atag == "`" \
		|| atag == "a" \
		|| atag == "t" \
		|| atag == "{" \
		|| atag == "}" \
		|| atag == "~" \
		)) { return; }
	if 	( \
		   atag == "and" \
		|| atag == "backspace" \
		|| atag == "beep" \
		|| atag == "da" \
		|| atag == "end" \
		|| atag == "help" \
		|| atag == "news" \
		|| atag == "index" \
		|| atag == "insert" \
		|| atag == "into" \
		|| atag == "starting" \
		|| atag == "toggle" \
		|| atag == "reference" \
		|| atag == "various" \
		|| atag == "version" \
		) { return; }
	if ( wasset == "yes" && lineset == NR ) {
	wasset="no";
	see_opt();
	if ( doneopt == "yes" ) {return;}
	}
	if ( wasset == "yes" && lineset != NR ) {
	wasset="no";
	}
	if ( atag == ":set" ) {
	wasset="yes";
	lineset=NR;
	}
	u=int(t/2);delta=int(u/2+1);
	while ( u <= t && u >= 0 && done=="no" && delta > 0  ) {
	#if ( atag == "CTRL-BREAK" ) {
	#print u ", delta=" delta ",odelta="  \
 	#odelta "@" tagkey[u] "@" tagref[u] "@";
	#}
	what=tagkey[u];
	if ( atag == what ) {
	        which=tagref[u];
		put_href2();
		done="yes";
		return;
		}
	if ( atag  > what ) 	{ u=int(u+delta);
				if ( u > t ) { u=t; }
				}
		else 	    	{ u=int(u-delta);
				if ( u < 1 ) { u=1; }
				}
	vvvodelta=vvodelta;
	vvodelta=vodelta;
	vodelta=odelta;
	odelta=delta;
	delta=int((delta+1)/2);
	if ( delta <= 0 ) { delta = 1; }
	if ( delta == vvvodelta ) { delta = 0; }
	}
}

function find_tag3() {
	vvvodelta=9999;
	vvodelta=9999;
	vodelta=9999;
	odelta=9999;
	done="no";
	nbla = split(btag,blata," ");
	if ( nbla > 1 ) { return; }
	ntags = split(btag,blata,"	");
	if ( ntags > 1 ) { return; }
	# no blanks present in a tag...
	# printf("\nbtag=%s\n",btag);
	if 	( ( allow_one_char == "no" ) && \
		(  btag == "!" \
		|| btag == "#" \
		|| btag == "$" \
		|| btag == "%" \
		|| btag == "\&" \
		|| btag == "'" \
		|| btag == "(" \
		|| btag == ")" \
		|| btag == "+" \
		|| btag == "," \
		|| btag == "-" \
		|| btag == "." \
		|| btag == "/" \
		|| btag == "0" \
		|| btag == ":" \
		|| btag == ";" \
		|| btag == "=" \
		|| btag == "?" \
		|| btag == "@" \
		|| btag == "A" \
		|| btag == "C" \
		|| btag == "I" \
		|| btag == "N" \
		|| btag == "X" \
		|| btag == "[" \
		|| btag == "]" \
		|| btag == "^" \
		|| btag == "_" \
		|| btag == "`" \
		|| btag == "a" \
		|| btag == "t" \
		|| btag == "{" \
		|| btag == "}" \
		|| btag == "~" \
		)) { return; }
	if 	( \
		   btag == "and" \
		|| btag == "backspace" \
		|| btag == "beep" \
		|| btag == "da" \
		|| btag == "end" \
		|| btag == "help" \
		|| btag == "news" \
		|| btag == "index" \
		|| btag == "insert" \
		|| btag == "into" \
		|| btag == "starting" \
		|| btag == "toggle" \
		|| btag == "reference" \
		|| btag == "various" \
		|| btag == "version" \
		) { return; }
	u=int(t/2);delta=int(u/2+1);
	while ( u <= t && u >= 0 && done=="no" && delta > 0  ) {
	#if ( btag == "CTRL-BREAK" ) {
	#print u ", delta=" delta ",odelta="  \
 	#odelta "@" tagkey[u] "@" tagref[u] "@";
	#}
	what=tagkey[u];
	if ( btag == what ) {
	        which=tagref[u];
		put_href3();
		done="yes";
		return;
		}
	if ( btag  > what ) 	{ u=int(u+delta);
				if ( u > t ) { u=t; }
				}
		else 	    	{ u=int(u-delta);
				if ( u < 1 ) { u=1; }
				}
	vvvodelta=vvodelta;
	vvodelta=vodelta;
	vodelta=odelta;
	odelta=delta;
	delta=int((delta+1)/2);
	if ( delta <= 0 ) { delta = 1; }
	if ( delta == vvvodelta ) { delta = 0; }
	}
}

function put_href() {
	if ( p[i] == "" ) { return; }
	if ( which == FILENAME ) {
		printf("<A HREF=\"\#%s\">\|%s\|</A>",p[i],p[i]);
		}
		else {
		nz=split(which,zz,".");
		if ( zz[2] == "txt" || zz[1] == "tags" ) {
		printf("<A HREF=\"%s.html\#%s\">\|%s\|</A>",zz[1],p[i],p[i]);
		}
		else {
		printf("<A HREF=\"intro.html\#%s\">\|%s\|</A>",p[i],p[i]);
		}
	}
}

function put_href2() {
	if ( atag == "" ) { return; }
	if ( which == FILENAME ) {
		printf("<A HREF=\"\#%s\">%s</A>",atag,atag);
		}
		else {
		nz=split(which,zz,".");
		if ( zz[2] == "txt" || zz[1] == "tags" ) {
		printf("<A HREF=\"%s.html\#%s\">%s</A>",zz[1],atag,atag);
		}
		else {
		printf("<A HREF=\"intro.html\#%s\">%s</A>",atag,atag);
		}
	}
}

function put_href3() {
	if ( btag == "" ) { return; }
	if ( which == FILENAME ) {
		printf("<A HREF=\"\#%s\">%s</A>",btag,btag2);
		}
		else {
		nz=split(which,zz,".");
		if ( zz[2] == "txt" || zz[1] == "tags" ) {
		printf("<A HREF=\"%s.html\#%s\">%s</A>",zz[1],btag,btag2);
		}
		else {
		printf("<A HREF=\"intro.html\#%s\">%s</A>",btag,btag2);
		}
	}
}

function put_this() {
	ntab=split(this,ta,"	");
	for ( nta=1 ; nta <= ntab ; nta++ ) {
		ata=ta[nta];
		lata=length(ata);
		aword="";
		for ( iata=1 ; iata <=lata ; iata++ ) {
			achar=substr(ata,iata,1);
			if ( achar != " " ) { aword=aword""achar; }
			else {
				if ( aword != "" ) { atag=aword;
					see_tag();
					aword="";
					printf(" "); }
				else	{
					printf(" ");
					}
			}
		}
		if ( aword != "" ) { atag=aword;
					see_tag();
					}
		if ( nta != ntab ) { printf("	"); }
	}
}

function if_already() {
	already="no";
	if  ( npipe < 2 ) { return; }
	if  ( atag == ":au" && p[2] == ":autocmd" ) { already="yes";return; }
	for ( npp=2 ; npp <= npipe ; npp=npp+2 ) {
		if 	(  (  (index(p[npp],atag)) != 0 \
			      && length(p[npp]) > length(atag) \
			      && length(atag) >= 1  \
			    ) \
			    || (p[npp] == atag) \
			) {
		# printf("p=|%s|,tag=|%s| ",p[npp],atag);
		already="yes"; return; }
	}
}

function see_opt() {
	doneopt="no";
	stag=atag;
	nfields = split(atag,tae,"=");
	if ( nfields > 1 )  {
		btag="'"tae[1]"'";
		btag2=tae[1];
	    	find_tag3();
		if (done == "yes") {
			for ( ntae=2 ; ntae <= nfields ; ntae++ ) {
				printf("=%s",tae[ntae]);
			}
			atag=stag;
			return;
		}
		btag=tae[1];
		btag2=tae[1];
	    	find_tag3();
		if (done == "yes") {
			for ( ntae=2 ; ntae <= nfields ; ntae++ ) {
				printf("=%s",tae[ntae]);
			}
			atag=stag;
			return;
		}
	}
	nfields = split(atag,tae,"&quot;");
	if ( nfields > 1 )  {
		btag="'"tae[1]"'";
		btag2=tae[1];
	    	find_tag3();
		if (done == "yes") {
			printf("&quot;");
			atag=stag;
			return;
		}
		btag=tae[1];
		btag2=tae[1];
	    	find_tag3();
		if (done == "yes") {
			printf("&quot;");
			atag=stag;
			return;
		}
	}
	btag="'"tae[1]"'";
	btag2=tae[1];
	find_tag3();
	if (done == "yes") {
		atag=stag;
		return;
	}
	btag=tae[1];
	btag2=tae[1];
	find_tag3();
	if (done == "yes") {
		atag=stag;
		return;
	}
	atag=stag;
}
