#$ DisableOutput() if (Project("JAVA_T_INCLUDED")); 
# 
# A template for exporting information about java tools to resulting makefile
#
#${

sub get_java_home {
    return unless $is_unix;
    return eval <<'EOF';
    if (-d "/usr/java") {
        my $jdk = pop @{[glob("/usr/java/j{2,}{s,}dk1.*")]};
        return $jdk if $jdk and Project("JAVA_PREFER_JDK");
        my $jre = pop @{[glob("/usr/java/j{2,}re1.*")]};
        return $jre if $jre;
        return $jdk if $jdk;
    }
    
#! check for gcj
    -x "/usr/bin/java" and return "/usr";

    return "";
EOF
}

sub get_java_home_win32 {
    return '' if $is_unix;
    my $python = file_tools("PYTHON");
    $python = expand_path(Project("PYTHON"));
    my $top_srcdir = Project("main_srcdir") ? Project("main_srcdir") : Project("top_srcdir");
    my $findjava = "$top_srcdir/tools/buildsys/findjava.py";
    $findjava .= ' -d' if Project("JAVA_PREFER_JDK");
    $findjava =~ s^/^\\^g;
    my $java_home = 'C:\\Program Files\\Java\\jre6';
    if ($python && -f $python) {
        if (open(FINDJAVA, "$python $findjava 2>nul |")) {
            while (<FINDJAVA>) {
                chomp;
                next unless $_;
                $java_home = $_;
                break;
            }
        }
    }
    return $java_home if -d $java_home;
    return '';
}

unless (Project("JAVA_T_INCLUDED")) {
    my %jdict = (
        linux => {
            JAVA_HOME   => Project("JAVA_HOME") ? Project("JAVA_HOME") : &get_java_home(),
            JAVA      => '$(JAVA_HOME)/bin/java',
            CLASSPATH_OPT  => "-classpath",
            CLASSPATH => (-f &get_java_home()."/lib/rt.jar") ? '$(JAVA_HOME)/lib/rt.jar' : '.'
        },
        win32 => {
            JAVA_HOME   => Project("JAVA_HOME") ? Project("JAVA_HOME") : &get_java_home_win32(),
            JAVA      => "java",
            CLASSPATH_OPT  => "-cp",
            CLASSPATH => "."
        }
    );
    my $platform = Project("PLATFORM");
    if (grep /^$platform/, keys %jdict) {
        my $tmp = $jdict{$platform};
        foreach (JAVA, JAVA_HOME, CLASSPATH, CLASSPATH_OPT) {
            Project("$_ = " . $$tmp{$_}) unless(Project($_));
        }
    }
    else {
        Project("JAVA = java") unless(Project("JAVA")); 
        Project("CLASSPATH_OPT = -classpath") unless(Project("CLASSPATH_OPT"));
    }
}
#$}
JAVA_HOME     = #$ ExpandPathName("JAVA_HOME");
JAVA          = #$ ExpandPathName("JAVA");
CLASSPATH     = #$ ExpandPathName("CLASSPATH");
CLASSPATH_OPT = #$ Expand("CLASSPATH_OPT");

#$ EnableOutput() if (Project("JAVA_T_INCLUDED")); 
#$ Project("JAVA_T_INCLUDED = 1");
