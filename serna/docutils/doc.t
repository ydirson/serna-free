#${
    my $srcdir = Project("srcdir");
    if (Project("SERNA_DOC") eq "DITA") {
        IncludeTemplate("$srcdir/dita.t");
    }
    else {
        IncludeTemplate("$srcdir/docbook.t");
    }
    IncludeTemplate("$srcdir/migration/db2dita.t");
    Project('ASST_ICON = $(DITA_HTML_DIR)/assistant.png');
    Project('ASST_ICON_SRC = $(DITA_SRC_DIR)/assistant.png');
    Project('ALL_DEPS += qhc');
#$}
TAG_CHK_SCRIPT = #$ Expand("TAG_CHK_SCRIPT");
TAG_LIST       = doctags.lst

check_tags: $(TAG_CHK_SCRIPT)
	$(PYTHON) $(TAG_CHK_SCRIPT) $(TAG_LIST) $(top_srcdir)/apps/serna

QHG             = #$ ExpandPathName("QHG");
QCG             = #$ ExpandPathName("QCG");
ASST_ICON       = #$ ExpandPathName("ASST_ICON");
ASST_ICON_SRC   = #$ ExpandPathName("ASST_ICON_SRC");

$(ASST_ICON): $(ASST_ICON_SRC)
	$(COPY) $(ASST_ICON_SRC) $(ASST_ICON)

$(DITA_HTML_DIR)/files.lst:
	$(PYTHON) $(srcdir)/qhc_list.py -o $@ -x serna-help.html -x ".*\.(x[sm]l|cmd|lst|qhc?p|qch|adp)" $(DITA_HTML_DIR)

lst: $(DITA_HTML_DIR)/files.lst

$(DITA_HTML_DIR)/serna.qhp: $(DITA_HTML_DIR)/serna.adp $(DITA_HTML_DIR)/files.lst
	$(XSLTPROC) -o $@ $(srcdir)/adp2qhp.xsl $(DITA_HTML_DIR)/serna.adp
	$(PYTHON) $(srcdir)/replace.py -p "@FILES@" -r $(DITA_HTML_DIR)/files.lst $(DITA_HTML_DIR)/serna.qhp

qhp: $(DITA_HTML_DIR)/serna.qhp

$(DITA_HTML_DIR)/serna.qch: $(DITA_HTML_DIR)/serna.qhp
	$(QHG) $(DITA_HTML_DIR)/serna.qhp -o $@

$(DITA_HTML_DIR)/serna.qhcp: $(srcdir)/serna.qhcp
	$(PYTHON) $(srcdir)/replace.py -s -p "@VERSION@" -r $(top_srcdir)/serna/app/APPVER \
        -o $@ $(srcdir)/serna.qhcp

$(DITA_HTML_DIR)/serna.qhc: $(DITA_HTML_DIR)/serna.qch $(DITA_HTML_DIR)/serna.qhcp $(ASST_ICON)
	$(QCG) $(DITA_HTML_DIR)/serna.qhcp -o $@

assistant.png: $(ASST_ICON_SRC)
	$(COPY) $(ASST_ICON_SRC) $@

ditahelp.qch: $(top_srcdir)/serna/dist/plugins/dita/ditahelp/ditahelp.qhp
	$(QHG) $(top_srcdir)/serna/dist/plugins/dita/ditahelp/ditahelp.qhp -o $@

DITAHELP_QHCP = #$ $text = '$(top_srcdir)/serna/dist/plugins/dita/ditahelp/ditahelp.qhcp'; $text =~ s^/+^\\^g unless $is_unix;

ditahelp.qhcp: $(DITAHELP_QHCP)
	$(COPY) $(DITAHELP_QHCP) $@

ditahelp.qhc: ditahelp.qch ditahelp.qhcp assistant.png
	$(QCG) ditahelp.qhcp -o $@

qhc: $(DITA_HTML_DIR)/serna.qhc ditahelp.qhc
