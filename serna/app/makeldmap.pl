#!/usr/bin/perl

my %symbols = ();

while (<>) {
    chomp;
    next unless /^_.*$/;
    next if /^_[ZNK\d]+SernaApi\d.*$/;
    s/\s.*$//;
    $symbols{$_} = 1;
}

print "{\n\tglobal:\n";

foreach $s (keys(%symbols)) {
    print "\t\t$s;\n";
}

print "\tlocal: *;\n};\n";
