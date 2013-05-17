#include "BufferFrame.hpp"

namespace dbi {

using namespace std;

char* BufferFrame::getData()
{
   return data.data();
}

}
