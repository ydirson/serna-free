#
#

TEMPLATE = 3rd

VERSION = 1.5.1
options = package=doxygen
unix:options += buildtype=configure
win32:options += buildtype=make

SUBDIRS = $$VERSION
