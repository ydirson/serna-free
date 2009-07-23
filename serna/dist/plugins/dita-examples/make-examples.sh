#!/bin/sh
cd ../../../dist
cp ../plugins/ditatopicref/ditatopicref.cxx plugins/dita-examples
zip /tmp/dita-examples.zip xml/templates/ditadw/*script*.xml plugins/dita-examples/*.{spd,py,xml} plugins/dita-examples/{README,ditatopicref.cxx}
rm plugins/dita-examples/ditatopicref.cxx
