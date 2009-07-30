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
