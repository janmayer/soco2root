#ifndef SOCO_HIT_HH
#define SOCO_HIT_HH

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
