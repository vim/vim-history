#!/usr/local/bin/perl -w

# pltags - create a tags file for Perl code, for use by vi(m)
# 
# Written by Michael Schaap <mscha@ctp.com>
#
# This script will not work with Perl 4 or below!

use strict;

my $file = "";
my $fn = "";
my @funcs = ();
my %tags = ();
my @conflicts = ();

print "\npltags - by Michael Schaap <mscha\@ctp.com>\n\n";

unless (@ARGV)
{
    print "Usage: $0 filename ...\n\n";
    print "Example: $0 *.pl *.pm ../shared/*.pm\n\n";
    exit;
}

foreach $file (@ARGV)
{
    next unless ((-f $file) && (-r $file) && ($file !~ /tags$/)
                 && ($file !~ /~$/));

    print "Tagging file $file...\n";

    open (IN, $file);
    @funcs = grep(/^sub /, <IN>);
    close (IN);
    foreach $fn (@funcs)
    {
        chomp $fn;
        $fn =~ s/^sub (\w+)(.*)/$1/;

        if (defined($tags{$fn}))
        {
            if ($tags{$fn} eq $file)
            {
                push(@conflicts, "'$fn' appears more than once in '$file'\n");
            }
            else
            {
                push(@conflicts, "skipped '$fn' in '$file' - already " .
                                 "appears in '$tags{$fn}'\n");
            }

            print "    skipping $fn\n";
        }
        else
        {
            $tags{$fn} = $file;

            print "    $fn\n";
        }
    }
}

if (%tags)
{
    print "\nWriting tags file.\n";

    open (OUT, ">tags");

    foreach $fn (sort keys %tags)
    {
        print OUT "$fn\t$tags{$fn}\t/^sub $fn/\n";
    }

    close (OUT);
}
else
{
    print "\nNo tags found.\n";
}

if (@conflicts)
{
    print "\nConflicts:\n\n";

    foreach (@conflicts)
    {
        print;
    }
}
