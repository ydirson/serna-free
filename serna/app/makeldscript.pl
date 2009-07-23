#!/usr/bin/perl

my %symbols = ();

while (<>) {
    next unless /: undefined reference to \`(.*)\'$/;
    $symbols{$1} = 1;
}

print "{\n\tglobal:\n";

for $s (keys(%symbols)) {
    print "\t\t$s;\n"
}

print "\tlocal: *;\n};\n";
