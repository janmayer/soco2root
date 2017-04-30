#ifndef SOCO_HIT_HH
#define SOCO_HIT_HH

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

// This file is based on SOCOv2, https://gitlab.ikp.uni-koeln.de/nima/soco-v2

#include <cassert>
#include <cstdint>
#include <ostream>
#include <vector>

#include "TObject.h"

namespace SOCO
{

class Hit : public TObject
{
    public:
    uint64_t timestamp;
    uint16_t id;
    uint16_t adc;

    public:
    Hit() = default;

    Hit(const uint16_t id_, const uint16_t adc_, const uint64_t timestamp_)
        : timestamp{timestamp_}
        , id{id_}
        , adc{adc_}
    {
    }

    Hit(const Hit& h) = default;

    Hit(Hit&& h) = default;

    Hit& operator=(const Hit& rhs) = default;

    Hit& operator=(Hit&& rhs) = default;

    virtual ~Hit() = default;

    inline bool operator==(const Hit& rhs) const
    {
        return (id == rhs.id && adc == rhs.adc && timestamp == rhs.timestamp);
    }

    inline bool operator<(const Hit& rhs) const { return (timestamp < rhs.timestamp); }

    inline void shiftTimestamp(const uint64_t shift)
    {
        assert(timestamp > shift);
        timestamp -= shift;
    }

    void write(std::ostream& out) const;

    ClassDef(Hit, 1)
};

} // namespace SOCO

#endif // SOCO_HIT_HH
