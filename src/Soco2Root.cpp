#include "Soco2Root.h"

#include <iostream>
#include <mutex>

#include "TFile.h"
#include "TTree.h"

#include "Event.h"
#include "EventReader.h"

static std::mutex cr;

auto threadsavecout = [](const std::string& x) {
    static std::mutex m;
    std::lock_guard<std::mutex> mylock(m);
    std::cout << x << std::endl;
    return;
};

Soco2Root::Soco2Root(const std::string& in, const std::string& out)
    : input(in)
    , output(out)
{
    threadsavecout(input + " -> " + output);
}

void Soco2Root::process()
{
    SOCO::Event event;

    SOCO::EventReader eventReader;
    eventReader.mapFile(input);

    // ROOT is not thread friendly
    // These operations access an implicit global state and have to be locked
    cr.lock();
    TFile tfile(output.c_str(), "RECREATE");
    TTree ttree("ttree", "SOCO Events");
    ttree.SetDirectory(&tfile);
    ttree.Branch("events", &event);
    cr.unlock();

    while (eventReader.getNextEvent(event))
    {
        ttree.Fill();
    }

    cr.lock();
    tfile.Write();
    tfile.Close();
    cr.unlock();
}
