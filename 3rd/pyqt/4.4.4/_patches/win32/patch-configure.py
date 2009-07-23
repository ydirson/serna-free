--- old/configure.py	2007-12-19 20:12:27.024811615 +0300
+++ new/configure.py	2007-12-19 21:26:53.804152233 +0300
@@ -56,6 +56,11 @@
 dbuslibdirs = []
 dbuslibs = []
 
+def qmake_cfg_args():
+    global opts
+    if opts.debug:
+        return 'CONFIG+="release debug"'
+    return 'CONFIG+="debug release"'
 
 # Under Windows qmake and the Qt DLLs must be into the system PATH otherwise
 # the dynamic linker won't be able to resolve the symbols.  On other systems we
@@ -363,9 +368,11 @@
         if "QtDesigner" in pyqt_modules:
             qpy_dir = os.path.abspath(os.path.join("qpy", "QtDesigner"))
 
+            qpydesigner_lib = "qpydesigner"
             if sys.platform == "win32":
                 if opts.debug:
                     qpy_lib_dir = os.path.join(qpy_dir, "debug")
+                    qpydesigner_lib = 'd' + qpydesigner_lib
                 else:
                     qpy_lib_dir = os.path.join(qpy_dir, "release")
             else:
@@ -374,13 +381,13 @@
             if opts.bigqt:
                 cons_xtra_incdirs.append(qpy_dir)
                 cons_xtra_libdirs.append(qpy_lib_dir)
-                cons_xtra_libs.append("qpydesigner")
+                cons_xtra_libs.append(qpydesigner_lib)
 
                 generate_code("QtDesigner")
             else:
                 generate_code("QtDesigner", extra_include_dirs=[qpy_dir],
                         extra_lib_dirs=[qpy_lib_dir],
-                        extra_libs=["qpydesigner"])
+                        extra_libs=[qpydesigner_lib])
 
         if "QAxContainer" in pyqt_modules:
             generate_code("QAxContainer")
@@ -575,7 +582,7 @@
             qpylibs["QtDesigner"] = "qpydesigner.pro"
 
         # Run qmake to generate the Makefiles.
-        qmake_args = fix_qmake_args()
+        qmake_args = fix_qmake_args(qmake_cfg_args())
         cwd = os.getcwd()
 
         for qpy, pro in qpylibs.iteritems():
@@ -739,7 +746,7 @@
                 sipconfig.inform("Creating Qt Designer plugin Makefile...")
 
                 # Run qmake to generate the Makefile.
-                qmake_args = fix_qmake_args()
+                qmake_args = fix_qmake_args(qmake_cfg_args())
                 cwd = os.getcwd()
                 os.chdir("designer")
 
@@ -1452,8 +1452,12 @@
     qmake_args = fix_qmake_args("-o " + make_file)
 
     if sys.platform == "win32":
-        exe_file = os.path.join("release", app + ".exe")
-        make_target = " release"
+        if opts.debug:
+            exe_file = os.path.join("debug", app + ".exe")
+            make_target = " debug"
+        else:
+           exe_file = os.path.join("release", app + ".exe")
+           make_target = " release"
     elif sys.platform == "darwin":
         exe_file = os.path.join(app + ".app", "Contents", "MacOS", app)
     else:
