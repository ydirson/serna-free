#!/bin/sh

TEXTS=`find . -type f -name "*.xml" -o -name "*.dita" -o -name "*.ditamap" -o -name "*.ent"`

dos2unix ${TEXTS}

svn ps svn:eol-style native ${TEXTS}
svn ps svn:mime-type text/xml ${TEXTS}

for i in png jpg gif; do
    PICS=`find . -type f -name "*.$i"`
    svn ps svn:mime-type image/$i ${PICS}
done
