
TEMPLATE = 3rd
VERSION = 1.11
SUBDIRS = $$VERSION

options = package=iconv
unix:options += buildtype=configure
win32:options += buildtype=make
