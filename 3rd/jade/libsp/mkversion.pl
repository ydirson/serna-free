#! /usr/bin/perl

$version = <>;
chop $version;
print <<END;
#define SP_VERSION SP_T("$version")
END
