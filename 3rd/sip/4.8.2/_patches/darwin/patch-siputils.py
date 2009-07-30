--- old/siputils.py	2006-12-10 02:49:43.000000000 +0300
+++ new/siputils.py	2007-01-18 13:43:23.000000000 +0300
@@ -686,10 +686,7 @@
         mname is the name of the module.
         """
         if mname == "QtAssistant":
-            if self.config.qt_version >= 0x040202 and sys.platform == "darwin":
-                lib = mname
-            else:
-                lib = "QtAssistantClient"
+            lib = "QtAssistantClient"
         else:
             lib = mname

@@ -1325,7 +1322,7 @@
             # libraries, but python.org versions need bundles (unless built
             # with DYNLOADFILE=dynload_shlib.o).
             if sys.platform == "darwin":
-                lflags_plugin = ["-bundle"]
+                lflags_plugin = ["-bundle", "-undefined dynamic_lookup"]
             else:
                 lflags_plugin = self.optional_list("LFLAGS_PLUGIN")
 
@@ -1336,7 +1333,7 @@
 
         self.LFLAGS.extend(self.optional_list(lflags_console))
 
-        if sys.platform == "darwin":
+        if False and sys.platform == "darwin":
             # We use the -F flag to explictly specify the directory containing
             # the Python framework rather than rely on the default search path.
             # This allows Apple's Python to be used even if a later python.org
@@ -2446,7 +2446,7 @@
 
         if gui:
             exe = exe[:-4] + "w.exe"
-
+        exe = os.environ.get('PYTHON_EXE', exe)
         wf.write("@\"%s\" \"%s\" %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9\n" % (exe, script))
     elif sys.platform == "darwin":
         # python, pythonw and sys.executable are all different images.  We
@@ -2426,6 +2426,7 @@
         else:
             exe = "python"
 
+        exe = os.environ.get('PYTHON_EXE', exe)
         wf.write("#!/bin/sh\n")
         wf.write("exec %s %s ${1+\"$@\"}\n" % (exe, script))
     else:

