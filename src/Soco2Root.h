#ifndef SOCO2ROOT_SOCO2ROOT_H
#define SOCO2ROOT_SOCO2ROOT_H

#include <string>

class Soco2Root
{
    public:
    Soco2Root(const std::string& in, const std::string& out);

    void process();

    private:
    std::string input;
    std::string output;
};

#endif // SOCO2ROOT_SOCO2ROOT_H
