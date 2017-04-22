#ifndef SOCO_EVENTREADER_HH
#define SOCO_EVENTREADER_HH

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
