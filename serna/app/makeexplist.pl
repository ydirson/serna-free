#!/usr/bin/perl

my %symbols = ();
my $rel = `uname -r`;
my $sympat = '^(__.*)';
$sympat = '^\s*"(__.*)".*' if $rel =~ "^9\.";

while (<>) {
    chomp;
    next unless /$sympat$/;
    $sym = $1;
    next if $sym =~ /^__[ZNK\d]+SernaApi\d.*$/;
    $symbols{$sym} = 1;
}

print join("\n", keys(%symbols)), "\n";
