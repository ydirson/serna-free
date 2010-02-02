#!/usr/bin/perl

my %symbols = ();

while (<>) {
    next unless /: undefined reference to \`(.*)\'$/;
    $symbols{$1} = 1;
}

print "{\n";
if (%symbols) {
	print "\tglobal:\n";

	for $s (keys(%symbols)) {
	    print "\t\t$s;\n"
	}
}

print "\tlocal: *;\n};\n";
