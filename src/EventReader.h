#ifndef SOCO_EVENTREADER_HH
#define SOCO_EVENTREADER_HH

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

#include "Event.h"
#include <string>

namespace SOCO
{

class EventReader
{
    protected:
    const uint8_t* raw_data_;
    size_t mapped_bytes_;
    size_t next_;
    size_t first_data_;
    uint64_t num_events_;
    std::string filename_;
    std::vector<std::string> metadata_;

    public:
    explicit EventReader();
    EventReader(EventReader&& r);
    ~EventReader();
    EventReader& operator=(EventReader&& rhs);

    // NonCopyable
    EventReader(const EventReader&) = delete;
    EventReader& operator=(const EventReader&) = delete;

    size_t metadataSize() const { return metadata_.size(); }

    const std::string& operator[](const size_t n) const { return metadata_[n]; }

    void mapFile(std::string filename, bool use_mmap = true);

    std::vector<Event> readAllEvents();
    bool getNextEvent(Event& h);

    const std::string& getFilename() const { return filename_; }

    uint64_t numberOfEvents() const { return num_events_; }

    bool isMapped() const { return (raw_data_ != nullptr); }

    private:
    void readHeader();
    void readMetadata();
};

} // namespace SOCO

#endif // SOCO_EVENTREADER_HH
