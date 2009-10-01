#!/bin/sh

`dirname $0`/../../../tools/manifest/lst2mft.py \
        -d '${inst_prefix}/${serna}/doc' \
        -s '${top_builddir}/${apps_serna}/dist/doc-dita' \
        -x serna-help.html -x files.lst -x assistant.png -x '.*\.qh.?p' \
        -o MANIFEST.doc-dita.built \
        doc-dita
