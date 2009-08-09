#${
    unless (Project("CLEANFILES") || Project("CLEANDIRS")) {
        DisableOutput();
    }
    else {
        foreach (qw/CLEANFILES CLEANDIRS/) {
            my @list = split /\s+/, Project($_);
            grep { s|/+|\\|g } (@list) unless $is_unix;
            Project("$_ = " . join(' ', @list));
        }
    }
#$}
CLEANFILES  = #$ Expand("CLEANFILES");
CLEANDIRS   = #$ Expand("CLEANDIRS");

clean:
#$ DisableOutput() if $is_unix;
#$ $text .= "\t-del /q \$(CLEANFILES)" if Project("CLEANFILES");
#$ $text .= "\t-rd /s /q \$(CLEANDIRS)" if Project("CLEANDIRS");
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless $is_unix;
#$ $text .= "\t-rm -f \$(CLEANFILES)" if Project("CLEANFILES");
#$ $text .= "\t-rm -rf \$(CLEANDIRS)" if Project("CLEANDIRS");
#$ EnableOutput() unless $is_unix;

.PHONY: clean
#$ EnableOutput() unless (Project("CLEANFILES") || Project("CLEANDIRS"));
