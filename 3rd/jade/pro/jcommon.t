#${
    Project('CONFIG          = release');

    Project('TMAKE_CXXFLAGS  = -DSTDC_HEADERS=1 \
-DHAVE_LIMITS_H=1 -DHAVE_ST_BLKSIZE=1');

    Project('TMAKE_CXX       = eg++');
    Project('TMAKE_CC        = egcs');
    Project('TMAKE_LINK      = eg++ -pthread');
    Project('INCLUDEPATH    += ../sp ../generic');
#$}
