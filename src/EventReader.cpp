#include "EventReader.h"

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "FSUtils.h"

#define SOCO_LIKELY_UNLIKELY 1
#if SOCO_LIKELY_UNLIKELY
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif /* SOCO_LIKELY_UNLIKELY */

using namespace std;

namespace
{

/* Using SFINAE to allow converting to base types and pointers seamlessly */

/* Only available to template deduction if T is not a pointer */
template <typename T>
typename std::enable_if<!std::is_pointer<T>::value, T>::type
interpret_as(const uint8_t* p, const size_t offset) noexcept
{
    return *reinterpret_cast<const T*>(p + offset);
}

/* Only available to template deduction if T is a pointer, using TMP to add
 * const specifier to the pointer type
 */
template <typename T>
typename std::enable_if<std::is_pointer<T>::value,
                        typename std::add_pointer<typename std::add_const<typename std::remove_pointer<T>::type>::type>::type>::type
interpret_as(const uint8_t* p, const size_t offset) noexcept
{
    using ReturnType =
        typename std::add_pointer<typename std::add_const<typename std::remove_pointer<T>::type>::type>::type;
    return reinterpret_cast<ReturnType>(p + offset);
}

} // namespace {anonymous}

namespace SOCO
{

constexpr size_t HIT_SIZE = (2 * sizeof(uint16_t) + sizeof(uint64_t));

EventReader::EventReader()
    : raw_data_{nullptr}
    , mapped_bytes_{0}
    , next_{0}
    , first_data_{0}
    , num_events_{0}
    , filename_{}
    , metadata_{}
{
}

EventReader::EventReader(EventReader&& r)
    : raw_data_{std::move(r.raw_data_)}
    , mapped_bytes_{std::move(r.mapped_bytes_)}
    , next_(std::move(r.next_))
    , first_data_{std::move(r.first_data_)}
    , num_events_{std::move(r.num_events_)}
    , filename_{std::move(r.filename_)}
    , metadata_{std::move(r.metadata_)}
{
    r.raw_data_     = nullptr;
    r.mapped_bytes_ = r.next_ = r.first_data_ = 0;
}

EventReader::~EventReader()
{
    if (raw_data_)
    {
        munmap(const_cast<uint8_t*>(raw_data_), mapped_bytes_);
        raw_data_ = nullptr;
    }
}

EventReader& EventReader::operator=(EventReader&& rhs)
{
    assert(this != &rhs);

    raw_data_     = std::move(rhs.raw_data_);
    mapped_bytes_ = std::move(rhs.mapped_bytes_);
    next_         = std::move(rhs.next_);
    filename_     = std::move(rhs.filename_);
    metadata_     = std::move(rhs.metadata_);

    rhs.raw_data_     = nullptr;
    rhs.mapped_bytes_ = rhs.next_ = 0;

    return *this;
}

void EventReader::mapFile(string filename, bool use_mmap)
{
    assert(raw_data_ == nullptr);
    filename_ = std::move(filename);

    // bool use_mmap = config.getBooleanValue("SOCO.UseMMAP");
    if (use_mmap && FSUtils::isRemoteOrSharedFS(filename_))
    {
        use_mmap = false;
    }

    struct stat sb;
    if (use_mmap)
    {
        raw_data_ = static_cast<const uint8_t*>(FSUtils::mmap(filename_, &sb));
    }
    else
    {
        FSUtils::stat(filename_, &sb);
        if (!S_ISREG(sb.st_mode))
        {
            throw std::runtime_error("EventReader::mapFile - " + filename_ +
                                     " is not a regular file");
        }

        int fd = open(filename_.c_str(), O_RDONLY | O_DIRECT);
        if (fd == -1)
        {
            // try opening file without O_DIRECT, may be unsupported
            fd = open(filename_.c_str(), O_RDONLY);
        }
        if (fd == -1)
        {
            throw std::runtime_error("EventReader::mapFile - can't open " + filename_ + ": " + strerror(errno));
        }

        uint8_t* data = static_cast<uint8_t*>(
            ::mmap(0, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0));
        assert(data && "mmap-style malloc failed");
        read(fd, data, sb.st_size);
        while (close(fd) == -1 && errno == EINTR)
            ;
        raw_data_ = data;
    }
    mapped_bytes_ = sb.st_size;
    next_         = 0;

    readHeader();
}

void EventReader::readHeader()
{
    assert(raw_data_ != nullptr);
    assert(next_ < mapped_bytes_);

    if (mapped_bytes_ < next_ + sizeof(EventHeader))
    {
        throw runtime_error(
            "EventReader::readHeader() - "
            "not enough data in file");
    }

    const EventHeader* header = interpret_as<EventHeader*>(raw_data_, next_);
    if (header->magic != SOCO_EVENT_MAGIC)
    {
        throw runtime_error(
            "EventReader::readHeader() - "
            "invalid magic");
    }
    num_events_ = header->event_count;

    next_ += sizeof(EventHeader);
    bool data_start_found = false;
    while (!data_start_found)
    {
        if (next_ + sizeof(uint64_t) > mapped_bytes_)
        {
            throw runtime_error(
                "EventReader::readHeader() - "
                "not enough data in file");
        }

        const uint64_t magic = interpret_as<uint64_t>(raw_data_, next_);
        if (magic == SOCO_META_MAGIC)
        {
            readMetadata();
        }
        else if (magic == SOCO_DATA_MAGIC)
        {
            next_ += sizeof(uint64_t);
            data_start_found = true;
        }
        else
        {
            throw runtime_error("EventReader::() - invalid magic");
        }
    }
    first_data_ = next_;
}

void EventReader::readMetadata()
{
    assert(raw_data_ != nullptr);
    assert(next_ < mapped_bytes_);

    while (next_ + sizeof(uint64_t) <= mapped_bytes_)
    {
        const uint64_t magic = interpret_as<uint64_t>(raw_data_, next_);
        if (magic != SOCO_META_MAGIC)
        {
            break;
        }

        if (next_ + sizeof(EventMetadataHeader) > mapped_bytes_)
        {
            throw runtime_error(
                "EventReader::readMetadata() - "
                "not enough data in file");
        }

        const EventMetadataHeader* header = interpret_as<EventMetadataHeader*>(raw_data_, next_);
        if (next_ + sizeof(EventMetadataHeader) + header->size > mapped_bytes_)
        {
            throw runtime_error(
                "EventReader::readMetadata() - "
                "not enough data in file");
        }
        next_ += sizeof(EventMetadataHeader);
        metadata_.emplace_back(interpret_as<char*>(raw_data_, next_),
                               interpret_as<char*>(raw_data_, next_ + header->size));
        next_ += header->size;
    }
}

std::vector<Event> EventReader::readAllEvents()
{
    std::vector<Event> events;
    if (!raw_data_)
    {
        return events;
    }
    size_t pos = first_data_;
    uint64_t timestamp;
    events.reserve(numberOfEvents());
    while (pos < mapped_bytes_)
    {
        timestamp = 0;

        const size_t multiplicity = raw_data_[pos++];
        const size_t size         = multiplicity * HIT_SIZE;
        if (unlikely((pos + size) > mapped_bytes_))
        {
            break;
        }
        const uint16_t trigger = interpret_as<uint16_t>(raw_data_, pos);
        pos += sizeof(uint16_t);


        std::vector<Hit> hits;
        hits.reserve(multiplicity);

        for (size_t i = 0; i < multiplicity; ++i)
        {
            const uint16_t id  = interpret_as<uint16_t>(raw_data_, pos);
            const uint64_t ts  = interpret_as<uint64_t>(raw_data_, pos + 2);
            const uint16_t adc = interpret_as<uint16_t>(raw_data_, pos + 10);
            if (id == trigger)
            {
                timestamp = ts;
            }
            hits.emplace_back(id, adc, ts);
            pos += HIT_SIZE;
        }
        events.emplace_back(trigger, timestamp, std::move(hits));
    }
    return events;
}

bool EventReader::getNextEvent(Event& e)
{
    if (unlikely(!raw_data_ || next_ >= mapped_bytes_))
    {
        return false;
    }

    const size_t multiplicity = raw_data_[next_++];
    const size_t event_size   = multiplicity * HIT_SIZE;

    if (unlikely((next_ + event_size) > mapped_bytes_))
    {
        return false;
    }

    // only now we are sure to have all the data and can modify e
    e.clear();
    e.trigger_id = interpret_as<uint16_t>(raw_data_, next_);
    next_ += sizeof(uint16_t);

    e.hits.reserve(multiplicity);
    for (size_t i = 0; i < multiplicity; ++i)
    {
        e.hits.emplace_back(interpret_as<uint16_t>(raw_data_, next_),
                            interpret_as<uint16_t>(raw_data_, next_ + 10),
                            interpret_as<uint64_t>(raw_data_, next_ + 2));
        next_ += HIT_SIZE;
    }
    return true;
}

} // namespace SOCO
