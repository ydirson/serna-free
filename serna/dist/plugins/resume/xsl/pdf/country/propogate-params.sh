#!/bin/sh

SOURCE=us.xsl

# propogate-translations.sh
# By Mark Miller, though he is not proud of it.
#
# This is a quick and VERY DIRTY way to add any new params defined
# in us.xsl (or $SOURCE) to the rest of the country.xsl stylesheets
# with a \"TRANSLATION NEEDED\" message.  It also checks for outdated 
# params existing in other country.xsl files but not in $SOURCE
#
# IMPORTANT: After running this script, you must manually edit each 
# .xsl file and make sure that all the xsl:param tags are INSIDE the
# <xsl:stylesheet> tags.  This script does NOT do that for you.
#
# WARNING: This script is by no means fool-proof, and only included
# to make life slightly easier for developers.  Please manually edit
# each .xsl file after running to be sure that the script has not
# screwed anything up. "

echo "======== Checking Files for outdated params..."
for country in `ls -1 *.xsl`; do 
  for param in `grep '<xsl:param' $country | \
   		perl -p -e 's/.*name=\"(.*)\"\>.*\<\/xsl:param\>/$1/g`; 
  do
    exists=`grep -c $param $SOURCE`
    if [ "0" = $exists ]; then 
      echo "Param $param in file $country does not exist in $SOURCE."
    fi
  done
done  

echo "======== Adding params in $SOURCE to other files as necessary..."
for country in `ls -1 *.xsl`; do 
  for param in `grep '<xsl:param' $SOURCE | \
   		perl -p -e 's/.*name=\"(.*)\"\>.*\<\/xsl:param\>/$1/g`; 
  do
    translated=`grep -c $param $country`
    if [ "0" = $translated ]; 
    then 
      echo "Adding param $param to $country"
      echo "  <xsl:param name=\"$param\">TRANSLATION NEEDED</xsl:param>" >> $country
    fi
  done
done
echo "Done."
