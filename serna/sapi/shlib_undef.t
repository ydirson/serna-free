#${
    if ($is_unix) {
        Project('TMAKE_LFLAGS *= $(LD_SHLIB_UNDEF)');
        my %flags = (darwin => '-Wl,-undefined,dynamic_lookup',
                     linux  => '-Wl,--allow-shlib-undefined');
        my $flag = $flags{Project("PLATFORM")};
        Project("LD_SHLIB_UNDEF *= $flag") if $flag;
    }
#$}
#$ DisableOutput() unless $is_unix;
LD_SHLIB_UNDEF ?= #$ Expand("LD_SHLIB_UNDEF");
#$ DisableOutput() unless "darwin" eq Project("PLATFORM");
MACOSX_DEPLOYMENT_TARGET = 10.4
export MACOSX_DEPLOYMENT_TARGET

#$ EnableOutput() unless "darwin" eq Project("PLATFORM");
#$ EnableOutput() unless $is_unix;
