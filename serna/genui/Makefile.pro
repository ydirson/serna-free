#
#
debug:XSLTPROC_PARAMS   += --stringparam build_mode debug
release:XSLTPROC_PARAMS += --stringparam build_mode release
win32:XSLTPROC_PARAMS   += --stringparam platform win
linux:XSLTPROC_PARAMS   += --stringparam platform linux
darwin:XSLTPROC_PARAMS     += --stringparam platform mac

TEMPLATE  = $$srcdir/genui
EXTRA_TEMPLATES = clean.t
