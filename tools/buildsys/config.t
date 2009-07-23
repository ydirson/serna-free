#$DisableOutput();
#
#  
#  This internal template sets tmake's intrinsic CONFIG options from our customized ones
#
#$EnableOutput();
#${
    my $config = Project("CONFIG");
    $config =~ s/[,;\s]+/ /g;
    Project("CONFIG = $config");

    foreach (split /[,;\s]/, Project("DEFAULT_CONFIG")) {
        SetConfig($_) unless(IsConfigSet($_));
    }

    $config = Project("CONFIG");
    Project("CONFIG = ");
    foreach (reverse split /\s/, $config) {
	SetConfig($_) unless(IsConfigSet($_));
    }

    if(index($config, "debug") > index($config, "release")) {
	SetConfig("debug");
    }

    if(Config("dynamic")) {
        Project("CONFIG *= dll", "CONFIG -= staticlib", "CONFIG -= static");
    }
    else {
       Project("CONFIG *= staticlib", "CONFIG -= dll");
    }

    if(Config("debug")) {
    	Project("CONFIG -= release");
    }
    else {
      	Project("CONFIG -= debug", "CONFIG *= release");
    }

    if(Config("app")) {
	Project("CONFIG -= dll", "CONFIG -= staticlib");
    }
	$config = Project("CONFIG");
    Project("DEFAULT_CONFIG = $config");
#$}
