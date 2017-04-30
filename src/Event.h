#ifndef SOCO_EVENT_HH
#define SOCO_EVENT_HH

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
#include <limits>
#include <ostream>
#include <vector>

#include "TObject.h"

#include "Hit.h"

namespace SOCO
{

constexpr uint64_t SOCO_EVENT_MAGIC = UINT64_C(0x534f434f45564e54);
constexpr uint64_t SOCO_META_MAGIC  = UINT64_C(0x4d45544144415441);
constexpr uint64_t SOCO_DATA_MAGIC  = UINT64_C(0x00534f434f444154);

struct EventHeader
{
    uint64_t magic;
    uint64_t event_count;
};

struct EventMetadataHeader
{
    uint64_t magic;
    size_t size;
};

class Event : public TObject
{
    public:
    std::vector<Hit> hits;
    uint16_t trigger_id;
    uint64_t timestamp;

    public:
    Event() noexcept
        : trigger_id{std::numeric_limits<uint16_t>::max()}
        , timestamp{0}
    {
    }
    Event(const Event&) = default;

    Event(Event&& rhs) noexcept
        : hits{std::move(rhs.hits)}
        , trigger_id{std::move(rhs.trigger_id)}
        , timestamp{std::move(rhs.timestamp)}
    {
    }

    Event(uint16_t trigger, uint64_t ts, std::vector<Hit>&& hits)
        : hits{std::move(hits)}
        , trigger_id{trigger}
        , timestamp{ts}
    {
    }

    Event& operator=(const Event&) = default;

    Event& operator=(Event&& rhs) noexcept
    {
        trigger_id = rhs.trigger_id;
        timestamp  = rhs.timestamp;
        hits       = std::move(rhs.hits);
        return *this;
    }

    virtual ~Event() = default;

    void clear() noexcept
    {
        trigger_id = std::numeric_limits<uint16_t>::max();
        timestamp  = 0;
        hits.clear();
    }

    void write(std::ostream& out) const;

    ClassDef(Event, 1)
};

} // namespace SOCO

#endif // SOCO_EVENT_HH
