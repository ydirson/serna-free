
BASEDIRS = sfworks serna

sfworks_MODULES = common grove spgrovebuilder qtgrovebuilder catmgr \
                  groveeditor urimgr xpath xslt xs formatter editableview \
                  sceneareaset proputils ui dav
                  
serna_MODULES = utils structeditor plaineditor docutils \
                core docview
                       
BASEDIR = $(top_builddir)/serna

MODULES = 	\
		sapi \
		plugins/tableplugin \
		plugins/xsltparam \
		plugins/customcontent \
		plugins/docinfo \
		plugins/speller \
		plugins/contentmap

OBJECTS_LIST = objects.lst
EXPORTS_LIST = exports.lst

unix:MAKE_LDSCRIPT = $(srcdir)/makeldscript.pl
darwin:MAKE_LDSCRIPT = $(srcdir)/makeexplist.pl
sunos:MAKE_LDSCRIPT = $(srcdir)/makeldmap.pl

TARGETDEPS *= $(EXPORTS_LIST) $(shell cat $(OBJECTS_LIST))
