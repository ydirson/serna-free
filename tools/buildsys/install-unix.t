#
#  3rd party components installation template (unix flavor)
#
#  
#
install: $(srcdir)/install.sh
	$< $(srcdir) $(THIRD_DIR) $(CONFIG)
