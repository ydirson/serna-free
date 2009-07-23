--- qtpropertybrowser-2.5-opensource.orig/qtpropertybrowser.pro  2007-04-24 16:15:09.196111543 +0400                     
+++ qtpropertybrowser-2.5-opensource/qtpropertybrowser.pro       2006-11-29 11:59:11.000000000 +0300                     
@@ -1,5 +1,15 @@
-TEMPLATE=subdirs
-CONFIG += ordered
-include(common.pri)
-qtpropertybrowser-uselib:SUBDIRS=buildlib
-SUBDIRS+=examples
+TEMPLATE=lib
+CONFIG += ordered dll
+
+QMAKE_INCDIR_QT = $(QTDIR)/include
+QMAKE_LIBDIR_QT = $(QTDIR)/lib
+QMAKE_RCC       = $(QTDIR)/bin/rcc
+QMAKE_MOC       = $(QTDIR)/bin/moc
+QMAKE_UIC       = $(QTDIR)/bin/uic
+win32:QMAKE_MOC       = $(QTDIR)\bin\moc
+win32:QMAKE_UIC       = $(QTDIR)\bin\uic
+
+mac:QMAKE_MAC_SDK   = /Developer/SDKs/MacOSX10.4u.sdk
+mac:CONFIG         += x86 ppc
+
+include(src/qtpropertybrowser.pri)
