@echo off
set ARGS=-d '${inst_prefix}/${serna}/doc' -s '${top_builddir}/${apps_serna}/dist/doc-dita' 
set ARGS=%ARGS% -x serna-help.html -x files.lst -x assistant.png -x .*\.qh.?p -o MANIFEST.doc-dita.built doc-dita
python %~dp0..\..\..\tools\manifest\lst2mft.py %ARGS%
