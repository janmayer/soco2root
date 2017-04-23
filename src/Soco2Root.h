#ifndef SOCO2ROOT_SOCO2ROOT_H
#define SOCO2ROOT_SOCO2ROOT_H

#include <string>

class Soco2Root
{
    public:
    Soco2Root(const std::string& in, const std::string& out);
    ~Soco2Root()                = default;             // Destructor
    Soco2Root(const Soco2Root&) = delete;              // Copy constructor
    Soco2Root(Soco2Root&&)      = delete;              // Move constructor
    Soco2Root& operator=(const Soco2Root&) & = delete; // Copy assignment operator
    Soco2Root& operator=(Soco2Root&&) & = delete;      // Move assignment operator

    void process();

    private:
    std::string input;
    std::string output;
};

#endif // SOCO2ROOT_SOCO2ROOT_H
