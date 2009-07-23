#
# Distribution generation template (all platforms)
# 
#
#${
    my @tm = localtime();
    my $release = sprintf("%4d%02d%02d", ($tm[5]+1900), ($tm[4]+1), $tm[3]);
    $release .= ".kdx" if (Config("kdexecutor"));
    Project("RELEASE = $release");
#$}
RELEASE = #$ Expand("RELEASE");

pkg:

#$ IncludeTemplate(0 == $is_unix ? "win32-dist" : "unix-dist");
