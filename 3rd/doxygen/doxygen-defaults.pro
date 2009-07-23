
BASENAME    = doxygen-$$VERSION
PACKAGE_DIR = $$BASENAME

unix:PACKAGE_CFG_OPTIONS = --release

win32:PACKAGE_MAKEFILE          = msvc
win32:PACKAGE_MAKE              = .\make.bat
win32:PACKAGE_MAKE_ENV_ADD      = PATH=+$$THIRD_DIR\bin;

PACKAGE_MAKEFILEOPT             = 

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.src.tar.gz
