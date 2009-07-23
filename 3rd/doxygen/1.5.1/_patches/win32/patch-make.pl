diff -bur doxygen-1.5.1/wintools/make.pl doxygen-1.5.1.win32/wintools/make.pl
--- doxygen-1.5.1/wintools/make.pl      2004-06-09 23:25:01.000000000 +0400
+++ doxygen-1.5.1.win32/wintools/make.pl        2006-12-29 16:41:18.000000000 +0300
@@ -7,15 +7,16 @@
   $make = "nmake";
 }
 
-use Cwd;
+#use Cwd;
 
 # get current working directory
-$pwd=cwd();
+$pwd=`cd`;
 
 # work-around slashes problems for nmake
 if ($target eq "msvc") 
 {
   $pwd=~s{/}{\\}g;
+  chomp $pwd;
 }
 
 # create config file
