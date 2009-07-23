#
#

TEMPLATE = 3rd
CONFIG 	*= static mt
VERSION = 2.7.1
SUBDIRS = $$VERSION
options = package=antlr
win32:options *= buildtype=make
unix:options *= buildtype=configure
