void TChainEventLoop()
{
    SOCO::Event* event = new SOCO::Event();

    TChain* chain = new TChain("ttree");
    chain->Add("Co-56*.root");
    chain->SetBranchAddress("events", &event);
    const Long64_t nEntries = chain->GetEntries();
    std::cout << "Total number of events: " << nEntries << std::endl;

    for (Long64_t i = 0; i < nEntries && i < 10; i++)
    {
        chain->GetEntry(i);
        std::cout << i << " - " << event->timestamp << ": " << event->hits.size() << std::endl;
        for (const auto& hit : event->hits)
        {
            std::cout << "\t" << hit.id << ":" << hit.adc;
        }
        std::cout << std::endl;
    }
}
