#
# Common tmake settings for plugins
#

DIST2 = 1

unix:STRIP_BINARY = 1
darwin:STRIP_FLAGS = -S -x

# darwin:FIX_INSTALL_NAMES = libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib
darwin:TMAKE_LFLAGS *= \
                       -Wl,-multiply_defined,suppress

linux:DBG_PLUGIN_LFLAGS *= -Wl,--no-undefined
#sunos:DBG_PLUGIN_LFLAGS *= -Wl,-z,defs
debug:PLUGIN_LFLAGS     *= $$DBG_PLUGIN_LFLAGS

linux:ALLOW_UNDEF_LFLAGS  *= -Wl,--allow-shlib-undefined
release:PLUGIN_LFLAGS     *= $$ALLOW_UNDEF_LFLAGS

plugin:LINUX_LFLAGS += -Wl,--version-script=$(top_srcdir)/serna/plugins/exports.map $$PLUGIN_LFLAGS
plugin:SUNOS_LFLAGS += -Wl,-M,$(top_srcdir)/serna/plugins/exports.map $$PLUGIN_LFLAGS

linux:TMAKE_LFLAGS += $$LINUX_LFLAGS
sunos:TMAKE_LFLAGS += $$SUNOS_LFLAGS

win32:DEFINES += NO_CMD_MAKER_EXPORT NO_UI_ITEM_MAKER_EXPORT
