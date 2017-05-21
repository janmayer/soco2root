#include "../src/Event.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TString.h"
#include <functional>
#include <iostream>
#include <map>

// Reads the demo Co-56 file, calibrates detectors and creates a basic gg matrix
// Run with
// $ root -l FlexibleCalibrationAndGGMatrix.C+
// Open results in hdtv:
// hdtv> root open out.root
// hdtv> root 14*
// hdtv> root matrix get sym matrix

class Calibrator
{

    public:
    Calibrator(std::map<UInt_t, std::function<Double_t(const Double_t)>> c)
        : calibrations(c)
    {
    }
    inline Double_t calibrate(const UInt_t id, const UInt_t adc) const
    {
        const Double_t e = Double_t(adc) + rng.Uniform(-0.5, 0.5);
        if (calibrations.find(id) == calibrations.end())
        {
            return -1;
        }
        return calibrations.at(id)(e);
    }

    private:
    std::map<UInt_t, std::function<Double_t(const Double_t)>> calibrations;
    mutable TRandom3 rng;
};

void FlexibleCalibrationAndGGMatrix()
{
    Calibrator calibrator({
        // Use whatever function you desire - you can even use lambda captures if needed
        {14060, [](const Double_t e) { return 0.406680419875 + 0.853743212775 * e; }},
        {14062, [](const Double_t e) { return -0.18738453161 + 0.832039125023 * e; }},
        //{14081, [](const Double_t e) { return -1; }},
        {14082, [](const Double_t e) { return -0.05176810965 + 0.849532979229 * e; }},
        {14100, [](const Double_t e) { return 0.088794690733 + 0.777294537216 * e; }},
        {14102, [](const Double_t e) { return -0.12331931777 + 0.858906160664 * e; }},
        {14103, [](const Double_t e) { return 0.778794791008 + 0.869949385127 * e; }},
        {14222, [](const Double_t e) { return 0.072079089595 + 0.849959160250 * e; }},
        {14230, [](const Double_t e) { return 0.801994576709 + 0.845256775154 * e; }},
        {14232, [](const Double_t e) { return 0.122219068065 + 0.886827134687 * e; }},
        {14260, [](const Double_t e) { return 0.281841350022 + 0.867083214548 * e; }},
        {14262, [](const Double_t e) { return 0.377303419815 + 0.850762935927 * e; }},
        {14263, [](const Double_t e) { return 0.320499526951 + 0.883220460266 * e; }},
        //{14220, [](const Double_t e) { return -1; }}
        //{4711, [](const Double_t e) { return 5 * sin(e) + 24; }}
    });

    SOCO::Event* event = new SOCO::Event();

    TChain* chain = new TChain("ttree");
    chain->Add("Co-56.root");
    chain->SetBranchAddress("events", &event);
    const Long64_t nEntries = chain->GetEntries();
    std::cout << "Total number of events: " << nEntries << std::endl;


    std::map<UInt_t, TH1D*> histograms;
    TH2D mat("matrix", "Gamma-Gamma Matrix", 2000, 0, 4000, 2000, 0, 4000);

    for (Long64_t i = 0; i < nEntries; i++)
    {
        chain->GetEntry(i);
        for (const auto& hit : event->hits)
        {
            if (histograms[hit.id] == nullptr)
            {
                histograms[hit.id] =
                    new TH1D(TString::Itoa(hit.id, 10), TString::Itoa(hit.id, 10), 10000, 0, 5000);
            }
            histograms.at(hit.id)->Fill(calibrator.calibrate(hit.id, hit.adc));
        }

        for (auto a = event->hits.cbegin(); a != event->hits.cend(); a++)
        {
            for (auto b = a + 1; b != event->hits.cend(); b++)
            {
                const Double_t ea = calibrator.calibrate(a->id, a->adc);
                const Double_t eb = calibrator.calibrate(b->id, b->adc);
                mat.Fill(ea, eb);
                mat.Fill(eb, ea);
            }
        }
    }

    TFile out("out.root", "RECREATE");

    for (auto idNHist : histograms)
    {
        idNHist.second->Write();
    }
    mat.Write();
    out.Close();

    mat.DrawClone("colz");
}
