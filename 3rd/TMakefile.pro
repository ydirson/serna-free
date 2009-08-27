TEMPLATE = 3rd
SUBDIRS = 
# win32:SUBDIRS += zlib-win rx

SUBDIRS +=  \
            python \
            jdk \
            docbook \
            dita-ot \
            jade \
            antlr \
            aspell \
            iconv \
            libxml:iconv;python \
            xsltproc:libxml \
            qt \
            qtpropertybrowser:qt \
            qscintilla:qt \
            sip:python;qt \
            pyqt:sip;iconv \
            doxygen

darwin:SUBDIRS -= jdk

win32:SUBDIRS += \
                libxml-python:python;xsltproc
