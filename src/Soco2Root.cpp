#include "Soco2Root.h"

#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "Event.h"
#include "EventReader.h"

Soco2Root::Soco2Root(const std::string& in, const std::string& out)
    : input(in)
    , output(out)
{
}

void Soco2Root::process()
{
    try
    {
        SOCO::Event event;

        SOCO::EventReader eventReader;
        eventReader.mapFile(input);

        TFile tfile(output.c_str(), "RECREATE");
        TTree ttree("ttree", "SOCO Events");
        ttree.Branch("events", &event);

        while (eventReader.getNextEvent(event))
        {
            ttree.Fill();
        }

        tfile.Write();
        ttree.Print();
        tfile.Close();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
