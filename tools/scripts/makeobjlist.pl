#!/usr/bin/perl

sub check_unix {
    my($r);
    $r = 0;
    if ( -f "/bin/uname" ) {
	$r = 1;
	(-f "\\bin\\uname") && ($r = 0);
    }
    if ( -f "/usr/bin/uname" ) {
	$r = 1;
	(-f "\\usr\\bin\\uname") && ($r = 0);
    }
    return $r;
}

my %objects = ();
my $output = shift;
my ($objvar, $sep) = ("", "\n");

if (defined $ENV{"OBJECTS_VARIABLE"}) {
    $objvar = $ENV{"OBJECTS_VARIABLE"};
    $objvar = "OBJECTS" unless $objvar;
    $sep = " \\\n\t";
    $objvar .= " = $sep";
}

my $is_unix = check_unix();
my $dsep = $is_unix ? '/' : '\\';

while ($_ = shift) {
    /^(.*?)[^\\\/]*$/ or die "Wrong target objects list file name\n";
    my $dir = $1;
    open(LIST, "<$_") or die "Can't open filelist $_\n";
    while (<LIST>) {
        chomp;
        my $file = $dir . $_;
        $file =~ s/[\\\/]/$dsep/g;
        $file =~ s/\\+/$dsep/g;
#        -r $file || die "Object file $file isn't readable\n" ;
        $objects{$file} = 1;
    }
}
close(LIST);
my @files = keys(%objects);
die "Objects list is empty\n" unless (@files);
open(LIST, ">$output") or die "Can't open output filelist $output\n";
print LIST "$objvar".join($sep, @files)."\n";
close(LIST);
