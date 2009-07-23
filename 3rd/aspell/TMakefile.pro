#
#

TEMPLATE = 3rd

VERSION = 0.50.5
options = package=aspell
unix:options += buildtype=configure
win32:options += buildtype=make

SUBDIRS = $$VERSION
