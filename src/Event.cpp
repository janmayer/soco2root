#include "Event.h"

namespace SOCO
{

static_assert(sizeof(EventHeader) == 16, "EventHeader must not have padding bytes");

void Event::write(std::ostream& out) const
{
    assert(hits.size() < std::numeric_limits<uint8_t>::max());
    const auto multiplicity = static_cast<uint8_t>(hits.size());

    out.write(reinterpret_cast<const char*>(&multiplicity), sizeof(uint8_t));
    out.write(reinterpret_cast<const char*>(&trigger_id), sizeof(uint16_t));

    for (const auto& hit : hits)
    {
        hit.write(out);
    }
}

} // namespace SOCO

ClassImp(SOCO::Event)
