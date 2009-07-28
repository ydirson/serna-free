TEMPLATE = 3rd
SUBDIRS = 
# win32:SUBDIRS += zlib-win rx

SUBDIRS +=  \
            docbook \
            dita-ot \
            jade \
            antlr \
            aspell \
            iconv \
            python \
            libxml:iconv;python \
            xsltproc:libxml \
            qt \
            qtpropertybrowser:qt \
            qscintilla:qt \
            sip:python;qt \
            pyqt:sip;iconv \
            doxygen

win32:SUBDIRS += \
                libxml-python:python;xsltproc\
                jdk

linux:SUBDIRS += \
                 jdk
