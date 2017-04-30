#ifndef SOCO2ROOT_SOCO2ROOT_H
#define SOCO2ROOT_SOCO2ROOT_H

/*
soco2root - Convert soco2 event files to root
https://gitlab.ikp.uni-koeln.de/jmayer/soco2root
Copyright (C) 2017  Jan Mayer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
