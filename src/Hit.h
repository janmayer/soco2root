#ifndef SOCO_HIT_HH
#define SOCO_HIT_HH

#include <cassert>
#include <cstdint>
#include <ostream>
#include <vector>

namespace SOCO
{

class Hit
{
    public:
    uint64_t timestamp;
    uint16_t id;
    uint16_t adc;

    public:
    Hit() noexcept = default;

    Hit(const uint16_t id_, const uint16_t adc_, const uint64_t timestamp_) noexcept
        : timestamp{timestamp_}
        , id{id_}
        , adc{adc_}
    {
    }

    Hit(const Hit& h) noexcept = default;

    Hit(Hit&& h) noexcept = default;

    Hit& operator=(const Hit& rhs) noexcept = default;

    Hit& operator=(Hit&& rhs) noexcept = default;

    inline bool operator==(const Hit& rhs) const noexcept
    {
        return (id == rhs.id && adc == rhs.adc && timestamp == rhs.timestamp);
    }

    inline bool operator<(const Hit& rhs) const noexcept { return (timestamp < rhs.timestamp); }

    inline void shiftTimestamp(const uint64_t shift) noexcept
    {
        assert(timestamp > shift);
        timestamp -= shift;
    }

    void write(std::ostream& out) const;
};

} // namespace SOCO

#endif // SOCO_HIT_HH
