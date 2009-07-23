$file = $ARGV[0];

%qtpaths = (
    prefix_path        => ["", 0],
    documentation_path => ["/doc", 0],
    headers_path       => ["/include", 0],
    libraries_path     => ["/lib", 0],
    binaries_path      => ["/bin", 0],
    plugins_path       => ["/bin", 0],
    data_path          => ["", 0],
    translations_path  => ["/translations", 0],
    settings_path      => ["/etc/settings", 0],
    examples_path      => ["/examples", 0],
    demos_path         => ["/demos", 0]
);

($changed, $result) = (0, "");

open(QCONFIG, "<$file") or die "Can't open $file\n";
while(<QCONFIG>) {
    foreach $k (keys(%qtpaths)) {
        if (/^static const char qt_configure_${k}_str/) {
            @{$qtpaths{$k}}[1] = 1;
            $changed = 1;
        }
    }
    s/(qt_.{4}path=)[^\"]+/\1$ENV{QTPREFIX}/ && s/static const char/static char/;
    $result .= $_;
}

close(QCONFIG);

if ($changed) {
    $result .= <<'EOF';

#include "qstring.h"

static void updatePath(const QString& path, char* array)
{
    QByteArray qba(path.toLocal8Bit());
    if (qba.length() < 255) {
        strncpy(array + 12, qba.data(), qba.size());
        array[12 + qba.size()] = '\0';
    }
}

void setInstallPath(const QString& path)
{
EOF

    foreach $k (keys(%qtpaths)) {
        if (@{$qtpaths{$k}}[1]) {
           $p = @{$qtpaths{$k}}[0];
           $p = " + QString::fromAscii(\"$p\")" if $p;
           $result .= "    updatePath(path$p,\t\t\tqt_configure_${k}_str);\n";
        }
    }
    $result .= "}\n";

    ($newfile, $oldfile)= ("$file.new", "$file.bak");
    open(QCONFIG_NEW, ">$newfile") or die "Can't open $newfile\n";
    print QCONFIG_NEW $result or die "Can't write $newfile\n";
    close(QCONFIG_NEW) or die "Can't close $newfile\n";
    rename($file, $oldfile) or die "Can't rename $file -> $oldfile\n";
    rename($newfile, $file);
    unlink($newfile, $oldfile);
}
