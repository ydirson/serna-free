@echo off
set ARGS=-d '${inst_prefix}/${serna}/doc' -s '${top_builddir}/${apps_serna}/dist/doc-dita' 
set ARGS=%ARGS% -x serna-help.html -o MANIFEST.doc-dita.built doc-dita
python ..\..\..\tools\manifest\lst2mft.py %ARGS%
