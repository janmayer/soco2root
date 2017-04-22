#include "RootWriter.h"

RootWriter::RootWriter(const std::string filename)
    : tfile(filename.c_str(), "RECREATE")
    , ttree("ttree", "SOCO2 Events")
{
}

void RootWriter::connect(const std::vector<SOCO::Hit>* pHits)
{
    ttree.Branch("hits", "std::vector<Hit>", &pHits);
}

void RootWriter::process() { ttree.Fill(); }

void RootWriter::disconnect()
{
    tfile.Write();
    ttree.Print();
    tfile.Close();
}
