#
#
TEMPLATE  = serna

LIBRARIES = pyconsole
CONFIG   += moc dynamic dll

INCLUDEPATH =   .; \
                $(top_builddir)/serna; \
                $(CLIB_SRC); \
                $(top_srcdir)/serna;

USE = QtGui QtCore QtXml QtNetwork

HEADERS_PATTERN = \.h$ 

SOURCES_PATTERN = \.cxx$ 
USE_SAPI = 1

EXTRA_TEMPLATES = genlist ../plugins_common 
DESTDIR = $$top_builddir/serna/lib/pyplugin
UI_DIRS        = $$srcdir
