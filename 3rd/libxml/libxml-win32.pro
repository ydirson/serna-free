
PACKAGE_CONFIGURE    = cscript configure.js
PACKAGE_TARGET       = install
MAKE_DIR             = $$PACKAGE_DIR\win32
PACKAGE_CFG_OPTIONS += \
                       xml_debug=no \
                       compiler=msvc \
                       iconv=yes \
                       include=.;$$THIRD_DIR\iconv \
                       lib=.;$$THIRD_DIR\lib \
                       prefix=$$THIRD_DIR\libxml\install

debug:PACKAGE_CFG_OPTIONS += cruntime=-MDd debug=yes
release:PACKAGE_CFG_OPTIONS += cruntime=-MD debug=no
