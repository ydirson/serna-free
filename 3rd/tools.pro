#
#

SED             = sed
ZIP             = zip
UNZIP           = unzip
unix:PATCH      = patch
BISON           = bison
CAT             = cat
TOUCH           = touch
TAR             = tar
BZIP2           = bzip2
GZIP            = gzip
unix:CP         = cp -f
PERL            = perl
CVS             = cvs
SVN             = svn
DOXYGEN         = $(THIRD_DIR)/bin/doxygen

unix:CPDIR      = cp -fpR
unix:CHDIR      = cd
unix:RM         = rm -f
unix:RMDIR      = rm -rf
unix:MKDIR      = mkdir -p
unix:SYMLINK    = ln -sf

win32:PATCH     = patch --no-backup-if-mismatch
win32:CP        = copy /b /y
win32:CPDIR     = xcopy /e /c /h /k /y
win32:CHDIR     = cd /d
win32:RM        = del /q /f
win32:RMDIR     = rd /s /q
win32:MKDIR     = mkdir
win32:SYMLINK   = copy /b
win32:IDC       = $(THIRD_DIR)\bin\idc
win32:DOXYGEN   = $(THIRD_DIR)\bin\doxygen

COPY            = $$CP

win32:XSLTPROC  = $(THIRD_DIR)\bin\xsltproc.cmd

unix:XSLTPROC   = $(THIRD_DIR)/bin/xsltproc

PYTHON          = $(THIRD_DIR)/bin/python

UIC             = $(THIRD_DIR)/bin/uic
LUPDATE         = $(THIRD_DIR)/bin/lupdate
LRELEASE        = $(THIRD_DIR)/bin/lrelease

