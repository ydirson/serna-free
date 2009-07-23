STYLE_PATH    = $(top_srcdir)\apps\serna\dist\xml\stylesheets

NW_ORIGNAME   = docbook-xsl-1.68.1
NW_DOCBOOK    = chunk.xsl
NW_CHNK_CMN   = chunk-common.xsl
NW_DBARCHIVE  = $(STYLE_PATH)\$(NW_ORIGNAME).tar.bz2

XSLTPROC      = $(THIRD_DIR)\bin\xsltproc.cmd --xinclude

DOCDIRS       = betatesters_guide users_guide developers_guide installation_guide faq xslbricks sapi_guide sapi sapi_py gui_items_guide
UGSRC         = $(docdir)\users_guide\ug.xml $(docdir)\users_guide\ug-contents.xml
DGSRC         = $(docdir)\developers_guide\dg.xml $(docdir)\developers_guide\dg-contents.xml
BGSRC         = $(docdir)\betatesters_guide\bg.xml $(docdir)\betatesters_guide\bg-content.xml
IGSRC         = $(docdir)\installation_guide\ig.xml
FAQSRC        = $(docdir)\faq\faq.xml $(docdir)\faq\faq-contents.xml
SGSRC         = $(docdir)\sapi_guide\sg.xml
GISRC         = $(docdir)\gui_items_guide\gg.xml
# XBSRC         = $(docdir)\xslbricks\xbguide.xml $(docdir)\xslbricks\xbguide-contents.xml

COMMON_ENT    = $(docdir)\common.ent
SOURCES       = $(docdir)\index.xml $(UGSRC) $(DGSRC) $(BGSRC) $(IGSRC) $(FAQSRC) $(SGSRC) $(XBSRC) $(GISRC) $(COMMON_ENT)
MKPROFILE     = $(srcdir)\xsl\mkprofile.xsl
MKSRC         = $(srcdir)\xsl\mkdoctags2.xsl
DBHTMLUTILS   = $(srcdir)\xsl\dbhtml-utils.xsl
CPP_SRC       = doctags.cpp
TAG_LIST      = doctags.lst
PROFILE       = serna.adp
SAPI_HTMLS    = sapi\index.html sapi_py\index.html

SAPI_GEN_DIR  = sapi
SAPI_DOC_MFT  = $(top_srcdir)/apps/serna/app/MANIFEST.sapi-docs

SAPI_PY_GEN_DIR  = sapi_py
SAPI_PY_DOC_MFT  = $(top_srcdir)/apps/serna/app/MANIFEST.sapi_py-docs

TAGS_CHK_SCRIPT = $(THIRD_DIR)\bin\python $(srcdir)\check_tags.py
MFT_CHK_SCRIPT  = $(THIRD_DIR)\bin\python $(srcdir)\check_mft.py

.SUFFIXES:

all: prep index.html $(PROFILE) $(SAPI_HTMLS) $(CPP_SRC) check_tags check_mft

$(NW_DOCBOOK): $(srcdir)\xsl\db-modified\$(NW_DOCBOOK)
	copy /y $? $@

$(NW_CHNK_CMN): $(srcdir)\xsl\db-modified\$(NW_CHNK_CMN)
	copy /y $? $@

$(CPP_SRC) $(TAG_LIST): $(PROFILE) $(COMMON_ENT) $(SOURCES) $(MKSRC)
	$(XSLTPROC) $(MKSRC) $(PROFILE)

check_tags: $(TAG_LIST)
	$(TAGS_CHK_SCRIPT) $(TAG_LIST) $(top_srcdir)\apps\serna

check_mft:
	$(MFT_CHK_SCRIPT) $(SAPI_DOC_MFT) $(SAPI_GEN_DIR) $(SAPI_PY_DOC_MFT) $(SAPI_PY_GEN_DIR)

.PHONY:

makedirs: .made_dirs

.made_dirs:
	for %d in ($(DOCDIRS)) do @if not exist %d md %d
	type nul >> $@

profile: $(PROFILE)

$(PROFILE): $(SOURCES) $(MKPROFILE) $(DBHTMLUTILS) $(COMMON_ENT)
	$(XSLTPROC) -o $@ $(MKPROFILE) index.xml

prep: copypics makedirs .unpack_done

copypics: .PHONY
	xcopy /s /q /r /y $(docdir)\*.*

.unpack_done: $(NW_DBARCHIVE)
	if exist $@ del /q $@
	$(THIRD_DIR)\bin\bzip2 -dc $(NW_DBARCHIVE) | $(THIRD_DIR)\bin\tar -xf -
	type nul > $@

index.html: $(SOURCES) $(NW_DOCBOOK) $(NW_CHNK_CMN) $(COMMON_ENT)
	$(XSLTPROC) $(NW_DOCBOOK) index.xml
	del $(SAPI_HTMLS)

sapi/index.html: sapi/index.xml index.html
	$(XSLTPROC) -o $@ --param use.id.as.filename 1 $(NW_ORIGNAME)/html/$(NW_DOCBOOK) sapi/index.xml

sapi_py/index.html: sapi_py/index.xml sapi/index.html index.html
	$(XSLTPROC) -o $@ --param use.id.as.filename 1 $(NW_ORIGNAME)/html/$(NW_DOCBOOK) sapi_py/index.xml

clean:
	for %d in ($(DOCDIRS)) do @del /q %d\*.html
	for %d in ($(DOCDIRS)) do @rd /q %d
	del /q *.html $(CPP_SRC) $(PROFILE)
