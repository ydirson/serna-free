*** xx/configure.py.orig        2005-05-11 20:01:53.719957680 +0400                                                     
--- xx/configure.py     2005-05-11 20:05:29.699123856 +0400                                                             
@@ -721,7 +721,7 @@
                 from distutils.sysconfig import get_config_vars
                 ducfg = get_config_vars()
 
-                if sys.platform == "darwin":
+                if False and sys.platform == "darwin":
                     # We need to work out how to specify the right framework
                     # version.
                     link = "-framework Python"
@@ -755,6 +755,8 @@
 
                 fout = open("python.pro", "w+")
 
+                if opts.debug:
+                    fout.write("CONFIG += debug\n")
                 if sipcfg.universal:
                     fout.write("CONFIG += ppc i386\n")
                     fout.write("QMAKE_MAC_SDK = %s\n" % sipcfg.universal)
@@ -1422,9 +1424,11 @@
 def fix_qmake_args(args=""):
     """Make any platform specific adjustments to the arguments passed to qmake.
     """
-    if sys.platform == "darwin":
+    if sys.platform == "darwin" and not os.environ.has_key('QMAKESPEC'):
         # The Qt binary installer has macx-xcode as the default.
-        args = "-spec macx-g++ " + args
+        if opts.debug:
+            args = "CONFIG+=debug " + args
+        args = "-spec ${QTDIR}/mkspecs/macx-g++ " + args
 
     return args
 
