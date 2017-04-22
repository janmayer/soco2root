#ifndef SOCO2ROOT_ROOTWRITER_H
#define SOCO2ROOT_ROOTWRITER_H

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "Hit.h"

class RootWriter
{
    private:
    TFile tfile;
    TTree ttree;

    public:
    RootWriter(const std::string filename);

    void connect(const std::vector<SOCO::Hit>* pHits);
    void process();
    void disconnect();
};

#endif // SOCO2ROOT_ROOTWRITER_H
