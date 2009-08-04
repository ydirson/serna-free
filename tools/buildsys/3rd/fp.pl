#!/usr/bin/perl

exit(0) if $#ARGV < 0;

my $sd = $0;
$sd =~ s|[/\\][^/\\]+$||;

open(T, "$sd/pkg-utils.t") or die "Can't open pkg-utils.t";
my $tpl = '';
while (<T>) {
    $tpl .= $_;
}
eval $tpl;

my $pkg;
my $pkgName = @ARGV[0];

if ($#ARGV == 2) {
    $pkg = find_package_by_files(@ARGV[1], @ARGV[2]);
}
else {
    my $pkg = find_package($pkgName);
    print dump_package($pkg);
}

write_package("$pkgName.pkg", $pkg);

exit(0);
