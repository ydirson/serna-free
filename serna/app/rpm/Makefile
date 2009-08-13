#
# Makefile for rpmbuild (called during rpmbuild invocation)
#

ST_DIR          ?= /home/abs/main
ST_BUILDDIR     ?= $(ST_DIR)/build

override prefix  := $(if $(prefix),$(shell echo $(prefix) | sed -e 's%^/%%'),usr/local)

top_srcdir	?= $(ST_DIR)
inst_prefix	?= ${prefix}
rpm_prefix      ?= ${prefix}

all-forward: rpm-install

ifneq (${MAKEFILE_INSTALL},)
    include ${MAKEFILE_INSTALL}
else
    $(error Cannot find makefile when preparing rpm)
endif 
