#
#  3rd party components installation template (win32 flavor)
#
#  
#
install: $(srcdir)\install.bat
	$(srcdir)\install.bat $(srcdir) $(THIRD_DIR) $(CONFIG)
