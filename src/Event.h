#ifndef SOCO_EVENT_HH
#define SOCO_EVENT_HH

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
