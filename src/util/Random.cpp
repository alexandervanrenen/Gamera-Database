#include "Random.hpp"
#include <limits>

using namespace std;

namespace dbi {

namespace util {

Random::Random(uint64_t seed)
: seed(seed)
{
}

uint64_t Random::rand()
{
   seed ^= (seed<<13);
   seed ^= (seed>>7);
   return (seed^=(seed<<17));
}

float Random::randScaleFactor()
{
   return static_cast<float>(rand())/numeric_limits<uint64_t>::max();
}

}

}
