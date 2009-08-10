#${
    foreach (qw/CLEANFILES CLEANDIRS/) {
        my @list = split /\s+/, Project($_);
        grep { s|/+|\\|g } (@list) unless $is_unix;
        Project("$_ = " . join(' ', @list));
    }
#$}
CLEANFILES  = #$ ExpandGlue("CLEANFILES", "\t", " \\\n\t\t", "\n");
CLEANDIRS   = #$ ExpandGlue("CLEANDIRS", "\t", " \\\n\t\t", "\n");

clean: lastclean

lastclean:
#$ DisableOutput() if $is_unix;
#$ $text .= "\t-del /q \$(CLEANFILES)" if Project("CLEANFILES");
#$ $text .= "\t-rd /s /q \$(CLEANDIRS)" if Project("CLEANDIRS");
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless $is_unix;
#$ $text .= "\t-rm -f \$(CLEANFILES)" if Project("CLEANFILES");
#$ $text .= "\t-rm -rf \$(CLEANDIRS)" if Project("CLEANDIRS");

.PHONY: clean lastclean
#$ EnableOutput() unless $is_unix;
