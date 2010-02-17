# TMake settings for qt as a Serna Editor 3rd component


PACKAGE_CFG_OPTIONS += \
                      -nomake examples \
                      -nomake demos \
                      -nomake docs \
                      -qt-gif \
                      -no-libmng \
                      -qt-libpng \
                      -qt-libtiff \
                      -qt-libjpeg \
                      -qt-zlib \
                      -no-xmlpatterns \
                      -no-phonon \
                      -no-webkit \
                      -no-3dnow \
                      -no-dbus \
                      -no-opengl \
                      -no-scripttools \
                      -qt-sql-sqlite \
                      -confirm-license \
                      -developer-build \
                      -opensource

release:PACKAGE_CFG_OPTIONS *= -release
debug:PACKAGE_CFG_OPTIONS *= -debug

win32:PACKAGE_CFG_OPTIONS *=    \
                                -no-direct3d \
                                -rtti \
                                -no-sql-odbc \

unix:PACKAGE_CFG_OPTIONS *=     \
                                -svg \
                                -no-openssl \
                                -no-iconv \
                                -no-cups \
                                -no-nis \
                                -no-rpath \
                                -no-sm \
                                -no-xinerama \
                                -no-pch \
                                -no-glib \
                                -no-xfixes \
                                -no-exceptions \
                                -separate-debug-info

PACKAGE_CFG_ENV     = QMAKESPEC=$(QMAKESPEC) QTPREFIX=$(QTPREFIX)
                                
QT_ASSISTANT_CUSTOM_IMAGES = qt/icons/assistant
unix:PACKAGE_MAKE_ENV = LD_LIBRARY_PATH=$(top_srcdir)/$(VERSION)/$(MAKE_DIR)/lib
darwin:PACKAGE_MAKE_ENV = DYLD_LIBRARY_PATH=$(top_srcdir)/$(VERSION)/$(MAKE_DIR)/lib

PACKAGE_TARGET  = all qm-qt
                 
#unix:PACKAGE_MAKEOPTS = -j3
INSTALL_ENV_VARS += QMAKESPEC

BASENAME            = qt-all-opensource-src-$$VERSION
ARCHIVE             = $$ARCHIVE_DIR/$$BASENAME.tar.bz2
PACKAGE_DIR         = $$BASENAME

CLEANFILES += $(top_srcdir)/MANIFEST $(top_srcdir)/q3porting.xml 
unix:CLEANFILES += $(top_srcdir)/translations
win32:CLEANDIRS = $(top_srcdir)/translations

CLEANDIRS += $(top_srcdir)/include $(top_srcdir)/plugins
