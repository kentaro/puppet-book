#!/usr/bin/env perl

use strict;
use warnings;

my ($toc, $dir) = @ARGV;

open my $fh, "< $toc" or die $!;
my $doc = do { local $/ = undef; <$fh> };
close $fh;

while ($doc =~ s{<navPoint [^>]+?>(.+?)</navPoint>}{}ims) {
    my $chapter = $1;
    (my $title) = $chapter =~ m{<text>([^<]+)</text>};
    (my $file)  = $chapter =~ m{<content src="([^"]+)"/>};

    open my $read_fh, "< ${dir}/${file}" or die $!;
    my $content = do { local $/ = undef; <$read_fh> };
    close $read_fh;

    $content =~ s{<title>[^<]+</title>}{<title>${title}</title>};

    open my $write_fh, "> ${dir}/${file}" or die $!;
    print $write_fh $content;
    close $write_fh;
}
