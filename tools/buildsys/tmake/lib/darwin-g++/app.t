#############################################################################
#!
#! This is a tmake template for building Unix applications.
#!
#${
    Project('TMAKE_APP_FLAG = 1');
    Project('TMAKE_LFLAGS *= $$TMAKE_LFLAGS_APP') if (Config("static"));
    IncludeTemplate("generic.t");
#$}
