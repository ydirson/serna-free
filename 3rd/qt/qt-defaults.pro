# TMake settings for qt as a Serna Editor 3rd component

PACKAGE_CFG_OPTIONS *= \
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
                      -confirm-license \
                      -opensource

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

PACKAGE_TARGET  = \
		sub-moc-make_default-ordered \
		sub-rcc-make_default-ordered \
		sub-uic-make_default-ordered \
		sub-corelib-make_default-ordered \
		sub-xml-make_default-ordered \
		sub-gui-make_default-ordered \
		sub-sql-make_default-ordered \
		sub-network-make_default-ordered \
		sub-svg-make_default-ordered \
		sub-qt3support-make_default-ordered \
		sub-uic3-make_default-ordered \
		sub-plugins-make_default-ordered \
		sub-tools-make_default-ordered

win32:PACKAGE_TARGET  = \
		sub-winmain-make_default-ordered \
		sub-moc-make_default-ordered \
		sub-rcc-make_default-ordered \
		sub-uic-make_default-ordered \
		sub-corelib-make_default-ordered \
		sub-xml-make_default-ordered \
		sub-gui-make_default-ordered \
		sub-sql-make_default-ordered \
		sub-network-make_default-ordered \
		sub-svg-make_default-ordered \
		sub-qt3support-make_default-ordered \
		sub-uic3-make_default-ordered \
		sub-plugins-make_default-ordered \
		sub-tools-make_default-ordered
                 
unix:PACKAGE_MAKEOPTS = -j3
INSTALL_ENV_VARS += QMAKESPEC

BASENAME            = qt-all-opensource-src-$$VERSION
ARCHIVE             = $$ARCHIVE_DIR/$$BASENAME.tar.bz2
PACKAGE_DIR         = $$BASENAME
