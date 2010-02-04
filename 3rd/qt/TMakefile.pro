#
#

TEMPLATE        = 3rd
QTBUILDBASE     = qt
CONFIG         *= mt thread dynamic
options         = package=qt,buildtype=configure

VERSION         = 4.5.3
SUBDIRS         = $$VERSION
