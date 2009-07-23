#!/usr/bin/perl

my ($srcdir, $srcname, $dstdir, $dstname) = (@ARGV);
shift @ARGV foreach (1..4);
my @script = ();
my $windows = "MSWin32" eq $^O;
my $mac = "darwin" eq $^O;

while (<STDIN>) {
    next unless /^\s*(\w+)\s*(\+=|=\+|=)\s*(.*)$/;
    my ($var, $op, $val, $line) = ($1, $2, $3, "");
    if ($windows) {
        $line = "set $var=$val" if '=' eq $op;
        $line = "set $var=$val\%$var\%" if '+=' eq $op;
        $line = "set $var=\%$var\%$val" if '=+' eq $op;
    }
    else {
        if ("LD_LIBRARY_PATH" eq $var and $mac) {
            $var = "DY$var";
            $val =~ s/(?<!DY)LD_LIBRARY_PATH/DYLD_LIBRARY_PATH/g;
        }
        $line = "$var=$val" if '=' eq $op;
        $line = "$var=$val\${$var}" if '+=' eq $op;
        $line = "$var=\${$var}$val" if '=+' eq $op;
        $line .= "; export $var" if $line;
    }
    push @script, $line if $line;
}

my $file = "$dstdir/$dstname";
my $exec = "$srcdir/$srcname";

if ($windows) {
    $exec =~ s^[/\\]^\\^g;
    $exec .= ' '.join(' ', @ARGV);
    unshift @script, "\@echo off\n";
    push @script, "\n$exec".' %*'."\n";
}
else {
    $exec =~ s^[/\\]^/^g;
    $exec .= " \\\n\t".join(" \\\n\t", @ARGV);
    unshift @script, "#!/bin/sh\n";
    push @script, "\nexec $exec".' "$@"'."\n";
}

open SCRIPT, ">$file" or die "$0: Can't open '$file' for writing";
print SCRIPT join("\n", @script);
close SCRIPT;
chmod 0755, $file;
