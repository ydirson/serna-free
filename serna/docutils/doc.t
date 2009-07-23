#${
    my $srcdir = Project("srcdir");
    if (Project("SERNA_DOC") eq "DITA") {
        IncludeTemplate("$srcdir/dita.t");
    }
    else {
        IncludeTemplate("$srcdir/docbook.t");
    }
    IncludeTemplate("$srcdir/migration/db2dita.t");
#$}
TAG_CHK_SCRIPT = #$ Expand("TAG_CHK_SCRIPT");
TAG_LIST       = doctags.lst

check_tags: $(TAG_CHK_SCRIPT)
	$(PYTHON) $(TAG_CHK_SCRIPT) $(TAG_LIST) $(top_srcdir)/apps/serna
