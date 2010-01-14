--- orig/qtsingleapplication.pro	2010-01-14 19:03:36.476785867 +0300
+++ new/qtsingleapplication.pro	2010-01-14 19:05:01.476801534 +0300
@@ -1,5 +1,9 @@
-TEMPLATE=subdirs
-CONFIG += ordered
-include(common.pri)
-qtsingleapplication-uselib:SUBDIRS=buildlib
-SUBDIRS+=examples
+TEMPLATE=lib
+CONFIG += ordered dll
+
+include(../custom.pri)
+
+mac:QMAKE_MAC_SDK   = /Developer/SDKs/MacOSX10.4u.sdk
+mac:CONFIG         += x86 ppc
+
+include(src/qtsingleapplication.pri)         
