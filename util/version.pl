#!/usr/bin/env perl
use 5.006;
use warnings;

my $VERSION_FILE  = 'include/patchlevel.h';
my $VERSION_PARTS = 4;

my %game_version;
open my $fh, '<', $VERSION_FILE or die "can't open file: $!";

LINE:
while (my $line = <$fh>) {
    chomp $line;

    if ($line =~ /^#define\s+(VERSION_(?:MAJOR|MINOR)|PATCHLEVEL|EDITLEVEL)\s+(\d+)/) {
        $game_version{$1} = $2;
        # Already have all four parts? Great, stop processing the file.
        last LINE if keys %game_version == $VERSION_PARTS;
    }
}

printf "%d.%d.%d-%d\n", @game_version{qw(VERSION_MAJOR VERSION_MINOR PATCHLEVEL EDITLEVEL)};
