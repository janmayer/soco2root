#include "Soco2Root.h"

int main()
{
    Soco2Root s2r("test.evt", "test.root");
    s2r.process();

    return 0;
}