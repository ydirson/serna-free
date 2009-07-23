--- old/src/qt3support/qt3support.pro	2007-01-19 19:04:49.624629948 +0300
+++ new/src/qt3support/qt3support.pro	2007-01-19 19:05:06.757692547 +0300
@@ -1,6 +1,6 @@
 TARGET	   = Qt3Support
 QPRO_PWD   = $$PWD
-QT         = core gui network sql
-DEFINES   += QT_BUILD_COMPAT_LIB
+QT         = core gui network
+DEFINES   += QT_BUILD_COMPAT_LIB QT_NO_SQL QT_NO_SQL_VIEW_WIDGETS QT_NO_SQL_EDIT_WIDGETS QT_NO_SQL_FORM
 win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x60000000
 
@@ -10,7 +10,6 @@
 !win32-icc:PRECOMPILED_HEADER = other/qt_compat_pch.h
 
 include(tools/tools.pri)
-include(sql/sql.pri)
 include(other/other.pri)
 include(itemviews/itemviews.pri)
 include(widgets/widgets.pri)
