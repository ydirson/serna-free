#$ DisableOutput();
#
# Syntext Editor template for building 3rd components
# Determines target & TMAKE_TEMPLATE
# 
#
#$ EnableOutput();
#${    
    my $Template = Project("buildtype");
    if (!$Template) {
        if ($Options{"buildtype"}) {
            $Template = "3rd/" . $Options{"buildtype"};
            Project("TMAKE_TEMPLATE = $Template");
        }
    }
#$}
