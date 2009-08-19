#${
    if (Config("moc")) {
        my @mocs;
        foreach (values %moc_output) {
            push @mocs, $_ if m/.*\.$moc_ext$/;
        }
        if (@mocs) {
            Project("MOC_FILES = " . join(' ', @mocs));
            Project('CLEANFILES *= $(MOC_FILES)');
        }
    }
#$}
#$ DisableOutput() unless Config("moc");
MOC_FILES   = #$ ExpandGlue("MOC_FILES", "\\\n\t\t", " \\\n\t\t", "\n");
#$ EnableOutput() unless Config("moc");
#$ IncludeTemplate("3rd/clean");
