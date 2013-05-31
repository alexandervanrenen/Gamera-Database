#include "Extent.hpp"
#include <sstream>

using namespace std;

namespace dbi {

ostream& operator<<(ostream& out, const Extent& e)
{
	return out << e.begin() << " " << e.end();
}

}
