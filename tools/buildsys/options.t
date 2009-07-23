#!
#!  Introduces new hash named %Options which contains parsed options
#!
#${
    %Options = ();
    my $options = Project("options");
    foreach (split /[\s,;]/, $options) {
        my ($key, $value) = split /=/;
        grep { s/^\s*//; s/\s*$// } ($key, $value);
        $Options{$key} = $value;
    }
    Project("options = " . join(',', map { $_.'='.$Options{$_}} keys %Options))
#$}
