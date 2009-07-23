#
#

TEMPLATE = serna
SUBDIRS  = common dav grove spgrovebuilder
SUBDIRS += catmgr \
          groveeditor urimgr xpath xslt xs formatter editableview \
          ui sceneareaset proputils
options = package=sfworks

# Comment this to switch off new xpath/xslt optimization
CONFIG += xpath3 qtexternal
