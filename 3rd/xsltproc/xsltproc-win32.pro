
PACKAGE_CONFIGURE    = cscript configure.js
PACKAGE_TARGET       = install
MAKE_DIR             = $$PACKAGE_DIR\win32
PACKAGE_CFG_OPTIONS +=  \
                        xslt_debug=no \
                        static=yes \
                        debugger=no \
                        compiler=msvc \
                        iconv=yes \
                        include=.;$$THIRD_DIR\iconv;$$THIRD_DIR\libxml\include \
                        lib=.;$$THIRD_DIR\lib \
                        prefix=$$THIRD_DIR\xsltproc\install


debug:PACKAGE_CFG_OPTIONS += cruntime=-MDd debug=yes
release:PACKAGE_CFG_OPTIONS += cruntime=-MD debug=no
