#
# Makefile for rpmbuild (called during rpmbuild invocation)
#

ST_DIR          ?= /home/abs/main
ST_BUILDDIR     ?= $(ST_DIR)/build

override prefix  := $(if $(prefix),$(shell echo $(prefix) | sed -e 's%^/%%'),usr/local)

top_srcdir	?= $(ST_DIR)
inst_prefix	?= ${prefix}

all-forward: all

ifneq (${RPM_SUBMAKEFILE},)
    include ${RPM_SUBMAKEFILE}
else
    $(error Cannot find makefile when preparing rpm)
endif 
