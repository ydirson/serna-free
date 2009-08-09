# TMake settings for qscintilla as a Syntext 3rd component
#

PACKAGE_DIR             = QScintilla-gpl-$$VERSION

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.gz

PACKAGE_MAKE        = $$MAKE
PACKAGE_MAKEOPTS    = QTDIR=$(QTDIR)
PACKAGE_MAKEFILE    = Makefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_CLEANOPTS	= clean
PACKAGE_TARGET  	= install

PACKAGE_CONFIGURE	= ${QTDIR}/bin/qmake
win32:PACKAGE_CONFIGURE	= %QTDIR%\bin\qmake

MAKE_DIR        	=   $$PACKAGE_DIR/Qt4
debug:PACKAGE_CFG_OPTIONS    *= "CONFIG+=debug"
release:PACKAGE_CFG_OPTIONS    *= "CONFIG+=release"

PACKAGE_CFG_OPTIONS	+=   QMAKE=$$PACKAGE_CONFIGURE qscintilla.pro

PACKAGE_CFG_OPTS         = -spec %QTDIR%\mkspecs\win32-msvc2008
vc2003:PACKAGE_CFG_OPTS  = -spec %QTDIR%\mkspecs\win32-msvc2003
                            
linux:PACKAGE_CFG_OPTIONS += -spec ${QTDIR}/mkspecs/linux-g++
win32:PACKAGE_CFG_OPTIONS += $$PACKAGE_CFG_OPTS
darwin:PACKAGE_CFG_OPTIONS += -spec ${QTDIR}/mkspecs/macx-g++
sunos:PACKAGE_CFG_OPTIONS += -spec ${QTDIR}/mkspecs/solaris-g++

PACKAGE_CFG_ENV         += \
                        QT_INSTALL_HEADERS=$$THIRD_DIR/qscintilla/include \
                        QT_INSTALL_LIBS=$$THIRD_DIR/lib \
                        QT_INSTALL_TRANSLATIONS=$$THIRD_DIR/qscintilla/translations \
                        QT_INSTALL_DATA=$$THIRD_DIR/qscintilla
                        
unix:PACKAGE_CFG_OPTIONS	+= \
                           QMAKE_INCDIR_QT=${QTDIR}/include \
                           QMAKE_LIBDIR_QT=${QTDIR}/lib \
                           QMAKE_MOC=${QTDIR}/bin/moc \
                           QMAKE_UIC=${QTDIR}/bin/uic
                           
win32:PACKAGE_CFG_OPTIONS	+= \
                           QMAKE_INCDIR_QT=%QTDIR%\include \
                           QMAKE_LIBDIR_QT=%QTDIR%\lib \
                           QMAKE_MOC=%QTDIR%\bin\moc \
                           QMAKE_UIC=%QTDIR%\bin\uic

CLEANFILES *= $(THIRD_DIR)/qscintilla/MANIFEST
