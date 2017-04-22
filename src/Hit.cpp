#include "Hit.h"

namespace SOCO
{

static_assert(std::is_trivial<Hit>::value,
              "SOCO::Hit must be trivial (see "
              "http://en.cppreference.com/w/cpp/"
              "concept/TrivialType");

void Hit::write(std::ostream& out) const
{
    out.write(reinterpret_cast<const char*>(&id), sizeof(id));
    out.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    out.write(reinterpret_cast<const char*>(&adc), sizeof(adc));

    if (out.bad())
    {
        throw std::runtime_error("Hit::write: failed to write hit!");
    }
}

} // namespace SOCO
