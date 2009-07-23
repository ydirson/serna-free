#
# 
#

SUBDIRS	=	libsp

all: FORCE

all install clean:
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@ debug=$(debug) static=$(static); done
	
FORCE:
