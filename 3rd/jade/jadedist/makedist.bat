copy files.txt+..\FILES all-files.txt
cd ..
del jadedist\jade.zip
del jadedist\jadew.zip
zip -q -@ <jadedist\all-files.txt jadedist\jade.zip
zip -q -j -@ <jadedist\bin-files.txt jadedist\jadew.zip
