#
#
debug:XSLTPROC_PARAMS   += --stringparam build_mode debug
release:XSLTPROC_PARAMS += --stringparam build_mode release
win32:XSLTPROC_PARAMS   += --stringparam platform win
linux:XSLTPROC_PARAMS   += --stringparam platform linux
darwin:XSLTPROC_PARAMS     += --stringparam platform mac

XSLTPROC_PARAMS += --stringparam edition free

UI_SRCDIR = $$srcdir

TEMPLATE  = $$srcdir/genui
EXTRA_TEMPLATES = clean.t
