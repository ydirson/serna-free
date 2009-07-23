#
#

STYLE_PATH    := $(top_srcdir)/apps/serna/dist/xml/stylesheets

NW_ORIGNAME   := docbook-xsl-1.68.1
NW_DOCBOOK    := chunk.xsl
NW_CHNK_CMN   := chunk-common.xsl
NW_DBARCHIVE  := $(STYLE_PATH)/$(NW_ORIGNAME).tar.bz2

XSLTPROC      := $(THIRD_DIR)/bin/xsltproc --xinclude

DOCDIRS       := betatesters_guide users_guide developers_guide installation_guide faq xslbricks sapi_guide gui_items_guide
UGSRC         := $(docdir)/users_guide/ug.xml $(docdir)/users_guide/ug-contents.xml
DGSRC         := $(docdir)/developers_guide/dg.xml $(docdir)/developers_guide/dg-contents.xml
BGSRC         := $(docdir)/betatesters_guide/bg.xml $(docdir)/betatesters_guide/bg-content.xml
IGSRC         := $(docdir)/installation_guide/ig.xml
FAQSRC        := $(docdir)/faq/faq.xml $(docdir)/faq/faq-contents.xml
SGSRC         := $(docdir)/sapi_guide/sg.xml
GISRC         := $(docdir)/gui_items_guide/gg.xml

COMMON_ENT    := $(docdir)/common.ent
SOURCES       := $(docdir)/index.xml $(UGSRC) $(DGSRC) $(BGSRC) $(IGSRC) $(FAQSRC) $(GISRC) $(SGSRC) $(XBSRC) $(COMMON_ENT)
MKPROFILE     := $(srcdir)/xsl/mkprofile.xsl
MKSRC         := $(srcdir)/xsl/mkdoctags2.xsl
DBHTMLUTILS   := $(srcdir)/xsl/dbhtml-utils.xsl

PROFILE       := serna.adp
CPP_SRC       := doctags.cpp
TAG_LIST      := doctags.lst
SAPI_HTMLS    := sapi/index.html sapi_py/index.html

SAPI_GEN_DIR  := sapi
SAPI_DOC_MFT  := $(top_srcdir)/apps/serna/app/MANIFEST.sapi-docs

SAPI_PY_GEN_DIR  := sapi_py
SAPI_PY_DOC_MFT  := $(top_srcdir)/apps/serna/app/MANIFEST.sapi_py-docs

TAGS_CHK_SCRIPT := $(THIRD_DIR)/bin/python $(srcdir)/check_tags.py
MFT_CHK_SCRIPT  := $(THIRD_DIR)/bin/python $(srcdir)/check_mft.py

%.xsl: $(srcdir)/xsl/db-modified/%.xsl
	cp -f $? $@

all: prep index.html $(SAPI_HTMLS) $(PROFILE) $(CPP_SRC) check_tags check_mft

$(CPP_SRC) $(TAG_LIST): $(PROFILE)
	$(XSLTPROC) $(MKSRC) $(PROFILE)

check_tags: $(TAG_LIST)
	$(TAGS_CHK_SCRIPT) $(TAG_LIST) $(top_srcdir)/apps/serna
        
check_mft:
	$(MFT_CHK_SCRIPT) $(SAPI_DOC_MFT) $(SAPI_GEN_DIR) $(SAPI_PY_DOC_MFT) $(SAPI_PY_GEN_DIR)

.PHONY: all copypics prep makedirs check_tags check_mft

makedirs: $(DOCDIRS)

$(DOCDIRS):
	for d in $(DOCDIRS); do [ -d $$d ] || mkdir -p $$d; done

$(PROFILE): $(SOURCES) $(MKPROFILE) $(DBHTMLUTILS) $(COMMON_ENT)
	$(XSLTPROC) -o $@ $(MKPROFILE) index.xml

prep: makedirs copypics .unpack_done

copypics:
	cp -f $(docdir)/*.* .
	USR=`id -un`; export USR;  \
	for d in $(DOCDIRS); do \
            (cd $(docdir); [ -d $$d ] && find $$d -name "*.*") | tr '\n' ' ' > /tmp/docgifs-$${USR}.lst; \
            if [ -s /tmp/docgifs-$${USR}.lst ]; then \
                (cd $(docdir); tar -cf - `cat /tmp/docgifs-$${USR}.lst`) | tar -xf -; \
            fi; \
        done

.unpack_done: $(NW_DBARCHIVE)
	rm -fr $(NW_ORIGNAME)
	echo $(basename $(notdir $?))
	tar -xjf $<
	touch $@

index.html: $(SOURCES) $(NW_DOCBOOK) $(NW_CHNK_CMN) $(COMMON_ENT)
	$(XSLTPROC) $(NW_DOCBOOK) index.xml
	rm -f $(SAPI_HTMLS)

sapi/index.html: sapi/index.xml index.html
	$(XSLTPROC) -o $@ --param use.id.as.filename 1 $(NW_ORIGNAME)/html/$(NW_DOCBOOK) sapi/index.xml

sapi_py/index.html: sapi_py/index.xml index.html
	$(XSLTPROC) -o $@ --param use.id.as.filename 1 $(NW_ORIGNAME)/html/$(NW_DOCBOOK) sapi_py/index.xml

clean:
	-rm -f *.html
	-for d in $(DOCDIRS); do rm $$d/*; done
	-rmdir $(DOCDIRS)
	-rm -f $(CPP_SRC) $(PROFILE) .copypics_done
