#!/bin/sh -x

set -e

PKG_PLIST=${PKGDIR}/Contents/Info.plist

perl -i.bak -ne '
    if (m^\s+\Q<key>IFPkgPathMappings</key>\E^) { $pmseen = 1; next }
    if (/\s+\<dict\>$/ and $pmseen) { $skip = 1; next }
    if (/\s+\<\/dict\>$/ and $pmseen) { $skip = 0; $pmseen = 0; next };
    print unless $skip;
' "${PKG_PLIST}"

test -f "${PKG_PLIST}.bak" && rm -f "${PKG_PLIST}.bak"
