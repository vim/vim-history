#!/usr/local/bin/perl

# converts vim documentation to simple html
# Sirtaj Singh Kang (taj@kde.org)

# Wed Oct  8 01:15:48 EST 1997

$date = `date`;
chop $date;

%url = ();

sub readTagFile
{
	my($tagfile) = @_;
	local( $tag, $file, $name );

	open(TAGS,"$tagfile") || die "can't read tags\n";

	while( <TAGS> ) {
		s/</&lt;/g;
		s/>/&gt;/g;

		/^(.*)\t(.*)\t/;

		$tag = $1;
		($file= $2) =~ s/.txt$/.html/g;

		$url{ $tag } = "<A HREF=\"$file#$tag\">$tag</A>";

		#print "($tag, $file, $tag)\n";
	}
	close( TAGS );
}

sub vim2html
{
	my( $infile ) = @_;
	local( $outfile );

	open(IN, "$infile" ) || die "Couldn't read from $infile.\n";

	($outfile = $infile) =~ s%.*/%%g;
	$outfile =~ s/\.txt$//g;

	open( OUT, ">$outfile.html" )
			|| die "Couldn't write to $outfile.html.\n";

	print OUT<<EOF;
<HTML>
<HEAD><TITLE>$outfile</TITLE></HEAD>
<BODY BGCOLOR="#ffffff">
<H1>Vim Documentation: $outfile</H1>
<HR>
<PRE>
EOF

	while( <IN> ) {
		s/</&lt;/g;
		s/>/&gt;/g;

		s/\*([^*]*)\*/\*<A NAME="$1"><\/A><B>$1<\/B>\*/g;
		s/\|([^|]*)\|/\|$url{$1}\|/g;

		print OUT $_;
	}
	print OUT<<EOF;
</PRE>
<p><i>Generated by vim2html on $date</i></p>
</BODY>
</HTML>
EOF

}

sub usage
{
die<<EOF;
vim2html.pl: converts vim documentation to HTML.
usage:

	vim2html.pl <tag file> <text files>
EOF
}

# main

if ( $#ARGV < 2 ) {
	usage();
}

print "Processing tags...\n";
readTagFile( $ARGV[ 0 ] );

foreach $file ( 1..$#ARGV ) {
	print "Processing ".$ARGV[ $file ]."...\n";
	vim2html( $ARGV[ $file ] );
}
