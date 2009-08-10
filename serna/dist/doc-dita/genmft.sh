#!/bin/sh

../../../tools/manifest/lst2mft.py \
        -d '${inst_prefix}/${serna}/doc' \
        -s '${top_builddir}/${apps_serna}/dist/doc-dita' \
        -x serna-help.html \
        -o MANIFEST.doc-dita.built \
        doc-dita
