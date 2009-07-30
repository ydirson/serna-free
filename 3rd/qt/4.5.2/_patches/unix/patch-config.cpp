--- qt-xxx-commercial-src-4.3.0.orig/tools/assistant/compat/config.cpp	2007-06-16 11:33:22.384715225 +0400
+++ qt-xxx-commercial-src-4.3.0/tools/assistant/compat/config.cpp	2007-06-16 11:43:49.805712734 +0400
@@ -64,6 +64,18 @@
 static QString get_prof_key() { return getKey() + QLatin1String("Profile/"); }
 static QString get_def_prof_key() { return get_prof_key() + QLatin1String("default/"); }
 
+static const QString& get_org()
+{
+    static QString org("SernaHelp");
+    return org;
+}
+
+static const QString& get_app()
+{
+    static QString app("serna_help");
+    return app;
+}
+
 static Config *static_configuration = 0;
 
 inline QString getVersionString()
@@ -143,7 +155,7 @@
 
     const QString profkey = key + QLatin1String("/Profile/") + profil->props[QLatin1String("name")] + QLatin1String("/");
 
-    QSettings settings;
+    QSettings settings(QSettings::IniFormat, QSettings::UserScope, get_org(), get_app());
 
     home = profil->props[QLatin1String("startpage")];;
     if (home.isEmpty() && isDefaultProfile)
@@ -188,7 +200,7 @@
 
     const QString profkey = get_prof_key() + profil->props[QLatin1String("name")] + QLatin1String("/");
 
-    QSettings settings;
+    QSettings settings(QSettings::IniFormat, QSettings::UserScope, get_org(), get_app());
 
     settings.setValue( profkey + QLatin1String("Source"), src );
     settings.setValue( key + QLatin1String("SideBarPage"), sideBarPage() );
@@ -221,7 +233,7 @@
 
 bool Config::defaultProfileExists()
 {
-    QSettings settings;
+    QSettings settings(QSettings::IniFormat, QSettings::UserScope, get_org(), get_app());
 //    const QString profKey = QLatin1String(QT_VERSION_STR) + QLatin1String("/Profile/default/");
     const QString profKey = get_def_prof_key();
 
@@ -239,7 +251,7 @@
 
 void Config::loadDefaultProfile()
 {
-    QSettings settings;
+    QSettings settings(QSettings::IniFormat, QSettings::UserScope, get_org(), get_app());
 //    const QString profKey = QLatin1String(QT_VERSION_STR) + QLatin1String("/Profile/default/");
     const QString profKey = get_def_prof_key();
 
@@ -312,7 +324,7 @@
         dcfs << Profile::storableFilePath(profile->dcfTitles[*it]);
     }
 
-    QSettings settings;
+    QSettings settings(QSettings::IniFormat, QSettings::UserScope, get_org(), get_app());
     settings.setValue( profKey + QLatin1String("Titles"), titles );
     settings.setValue( profKey + QLatin1String("DocFiles"), dcfs );
     settings.setValue( profKey + QLatin1String("IndexPages"), indexes );
