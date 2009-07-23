//
// Test utility for Trace.h functions
//

#include "machdep.h"

#undef NDEBUG

#define TRACE_MODULE    MOD(advent)
#define TRACE_TAGS      TAG(xyzzy) TAG(plugh) TAG(y2) TAG(fee) TAG(fae) TAG(foo)

#include "common/Trace.h"

#define TRACE_MODULE    MOD(planets)
#define TRACE_TAGS      TAG(Mercury) TAG(Venus) TAG(Earth) TAG(Mars) TAG(Jupiter) \
                        TAG(Saturn) TAG(Uranus) TAG(Neptune) TAG(Pluto)

#include "common/Trace.h"

#define TRACE_MODULE    MOD(En_Sof)
#define TRACE_TAGS      TAG(Kether) TAG(Chokmah) TAG(Binah) TAG(Chesed) TAG(Geburah) \
                        TAG(Tipareth) TAG(Netzach) TAG(Hod) TAG(Yesod) TAG(Malkuth)

#include "common/Trace.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#ifdef unix
#include <unistd.h>
#endif

using namespace std;
USING_COMMON_NS



TRACE_DEF(advent);
TRACE_DEF(planets);
TRACE_DEF(En_Sof);

void
test_tags(std::ostream& of, const char* tags)
{
    of << "\n--- test for: '" << tags << "'\n";
    TRACE_INIT(tags);
    Trace::print_tag_status();
    of << "\n";
    TRACE(advent.xyzzy, ("try xyzzy"));
    TRACE(planets.Mercury, ("try Mercury"));
    TRACE_CALL(advent.y2, ("call %s", "Y2"));
    TRACE(planets.Earth, ("try Earth"));
    TRACE(En_Sof.Hod, ("try Hon"));
}

int
main(int ac, char** av)
{
    char rbuf[512], dbuf[512];
    int ln = 0;
    std::ifstream* res;
    std::ifstream* data;

    std::ofstream of("_trace_out");

    if (!of) {
        std::cerr << "trace_test: cannot create _trace_out\n";
        std::cerr << "trace_test: FAILED\n";
        exit(1);
    }
    TRACE_OUTPUT(&of);
    test_tags(of, "");
    test_tags(of, "*");
    test_tags(of, "advent,planets.Earth");
    test_tags(of, "En_Sof,!En_Sof.Binah,advent.y2");
    test_tags(of, "*,!En_Sof");

    of.close();

    if (ac == 1) {
        std::cerr << "trace_test: result is in _trace_out\n";
        exit(0);
    }

    res = new std::ifstream("_trace_out");
    if (!res) {
        std::cerr << "trace_test: cannot open _trace_out for reading\n";
        goto fail;
    }

    data = new std::ifstream(av[1]);
    if (!data) {
        std::cerr << "trace_test: cannot open " << av[1] << std::endl;
        goto fail;
    }

    while (data->getline(dbuf, sizeof dbuf)) {
        if (!res->getline(rbuf, sizeof rbuf)) {
            std::cerr << "trace_test: premature end of the result file\n";
            goto fail;
        }
        ln++;
        if (strcmp(rbuf, dbuf)) {
            std::cerr << "trace_test: result and regression test data " << av[1]
                 << " differ in line " << ln << std::endl;
            goto fail;
        }
    }
    if (res->getline(rbuf, sizeof rbuf)) {
        std::cerr << "trace_test: result is longer than regression test data " << av[1] << std::endl;
        goto fail;
    }

    unlink("_trace_out");
    std::cerr << "trace_test: PASSED\n";
    exit(0);

fail:
    std::cerr << "trace_test: result is left in _trace_out\n";
    std::cerr << "trace_test: FAILED\n";
    exit(1);
}
