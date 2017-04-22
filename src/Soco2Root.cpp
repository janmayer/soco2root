#include "Soco2Root.h"

#include <iostream>

#include "Event.h"
#include "EventReader.h"
#include "RootWriter.h"

Soco2Root::Soco2Root(const std::string& in, const std::string& out)
    : input(in)
    , output(out)
{
}

void Soco2Root::process()
{
    try
    {
        SOCO::Event e;

        SOCO::EventReader eR;
        eR.mapFile("test.evt");

        RootWriter eW("test.root");
        eW.connect(&e.hits);

        while (eR.getNextEvent(e))
        {
            eW.process();
        }
        eW.disconnect();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
