#
#

TEMPLATE = 3rd

options = package=Python
unix:options += buildtype=configure
win32:options += buildtype=make

VERSION	= 2.6.2
SUBDIRS = $$VERSION
