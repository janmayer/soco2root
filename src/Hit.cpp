#include "Hit.h"

namespace SOCO
{

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

ClassImp(SOCO::Hit)
